#include "gsc_sqlite.hpp"

#if COMPILE_SQLITE == 1

#include "../qcommon/sys_thread.h"
#include "sqlite/sqlite3.h"

#define MAX_SQLITE_FIELDS 128
#define MAX_SQLITE_ROWS 128
#define MAX_SQLITE_ROW_LENGTH 256
#define MAX_SQLITE_TASKS 512
#define MAX_SQLITE_DB_STORES 64

#define SQLITE_TIMEOUT 4000

enum
{
	INT_VALUE,
	FLOAT_VALUE,
	STRING_VALUE,
	VECTOR_VALUE,
	OBJECT_VALUE
};

struct async_sqlite_task
{
	async_sqlite_task *prev;
	async_sqlite_task *next;
	sqlite3 *db;
	sqlite3_stmt *statement;
	char query[COD2_MAX_STRINGLENGTH];
	char row[MAX_SQLITE_FIELDS][MAX_SQLITE_ROWS][MAX_SQLITE_ROW_LENGTH];
	int result;
	int fields_size;
	int rows_size;
	int callback;
	bool done;
	bool save;
	bool error;
	bool remove;
	char errorMessage[COD2_MAX_STRINGLENGTH];
	bool hasargument;
	int valueType;
	int intValue;
	float floatValue;
	char stringValue[COD2_MAX_STRINGLENGTH];
	vec3_t vectorValue;
	unsigned int objectValue;
	bool hasentity;
	gentity_t *gentity;
};

struct sqlite_db_store
{
	sqlite_db_store *prev;
	sqlite_db_store *next;
	sqlite3 *db;
};

async_sqlite_task *first_async_sqlite_task = NULL;
sqlite_db_store *first_sqlite_db_store = NULL;
int async_sqlite_initialized = 0;

void free_sqlite_db_stores_and_tasks()
{
	Sys_EnterCriticalSection(CRITSECT_SQLITE);

	async_sqlite_task *current = first_async_sqlite_task;

	while (current != NULL)
	{
		async_sqlite_task *task = current;
		current = current->next;

		if (task->statement != NULL)
		{
			sqlite3_finalize(task->statement);
			task->statement = NULL;
		}

		if (task->next != NULL)
			task->next->prev = task->prev;

		if (task->prev != NULL)
			task->prev->next = task->next;
		else
			first_async_sqlite_task = task->next;

		delete task;
	}

	sqlite_db_store *current_store = first_sqlite_db_store;

	while (current_store != NULL)
	{
		sqlite_db_store *store = current_store;
		current_store = current_store->next;

		if (store->db != NULL)
			sqlite3_close(store->db);

		if (store->next != NULL)
			store->next->prev = store->prev;

		if (store->prev != NULL)
			store->prev->next = store->next;
		else
			first_sqlite_db_store = store->next;

		delete store;
	}

	Sys_LeaveCriticalSection(CRITSECT_SQLITE);
}

