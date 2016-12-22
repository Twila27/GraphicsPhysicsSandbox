#pragma once


//-----------------------------------------------------------------------------
#include "Engine/EngineCommon.hpp"
#include "Engine/Renderer/DebugRenderCommand.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Matrix4x4.hpp"


//-----------------------------------------------------------------------------
#define UNUSED(x) (void)(x);
#define STATIC
#define VK_SHIFT 0x10
#define VK_SPACE 0x20
#define VK_F1 0x70
#define VK_F5 0x74


//-----------------------------------------------------------------------------
//Operating System
static const char* g_appName = "Protogame (SD4 A00) by Benjamin Gibson";


//-----------------------------------------------------------------------------
//Debug Flags
extern bool g_renderDebugInfo;


//-----------------------------------------------------------------------------
//Naming Keys
static const char KEY_TO_TOGGLE_DEBUG_INFO = VK_F1;

static const char KEY_TO_MOVE_FASTER = VK_SHIFT;
static const char KEY_TO_MOVE_FORWARD = 'W';
static const char KEY_TO_MOVE_BACKWARD = 'S';
static const char KEY_TO_MOVE_LEFT = 'A';
static const char KEY_TO_MOVE_RIGHT = 'D';
static const char KEY_TO_MOVE_UP = VK_SPACE;
static const char KEY_TO_MOVE_DOWN = 'X'; //Be careful, also mutes BGM from Main_Win32.
static const char KEY_TO_TOGGLE_ORIGIN_AXES = 'O';

//-----------------------------------------------------------------------------
//Coordinate System

typedef Vector3 WorldCoords; //May be negative.

extern Matrix4x4 GetWorldChangeOfBasis( Ordering ordering );


//-----------------------------------------------------------------------------
//Camera
static const float FLYCAM_SPEED_SCALAR = 8.f;
static const Vector3 CAMERA_DEFAULT_POSITION = Vector3::ZERO;


 //-----------------------------------------------------------------------------
 //HUD

static const Vector2 HUD_BOTTOM_LEFT_POSITION = Vector2( 100.f, 27.f ); //In from left, up from bottom of screen.
static const float HUD_WIDTH_BETWEEN_ELEMENTS = 25.f;
static const float HUD_ELEMENT_WIDTH = 50.f;
static const float HUD_HEIGHT = HUD_ELEMENT_WIDTH; //Ensures HUD choices are square.
static const float HUD_CROSSHAIR_RADIUS = 20.f;
static const float HUD_CROSSHAIR_THICKNESS = 4.f;