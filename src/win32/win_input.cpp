/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.

This file is part of Quake III Arena source code.

Quake III Arena source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Quake III Arena source code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Foobar; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/
// win_input.c -- win32 mouse and joystick code
// 02/21/97 JCB Added extended DirectInput code to support external controllers.

#include "win_local.h"
#include "../ui/keycodes.h"

dvar_t *in_mouse;

/*
===========
IN_Startup
===========
*/
void IN_Startup( void )
{
}

/*
===========
IN_Shutdown
===========
*/
void IN_Shutdown( void )
{
}


/*
===========
IN_Init
===========
*/
void IN_Init( void )
{
	in_mouse = Dvar_RegisterBool("in_mouse", 1, 4129);

	if ( !in_mouse->current.boolean )
	{
		Com_Printf("Mouse control not active.\n");
	}

	Dvar_ClearModified(in_mouse);
}

/*
===========
IN_MouseEvent
===========
*/
void IN_MouseEvent (int mstate)
{
}


/*
===========
IN_Activate

Called when the main window gains or loses focus.
The window may have been destroyed and recreated
between a deactivate and an activate.
===========
*/
void IN_Activate (qboolean active)
{
}


/*
==================
IN_Frame

Called every frame, even if not generating commands
==================
*/
void IN_Frame (void)
{
}


/*
===================
IN_ClearStates
===================
*/
void IN_ClearStates (void)
{
}