void *async_sqlite_query_handler(void*)
{
	while(1)
	{
		Sys_EnterCriticalSection(CRITSECT_SQLITE);

		async_sqlite_task *current = first_async_sqlite_task;

		while (current != NULL)
		{
			async_sqlite_task *task = current;
			current = current->next;

			if (!task->done)
			{
				task->result = sqlite3_prepare_v2(task->db, task->query, COD2_MAX_STRINGLENGTH, &task->statement, 0);

				if (task->result != SQLITE_OK)
				{
					task->error = true;
					strncpy(task->errorMessage, sqlite3_errmsg(task->db), COD2_MAX_STRINGLENGTH - 1);
					task->errorMessage[COD2_MAX_STRINGLENGTH - 1] = '\0';
				}

				if (!task->error)
				{
					task->result = sqlite3_step(task->statement);
					task->fields_size = 0;

					while (task->result != SQLITE_DONE)
					{
						if (task->result == SQLITE_ROW)
						{
							if (task->save && task->callback)
							{
								if (task->fields_size >= MAX_SQLITE_FIELDS)
									break;

								task->rows_size = 0;

								for (int i = 0; i < sqlite3_column_count(task->statement); i++)
								{
									if (task->rows_size >= MAX_SQLITE_ROWS)
										break;

									const unsigned char *text = sqlite3_column_text(task->statement, i);

									if (text != NULL)
									{
										strncpy(task->row[task->fields_size][task->rows_size], reinterpret_cast<const char*>(text), MAX_SQLITE_ROW_LENGTH - 1);
										task->row[task->fields_size][task->rows_size][MAX_SQLITE_ROW_LENGTH - 1] = '\0';
									}

									task->rows_size++;
								}

								task->fields_size++;
							}
						}
						else
						{
							task->error = true;
							strncpy(task->errorMessage, sqlite3_errmsg(task->db), COD2_MAX_STRINGLENGTH - 1);
							task->errorMessage[COD2_MAX_STRINGLENGTH - 1] = '\0';
							break;
						}

						task->result = sqlite3_step(task->statement);
					}

					if (task->statement != NULL)
					{
						sqlite3_finalize(task->statement);
						task->statement = NULL;
					}
				}

				task->done = true;
			}
		}

		Sys_LeaveCriticalSection(CRITSECT_SQLITE);

		Sys_SleepMSec(50);
	}

	return NULL;
}

void gsc_async_sqlite_initialize()
{
	threadid_t tinfo;

	if (!async_sqlite_initialized)
	{
		Sys_CreateNewThread(async_sqlite_query_handler, &tinfo, NULL);
		async_sqlite_initialized = 1;
	}
	else
		Com_DPrintf("gsc_async_sqlite_initialize() async handler already initialized.\n");

	stackPushInt(async_sqlite_initialized);
}

