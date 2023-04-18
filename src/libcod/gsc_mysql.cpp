#include "gsc_mysql.hpp"

#if COMPILE_MYSQL == 1

#include "../qcommon/sys_thread.h"
#include <mysql/mysql.h>

struct mysql_async_task
{
	mysql_async_task *prev;
	mysql_async_task *next;
	int id;
	MYSQL_RES *result;
	bool done;
	bool started;
	bool save;
	char query[COD2_MAX_STRINGLENGTH + 1];
};

struct mysql_async_connection
{
	mysql_async_connection *prev;
	mysql_async_connection *next;
	mysql_async_task* task;
	MYSQL *connection;
};

mysql_async_connection *first_async_connection = NULL;
mysql_async_task *first_async_task = NULL;
MYSQL *cod_mysql_connection = NULL;

void *mysql_async_execute_query(void *input_c) //cannot be called from gsc, is threaded.
{
	mysql_async_connection *c = (mysql_async_connection *) input_c;
	int res = mysql_query(c->connection, c->task->query);
	if(!res && c->task->save)
		c->task->result = mysql_store_result(c->connection);
	else if(res)
	{
		//mysql show error here?
	}
	c->task->done = true;
	c->task = NULL;
	return NULL;
}

void *mysql_async_query_handler(void* input_nothing) //is threaded after initialize
{
	static bool started = false;
	if(started)
	{
		Com_DPrintf("mysql_async_query_handler() async handler already started. Returning\n");
		return NULL;
	}
	started = true;
	mysql_async_connection *c = first_async_connection;
	if(c == NULL)
	{
		Com_DPrintf("mysql_async_query_handler() async handler started before any connection was initialized\n"); //this should never happen
		started = false;
		return NULL;
	}
	mysql_async_task *q;
	while(true)
	{
		Sys_EnterCriticalSection(CRITSECT_MYSQL);
		q = first_async_task;
		c = first_async_connection;
		while(q != NULL)
		{
			if(!q->started)
			{
				while(c != NULL && c->task != NULL)
					c = c->next;
				if(c == NULL)
				{
					//out of free connections
					break;
				}
				q->started = true;
				c->task = q;
				threadid_t query_doer;
				Sys_CreateNewThread(mysql_async_execute_query, &query_doer, c);
				c = c->next;
			}
			q = q->next;
		}
		Sys_LeaveCriticalSection(CRITSECT_MYSQL);
		Sys_SleepMSec(10);
	}
	return NULL;
}

int mysql_async_query_initializer(const char *sql, bool save) //cannot be called from gsc, helper function
{
	static int id = 0;
	id++;
	Sys_EnterCriticalSection(CRITSECT_MYSQL);
	mysql_async_task *current = first_async_task;
	while(current != NULL && current->next != NULL)
		current = current->next;
	mysql_async_task *newtask = new mysql_async_task;
	newtask->id = id;
	strncpy(newtask->query, sql, COD2_MAX_STRINGLENGTH);
	newtask->prev = current;
	newtask->result = NULL;
	newtask->save = save;
	newtask->done = false;
	newtask->next = NULL;
	newtask->started = false;
	if(current != NULL)
		current->next = newtask;
	else
		first_async_task = newtask;
	Sys_LeaveCriticalSection(CRITSECT_MYSQL);
	return id;
}

