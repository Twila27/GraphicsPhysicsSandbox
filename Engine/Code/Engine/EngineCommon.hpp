#pragma once

#include "Engine/Math/Vector3.hpp"

//---------------------------------------------------------------------------------------------
// FIXMEs / TODOs / NOTE macros
// http://flipcode.com/archives/FIXME_TODO_Notes_As_Warnings_In_Compiler_Output.shtml
// Fixes by Chris Forseth
//---------------------------------------------------------------------------------------------
//#define SHOW_TODO
#ifdef SHOW_TODO


#define TODO( x )  NOTE( __FILE__LINE__"\n"           \
		" ------------------------------------------------\n" \
		"|  TODO :   " ##x "\n" \
		" -------------------------------------------------\n" )
#define todo( x )  NOTE( __FILE__LINE__" TODO :   " #x "\n" ) 
#else
#define TODO( x ) 
#endif

#define PRAGMA(p)  __pragma( p )
#define NOTE( x )  PRAGMA( message( x ) )
#define FILE_LINE  NOTE( __FILE__LINE__ )
#define _QUOTE(x) # x
#define QUOTE(x) _QUOTE(x)
#define __FILE__LINE__ __FILE__ "(" QUOTE(__LINE__) ") : "
#define FIXME( x )  NOTE(  __FILE__LINE__"\n"           \
		" -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n" \
		"|  FIXME :  " ##x "\n" \
		" -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n" )
#define fixme( x )  NOTE( __FILE__LINE__" FIXME:   " #x "\n" ) 

//---------------------------------------------------------------------------------------------
#define STATIC //Do-nothing to afford 'static' being used in .cpp's.
#define MAX_LIGHTS 16

#define VK_LBUTTON 0x01
#define VK_RBUTTON 0x02

#define VK_TAB 0x09

#define VK_ENTER 0x0D
#define VK_ESCAPE 0x1B
#define VK_BACKSPACE 0x08
#define VK_PAGEUP 0x21
#define VK_PAGEDOWN 0x22
#define VK_END 0x23
#define VK_HOME 0x24
#define VK_LEFT 0x25
#define VK_RIGHT 0x27
#define VK_UP 0x26
#define VK_DOWN 0x28
#define VK_DELETE 0x2E

static const unsigned char KEY_TO_OPEN_CONSOLE = 192; //'~' and '`' do not seem to take!


enum PrimitiveType { PRIMITIVE_TYPE_POINT, PRIMITIVE_TYPE_LINE, PRIMITIVE_TYPE_TRIANGLES };

//--- Coordinate System ---//

static const Vector3 WORLD_FORWARD = Vector3( 1.f, 0.f, 0.f );
static const Vector3 WORLD_BACKWARD = Vector3( -1.f, 0.f, 0.f );
static const Vector3 WORLD_LEFT = Vector3( 0.f, 1.f, 0.f );
static const Vector3 WORLD_RIGHT = Vector3( 0.f, -1.f, 0.f );
static const Vector3 WORLD_UP = Vector3( 0.f, 0.f, 1.f );
static const Vector3 WORLD_DOWN = Vector3( 0.f, 0.f, -1.f );