void gsc_async_sqlite_create_query()
{
	int db;
	const char *query;

	if ( ! stackGetParams("is", &db, &query))
	{
		stackError("gsc_async_sqlite_create_query() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (!async_sqlite_initialized)
	{
		stackError("gsc_async_sqlite_create_query() async handler has not been initialized");
		stackPushUndefined();
		return;
	}

	async_sqlite_task *current = first_async_sqlite_task;

	int task_count = 0;

	while (current != NULL && current->next != NULL)
	{
		if (task_count >= MAX_SQLITE_TASKS - 2)
		{
			stackError("gsc_async_sqlite_create_query() exceeded async task limit");
			stackPushUndefined();
			return;
		}

		current = current->next;
		task_count++;
	}

	async_sqlite_task *newtask = new async_sqlite_task;

	newtask->prev = current;
	newtask->next = NULL;

	newtask->db = (sqlite3 *)db;

	strncpy(newtask->query, query, COD2_MAX_STRINGLENGTH - 1);
	newtask->query[COD2_MAX_STRINGLENGTH - 1] = '\0';

	int callback;

	if (!stackGetParamFunction(2, &callback))
		newtask->callback = 0;
	else
		newtask->callback = callback;

	newtask->done = false;
	newtask->save = true;
	newtask->error = false;
	newtask->remove = false;
	newtask->hasargument = true;
	newtask->hasentity = false;
	newtask->gentity = NULL;

	int valueInt;
	float valueFloat;
	const char *valueString;
	vec3_t valueVector;
	unsigned int valueObject;

	if (stackGetParamInt(3, &valueInt))
	{
		newtask->valueType = INT_VALUE;
		newtask->intValue = valueInt;
	}
	else if (stackGetParamFloat(3, &valueFloat))
	{
		newtask->valueType = FLOAT_VALUE;
		newtask->floatValue = valueFloat;
	}
	else if (stackGetParamString(3, &valueString))
	{
		newtask->valueType = STRING_VALUE;
		strcpy(newtask->stringValue, valueString);
	}
	else if (stackGetParamVector(3, valueVector))
	{
		newtask->valueType = VECTOR_VALUE;
		newtask->vectorValue[0] = valueVector[0];
		newtask->vectorValue[1] = valueVector[1];
		newtask->vectorValue[2] = valueVector[2];
	}
	else if (stackGetParamObject(3, &valueObject))
	{
		newtask->valueType = OBJECT_VALUE;
		newtask->objectValue = valueObject;
	}
	else
		newtask->hasargument = false;

	if (current != NULL)
		current->next = newtask;
	else
		first_async_sqlite_task = newtask;

	stackPushBool(qtrue);
}

void gsc_async_sqlite_create_query_nosave()
{
	int db;
	const char *query;

	if ( ! stackGetParams("is", &db, &query))
	{
		stackError("gsc_async_sqlite_create_query_nosave() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (!async_sqlite_initialized)
	{
		stackError("gsc_async_sqlite_create_query_nosave() async handler has not been initialized");
		stackPushUndefined();
		return;
	}

	async_sqlite_task *current = first_async_sqlite_task;

	int task_count = 0;

	while (current != NULL && current->next != NULL)
	{
		if (task_count >= MAX_SQLITE_TASKS - 2)
		{
			stackError("gsc_async_sqlite_create_query_nosave() exceeded async task limit");
			stackPushUndefined();
			return;
		}

		current = current->next;
		task_count++;
	}

	async_sqlite_task *newtask = new async_sqlite_task;

	newtask->prev = current;
	newtask->next = NULL;

	newtask->db = (sqlite3 *)db;

	strncpy(newtask->query, query, COD2_MAX_STRINGLENGTH - 1);
	newtask->query[COD2_MAX_STRINGLENGTH - 1] = '\0';

	int callback;

	if (!stackGetParamFunction(2, &callback))
		newtask->callback = 0;
	else
		newtask->callback = callback;

	newtask->done = false;
	newtask->save = false;
	newtask->error = false;
	newtask->remove = false;
	newtask->hasargument = true;
	newtask->hasentity = false;
	newtask->gentity = NULL;

	int valueInt;
	float valueFloat;
	const char *valueString;
	vec3_t valueVector;
	unsigned int valueObject;

	if (stackGetParamInt(3, &valueInt))
	{
		newtask->valueType = INT_VALUE;
		newtask->intValue = valueInt;
	}
	else if (stackGetParamFloat(3, &valueFloat))
	{
		newtask->valueType = FLOAT_VALUE;
		newtask->floatValue = valueFloat;
	}
	else if (stackGetParamString(3, &valueString))
	{
		newtask->valueType = STRING_VALUE;
		strcpy(newtask->stringValue, valueString);
	}
	else if (stackGetParamVector(3, valueVector))
	{
		newtask->valueType = VECTOR_VALUE;
		newtask->vectorValue[0] = valueVector[0];
		newtask->vectorValue[1] = valueVector[1];
		newtask->vectorValue[2] = valueVector[2];
	}
	else if (stackGetParamObject(3, &valueObject))
	{
		newtask->valueType = OBJECT_VALUE;
		newtask->objectValue = valueObject;
	}
	else
		newtask->hasargument = false;

	if (current != NULL)
		current->next = newtask;
	else
		first_async_sqlite_task = newtask;

	stackPushBool(qtrue);
}

void gsc_async_sqlite_create_entity_query(scr_entref_t entid)
{
	int db;
	const char *query;

	if ( ! stackGetParams("is", &db, &query))
	{
		stackError("gsc_async_sqlite_create_entity_query() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (!async_sqlite_initialized)
	{
		stackError("gsc_async_sqlite_create_entity_query() async handler has not been initialized");
		stackPushUndefined();
		return;
	}

	async_sqlite_task *current = first_async_sqlite_task;

	int task_count = 0;

	while (current != NULL && current->next != NULL)
	{
		if (task_count >= MAX_SQLITE_TASKS - 2)
		{
			stackError("gsc_async_sqlite_create_entity_query() exceeded async task limit");
			stackPushUndefined();
			return;
		}

		current = current->next;
		task_count++;
	}

	async_sqlite_task *newtask = new async_sqlite_task;

	newtask->prev = current;
	newtask->next = NULL;

	newtask->db = (sqlite3 *)db;

	strncpy(newtask->query, query, COD2_MAX_STRINGLENGTH - 1);
	newtask->query[COD2_MAX_STRINGLENGTH - 1] = '\0';

	int callback;

	if (!stackGetParamFunction(2, &callback))
		newtask->callback = 0;
	else
		newtask->callback = callback;

	newtask->done = false;
	newtask->save = true;
	newtask->error = false;
	newtask->remove = false;
	newtask->hasargument = true;
	newtask->hasentity = true;
	newtask->gentity = &g_entities[entid.entnum];

	int valueInt;
	float valueFloat;
	const char *valueString;
	vec3_t valueVector;
	unsigned int valueObject;

	if (stackGetParamInt(3, &valueInt))
	{
		newtask->valueType = INT_VALUE;
		newtask->intValue = valueInt;
	}
	else if (stackGetParamFloat(3, &valueFloat))
	{
		newtask->valueType = FLOAT_VALUE;
		newtask->floatValue = valueFloat;
	}
	else if (stackGetParamString(3, &valueString))
	{
		newtask->valueType = STRING_VALUE;
		strcpy(newtask->stringValue, valueString);
	}
	else if (stackGetParamVector(3, valueVector))
	{
		newtask->valueType = VECTOR_VALUE;
		newtask->vectorValue[0] = valueVector[0];
		newtask->vectorValue[1] = valueVector[1];
		newtask->vectorValue[2] = valueVector[2];
	}
	else if (stackGetParamObject(3, &valueObject))
	{
		newtask->valueType = OBJECT_VALUE;
		newtask->objectValue = valueObject;
	}
	else
		newtask->hasargument = false;

	if (current != NULL)
		current->next = newtask;
	else
		first_async_sqlite_task = newtask;

	stackPushBool(qtrue);
}

void gsc_async_sqlite_create_entity_query_nosave(scr_entref_t entid)
{
	int db;
	const char *query;

	if ( ! stackGetParams("is", &db, &query))
	{
		stackError("gsc_async_sqlite_create_entity_query_nosave() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (!async_sqlite_initialized)
	{
		stackError("gsc_async_sqlite_create_entity_query_nosave() async handler has not been initialized");
		stackPushUndefined();
		return;
	}

	async_sqlite_task *current = first_async_sqlite_task;

	int task_count = 0;

	while (current != NULL && current->next != NULL)
	{
		if (task_count >= MAX_SQLITE_TASKS - 2)
		{
			stackError("gsc_async_sqlite_create_entity_query_nosave() exceeded async task limit");
			stackPushUndefined();
			return;
		}

		current = current->next;
		task_count++;
	}

	async_sqlite_task *newtask = new async_sqlite_task;

	newtask->prev = current;
	newtask->next = NULL;

	newtask->db = (sqlite3 *)db;

	strncpy(newtask->query, query, COD2_MAX_STRINGLENGTH - 1);
	newtask->query[COD2_MAX_STRINGLENGTH - 1] = '\0';

	int callback;

	if (!stackGetParamFunction(2, &callback))
		newtask->callback = 0;
	else
		newtask->callback = callback;

	newtask->done = false;
	newtask->save = false;
	newtask->error = false;
	newtask->remove = false;
	newtask->hasargument = true;
	newtask->hasentity = true;
	newtask->gentity = &g_entities[entid.entnum];

	int valueInt;
	float valueFloat;
	const char *valueString;
	vec3_t valueVector;
	unsigned int valueObject;

	if (stackGetParamInt(3, &valueInt))
	{
		newtask->valueType = INT_VALUE;
		newtask->intValue = valueInt;
	}
	else if (stackGetParamFloat(3, &valueFloat))
	{
		newtask->valueType = FLOAT_VALUE;
		newtask->floatValue = valueFloat;
	}
	else if (stackGetParamString(3, &valueString))
	{
		newtask->valueType = STRING_VALUE;
		strcpy(newtask->stringValue, valueString);
	}
	else if (stackGetParamVector(3, valueVector))
	{
		newtask->valueType = VECTOR_VALUE;
		newtask->vectorValue[0] = valueVector[0];
		newtask->vectorValue[1] = valueVector[1];
		newtask->vectorValue[2] = valueVector[2];
	}
	else if (stackGetParamObject(3, &valueObject))
	{
		newtask->valueType = OBJECT_VALUE;
		newtask->objectValue = valueObject;
	}
	else
		newtask->hasargument = false;

	if (current != NULL)
		current->next = newtask;
	else
		first_async_sqlite_task = newtask;

	stackPushBool(qtrue);
}

void gsc_async_sqlite_checkdone()
{
	async_sqlite_task *current = first_async_sqlite_task;

	while (current != NULL)
	{
		async_sqlite_task *task = current;
		current = current->next;

		if (!task->remove)
		{
			if (task->done)
			{
				if (!task->error)
				{
					if (task->save && task->callback)
					{
						if (task->hasentity)
						{
							if (task->gentity != NULL)
							{
								if (task->hasargument)
								{
									switch(task->valueType)
									{
									case INT_VALUE:
										stackPushInt(task->intValue);
										break;

									case FLOAT_VALUE:
										stackPushFloat(task->floatValue);
										break;

									case STRING_VALUE:
										stackPushString(task->stringValue);
										break;

									case VECTOR_VALUE:
										stackPushVector(task->vectorValue);
										break;

									case OBJECT_VALUE:
										stackPushObject(task->objectValue);
										break;

									default:
										stackPushUndefined();
										break;
									}
								}

								stackPushArray();

								for (int i = 0; i < task->fields_size; i++)
								{
									stackPushArray();

									for (int x = 0; x < task->rows_size; x++)
									{
										if (task->row[i][x] != NULL)
										{
											stackPushString(task->row[i][x]);
											stackPushArrayLast();
										}
									}

									stackPushArrayLast();
								}

								short ret = Scr_ExecEntThread(task->gentity, task->callback, task->save + task->hasargument);
								Scr_FreeThread(ret);
							}
						}
						else
						{
							if (task->hasargument)
							{
								switch(task->valueType)
								{
								case INT_VALUE:
									stackPushInt(task->intValue);
									break;

								case FLOAT_VALUE:
									stackPushFloat(task->floatValue);
									break;

								case STRING_VALUE:
									stackPushString(task->stringValue);
									break;

								case VECTOR_VALUE:
									stackPushVector(task->vectorValue);
									break;

								default:
									stackPushUndefined();
									break;
								}
							}

							stackPushArray();

							for (int i = 0; i < task->fields_size; i++)
							{
								stackPushArray();

								for (int x = 0; x < task->rows_size; x++)
								{
									stackPushString(task->row[i][x]);
									stackPushArrayLast();
								}

								stackPushArrayLast();
							}

							short ret = Scr_ExecThread(task->callback, task->save + task->hasargument);
							Scr_FreeThread(ret);
						}
					}
				}
				else if (task->query && task->errorMessage)
					stackError("gsc_async_sqlite_checkdone() query error in '%s' - '%s'", task->query, task->errorMessage);

				task->remove = true;
			}
		}
		else
		{
			if (Sys_TryEnterCriticalSection(CRITSECT_SQLITE) == 0)
			{
				if (task->next != NULL)
					task->next->prev = task->prev;

				if (task->prev != NULL)
					task->prev->next = task->next;
				else
					first_async_sqlite_task = task->next;

				delete task;

				Sys_LeaveCriticalSection(CRITSECT_SQLITE);
			}
		}
	}
}

void gsc_sqlite_open()
{
	const char *database;

	if ( ! stackGetParams("s", &database))
	{
		stackError("gsc_sqlite_open() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	sqlite3 *db;

	int rc = sqlite3_open(database, &db);

	if (rc != SQLITE_OK)
	{
		stackError("gsc_sqlite_open() cannot open database: %s", sqlite3_errmsg(db));
		stackPushUndefined();
		return;
	}

	rc = sqlite3_busy_timeout(db, SQLITE_TIMEOUT);

	if (rc != SQLITE_OK)
	{
		stackError("gsc_sqlite_open() cannot set database busy timeout: %s", sqlite3_errmsg(db));
		sqlite3_close(db);
		stackPushUndefined();
		return;
	}

	sqlite_db_store *current = first_sqlite_db_store;

	int store_count = 0;

	while (current != NULL && current->next != NULL)
	{
		if (store_count >= MAX_SQLITE_DB_STORES - 2)
		{
			stackError("gsc_sqlite_open() exceeded db store limit");
			sqlite3_close(db);
			stackPushUndefined();
			return;
		}

		current = current->next;
		store_count++;
	}

	sqlite_db_store *newstore = new sqlite_db_store;

	newstore->prev = current;
	newstore->next = NULL;

	newstore->db = db;

	if (current != NULL)
		current->next = newstore;
	else
		first_sqlite_db_store = newstore;

	stackPushInt((int)db);
}

void gsc_sqlite_query()
{
	int db;
	const char *query;

	if ( ! stackGetParams("is", &db, &query))
	{
		stackError("gsc_sqlite_query() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	sqlite3_stmt *statement;
	int result;

	result = sqlite3_prepare_v2((sqlite3 *)db, query, COD2_MAX_STRINGLENGTH, &statement, 0);

	if (result != SQLITE_OK)
	{
		stackError("gsc_sqlite_query() failed to fetch query data: %s", sqlite3_errmsg((sqlite3 *)db));
		stackPushUndefined();
	}

	stackPushArray();

	result = sqlite3_step(statement);

	while (result != SQLITE_DONE)
	{
		if (result == SQLITE_ROW)
		{
			stackPushArray();

			for (int i = 0; i < sqlite3_column_count(statement); i++)
			{
				const unsigned char *text = sqlite3_column_text(statement, i);

				if (text != NULL)
				{
					stackPushString(reinterpret_cast<const char*>(text));
					stackPushArrayLast();
				}
			}

			stackPushArrayLast();
		}
		else
		{
			stackError("gsc_sqlite_query() failed to execute query: %s", sqlite3_errmsg((sqlite3 *)db));
			stackPushUndefined();
			sqlite3_finalize(statement);
			return;
		}

		result = sqlite3_step(statement);
	}

	sqlite3_finalize(statement);
}

void gsc_sqlite_close()
{
	int db;

	if ( ! stackGetParams("i", &db))
	{
		stackError("gsc_sqlite_close() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	int rc = sqlite3_close((sqlite3 *)db);

	if (rc != SQLITE_OK)
	{
		stackError("gsc_sqlite_close() cannot close database: %s", sqlite3_errmsg((sqlite3 *)db));
		stackPushUndefined();
		return;
	}

	Sys_EnterCriticalSection(CRITSECT_SQLITE);

	sqlite_db_store *current = first_sqlite_db_store;

	while (current != NULL)
	{
		sqlite_db_store *store = current;
		current = current->next;

		if (store->db == (sqlite3 *)db)
		{
			if (store->next != NULL)
				store->next->prev = store->prev;

			if (store->prev != NULL)
				store->prev->next = store->next;
			else
				first_sqlite_db_store = store->next;

			delete store;
		}
	}

	stackPushBool(qtrue);

	Sys_LeaveCriticalSection(CRITSECT_SQLITE);
}

void gsc_sqlite_escape_string()
{
	const char *string;

	if ( ! stackGetParams("s", &string))
	{
		stackError("gsc_sqlite_escape_string() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	char *result = sqlite3_mprintf("%q", string);

	stackPushString(result);
	sqlite3_free(result);
}

void gsc_sqlite_databases_count()
{
	sqlite_db_store *current = first_sqlite_db_store;

	int store_count = 0;

	while (current != NULL)
	{
		current = current->next;
		store_count++;
	}

	stackPushInt(store_count);
}

void gsc_sqlite_tasks_count()
{
	async_sqlite_task *current = first_async_sqlite_task;

	int task_count = 0;

	while (current != NULL)
	{
		current = current->next;
		task_count++;
	}

	stackPushInt(task_count);
}

#endif