void gsc_mysql_async_create_query_nosave()
{
	const char *query;
	if ( ! stackGetParams("s", &query))
	{
		stackError("gsc_mysql_async_create_query_nosave() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}
	int id = mysql_async_query_initializer(query, false);
	stackPushInt(id);
	return;
}

void gsc_mysql_async_create_query()
{
	const char *query;
	if ( ! stackGetParams("s", &query))
	{
		stackError("gsc_mysql_async_create_query() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}
	int id = mysql_async_query_initializer(query, true);
	stackPushInt(id);
	return;
}

void gsc_mysql_async_getdone_list()
{
	Sys_EnterCriticalSection(CRITSECT_MYSQL);
	mysql_async_task *current = first_async_task;
	stackPushArray();
	while(current != NULL)
	{
		if(current->done)
		{
			stackPushInt((int)current->id);
			stackPushArrayLast();
		}
		current = current->next;
	}
	Sys_LeaveCriticalSection(CRITSECT_MYSQL);
}

void gsc_mysql_async_getresult_and_free() //same as above, but takes the id of a function instead and returns 0 (not done), undefined (not found) or the mem address of result
{
	int id;
	if(!stackGetParams("i", &id))
	{
		stackError("gsc_mysql_async_getresult_and_free() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}
	Sys_EnterCriticalSection(CRITSECT_MYSQL);
	mysql_async_task *c = first_async_task;
	if(c != NULL)
	{
		while(c != NULL && c->id != id)
			c = c->next;
	}
	if(c != NULL)
	{
		if(!c->done)
		{
			stackPushUndefined(); //should never happend, query not done yet
			Sys_LeaveCriticalSection(CRITSECT_MYSQL);
			return;
		}
		if(c->next != NULL)
			c->next->prev = c->prev;
		if(c->prev != NULL)
			c->prev->next = c->next;
		else
			first_async_task = c->next;
		if(c->save)
		{
			int ret = (int)c->result;
			stackPushInt(ret);
		}
		else
			stackPushInt(0);
		delete c;
		Sys_LeaveCriticalSection(CRITSECT_MYSQL);
		return;
	}
	else
	{
		stackError("gsc_mysql_async_getresult_and_free() mysql async query id not found");
		stackPushUndefined();
		Sys_LeaveCriticalSection(CRITSECT_MYSQL);
		return;
	}
}

void gsc_mysql_async_initializer()//returns array with mysql connection handlers
{
	if(first_async_connection != NULL)
	{
		Com_DPrintf("gsc_mysql_async_initializer() async mysql already initialized. Returning before adding additional connections\n");
		stackPushUndefined();
		return;
	}

	int port, connection_count;
	const char *host, *user, *pass, *db;

	if ( ! stackGetParams("ssssii", &host, &user, &pass, &db, &port, &connection_count))
	{
		stackError("gsc_mysql_async_initializer() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}
	if(connection_count <= 0)
	{
		stackError("gsc_mysql_async_initializer() need a positive connection_count in mysql_async_initializer");
		stackPushUndefined();
		return;
	}
	int i;
	stackPushArray();
	mysql_async_connection *current = first_async_connection;
	for(i = 0; i < connection_count; i++)
	{
		mysql_async_connection *newconnection = new mysql_async_connection;
		newconnection->next = NULL;
		newconnection->connection = mysql_init(NULL);
		newconnection->connection = mysql_real_connect((MYSQL*)newconnection->connection, host, user, pass, db, port, NULL, 0);
		bool reconnect = true;
		mysql_options(newconnection->connection, MYSQL_OPT_RECONNECT, &reconnect);
		newconnection->task = NULL;
		if(current == NULL)
		{
			newconnection->prev = NULL;
			first_async_connection = newconnection;
		}
		else
		{
			while(current->next != NULL)
				current = current->next;
			current->next = newconnection;
			newconnection->prev = current;
		}
		current = newconnection;
		stackPushInt((int)newconnection->connection);
		stackPushArrayLast();
	}
	threadid_t async_handler;
	Sys_CreateNewThread(mysql_async_query_handler, &async_handler, NULL);
}

void gsc_mysql_init()
{
	MYSQL *my = mysql_init(NULL);
	stackPushInt((int) my);
}

void gsc_mysql_reuse_connection()
{
	if(cod_mysql_connection == NULL)
	{
		stackPushUndefined();
		return;
	}
	else
	{
		stackPushInt((int) cod_mysql_connection);
		return;
	}
}

void gsc_mysql_real_connect()
{
	int mysql, port;
	const char *host, *user, *pass, *db;

	if ( ! stackGetParams("issssi", &mysql, &host, &user, &pass, &db, &port))
	{
		stackError("gsc_mysql_real_connect() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	mysql = (int) mysql_real_connect((MYSQL *)mysql, host, user, pass, db, port, NULL, 0);
	bool reconnect = true;
	mysql_options((MYSQL*)mysql, MYSQL_OPT_RECONNECT, &reconnect);
	if(cod_mysql_connection == NULL)
		cod_mysql_connection = (MYSQL*) mysql;
	stackPushInt(mysql);
}

void gsc_mysql_close()
{
	int mysql;

	if ( ! stackGetParams("i", &mysql))
	{
		stackError("gsc_mysql_close() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	mysql_close((MYSQL *)mysql);
	stackPushInt(0);
}

void gsc_mysql_query()
{
	int mysql;
	const char *query;

	if ( ! stackGetParams("is", &mysql, &query))
	{
		stackError("gsc_mysql_query() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	int ret = mysql_query((MYSQL *)mysql, query);
	stackPushInt(ret);
}

void gsc_mysql_errno()
{
	int mysql;

	if ( ! stackGetParams("i", &mysql))
	{
		stackError("gsc_mysql_errno() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	int ret = mysql_errno((MYSQL *)mysql);
	stackPushInt(ret);
}

void gsc_mysql_error()
{
	int mysql;

	if ( ! stackGetParams("i", &mysql))
	{
		stackError("gsc_mysql_error() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	char *ret = (char *)mysql_error((MYSQL *)mysql);
	stackPushString(ret);
}

void gsc_mysql_affected_rows()
{
	int mysql;

	if ( ! stackGetParams("i", &mysql))
	{
		stackError("gsc_mysql_affected_rows() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	int ret = mysql_affected_rows((MYSQL *)mysql);
	stackPushInt(ret);
}

void gsc_mysql_store_result()
{
	int mysql;

	if ( ! stackGetParams("i", &mysql))
	{
		stackError("gsc_mysql_store_result() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	MYSQL_RES *result = mysql_store_result((MYSQL *)mysql);
	stackPushInt((int) result);
}

void gsc_mysql_num_rows()
{
	int result;

	if ( ! stackGetParams("i", &result))
	{
		stackError("gsc_mysql_num_rows() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	int ret = mysql_num_rows((MYSQL_RES *)result);
	stackPushInt(ret);
}

void gsc_mysql_num_fields()
{
	int result;

	if ( ! stackGetParams("i", &result))
	{
		stackError("gsc_mysql_num_fields() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	int ret = mysql_num_fields((MYSQL_RES *)result);
	stackPushInt(ret);
}

void gsc_mysql_field_seek()
{
	int result;
	int offset;

	if ( ! stackGetParams("ii", &result, &offset))
	{
		stackError("gsc_mysql_field_seek() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	int ret = mysql_field_seek((MYSQL_RES *)result, offset);
	stackPushInt(ret);
}

void gsc_mysql_fetch_field()
{
	int result;

	if ( ! stackGetParams("i", &result))
	{
		stackError("gsc_mysql_fetch_field() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	MYSQL_FIELD *field = mysql_fetch_field((MYSQL_RES *)result);
	if (field == NULL)
	{
		stackPushUndefined();
		return;
	}
	char *ret = field->name;
	stackPushString(ret);
}

void gsc_mysql_fetch_row()
{
	int result;

	if ( ! stackGetParams("i", &result))
	{
		stackError("gsc_mysql_fetch_row() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	MYSQL_ROW row = mysql_fetch_row((MYSQL_RES *)result);
	if (!row)
	{
		stackPushUndefined();
		return;
	}

	stackPushArray();

	int numfields = mysql_num_fields((MYSQL_RES *)result);
	for (int i=0; i<numfields; i++)
	{
		if (row[i] == NULL)
			stackPushUndefined();
		else
			stackPushString(row[i]);

		stackPushArrayLast();
	}
}

void gsc_mysql_free_result()
{
	int result;

	if ( ! stackGetParams("i", &result))
	{
		stackError("gsc_mysql_free_result() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}
	if(result == 0)
	{
		stackError("mysql_free_result() input is a NULL-pointer");
		stackPushUndefined();
		return;
	}
	mysql_free_result((MYSQL_RES *)result);
	stackPushUndefined();
}

void gsc_mysql_real_escape_string()
{
	int mysql;
	const char *str;

	if ( ! stackGetParams("is", &mysql, &str))
	{
		stackError("gsc_mysql_real_escape_string() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	char *to = (char *) malloc(strlen(str) * 2 + 1);
	mysql_real_escape_string((MYSQL *)mysql, to, str, strlen(str));
	stackPushString(to);
	free(to);
}

#endif
