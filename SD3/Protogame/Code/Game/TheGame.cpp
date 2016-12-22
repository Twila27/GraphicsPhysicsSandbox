#include "Game/TheGame.hpp"

#include "Engine/Math/MathUtils.hpp"
#include "Engine/Physics/PhysicsUtils.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Renderer/TheRenderer.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Light.hpp"
#include "Engine/Input/TheInput.hpp"
#include "Engine/Audio/TheAudio.hpp"
#include "Engine/String/StringUtils.hpp"
#include "Engine/Core/Command.hpp"

#include "Game/GameCommon.hpp"
#include "Game/TheApp.hpp" 
#include "Game/Camera3D.hpp"
#include "Game/Entity2D.hpp"

#include <cstdlib>
#include "Engine/Core/TheConsole.hpp"

//SD3: MOVE ALL INTO RENDERER!!!
#include "Engine/Renderer/Sampler.hpp"
#include "Engine/Renderer/Framebuffer.hpp"
#include "Engine/Renderer/MeshRenderer.hpp"
#include "Engine/Renderer/Mesh.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/RenderState.hpp"
#include "Engine/Renderer/ShaderProgram.hpp"


//--------------------------------------------------------------------------------------------------------------
#pragma region GlobalState
//A7 Globals
static FrameBuffer* gFBO = nullptr;
static bool gEnableCustomFBOs = false;
static unsigned int gCurrentEffectIndex = 0;
static bool gPauseTimerUniform = false;
static float gWrappedTimerDurationInSeconds = 1.f;
//A5 Console Command State Globals
static int gShowTangents = 0;
static int gShowBitangents = 0;
//A4 Console Command State Globals
static Vector3 gLightCenter = Vector3::ZERO;
static Vector3 uLightPosition = Vector3::ZERO;
static RenderState gRenderState;
static int gUseFog = 1;
static int gUseDiffuse = 1;
static int gUseSpecular = 1;
static int gUseAmbientAndDOT3 = 1;
static int gUseEmissive = 1;
static int gUseFalloffForSpecular = 1;
static int gUseFalloffForAmbientAndDOT3 = 1;
static int gIsLightMoving = true;
static int gIsLightOnCamera = true;
//A3 Console Command State Globals
static int gUseTimeEffect = 1;
static bool gIsShaderWorldVisible = true; TODO( "Remove altogether, have on at all times!" );
static bool gUseOrtho = false;
TODO( "Turn these global containers into definition-style static singletons in their classes." );
static std::vector<MeshRenderer*> gMeshRenderers;
static std::vector<MeshRenderer*> gPostProcessingMeshRenderers;
#pragma endregion


//--------------------------------------------------------------------------------------------------------------
#pragma region ConsoleCommands
void ToggleOriginAxes( Command& /*args*/ )
{
	g_theGame->ToggleAxes();
}


//--------------------------------------------------------------------------------------------------------------
void ShowShaders( Command& args )
{
	UNUSED( args );

	gIsShaderWorldVisible = !gIsShaderWorldVisible;
}


//--------------------------------------------------------------------------------------------------------------
void UseOrthogonalProjectionForViewMatrix( Command& args )
{
	UNUSED( args );

	gUseOrtho = !gUseOrtho;
}


//--------------------------------------------------------------------------------------------------------------
void SetLightIntensity( Command& args )
{
	float out;

	bool success = args.GetNextFloat( &out, 0.0f );
	if ( !success )
	{
		g_theConsole->Printf( "Incorrect arguments." );
		g_theConsole->Printf( "Usage: SetLightIntensity <0.0-1.0>" );
		return;
	}

	for ( auto& sp : *ShaderProgram::GetRegistry() )
		sp.second->SetFloat( "uLightIntensity", &out );
}


//--------------------------------------------------------------------------------------------------------------
void SetAmbientColor( Command& args )
{
	Rgba out;

	bool success = args.GetNextColor( &out, Rgba::WHITE );
	if ( !success )
	{
		g_theConsole->Printf( "Incorrect arguments." );
		g_theConsole->Printf( "Usage: SetAmbientColor <0-255> <0-255> <0-255> <0-255>" );
		return;
	}

	for ( auto& sp : *ShaderProgram::GetRegistry() )
		sp.second->SetColor( "uAmbientLight", &out );
}


//--------------------------------------------------------------------------------------------------------------
void SetLightColor( Command& args )
{
	Rgba out;

	bool success = args.GetNextColor( &out, Rgba::WHITE );
	if ( !success )
	{
		g_theConsole->Printf( "Incorrect arguments." );
		g_theConsole->Printf( "Usage: SetLightColor <0-255> <0-255> <0-255> <0-255>" );
		return;
	}

	for ( auto& sp : *ShaderProgram::GetRegistry() )
		sp.second->SetColor( "uLightColor", &out );
}


//--------------------------------------------------------------------------------------------------------------
void ToggleTimeEffect( Command& args )
{
	UNUSED( args );

	gUseTimeEffect = ( gUseTimeEffect == 1 ) ? 0 : 1;

	for ( auto& sp : *ShaderProgram::GetRegistry() )
		sp.second->SetInt( "uUseTime", &gUseTimeEffect );
}//--------------------------------------------------------------------------------------------------------------

 
//--------------------------------------------------------------------------------------------------------------
void ToggleDiffuse( Command& args )
{
	UNUSED( args );

	gUseDiffuse = ( gUseDiffuse == 1 ) ? 0 : 1;

	for ( auto& sp : *ShaderProgram::GetRegistry() )
		sp.second->SetInt( "uUseDiffuse", &gUseDiffuse );
}


//--------------------------------------------------------------------------------------------------------------
void ToggleSpecular( Command& args )
{
	UNUSED( args );

	gUseSpecular = ( gUseSpecular == 1 ) ? 0 : 1;

	for ( auto& sp : *ShaderProgram::GetRegistry() )
		sp.second->SetInt( "uUseSpecular", &gUseSpecular );
}


//--------------------------------------------------------------------------------------------------------------
void ToggleEmissive( Command& args )
{
	UNUSED( args );

	gUseEmissive = ( gUseEmissive == 1 ) ? 0 : 1;

	for ( auto& sp : *ShaderProgram::GetRegistry() )
		sp.second->SetInt( "uUseEmissive", &gUseEmissive );
}


//--------------------------------------------------------------------------------------------------------------
void ToggleAmbientAndDOT3( Command& args )
{
	UNUSED( args );

	gUseAmbientAndDOT3 = ( gUseAmbientAndDOT3 == 1 ) ? 0 : 1;

	for ( auto& sp : *ShaderProgram::GetRegistry() )
		sp.second->SetInt( "uUseAmbientAndDOT3", &gUseAmbientAndDOT3 );
}


//--------------------------------------------------------------------------------------------------------------
void ToggleFog( Command& args )
{
	UNUSED( args );

	gUseFog = ( gUseFog == 1 ) ? 0 : 1;

	for ( auto& sp : *ShaderProgram::GetRegistry() )
		sp.second->SetInt( "uUseFog", &gUseFog );
}


//--------------------------------------------------------------------------------------------------------------
void ToggleFalloffForSpecular( Command& args )
{
	UNUSED( args );

	gUseFalloffForSpecular = ( gUseFalloffForSpecular == 1 ) ? 0 : 1;

	for ( auto& sp : *ShaderProgram::GetRegistry() )
		sp.second->SetInt( "uUseFalloffForSpecular", &gUseFalloffForSpecular );
}


//--------------------------------------------------------------------------------------------------------------
void ToggleFalloffForAmbientAndDOT3( Command& args )
{
	UNUSED( args );

	gUseFalloffForAmbientAndDOT3 = ( gUseFalloffForAmbientAndDOT3 == 1 ) ? 0 : 1;

	for ( auto& sp : *ShaderProgram::GetRegistry() )
		sp.second->SetInt( "uUseFalloffForAmbientAndDOT3", &gUseFalloffForAmbientAndDOT3 );
}


//--------------------------------------------------------------------------------------------------------------
void ShowTangents( Command& args )
{
	UNUSED( args );

	gShowTangents = ( gShowTangents == 1 ) ? 0 : 1;
	gShowBitangents = 0;

	for ( auto& sp : *ShaderProgram::GetRegistry() )
	{
		sp.second->SetInt( "uShowTangent", &gShowTangents );
		sp.second->SetInt( "uShowBitangent", &gShowBitangents );
	}
}


//--------------------------------------------------------------------------------------------------------------
void ShowBitangents( Command& args )
{
	UNUSED( args );

	gShowBitangents = ( gShowBitangents == 1 ) ? 0 : 1;
	gShowTangents = 0;


	for ( auto& sp : *ShaderProgram::GetRegistry() )
	{
		sp.second->SetInt( "uShowTangent", &gShowTangents );
		sp.second->SetInt( "uShowBitangent", &gShowBitangents );
	}
}


//--------------------------------------------------------------------------------------------------------------
void ToggleCustomFBOs( Command& args )
{
	UNUSED( args );

	gEnableCustomFBOs = !gEnableCustomFBOs;
	if ( gEnableCustomFBOs )
		g_theConsole->Printf( Stringf( "Current FBO Effect: %s", gPostProcessingMeshRenderers[ gCurrentEffectIndex ]->GetMaterialName().c_str() ).c_str() );
}


//--------------------------------------------------------------------------------------------------------------
void SetFBOsColorMask( Command& args )
{
	Rgba out;

	bool success = args.GetNextColor( &out, Rgba::WHITE );
	if ( !success )
	{
		g_theConsole->Printf( "Incorrect arguments." );
		g_theConsole->Printf( "Usage: SetFBOsColorMask <0-255> <0-255> <0-255> <0-255>" );
		return;
	}

	for ( auto& sp : *ShaderProgram::GetRegistry() )
		sp.second->SetColor( "uColorMask", &out );
}


//--------------------------------------------------------------------------------------------------------------
void SetFBOsHueShift( Command& args )
{
	float out;

	bool success = args.GetNextFloat( &out, 0.f );
	if ( !success || out < 0.f || out > 360.f )
	{
		g_theConsole->Printf( "Incorrect arguments." );
		g_theConsole->Printf( "Usage: SetFBOsHueShift <0.f - 360.f>" );
		return;
	}

	for ( auto& sp : *ShaderProgram::GetRegistry() )
		sp.second->SetFloat( "uHueShift", &out );
}


//--------------------------------------------------------------------------------------------------------------
void SetFBOsCurrentEffectIndex( Command& args )
{
	int out;

	bool success = args.GetNextInt( &out, 0 );
	if ( !success || out < 0 )
	{
		g_theConsole->Printf( "Incorrect arguments." );
		g_theConsole->Printf( "Usage: SetFBOsCurrentEffectIndex <unsigned int>" );
		return;
	}

	if ( !success || out >= (int)gPostProcessingMeshRenderers.size() )
	{
		g_theConsole->Printf( "Not enough post-process effects setup." );
		g_theConsole->Printf( Stringf( "Current number of readied effects : %d", gPostProcessingMeshRenderers.size() ).c_str() );
		return;
	}

	gCurrentEffectIndex = out;
	g_theConsole->Printf( Stringf( "Current FBO Effect: %s", gPostProcessingMeshRenderers[ gCurrentEffectIndex ]->GetMaterialName().c_str() ).c_str() );
}


//--------------------------------------------------------------------------------------------------------------
void ToggleLightMovement( Command& args )
{
	UNUSED( args );

	gIsLightMoving = ( gIsLightMoving == 1 ) ? 0 : 1;
}


//--------------------------------------------------------------------------------------------------------------
void ToggleLightOnCamera( Command& args )
{
	UNUSED( args );

	gIsLightOnCamera = ( gIsLightOnCamera == 1 ) ? 0 : 1;
}


//--------------------------------------------------------------------------------------------------------------
void TimerUniformPause( Command& args )
{
	UNUSED( args );

	gPauseTimerUniform = ( gPauseTimerUniform == 1 ) ? 0 : 1;
}


//--------------------------------------------------------------------------------------------------------------
void TimerUniformSetWrappingDurationInSeconds( Command& args )
{
	float out;

	bool success = args.GetNextFloat( &out, 0.f );
	if ( !success || out < 0.f )
	{
		g_theConsole->Printf( "Incorrect arguments." );
		g_theConsole->Printf( "Usage: TimerUniformSetWrappingDurationInSeconds <0.f+>" );
		return;
	}

	gWrappedTimerDurationInSeconds = out;

	for ( auto& sp : *ShaderProgram::GetRegistry() )
		sp.second->SetFloat( "uWrappingTimerDuration", &out );
}


//--------------------------------------------------------------------------------------------------------------
void SetFBOsAnaglyphOffset( Command& args )
{
	float offsetU;
	float offsetV;

	bool successU = args.GetNextFloat( &offsetU, 0.f );
	bool successV = args.GetNextFloat( &offsetV, 0.f );
	if ( !( successU && successV ) || ( offsetU < -.25f || offsetU > .25f || offsetV < -.25f || offsetV > .25f ) )
	{
		g_theConsole->Printf( "Incorrect arguments." );
		g_theConsole->Printf( "Usage: SetAnaglyphOffset <-.25f to .25f> <-.25f to .25f>" );
		return;
	}

	Vector2 offset = Vector2( offsetU, offsetV );

	for ( auto& sp : *ShaderProgram::GetRegistry() )
		sp.second->SetVector2( "uOffset", &offset );
}


//--------------------------------------------------------------------------------------------------------------
void SetFBOsToonShaderQuantizeLevel( Command& args )
{
	float out;

	bool success = args.GetNextFloat( &out, 0.f );
	if ( !success )
	{
		g_theConsole->Printf( "Incorrect arguments." );
		g_theConsole->Printf( "Usage: SetFBOsToonShaderQuantizeLevel <float>" );
		return;
	}

	for ( auto& sp : *ShaderProgram::GetRegistry() )
		sp.second->SetFloat( "uQuantizationLevel", &out );
}


//--------------------------------------------------------------------------------------------------------------
void TimerUniformSetCurrentValue( Command& args )
{
	float out;

	bool success = args.GetNextFloat( &out, 0.f );
	if ( !success )
	{
		g_theConsole->Printf( "Incorrect arguments." );
		g_theConsole->Printf( "Usage: TimerUniformSetCurrentValue <float>" );
		return;
	}

	for ( auto& sp : *ShaderProgram::GetRegistry() )
	{
		sp.second->SetFloat( "uWrappingTimer", &out );
		sp.second->SetFloat( "uUnwrappedTimer", &out );
	}
}


//--------------------------------------------------------------------------------------------------------------
static bool gIsPilotingLight = false;
static unsigned int currentPilotedLightID = 0;
void PilotLight( Command& args )
{
	int out;

	bool success = args.GetNextInt( &out, 0 );
	if ( !success || out >= MAX_LIGHTS )
	{
		gIsPilotingLight = false;
		g_theConsole->Printf( "Incorrect arguments." );
		g_theConsole->Printf( Stringf( "Usage: PilotLight <0-%d>", MAX_LIGHTS-1 ).c_str() );
		return;
	}

	if ( Light::GetLight( out )->GetIsLightOn() == false )
	{
		gIsPilotingLight = false;
		g_theConsole->Printf( "Please ToggleLight to be on prior to piloting." );
		return;
	}

	gIsPilotingLight = true;
	currentPilotedLightID = out;
}


//--------------------------------------------------------------------------------------------------------------
void SetPilotedLightColor( Command& args )
{
	Rgba out;

	if ( !gIsPilotingLight )
	{
		g_theConsole->Printf( "Please enter PilotLight mode first." );
		return;
	}

	bool success = args.GetNextColor( &out, Rgba::WHITE );
	if ( !success )
	{
		g_theConsole->Printf( "Incorrect arguments." );
		g_theConsole->Printf( "Usage: PilotLight <0-255> <0-255> <0-255> <0-255>" );
		return;
	}

	Light::GetLight( currentPilotedLightID )->SetColor( out );
}


//--------------------------------------------------------------------------------------------------------------
void SetPilotedLightDirection( Command& args )
{
	float outX;
	float outY;
	float outZ;

	if ( !gIsPilotingLight )
	{
		g_theConsole->Printf( "Please enter PilotLight mode first." );
		return;
	}

	bool success = args.GetNextFloat( &outX, 0.f );
	if ( !success )
	{
		g_theConsole->Printf( "Failed to parse x-component." );
		g_theConsole->Printf( "Usage: SetPilotedLightDirection <float> <float> <float>" );
		return;
	}

	success = args.GetNextFloat( &outY, 0.f );
	if ( !success )
	{
		g_theConsole->Printf( "Failed to parse y-component." );
		g_theConsole->Printf( "Usage: SetPilotedLightDirection <float> <float> <float>" );
		return;
	}

	success = args.GetNextFloat( &outZ, 0.f );
	if ( !success )
	{
		g_theConsole->Printf( "Failed to parse z-component." );
		g_theConsole->Printf( "Usage: SetPilotedLightDirection <float> <float> <float>" );
		return;
	}

	Light::GetLight( currentPilotedLightID )->SetDirection( Vector3( outX, outY, outZ ) );
}


//--------------------------------------------------------------------------------------------------------------
void SetPilotedLightMinDistance( Command& args )
{
	float out;

	if ( !gIsPilotingLight )
	{
		g_theConsole->Printf( "Please enter PilotLight mode first." );
		return;
	}

	bool success = args.GetNextFloat( &out, 0.f );
	if ( !success )
	{
		g_theConsole->Printf( "Incorrect arguments." );
		g_theConsole->Printf( "Usage: SetPilotedLightMinDistance <float>" );
		return;
	}

	Light::GetLight( currentPilotedLightID )->SetMinDistance( out );
}


//--------------------------------------------------------------------------------------------------------------
void SetPilotedLightMaxDistance( Command& args )
{
	float out;

	if ( !gIsPilotingLight )
	{
		g_theConsole->Printf( "Please enter PilotLight mode first." );
		return;
	}

	bool success = args.GetNextFloat( &out, 0.f );
	if ( !success )
	{
		g_theConsole->Printf( "Incorrect arguments." );
		g_theConsole->Printf( "Usage: SetPilotedLightMaxDistance <float>" );
		return;
	}

	Light::GetLight( currentPilotedLightID )->SetMaxDistance( out );
}


//--------------------------------------------------------------------------------------------------------------
void SetPilotedLightPowerAtMinDistance( Command& args )
{
	float out;

	if ( !gIsPilotingLight )
	{
		g_theConsole->Printf( "Please enter PilotLight mode first." );
		return;
	}

	bool success = args.GetNextFloat( &out, 0.f );
	if ( !success )
	{
		g_theConsole->Printf( "Incorrect arguments." );
		g_theConsole->Printf( "Usage: SetPilotedLightPowerAtMinDistance <float>" );
		return;
	}

	Light::GetLight( currentPilotedLightID )->SetPowerAtMinDistance( out );
}


//--------------------------------------------------------------------------------------------------------------
void SetPilotedLightPowerAtMaxDistance( Command& args )
{
	float out;

	if ( !gIsPilotingLight )
	{
		g_theConsole->Printf( "Please enter PilotLight mode first." );
		return;
	}

	bool success = args.GetNextFloat( &out, 0.f );
	if ( !success )
	{
		g_theConsole->Printf( "Incorrect arguments." );
		g_theConsole->Printf( "Usage: SetPilotedLightPowerAtMaxDistance <float>" );
		return;
	}

	Light::GetLight( currentPilotedLightID )->SetPowerAtMaxDistance( out );
}


//--------------------------------------------------------------------------------------------------------------
void SetPilotedLightInnerAngleCosine( Command& args )
{
	float out;

	if ( !gIsPilotingLight )
	{
		g_theConsole->Printf( "Please enter PilotLight mode first." );
		return;
	}

	bool success = args.GetNextFloat( &out, 0.f );
	if ( !success )
	{
		g_theConsole->Printf( "Incorrect arguments." );
		g_theConsole->Printf( "Usage: SetPilotedLightInnerAngleCosine <float>" );
		return;
	}

	Light::GetLight( currentPilotedLightID )->SetInnerAngleCosine( out );
}


//--------------------------------------------------------------------------------------------------------------
void SetPilotedLightOuterAngleCosine( Command& args )
{
	float out;

	if ( !gIsPilotingLight )
	{
		g_theConsole->Printf( "Please enter PilotLight mode first." );
		return;
	}

	bool success = args.GetNextFloat( &out, 0.f );
	if ( !success )
	{
		g_theConsole->Printf( "Incorrect arguments." );
		g_theConsole->Printf( "Usage: SetPilotedLightOuterAngleCosine <float>" );
		return;
	}

	Light::GetLight( currentPilotedLightID )->SetOuterAngleCosine( out );
}


//--------------------------------------------------------------------------------------------------------------
void SetPilotedLightPowerInsideInnerAngle( Command& args )
{
	float out;

	if ( !gIsPilotingLight )
	{
		g_theConsole->Printf( "Please enter PilotLight mode first." );
		return;
	}

	bool success = args.GetNextFloat( &out, 0.f );
	if ( !success )
	{
		g_theConsole->Printf( "Incorrect arguments." );
		g_theConsole->Printf( "Usage: SetPilotedLightPowerInsideInnerAngle <float>" );
		return;
	}

	Light::GetLight( currentPilotedLightID )->SetPowerInsideInnerAngleCosine( out );
}


//--------------------------------------------------------------------------------------------------------------
void SetPilotedLightPowerOutsideOuterAngle( Command& args )
{
	float out;

	if ( !gIsPilotingLight )
	{
		g_theConsole->Printf( "Please enter PilotLight mode first." );
		return;
	}

	bool success = args.GetNextFloat( &out, 0.f );
	if ( !success )
	{
		g_theConsole->Printf( "Incorrect arguments." );
		g_theConsole->Printf( "Usage: SetPilotedLightPowerOutsideOuterAngle <float>" );
		return;
	}

	Light::GetLight( currentPilotedLightID )->SetPowerOutsideOuterAngleCosine( out );
}


//--------------------------------------------------------------------------------------------------------------
void SetPilotedLightIsDirectional( Command& args )
{
	int out;

	if ( !gIsPilotingLight )
	{
		g_theConsole->Printf( "Please enter PilotLight mode first." );
		return;
	}

	bool success = args.GetNextInt( &out, 0 );
	if ( !success || ( out != 0 && out != 1 ) )
	{
		g_theConsole->Printf( "Incorrect arguments." );
		g_theConsole->Printf( "Usage: SetPilotedLightIsDirectional <0 or 1>" );
		return;
	}

	Light::GetLight( currentPilotedLightID )->SetIsDirectional( out );
}


//--------------------------------------------------------------------------------------------------------------
static void RegisterConsoleCommands()
{
	//SD3 A2
	g_theConsole->RegisterCommand( "ToggleOriginAxes", ToggleOriginAxes );

	//SD3 A3
	g_theConsole->RegisterCommand( "ShowShaders", ShowShaders );
	g_theConsole->RegisterCommand( "UseOrthogonalProjectionForViewMatrix", UseOrthogonalProjectionForViewMatrix );

	//SD3 A4
	g_theConsole->RegisterCommand( "ToggleTimeEffect", ToggleTimeEffect );
	g_theConsole->RegisterCommand( "ToggleDiffuse", ToggleDiffuse );
	g_theConsole->RegisterCommand( "ToggleSpecular", ToggleSpecular );
	g_theConsole->RegisterCommand( "ToggleAmbientAndDOT3", ToggleAmbientAndDOT3 );
	g_theConsole->RegisterCommand( "ToggleEmissive", ToggleEmissive );
	g_theConsole->RegisterCommand( "ToggleFog", ToggleFog );
	g_theConsole->RegisterCommand( "ToggleFalloffForSpecular", ToggleFalloffForSpecular );
	g_theConsole->RegisterCommand( "ToggleFalloffForAmbientAndDOT3", ToggleFalloffForAmbientAndDOT3 );
	g_theConsole->RegisterCommand( "ToggleLightMovement", ToggleLightMovement );
	g_theConsole->RegisterCommand( "ToggleLightOnCamera", ToggleLightOnCamera );
	g_theConsole->RegisterCommand( "SetLightIntensity", SetLightIntensity );
	g_theConsole->RegisterCommand( "SetLightColor", SetLightColor );
	g_theConsole->RegisterCommand( "SetAmbientColor", SetAmbientColor );

	//SD3 A5
	g_theConsole->RegisterCommand( "ShowTangents", ShowTangents );
	g_theConsole->RegisterCommand( "ShowBitangents", ShowBitangents );

	//SD3 A6
	g_theConsole->RegisterCommand( "PilotLight", PilotLight );
	g_theConsole->RegisterCommand( "SetPilotedLightColor", SetPilotedLightColor );
	g_theConsole->RegisterCommand( "SetPilotedLightDirection", SetPilotedLightDirection );
	g_theConsole->RegisterCommand( "SetPilotedLightMinDistance", SetPilotedLightMinDistance );
	g_theConsole->RegisterCommand( "SetPilotedLightMaxDistance", SetPilotedLightMaxDistance );
	g_theConsole->RegisterCommand( "SetPilotedLightPowerAtMinDistance", SetPilotedLightPowerAtMinDistance );
	g_theConsole->RegisterCommand( "SetPilotedLightPowerAtMaxDistance", SetPilotedLightPowerAtMaxDistance );
	g_theConsole->RegisterCommand( "SetPilotedLightInnerAngleCosine", SetPilotedLightInnerAngleCosine );
	g_theConsole->RegisterCommand( "SetPilotedLightOuterAngleCosine", SetPilotedLightOuterAngleCosine );
	g_theConsole->RegisterCommand( "SetPilotedLightPowerInsideInnerAngle", SetPilotedLightPowerInsideInnerAngle );
	g_theConsole->RegisterCommand( "SetPilotedLightPowerOutsideOuterAngle", SetPilotedLightPowerOutsideOuterAngle );
	g_theConsole->RegisterCommand( "SetPilotedLightIsDirectional", SetPilotedLightIsDirectional );

	//SD3 A7
	g_theConsole->RegisterCommand( "ToggleFBOs", ToggleCustomFBOs );
	g_theConsole->RegisterCommand( "SetFBOsColorMask", SetFBOsColorMask );
	g_theConsole->RegisterCommand( "SetFBOsHueShift", SetFBOsHueShift );
	g_theConsole->RegisterCommand( "SetFBOsCurrentEffectIndex", SetFBOsCurrentEffectIndex );
	g_theConsole->RegisterCommand( "SetFBOsAnaglyphOffset", SetFBOsAnaglyphOffset );
	g_theConsole->RegisterCommand( "SetFBOsToonShaderQuantizeLevel", SetFBOsToonShaderQuantizeLevel );
	g_theConsole->RegisterCommand( "TimerUniformPause", TimerUniformPause );
	g_theConsole->RegisterCommand( "TimerUniformSetCurrentValue", TimerUniformSetCurrentValue );
	g_theConsole->RegisterCommand( "TimerUniformSetWrappingDurationInSeconds", TimerUniformSetWrappingDurationInSeconds );
}
#pragma endregion


//-----------------------------------------------------------------------------
TheGame* g_theGame = nullptr;


//-----------------------------------------------------------------------------
TheGame::TheGame( )
	: m_playerCamera( new Camera3D( CAMERA_DEFAULT_POSITION ) )
	, m_activeFont( nullptr )
	, m_arialFont( BitmapFont::CreateOrGetFont( "Data/Fonts/Arial.fnt" ) )
	, m_papyrusFont( BitmapFont::CreateOrGetFont( "Data/Fonts/Papyrus.fnt" ) )
	, m_cloth( new Cloth( Vector3( 0.f, 0.f, 0.f ), PARTICLE_AABB3, 1.f, .01f, 5, 5, 2 ) )
	, m_showAxesAtOrigin( false )
	, m_activeEmitter( 0 )
{
	//Remember to initialize any pointers to initial values or to nullptr in here.
	// for ( int i = 0; i < TheGame::INITIAL_NUM_ASTEROIDS; i++ ) m_asteroids[ i ] = new Asteroid(...);
	// for ( int i = 0; i < MAX_NUMBER_OF_BULLETS; i++ ) m_bullets[ i ] = nullptr; //Etc.

	for ( int i = 0; i < 10; i++ ) {
		m_entities.push_back( new Entity2D( "" ) );
		m_entities[ i ]->SetPosition( Vector2( (float)( i * 72 ), (float)( i * 72 ) ) );
	}

	//Position player representation if camera isn't translated to player (i.e. if camera is fixed in world).
	
	//m_ship = new Ship( static_cast<float>( screenWidth ) / 2.f, static_cast<float>( screenHeight ) / 2.f ); 

	//Particle Systems Assignment Code:
	m_particleSystems.push_back( new ParticleSystem( //Explosion.
		Vector3::ZERO,
		ParticleType::PARTICLE_AABB3,
		.1f,
		1.f,
		10.f,
		360.f, 0.f,
		360.f, 0.f,
		5.f,
		3.f,
		1000,
		540 
	) );

	m_particleSystems.push_back( new ParticleSystem( //Fireworks.
		Vector3::ZERO,
		ParticleType::PARTICLE_AABB3,
		.1f,
		10.f,
		5.4f,
		360.f, 0.f,
		360.f, 0.f,
		5.f,
		6.f,
		1000,
		540 
	) );
	m_particleSystems.back()->AddForce( new GravityForce( 2.7f ) );

	m_particleSystems.push_back( new ParticleSystem( //Smoke.
		Vector3::ZERO,
		ParticleType::PARTICLE_AABB3,
		.1f,
		1.f,
		5.4f,
		45.f, 0.f,
		360.f, 0.f,
		5.f,
		6.f,
		1000,
		540 
	) );
	m_particleSystems.back()->AddForce( new ConstantWindForce( 1.f, WORLD_RIGHT ) );


	m_particleSystems.push_back( new ParticleSystem( //Fountain.
		Vector3::ZERO,
		ParticleType::PARTICLE_AABB3,
		.1f,
		1.f,
		5.4f,
		45.f, 0.f,
		360.f, 0.f,
		5.f,
		10.f,
		1000,
		540 
	) );
	m_particleSystems.back()->AddForce( new GravityForce( 1.f ) );

	m_particleSystems.push_back( new ParticleSystem( //Debris. Doesn't work so well because drag can't directly scale down velocity, as the state is const.
		Vector3::ZERO,
		ParticleType::PARTICLE_AABB3,
		.1f,
		100.f,
		1.f,
		30.f, 0.f,
		360.f, 0.f,
		5.f,
		30.f,
		1000,
		200 
	) );
	m_particleSystems.back()->AddForce( new DebrisForce( 1.f ) );

	m_particleSystems.push_back( new ParticleSystem( //My varying wind force: wormhole!
		Vector3::ZERO,
		ParticleType::PARTICLE_AABB3,
		.1f,
		1.f,
		30.f,
		360.f, 0.f,
		360.f, 0.f,
		20.f,
		20.f,
		1000,
		1000 
	) );
	m_particleSystems.back()->AddForce( new WormholeForce( Vector3::ZERO, 1.f, WORLD_UP ) );
}


//-----------------------------------------------------------------------------
TheGame::~TheGame( )
{
	delete m_playerCamera;
	delete m_arialFont;
	delete m_papyrusFont;
	//delete m_ship; //Whatever represents player.
	//for ( int i = 0; i < m_numBulletsAllocated; i++ ) delete m_bullets[ i ]; //Etc.

	for ( int particleSystemIndex = 0; particleSystemIndex < (int)m_particleSystems.size(); particleSystemIndex++ )
		delete m_particleSystems[ particleSystemIndex ]; //Etc.
	delete m_cloth;
}


//-----------------------------------------------------------------------------
void TheGame::Shutdown()
{
	TODO( "Delete OpenGL Resources!" );
	for ( MeshRenderer* mr : gMeshRenderers )
		delete mr;
	delete gFBO;
}


//-----------------------------------------------------------------------------
const Camera3D* TheGame::GetActiveCamera() const
{
	return m_playerCamera;
}


//-----------------------------------------------------------------------------
void TheGame::UpdateFromKeyboard( float deltaSeconds )
{
	//Changing particle systems.
	if ( g_theInput->IsKeyDown( '1' ) ) m_activeEmitter = 0;
	else if ( g_theInput->IsKeyDown( '2' ) ) m_activeEmitter = 1;
	else if ( g_theInput->IsKeyDown( '3' ) ) m_activeEmitter = 2;
	else if ( g_theInput->IsKeyDown( '4' ) ) m_activeEmitter = 3;
	else if ( g_theInput->IsKeyDown( '5' ) ) m_activeEmitter = 4;
	else if ( g_theInput->IsKeyDown( '6' ) ) m_activeEmitter = 5;

	if ( g_theInput->WasKeyPressedOnce( '7' ) ) m_activeFont = nullptr;
	if ( g_theInput->WasKeyPressedOnce( '8' ) ) m_activeFont = m_arialFont;
	if ( g_theInput->WasKeyPressedOnce( '9' ) ) m_activeFont = m_papyrusFont;
	if ( g_theInput->WasKeyPressedOnce( KEY_TO_TOGGLE_ORIGIN_AXES ) ) m_showAxesAtOrigin = !m_showAxesAtOrigin;


	//Super speed handling.
	float speed = 0.f;
	if ( g_theInput->IsKeyDown( KEY_TO_MOVE_FASTER ) )
		speed = FLYCAM_SPEED_SCALAR;
	else speed = 1.f;


	float deltaMove = ( speed * deltaSeconds ); //speed is then in blocks per second!

	Vector3 camForwardXY = m_playerCamera->GetForwardXY();
	Vector3 camLeftXY = m_playerCamera->GetLeftXY();

	if ( gIsPilotingLight )
	{
		Light* pilotedLight = Light::GetLight( currentPilotedLightID );

		if ( g_theInput->IsKeyDown( KEY_TO_MOVE_FORWARD ) )
			pilotedLight->Translate( camForwardXY * deltaMove );

		if ( g_theInput->IsKeyDown( KEY_TO_MOVE_BACKWARD ) )
			pilotedLight->Translate( -camForwardXY * deltaMove );

		if ( g_theInput->IsKeyDown( KEY_TO_MOVE_LEFT ) )
			pilotedLight->Translate( camLeftXY * deltaMove );

		if ( g_theInput->IsKeyDown( KEY_TO_MOVE_RIGHT ) )
			pilotedLight->Translate( -camLeftXY * deltaMove );

		if ( g_theInput->IsKeyDown( KEY_TO_MOVE_UP ) )
			pilotedLight->Translate( Vector3( 0.f, 0.f, deltaMove ) ); //Scaled by z-axis, so *1.0f.

		if ( g_theInput->IsKeyDown( KEY_TO_MOVE_DOWN ) )
			pilotedLight->Translate( Vector3( 0.f, 0.f, -deltaMove ) ); //Scaled by z-axis, so *1.0f.


		if ( g_theInput->IsKeyDown( 'I' ) )
			m_playerCamera->m_worldPosition += camForwardXY * deltaMove;

		if ( g_theInput->IsKeyDown( 'K' ) )
			m_playerCamera->m_worldPosition -= camForwardXY * deltaMove;

		if ( g_theInput->IsKeyDown( 'J' ) )
			m_playerCamera->m_worldPosition += camLeftXY * deltaMove;

		if ( g_theInput->IsKeyDown( 'L' ) )
			m_playerCamera->m_worldPosition -= camLeftXY * deltaMove;

		if ( g_theInput->IsKeyDown( 'U' ) )
			m_playerCamera->m_worldPosition.z += deltaMove; //Scaled by z-axis, so *1.0f.

		if ( g_theInput->IsKeyDown( 'O' ) )
			m_playerCamera->m_worldPosition.z -= deltaMove; //Scaled by z-axis, so *1.0f.

		return;
	}

	if ( g_theInput->IsKeyDown( KEY_TO_MOVE_FORWARD ) )
		m_playerCamera->m_worldPosition += camForwardXY * deltaMove;

	if ( g_theInput->IsKeyDown( KEY_TO_MOVE_BACKWARD ) )
		m_playerCamera->m_worldPosition -= camForwardXY * deltaMove;

	if ( g_theInput->IsKeyDown( KEY_TO_MOVE_LEFT ) )
		m_playerCamera->m_worldPosition += camLeftXY * deltaMove;

	if ( g_theInput->IsKeyDown( KEY_TO_MOVE_RIGHT ) )
		m_playerCamera->m_worldPosition -= camLeftXY * deltaMove;

	if ( g_theInput->IsKeyDown( KEY_TO_MOVE_UP ) )
		m_playerCamera->m_worldPosition.z += deltaMove; //Scaled by z-axis, so *1.0f.

	if ( g_theInput->IsKeyDown( KEY_TO_MOVE_DOWN ) )
		m_playerCamera->m_worldPosition.z -= deltaMove; //Scaled by z-axis, so *1.0f.
}


//-----------------------------------------------------------------------------
void TheGame::UpdateFromMouse() 
{
	//The following is basis-independent: mouse X controls yaw around camera/view up j-vector, mouse Y controls pitch around camera/view right i-vector.
	const float mouseSensitivityYaw = 0.044f;
	m_playerCamera->m_orientation.m_yawDegrees -= mouseSensitivityYaw * (float)g_theInput->GetCursorDeltaX();
	m_playerCamera->m_orientation.m_pitchDegrees += mouseSensitivityYaw * (float)g_theInput->GetCursorDeltaY();
	m_playerCamera->FixAndClampAngles(); //This function body, however, is is basis-dependent, as what i-j-k equals is defined by the basis.
}


//-----------------------------------------------------------------------------
void TheGame::UpdateCamera( float deltaSeconds )
{
	UpdateFromKeyboard( deltaSeconds );
	UpdateFromMouse();
}


//-----------------------------------------------------------------------------
float g_counter = 0.f;
void UpdateShaderTimers( float deltaSeconds )
{
	//For time effect.
	static float wrappedDeltaSeconds = 0.f;
	static float unwrappedDeltaSeconds = 0.f;
	if ( !gPauseTimerUniform )
	{
		wrappedDeltaSeconds += deltaSeconds;
		if ( wrappedDeltaSeconds > gWrappedTimerDurationInSeconds )
			wrappedDeltaSeconds = 0.f;

		unwrappedDeltaSeconds += deltaSeconds;
	}

	static const float radius = 1.0f; //Radius of light's orbit if not constrained to camera.
	if ( gIsLightMoving )
	{
		if ( gIsLightOnCamera )
		{
			uLightPosition = g_theGame->GetActiveCamera()->m_worldPosition;
		}
		else
		{
			uLightPosition = gLightCenter
				+ WORLD_FORWARD * ( radius * cosf( unwrappedDeltaSeconds ) ) //use j-vec!
				+ WORLD_UP * ( radius * sinf( 2.0f * unwrappedDeltaSeconds ) ); //use i-vec!
		}
	}

	if ( gIsShaderWorldVisible )
	{
		Vector3 camPos = g_theGame->GetActiveCamera()->m_worldPosition;
		for ( auto& sp : *ShaderProgram::GetRegistry() )
		{
			if ( !gPauseTimerUniform )
			{
				sp.second->SetFloat( "uWrappingTimer", &wrappedDeltaSeconds );
				sp.second->SetFloat( "uUnwrappedTimer", &unwrappedDeltaSeconds );
			}
			sp.second->SetVector3( "uLightPosition", &uLightPosition );
			sp.second->SetVector3( "uCameraPosition", &camPos );
		}
	}
}
void TheGame::Update( float deltaSeconds )
{
	//For quick hacky animations.
	g_counter++;
	g_counter = WrapNumberWithinCircularRange( g_counter, 0.f, 360.f ); //For easy use inside trig fcns.
	if ( g_theInput->IsKeyDown( 'A' ) && g_theInput->WasKeyJustPressed( 'A' ) ) g_counter += 10.f;

	m_cloth->Update( deltaSeconds );

	UpdateShaderTimers( deltaSeconds );

	//m_ship->Update( deltaSeconds ); //Update player (or world!) representation.
	UpdateCamera( deltaSeconds );

	//Debug GameCommon flag setting.
	if ( g_theInput->WasKeyPressedOnce( KEY_TO_TOGGLE_DEBUG_INFO ) ) g_renderDebugInfo = !g_renderDebugInfo;

	for ( auto entityIter = m_entities.begin(); entityIter != m_entities.end(); ++entityIter )
	{
		Entity2D* currentEntity2D = *entityIter;
		currentEntity2D->Update( deltaSeconds );
	}

	//-----------------------------------------------------------------------------
	TODO( "MOVE INSIDE THE ENGINE SINGLETON'S UPDATE, CALLED FROM MAIN_WIN32" );
	//-----------------------------------------------------------------------------
	if ( m_particleSystems.size() >= m_activeEmitter + 1 )
		m_particleSystems[ m_activeEmitter ]->UpdateParticles( deltaSeconds );
	UpdateDebugCommands( deltaSeconds );
}


//-----------------------------------------------------------------------------
#pragma region Rendering Functions
void TheGame::SetUpPerspectiveProjection()
{
	float aspect = ( 16.f / 9.f ); //Not speed-critical /'s because const expression handled during compilation.
	//float fovDegreesHorizontal = 180.f; //Lowering me zooms in! Raising me produces fisheye effect!
	float fovDegreesVertical = 60.f; // was ( fovDegreesHorizontal / aspect );
	float zNear = .1f;
	float zFar = 1000.f;

	g_theRenderer->SetPerspective( fovDegreesVertical, aspect, zNear, zFar ); // The math for how this works is very nuanced and above this class, will be in SD3. 
}


//-----------------------------------------------------------------------------
void TheGame::ApplyCameraTransform() //Y-up and left-handed for Forseth's class.
{
	//Change of basis detour application to move from OGL (y-up right-handed world-not-screen) to our coordinate system (y-up left-handed world) orientation.
	g_theRenderer->RotateViewByDegrees( -90.f, Vector3( 1.f, 0.f, 0.f ) ); //Rotate z-forward to z-up, a negative rotation about the x-axis.
	g_theRenderer->RotateViewByDegrees( 90.f, Vector3( 0.f, 0.f, 1.f ) ); //Rotate x-right to x-forward, a positive rotation about the z-axis.

	//Anti-rotation. (MP1 Recap: transposing rotation == inverse... so why does negating work?)
	g_theRenderer->RotateViewByDegrees( -1.f * m_playerCamera->m_orientation.m_rollDegrees, Vector3( 1.f, 0.f, 0.f ) ); //Anti-roll.
	g_theRenderer->RotateViewByDegrees( -1.f * m_playerCamera->m_orientation.m_pitchDegrees, Vector3( 0.f, 1.f, 0.f ) ); //Anti-pitch.
	g_theRenderer->RotateViewByDegrees( -1.f * m_playerCamera->m_orientation.m_yawDegrees, Vector3( 0.f, 0.f, 1.f ) ); //Anti-yaw.

	//Anti-translation. (MP1 Recap: negating translation == inverse!)
	g_theRenderer->TranslateView( m_playerCamera->m_worldPosition * -1.f );
}


//-----------------------------------------------------------------------------
void TheGame::SetupView3D()
{
	g_theRenderer->ClearScreenToColor( Rgba::DARK_GRAY ); //BG color of FBOs-off world.
	g_theRenderer->ClearScreenDepthBuffer();

	if ( gEnableCustomFBOs )
	{
		g_theRenderer->BindFBO( gFBO );
		g_theRenderer->ClearScreenToColor( Rgba::DARK_GRAY ); //BG color of FBOs-on world.
		g_theRenderer->ClearScreenDepthBuffer();
	}

	SetUpPerspectiveProjection();

	ApplyCameraTransform();

	g_theRenderer->EnableDepthTesting( true );
	g_theRenderer->EnableBackfaceCulling( true );
	//g_theRenderer->EnableAlphaTesting( true );
}


//-----------------------------------------------------------------------------
void TheGame::Render3D() //Look below in RenderDebug3D for DebugRenderCommand use.
{
	//Render 3D world/map/scene or player representation.
	if ( m_particleSystems.size() >= m_activeEmitter + 1 )
		m_particleSystems[ m_activeEmitter ]->RenderThenExpireParticles();
	m_cloth->Render( true, true, true );

	//Render any 3D overlays.
}


//-----------------------------------------------------------------------------
void TheGame::RenderDebug3D() //DebugRenderCommands from Engine, etc.
{
	float x = CosDegrees( g_counter );
	float y = SinDegrees( g_counter );
	Vector3 movingCirclePos = Vector3( x, y, 0.f );
	Vector3 camPos = m_playerCamera->m_worldPosition;

	//Box.
	AABB3 bounds;
	bounds.mins = Vector3( 4.f, 4.f, 4.f );
	bounds.maxs = Vector3( 5.f, 5.f, 5.f );
//	AddDebugDebugRenderCommand( new DebugRenderCommandAABB3( bounds, true, 0.f, DepthMode::DEPTH_TEST_ON, 4.f, Rgba::CYAN, Rgba::GRAY ) );

	//Ball.
//	AddDebugDebugRenderCommand( new DebugRenderCommandSphere( Vector3::ZERO, 1.f, 0.f, DepthMode::DEPTH_TEST_OFF, Rgba::WHITE, 4.f ) );

	//Axes.
	if ( m_showAxesAtOrigin )
	{
		AddDebugDebugRenderCommand( new DebugRenderCommandBasis( Vector3::ZERO, 1000.f, true, 0.f, DEPTH_TEST_DUAL, 1.f ) );
	}

	//Lines and arrows and points.
	Vector3 lineStart = Vector3( -5.f, -5.f, -5.f );
	Vector3 lineEnd = Vector3( -6.f, -6.f, -6.f );
//	AddDebugDebugRenderCommand( new DebugRenderCommandLine( lineStart, lineEnd, 0.f, DepthMode::DEPTH_TEST_DUAL, Rgba::CYAN, 4.f ) );
	Vector3 arrowStart = Vector3( 0.f, 0.f, 0.f );
	Vector3 arrowEnd = Vector3( -4.f, -4.f, -4.f );
	//	AddDebugDebugRenderCommand( new DebugRenderCommandArrow( arrowStart, arrowEnd - movingCirclePos, 0.f, DepthMode::DEPTH_TEST_DUAL, Rgba::YELLOW, 4.f ) );
	//	AddDebugDebugRenderCommand( new DebugRenderCommandPoint( movingCirclePos, 5.f, DepthMode::DEPTH_TEST_DUAL, Rgba::MAGENTA, 4.f ) );
	//	AddDebugDebugRenderCommand( new DebugRenderCommandPoint( arrowEnd - Vector3::ONE - movingCirclePos, 0.f, DepthMode::DEPTH_TEST_DUAL, Rgba::MAGENTA, 4.f ) );

	RenderThenExpireDebugCommands();
}


//-----------------------------------------------------------------------------
void TheGame::SetupView2D()
{
	g_theRenderer->EnableDepthTesting( false );
	g_theRenderer->EnableBackfaceCulling( false );

	g_theRenderer->SetOrtho( Vector2( 0.f, 0.f ),
							 Vector2( (float)g_theApp->GetScreenWidth(), (float)g_theApp->GetScreenHeight() ) );
}


//-----------------------------------------------------------------------------
void TheGame::Render2D()
{
	//Render 2D world/map/scene or player representation, and HUD.

	for ( auto entityIter = m_entities.begin(); entityIter != m_entities.end(); ++entityIter )
	{
		Entity2D* currentEntity2D = *entityIter;
		currentEntity2D->Render();
	}

	g_theRenderer->DrawAABB( TheRenderer::VertexGroupingRule::AS_QUADS, AABB2( 1000.f, 200.f, 1400.f, 600.f ), *Texture::CreateOrGetTexture( "Data/Images/Paused.png" ) ); //Uncomment #include Texture above to use.

	g_theRenderer->DrawPolygon( TheRenderer::VertexGroupingRule::AS_LINE_LOOP, Vector2( 100.f, 100.f ), 50.f, 20.f, 90.f );

	//g_theRenderer->SetOrtho( Vector2( 0.f, 0.f ), Vector2( 800.f, 450.f ) ); //Half-sizing the viewport to show how rendering to two scales things. Should go in SetupView2D.

	g_theRenderer->DrawAABB( TheRenderer::VertexGroupingRule::AS_LINE_LOOP, AABB2( 100.f + g_counter, 100.f, 400.f + g_counter, 400.f ), Rgba( 0.1f, 0.3f, 0.9f, 0.5f ) ); //Takes up half the height due to ortho call.
}


//-----------------------------------------------------------------------------
void TheGame::RenderDebug2D()
{
	RenderReticle(); //May later be considered part of HUD and moved to Render2D().

	RenderLeftDebugText2D();
	RenderRightDebugText2D();
}


//-----------------------------------------------------------------------------
void TheGame::RenderReticle()
{
	Vector2 screenCenter = g_theApp->GetScreenCenter();

	Vector2 crosshairLeft = Vector2( screenCenter.x - HUD_CROSSHAIR_RADIUS, screenCenter.y );
	Vector2 crosshairRight = Vector2( screenCenter.x + HUD_CROSSHAIR_RADIUS, screenCenter.y );
	Vector2 crosshairUp = Vector2( screenCenter.x, screenCenter.y - HUD_CROSSHAIR_RADIUS );
	Vector2 crosshairDown = Vector2( screenCenter.x, screenCenter.y + HUD_CROSSHAIR_RADIUS );

	TODO( "Replace OpenGL constant values with encapsulations, or better yet make RenderState class for this!" );
	g_theRenderer->SetBlendFunc( 0x0307, 0 ); //GL CONSTANTS.
	g_theRenderer->DrawLine( crosshairLeft, crosshairRight, Rgba(), Rgba(), HUD_CROSSHAIR_THICKNESS );
	g_theRenderer->DrawLine( crosshairUp, crosshairDown, Rgba(), Rgba(), HUD_CROSSHAIR_THICKNESS );
	g_theRenderer->SetBlendFunc( 0x0302, 0x0303 );
}


//-----------------------------------------------------------------------------
void TheGame::RenderRightDebugText2D()
{
	g_theRenderer->DrawTextMonospaced2D
	(
		Vector2( (float)g_theApp->GetScreenWidth() - 375.f, (float)g_theApp->GetScreenHeight() - 50.f ),
		Stringf( "Application Size: %.0f x %.0f", g_theApp->GetScreenWidth(), g_theApp->GetScreenHeight() ),
		18.f,
		Rgba::GREEN,
		m_activeFont,
		.65f
	);

	char* emitterTypeName = "";
	switch ( m_activeEmitter )
	{
	case 0: emitterTypeName = "Explosion"; break;
	case 1: emitterTypeName = "Fireworks"; break;
	case 2: emitterTypeName = "Smoke Cloud"; break;
	case 3: emitterTypeName = "Fountain"; break;
	case 4: emitterTypeName = "Gushing Debris"; break;
	case 5: emitterTypeName = "OH MY GOD IT'S A W--"; break;
	}

	g_theRenderer->DrawTextProportional2D
		(
			Vector2( (float)g_theApp->GetScreenWidth() - 375.f, (float)g_theApp->GetScreenHeight() - 50.f ),
			Stringf( "Emitter Type: %s", emitterTypeName ),
			.20f,
			nullptr,
			Rgba::GREEN
			);


	if ( m_particleSystems.size() >= m_activeEmitter + 1 )
		g_theRenderer->DrawTextProportional2D
		(
			Vector2( (float)g_theApp->GetScreenWidth() - 375.f, (float)g_theApp->GetScreenHeight() - 100.f ),
			Stringf( "Countdown: %f", m_particleSystems[ m_activeEmitter ]->GetSecondsUntilNextEmit() ),
			.25f,
			nullptr,
			Rgba::RED
			);
}


//-----------------------------------------------------------------------------
float g_shadowAlphaCounter = 0.f;
void TheGame::RenderLeftDebugText2D()
{
	WorldCoords camPos = m_playerCamera->m_worldPosition;
	EulerAngles camOri = m_playerCamera->m_orientation;
	Vector3 camDir = m_playerCamera->GetForwardXYZ();

	const std::string camPosString = Stringf( "Camera Position: %f %f %f", camPos.x, camPos.y, camPos.z );

	g_theRenderer->DrawTextProportional2D
	(
		Vector2( 100.f, (float)g_theApp->GetScreenHeight() - 50.f ), 
		camPosString,
		.25f,
		m_activeFont
	);
	g_theRenderer->DrawTextProportional2D
	( 
		Vector2( 100.f + g_theRenderer->CalcTextPxWidthUpToIndex( camPosString, camPosString.size(), .25f, m_activeFont ), (float)g_theApp->GetScreenHeight() - 50.f /*- g_theRenderer->CalcTextPxHeight( camPosString, .25f, m_activeFont )*/ ),
		( m_activeFont == nullptr ) ? Stringf( "he won't get off me :(" ) : Stringf( "OH GOD STILL i'M StUcK To iT SOMEONE HELP" ),
		.25f,
		m_activeFont,
		Rgba( .7f, .2f, .7f ),
		true,
		Rgba( 1.f, 1.f, 0.f, g_shadowAlphaCounter++ )
	);
	WrapNumberWithinCircularRange( g_shadowAlphaCounter, 0.f, 1.f );
	g_theRenderer->DrawTextProportional2D
	( 
		Vector2( 100.f, (float)g_theApp->GetScreenHeight() - 100.f ),
		Stringf( "Camera Orientation: roll(%f) pitch(%f) yaw(%f)", camOri.m_rollDegrees, camOri.m_pitchDegrees, camOri.m_yawDegrees ), 
		.25f,
		m_activeFont,
		Rgba( .7f, .2f, .7f )
	);
	g_theRenderer->DrawTextProportional2D
	( 
		Vector2( 100.f, (float)g_theApp->GetScreenHeight() - 150.f ),
		Stringf( "Camera Forward XYZ: %f %f %f", camDir.x, camDir.y, camDir.z ), 
		.25f,
		m_activeFont,
		Rgba( .7f, .7f, .7f )
	);
}


//-----------------------------------------------------------------------------//SD3 Initialize Functions//-----------------------------------------------------------------------------
Mesh* BuildSphereMeshTBN( const Vector3& spherePos, float radiusSizeRho, float numCirclesVertical, float numCirclesHorizontal, const Rgba& tint = Rgba::WHITE )
{
	std::vector< Vertex3D_PCUTB > sphere;
	std::vector< unsigned int > sphereIndices;
	float maxThetaDegrees = 360.f;
	float maxPhiDegrees = 180.f; //Should really be between -90 and 90 tho.
	float thetaStep = maxThetaDegrees / numCirclesHorizontal;
	float phiStep = maxPhiDegrees / numCirclesVertical;

	unsigned int numIterations = static_cast<unsigned int>( numCirclesVertical * numCirclesHorizontal );
	sphere.reserve( numIterations * 4 ); //4 vertices per quad * num iterations
	sphereIndices.reserve( numIterations * 6 ); //6 indices to make 2 triangles of said quad

	//Per iteration, create a quad by grabbing other iterations' values (theta+thetaStep, phi+phiStep), (-,+), (+,-).
#pragma region BallVBO
	unsigned int numIteration = 0;
	for ( float thetaDegrees = 0.f; thetaDegrees < maxThetaDegrees; thetaDegrees += thetaStep )
	{
		float sinThisTheta = SinDegrees( thetaDegrees );
		float cosThisTheta = CosDegrees( thetaDegrees );
		float sinNextTheta = SinDegrees( thetaDegrees + thetaStep );
		float cosNextTheta = CosDegrees( thetaDegrees + thetaStep );

		//--U's--

		float thisU = thetaDegrees / maxThetaDegrees;
		float nextU = ( thetaDegrees + thetaStep ) / maxThetaDegrees;

		for ( float phiDegrees = 0.f; phiDegrees < maxPhiDegrees; phiDegrees += phiStep )
		{
			float sinThisPhi = SinDegrees( phiDegrees );
			float cosThisPhi = CosDegrees( phiDegrees );
			float sinNextPhi = SinDegrees( phiDegrees + phiStep );
			float cosNextPhi = CosDegrees( phiDegrees + phiStep );

			//--V's--

			float thisV = phiDegrees / maxPhiDegrees;
			float nextV = ( phiDegrees + phiStep ) / maxPhiDegrees;

#pragma region Positions
			float currentPosX = radiusSizeRho * sinThisPhi * cosThisTheta;
			float currentPosY = radiusSizeRho * sinThisPhi * sinThisTheta;
			float currentPosZ = radiusSizeRho * cosThisPhi;

			//One over on theta: same phi-longitude.
			float nextPosOnThetaX = radiusSizeRho * sinThisPhi * cosNextTheta;
			float nextPosOnThetaY = radiusSizeRho * sinThisPhi * sinNextTheta;
			float nextPosOnThetaZ = radiusSizeRho * cosThisPhi;


			//One over on phi: same theta-latitude.
			float nextPosOnPhiX = radiusSizeRho * sinNextPhi * cosThisTheta;
			float nextPosOnPhiY = radiusSizeRho * sinNextPhi * sinThisTheta;
			float nextPosOnPhiZ = radiusSizeRho * cosNextPhi;

			//One over on both theta and phi.
			float nextPosOnThetaAndPhiX = radiusSizeRho * sinNextPhi * cosNextTheta;
			float nextPosOnThetaAndPhiY = radiusSizeRho * sinNextPhi * sinNextTheta;
			float nextPosOnThetaAndPhiZ = radiusSizeRho * cosNextPhi;

			//Note the above is PRE-change of basis. Need to go [x,y,z] to [-y,z,x].
			Vector3 currentPos = Vector3( -currentPosY, currentPosZ, currentPosX );
			Vector3 nextPosOnTheta = Vector3( -nextPosOnThetaY, nextPosOnThetaZ, nextPosOnThetaX );
			Vector3 nextPosOnPhi = Vector3( -nextPosOnPhiY, nextPosOnPhiZ, nextPosOnPhiX );
			Vector3 nextPosOnThetaAndPhi = Vector3( -nextPosOnThetaAndPhiY, nextPosOnThetaAndPhiZ, nextPosOnThetaAndPhiX );

			//Offset the model position by supplied world position (ensure latter is given relative to post-change of basis).
			currentPos += spherePos;
			nextPosOnTheta += spherePos;
			nextPosOnPhi += spherePos;
			nextPosOnThetaAndPhi += spherePos;
#pragma endregion
#pragma region Tangents
			//Being tangents, they == d(pos)/dtheta.
			float currentTangentX = radiusSizeRho * sinThisPhi * -sinThisTheta;
			float currentTangentY = radiusSizeRho * sinThisPhi * cosThisTheta;
			float currentTangentZ = 0.f;

			//One over on theta: same phi-longitude.
			float nextTangentOnThetaX = radiusSizeRho * sinThisPhi * -sinNextTheta;
			float nextTangentOnThetaY = radiusSizeRho * sinThisPhi * cosNextTheta;
			float nextTangentOnThetaZ = 0.f;

			//One over on phi: same theta-latitude.
			float nextTangentOnPhiX = radiusSizeRho * sinNextPhi * -sinThisTheta;
			float nextTangentOnPhiY = radiusSizeRho * sinNextPhi * cosThisTheta;
			float nextTangentOnPhiZ = 0.f;

			//One over on both theta and phi.
			float nextTangentOnThetaAndPhiX = radiusSizeRho * sinNextPhi * -sinNextTheta;
			float nextTangentOnThetaAndPhiY = radiusSizeRho * sinNextPhi * cosNextTheta;
			float nextTangentOnThetaAndPhiZ = 0.f;

			//Note the above is PRE-change of basis. Need to go [x,y,z] to [-y,z,x].
			Vector3 currentTangent = Vector3( -currentTangentY, currentTangentZ, currentTangentX );
			Vector3 nextTangentOnTheta = Vector3( -nextTangentOnThetaY, nextTangentOnThetaZ, nextTangentOnThetaX );
			Vector3 nextTangentOnPhi = Vector3( -nextTangentOnPhiY, nextTangentOnPhiZ, nextTangentOnPhiX );
			Vector3 nextTangentOnThetaAndPhi = Vector3( -nextTangentOnThetaAndPhiY, nextTangentOnThetaAndPhiZ, nextTangentOnThetaAndPhiX );

#pragma endregion
#pragma region Bitangents
			//Being the other direction's tangent, they == d(pos)/dphi.
			float currentBitangentX = radiusSizeRho * cosThisPhi * cosThisTheta;
			float currentBitangentY = radiusSizeRho * cosThisPhi * sinThisTheta;
			float currentBitangentZ = radiusSizeRho * -sinThisPhi;

			//One over on theta: same phi-longitude.
			float nextBitangentOnThetaX = radiusSizeRho * cosThisPhi * cosNextTheta;
			float nextBitangentOnThetaY = radiusSizeRho * cosThisPhi * sinNextTheta;
			float nextBitangentOnThetaZ = radiusSizeRho * -sinThisPhi;


			//One over on phi: same theta-latitude.
			float nextBitangentOnPhiX = radiusSizeRho * cosNextPhi * cosThisTheta;
			float nextBitangentOnPhiY = radiusSizeRho * cosNextPhi * sinThisTheta;
			float nextBitangentOnPhiZ = radiusSizeRho * -sinNextPhi;

			//One over on both theta and phi.
			float nextBitangentOnThetaAndPhiX = radiusSizeRho * cosNextPhi * cosNextTheta;
			float nextBitangentOnThetaAndPhiY = radiusSizeRho * cosNextPhi * sinNextTheta;
			float nextBitangentOnThetaAndPhiZ = radiusSizeRho * -sinNextPhi;

			//Note the above is PRE-change of basis. Need to go [x,y,z] to [-y,z,x].
			Vector3 currentBitangent = Vector3( -currentBitangentY, currentBitangentZ, currentBitangentX );
			Vector3 nextBitangentOnTheta = Vector3( -nextBitangentOnThetaY, nextBitangentOnThetaZ, nextBitangentOnThetaX );
			Vector3 nextBitangentOnPhi = Vector3( -nextBitangentOnPhiY, nextBitangentOnPhiZ, nextBitangentOnPhiX );
			Vector3 nextBitangentOnThetaAndPhi = Vector3( -nextBitangentOnThetaAndPhiY, nextBitangentOnThetaAndPhiZ, nextBitangentOnThetaAndPhiX );

#pragma endregion

			//Vertex construction: for tex coords, u goes with theta, v goes with phi, as above.
			Vertex3D_PCUTB currentVertex = Vertex3D_PCUTB( currentPos, currentTangent, currentBitangent, Vector2( thisU, thisV ), tint ); //0
			Vertex3D_PCUTB nextVertexOnTheta = Vertex3D_PCUTB( nextPosOnTheta, nextTangentOnTheta, nextBitangentOnTheta, Vector2( nextU, thisV ), tint ); //1
			Vertex3D_PCUTB nextVertexOnPhi = Vertex3D_PCUTB( nextPosOnPhi, nextTangentOnPhi, nextBitangentOnPhi, Vector2( thisU, nextV ), tint ); //2
			Vertex3D_PCUTB nextVertexOnThetaAndPhi = Vertex3D_PCUTB( nextPosOnThetaAndPhi, nextTangentOnThetaAndPhi, nextBitangentOnThetaAndPhi, Vector2( nextU, nextV ), tint ); //3

			//Push back into VBO. Could just add that string to IBO.
			sphere.push_back( currentVertex ); //0
			sphere.push_back( nextVertexOnTheta ); //1
			sphere.push_back( nextVertexOnPhi ); //2
			sphere.push_back( nextVertexOnThetaAndPhi ); //3
														 //			sphere.push_back( nextVertexOnPhi ); //2
														 //			sphere.push_back( nextVertexOnTheta ); //1

			//Push back into IBO: the indices used go 0-3, then 4-8, etc. hence the offset by 4*numIteration.
			sphereIndices.push_back( 0 + ( 4 * numIteration ) );
			sphereIndices.push_back( 1 + ( 4 * numIteration ) );
			sphereIndices.push_back( 2 + ( 4 * numIteration ) );
			sphereIndices.push_back( 3 + ( 4 * numIteration ) );
			sphereIndices.push_back( 2 + ( 4 * numIteration ) );
			sphereIndices.push_back( 1 + ( 4 * numIteration ) );

			numIteration++;
		}
	}
#pragma endregion

	DrawInstruction sphereDrawInstructions[] = { DrawInstruction( PRIMITIVE_TYPE_TRIANGLES, 0, 6 * numIterations, false ) };
	return new Mesh( BufferUsage::STATIC_DRAW, Vertex3D_PCUTB::DEFINITION, sphere.size(), sphere.data(), sphereIndices.size(), sphereIndices.data(), 1, sphereDrawInstructions );
}
Mesh* BuildCubeMeshTBN( const AABB3& bounds )
{
	AABB2 texCoords[ 6 ];
	texCoords[ 0 ] = AABB2( Vector2::ZERO, Vector2::ONE );
	texCoords[ 1 ] = AABB2( Vector2::ZERO, Vector2::ONE );
	texCoords[ 2 ] = AABB2( Vector2::ZERO, Vector2::ONE );
	texCoords[ 3 ] = AABB2( Vector2::ZERO, Vector2::ONE );
	texCoords[ 4 ] = AABB2( Vector2::ZERO, Vector2::ONE );
	texCoords[ 5 ] = AABB2( Vector2::ZERO, Vector2::ONE );
	Vertex3D_PCUTB cube[ 24 ] = //No IBOs means repeating vertexes, hence would need +12.
#pragma region CubeVBO
	{
		//Using clockwise windings for now. Commented out lines were pre-IBO.

		//Bottom face. REMEMBER TO SEND [BI]TANGENT IN THE ENGINE'S Z-UP BASIS!
		Vertex3D_PCUTB( Vector3( bounds.maxs.x, bounds.maxs.y, bounds.mins.z ), WORLD_RIGHT, WORLD_BACKWARD, Rgba::CYAN, Vector2( texCoords[ 0 ].mins.x, texCoords[ 0 ].maxs.y ) ), //0
		Vertex3D_PCUTB( Vector3( bounds.maxs.x, bounds.mins.y, bounds.mins.z ), WORLD_RIGHT, WORLD_BACKWARD, Rgba::CYAN, Vector2( texCoords[ 0 ].maxs.x, texCoords[ 0 ].maxs.y ) ), //1
		Vertex3D_PCUTB( Vector3( bounds.mins.x, bounds.mins.y, bounds.mins.z ), WORLD_RIGHT, WORLD_BACKWARD, Rgba::CYAN, Vector2( texCoords[ 0 ].maxs.x, texCoords[ 0 ].mins.y ) ), //2
																																													//		Vertex3D_PCUTB( Vector3( bounds.maxs.x, bounds.maxs.y, bounds.mins.z ), Vector3( 1,0,0 ), Vector3( 0,0,-1 ), Rgba::CYAN, Vector2( texCoords[ 0 ].mins.x, texCoords[ 0 ].maxs.y ) ), //0
																																													//		Vertex3D_PCUTB( Vector3( bounds.mins.x, bounds.mins.y, bounds.mins.z ), Vector3( 1,0,0 ), Vector3( 0,0,-1 ), Rgba::CYAN, Vector2( texCoords[ 0 ].maxs.x, texCoords[ 0 ].mins.y ) ), //2
		Vertex3D_PCUTB( Vector3( bounds.mins.x, bounds.maxs.y, bounds.mins.z ), WORLD_RIGHT, WORLD_BACKWARD, Rgba::CYAN, Vector2( texCoords[ 0 ].mins.x, texCoords[ 0 ].mins.y ) ), //3

		//Top face.
		Vertex3D_PCUTB( Vector3( bounds.mins.x, bounds.maxs.y, bounds.maxs.z ), WORLD_RIGHT, WORLD_FORWARD, Rgba::BLUE, Vector2( texCoords[ 1 ].mins.x, texCoords[ 1 ].maxs.y ) ), //0
		Vertex3D_PCUTB( Vector3( bounds.mins.x, bounds.mins.y, bounds.maxs.z ), WORLD_RIGHT, WORLD_FORWARD, Rgba::BLUE, Vector2( texCoords[ 1 ].maxs.x, texCoords[ 1 ].maxs.y ) ), //1
		Vertex3D_PCUTB( Vector3( bounds.maxs.x, bounds.mins.y, bounds.maxs.z ), WORLD_RIGHT, WORLD_FORWARD, Rgba::BLUE, Vector2( texCoords[ 1 ].maxs.x, texCoords[ 1 ].mins.y ) ), //2
																																												   //		Vertex3D_PCUTB( Vector3( bounds.mins.x, bounds.maxs.y, bounds.maxs.z ), Vector3( 1,0,0 ), Vector3( 0,0,1 ), Rgba::BLUE, Vector2( texCoords[ 1 ].mins.x, texCoords[ 1 ].maxs.y ) ), //0
																																												   //		Vertex3D_PCUTB( Vector3( bounds.maxs.x, bounds.mins.y, bounds.maxs.z ), Vector3( 1,0,0 ), Vector3( 0,0,1 ), Rgba::BLUE, Vector2( texCoords[ 1 ].maxs.x, texCoords[ 1 ].mins.y ) ), //2
		Vertex3D_PCUTB( Vector3( bounds.maxs.x, bounds.maxs.y, bounds.maxs.z ), WORLD_RIGHT, WORLD_FORWARD, Rgba::BLUE, Vector2( texCoords[ 1 ].mins.x, texCoords[ 1 ].mins.y ) ), //3

		//Left.
		Vertex3D_PCUTB( Vector3( bounds.maxs.x, bounds.maxs.y, bounds.mins.z ), WORLD_BACKWARD, WORLD_UP, Rgba::GREEN, Vector2( texCoords[ 2 ].mins.x, texCoords[ 2 ].maxs.y ) ), //0
		Vertex3D_PCUTB( Vector3( bounds.mins.x, bounds.maxs.y, bounds.mins.z ), WORLD_BACKWARD, WORLD_UP, Rgba::GREEN, Vector2( texCoords[ 2 ].maxs.x, texCoords[ 2 ].maxs.y ) ), //1
		Vertex3D_PCUTB( Vector3( bounds.mins.x, bounds.maxs.y, bounds.maxs.z ), WORLD_BACKWARD, WORLD_UP, Rgba::GREEN, Vector2( texCoords[ 2 ].maxs.x, texCoords[ 2 ].mins.y ) ), //2
																																												  //		Vertex3D_PCUTB( Vector3( bounds.maxs.x, bounds.maxs.y, bounds.mins.z ), Vector3( 0,0,-1 ), Vector3( 0,1,0 ), Rgba::GREEN, Vector2( texCoords[ 2 ].mins.x, texCoords[ 2 ].maxs.y ) ), //0
																																												  //		Vertex3D_PCUTB( Vector3( bounds.mins.x, bounds.maxs.y, bounds.maxs.z ), Vector3( 0,0,-1 ), Vector3( 0,1,0 ), Rgba::GREEN, Vector2( texCoords[ 2 ].maxs.x, texCoords[ 2 ].mins.y ) ), //2
		Vertex3D_PCUTB( Vector3( bounds.maxs.x, bounds.maxs.y, bounds.maxs.z ), WORLD_BACKWARD, WORLD_UP, Rgba::GREEN, Vector2( texCoords[ 2 ].mins.x, texCoords[ 2 ].mins.y ) ), //3

		//Right.
		Vertex3D_PCUTB( Vector3( bounds.mins.x, bounds.mins.y, bounds.mins.z ), WORLD_FORWARD, WORLD_UP, Rgba::GREEN * Rgba::GRAY, Vector2( texCoords[ 3 ].mins.x, texCoords[ 3 ].maxs.y ) ), //0
		Vertex3D_PCUTB( Vector3( bounds.maxs.x, bounds.mins.y, bounds.mins.z ), WORLD_FORWARD, WORLD_UP, Rgba::GREEN * Rgba::GRAY, Vector2( texCoords[ 3 ].maxs.x, texCoords[ 3 ].maxs.y ) ), //1
		Vertex3D_PCUTB( Vector3( bounds.maxs.x, bounds.mins.y, bounds.maxs.z ), WORLD_FORWARD, WORLD_UP, Rgba::GREEN * Rgba::GRAY, Vector2( texCoords[ 3 ].maxs.x, texCoords[ 3 ].mins.y ) ), //2
																																															  //		Vertex3D_PCUTB( Vector3( bounds.mins.x, bounds.mins.y, bounds.mins.z ), Vector3( 0,0,1 ), Vector3( 0,1,0 ), Rgba::GREEN * Rgba::GRAY, Vector2( texCoords[ 3 ].mins.x, texCoords[ 3 ].maxs.y ) ), //0
																																															  //		Vertex3D_PCUTB( Vector3( bounds.maxs.x, bounds.mins.y, bounds.maxs.z ), Vector3( 0,0,1 ), Vector3( 0,1,0 ), Rgba::GREEN * Rgba::GRAY, Vector2( texCoords[ 3 ].maxs.x, texCoords[ 3 ].mins.y ) ), //2
		Vertex3D_PCUTB( Vector3( bounds.mins.x, bounds.mins.y, bounds.maxs.z ), WORLD_FORWARD, WORLD_UP, Rgba::GREEN * Rgba::GRAY, Vector2( texCoords[ 3 ].mins.x, texCoords[ 3 ].mins.y ) ), //3

		//Front.
		Vertex3D_PCUTB( Vector3( bounds.mins.x, bounds.maxs.y, bounds.mins.z ), WORLD_RIGHT, WORLD_UP, Rgba::RED * Rgba::GRAY, Vector2( texCoords[ 4 ].mins.x, texCoords[ 4 ].maxs.y ) ), //0
		Vertex3D_PCUTB( Vector3( bounds.mins.x, bounds.mins.y, bounds.mins.z ), WORLD_RIGHT, WORLD_UP, Rgba::RED * Rgba::GRAY, Vector2( texCoords[ 4 ].maxs.x, texCoords[ 4 ].maxs.y ) ), //1
		Vertex3D_PCUTB( Vector3( bounds.mins.x, bounds.mins.y, bounds.maxs.z ), WORLD_RIGHT, WORLD_UP, Rgba::RED * Rgba::GRAY, Vector2( texCoords[ 4 ].maxs.x, texCoords[ 4 ].mins.y ) ), //2
																																														  //		Vertex3D_PCUTB( Vector3( bounds.mins.x, bounds.maxs.y, bounds.mins.z ), Vector3( 1,0,0 ), Vector3( 0,1,0 ), Rgba::RED * Rgba::GRAY, Vector2( texCoords[ 4 ].mins.x, texCoords[ 4 ].maxs.y ) ), //0
																																														  //		Vertex3D_PCUTB( Vector3( bounds.mins.x, bounds.mins.y, bounds.maxs.z ), Vector3( 1,0,0 ), Vector3( 0,1,0 ), Rgba::RED * Rgba::GRAY, Vector2( texCoords[ 4 ].maxs.x, texCoords[ 4 ].mins.y ) ), //2
		Vertex3D_PCUTB( Vector3( bounds.mins.x, bounds.maxs.y, bounds.maxs.z ), WORLD_RIGHT, WORLD_UP, Rgba::RED * Rgba::GRAY, Vector2( texCoords[ 4 ].mins.x, texCoords[ 4 ].mins.y ) ), //3

		//Back.
		Vertex3D_PCUTB( Vector3( bounds.maxs.x, bounds.mins.y, bounds.mins.z ), WORLD_LEFT, WORLD_UP, Rgba::RED, Vector2( texCoords[ 5 ].mins.x, texCoords[ 5 ].maxs.y ) ), //0
		Vertex3D_PCUTB( Vector3( bounds.maxs.x, bounds.maxs.y, bounds.mins.z ), WORLD_LEFT, WORLD_UP, Rgba::RED, Vector2( texCoords[ 5 ].maxs.x, texCoords[ 5 ].maxs.y ) ), //1
		Vertex3D_PCUTB( Vector3( bounds.maxs.x, bounds.maxs.y, bounds.maxs.z ), WORLD_LEFT, WORLD_UP, Rgba::RED, Vector2( texCoords[ 5 ].maxs.x, texCoords[ 5 ].mins.y ) ), //2
																																											//	Vertex3D_PCUTB( Vector3( bounds.maxs.x, bounds.mins.y, bounds.mins.z ), Vector3( 1,0,0 ), Vector3( 0,1,0 ), Rgba::RED, Vector2( texCoords[ 5 ].mins.x, texCoords[ 5 ].maxs.y ) ), //0
																																											//	Vertex3D_PCUTB( Vector3( bounds.maxs.x, bounds.maxs.y, bounds.maxs.z ), Vector3( 1,0,0 ), Vector3( 0,1,0 ), Rgba::RED, Vector2( texCoords[ 5 ].maxs.x, texCoords[ 5 ].mins.y ) ), //2
		Vertex3D_PCUTB( Vector3( bounds.maxs.x, bounds.mins.y, bounds.maxs.z ), WORLD_LEFT, WORLD_UP, Rgba::RED, Vector2( texCoords[ 5 ].mins.x, texCoords[ 5 ].mins.y ) ) //3
	};
#pragma endregion
	unsigned int cubeIndices[] = //Not actually used, because one vertex can't store 3 UV pairs for each face the corner's used for.
#pragma region CubeIBO //Clockwise
	{
		//Bottom.
		0, 1, 2,
		0, 2, 3,
		//Top.
		0 + 4, 1 + 4, 2 + 4,
		0 + 4, 2 + 4, 3 + 4,
		//Left.
		0 + 4 * 2, 1 + 4 * 2, 2 + 4 * 2,
		0 + 4 * 2, 2 + 4 * 2, 3 + 4 * 2,
		//Right.
		0 + 4 * 3, 1 + 4 * 3, 2 + 4 * 3,
		0 + 4 * 3, 2 + 4 * 3, 3 + 4 * 3,
		//Front.
		0 + 4 * 4, 1 + 4 * 4, 2 + 4 * 4,
		0 + 4 * 4, 2 + 4 * 4, 3 + 4 * 4,
		//Back.
		0 + 4 * 5, 1 + 4 * 5, 2 + 4 * 5,
		0 + 4 * 5, 2 + 4 * 5, 3 + 4 * 5
	};
#pragma endregion

	DrawInstruction cubeDrawInstructions[] = { DrawInstruction( PRIMITIVE_TYPE_TRIANGLES, 0, 36, true ) };
	return new Mesh( BufferUsage::STATIC_DRAW, Vertex3D_PCUTB::DEFINITION, _countof( cube ), cube, _countof( cubeIndices ), cubeIndices, 1, cubeDrawInstructions );
}
//Positions given in NEW basis ( [x,y,z] to [-y,z,x] ) with ADJUSTED texture coords.
MeshRenderer* BuildBlinnPhongQuadWithTangentsAndBitangents( const AABB2& planarBounds, float depth, const Rgba& tint, const std::string& shaderProgramName, const RenderState& renderState )
{
	Vertex3D_PCUTB quad[ 4 ] = //REMEMBER TO SEND[ BI ]TANGENT IN THE ENGINE'S Z-UP BASIS!
	{
		Vertex3D_PCUTB( Vector3( depth, planarBounds.mins.x, planarBounds.mins.y ), WORLD_RIGHT, WORLD_UP, tint, Vector2( 1.f, 1.f ) ), //Top-left.		//0
		Vertex3D_PCUTB( Vector3( depth, planarBounds.maxs.x, planarBounds.maxs.y ), WORLD_RIGHT, WORLD_UP, tint, Vector2( 0.f, 0.f ) ), //Bottom-right.	//1
		Vertex3D_PCUTB( Vector3( depth, planarBounds.maxs.x, planarBounds.mins.y ), WORLD_RIGHT, WORLD_UP, tint, Vector2( 0.f, 1.f ) ), //Bottom-left.	//2
		Vertex3D_PCUTB( Vector3( depth, planarBounds.mins.x, planarBounds.maxs.y ), WORLD_RIGHT, WORLD_UP, tint, Vector2( 1.f, 0.f ) ), //Top-right.	//3
	};
	unsigned int quadIndices[] =
	{
		0, 1, 2, //Clockwise
		3, 1, 0
	};

	DrawInstruction quadDrawInstructions[] = { DrawInstruction( PRIMITIVE_TYPE_TRIANGLES, 0, 6, true ) };
	Mesh* quadMesh = new Mesh( BufferUsage::STATIC_DRAW, Vertex3D_PCUTB::DEFINITION, _countof( quad ), quad, _countof( quadIndices ), quadIndices, 1, quadDrawInstructions );

	Material* quadMaterial = Material::CreateOrGetMaterial( "QuadTBN_" + shaderProgramName, &renderState, &Vertex3D_PCUTB::DEFINITION, shaderProgramName.c_str() );

	return new MeshRenderer( quadMesh, quadMaterial );
}
MeshRenderer* BuildBlinnPhongQuad( const AABB2& planarBounds, float depth, const Rgba& tint, const std::string& shaderProgramName, const RenderState& renderState )
{
	Vertex3D_PCT quad[ 4 ] = //Appears off to the left in the new basis, as i-vector is then -y.
	{
		Vertex3D_PCT( Vector3( depth, planarBounds.mins.x, planarBounds.mins.y ), tint, Vector2( 1.f, 1.f ) ), //Top-left.			//0
		Vertex3D_PCT( Vector3( depth, planarBounds.maxs.x, planarBounds.maxs.y ), tint, Vector2( 0.f, 0.f ) ), //Bottom-right.		//1
		Vertex3D_PCT( Vector3( depth, planarBounds.maxs.x, planarBounds.mins.y ), tint, Vector2( 0.f, 1.f ) ), //Bottom-left.		//2
		Vertex3D_PCT( Vector3( depth, planarBounds.mins.x, planarBounds.maxs.y ), tint, Vector2( 1.f, 0.f ) ), //Top-right.		//3
	};
	unsigned int quadIndices[] =
	{
		0, 1, 2, //Clockwise
		3, 1, 0
	};

	DrawInstruction quadDrawInstructions[] = { DrawInstruction( PRIMITIVE_TYPE_TRIANGLES, 0, 6, true ) };
	Mesh* quadMesh = new Mesh( BufferUsage::STATIC_DRAW, Vertex3D_PCT::DEFINITION, _countof( quad ), quad, _countof( quadIndices ), quadIndices, 1, quadDrawInstructions );

	Material* quadMaterial = Material::CreateOrGetMaterial( "QuadNoTBN_" + shaderProgramName, &renderState, &Vertex3D_PCT::DEFINITION, shaderProgramName.c_str() );

	return new MeshRenderer( quadMesh, quadMaterial );
}
MeshRenderer* BuildTemporalQuad( const AABB2& planarBounds, float depth, const Rgba& tint, const std::string& shaderProgramName, const RenderState& renderState )
{
	Vertex3D_PCT timeQuad[ 4 ] =
	{
		Vertex3D_PCT( Vector3( planarBounds.mins.x,	depth, planarBounds.mins.y ), tint, Vector2( 0.f, 0.f ) ), //Top-left.		//0
		Vertex3D_PCT( Vector3( planarBounds.maxs.x,	depth, planarBounds.maxs.y ), tint, Vector2( 1.f, 1.f ) ), //Bottom-right.	//1
		Vertex3D_PCT( Vector3( planarBounds.maxs.x,	depth, planarBounds.mins.y ), tint, Vector2( 0.f, 1.f ) ), //Bottom-left.		//2
		Vertex3D_PCT( Vector3( planarBounds.mins.x,	depth, planarBounds.maxs.y ), tint, Vector2( 1.f, 0.f ) ), //Top-right.		//3
	};

	unsigned int timeQuadIndices[ 6 ] = {
		2, 1, 0, //Counter-Clockwise
		0, 1, 3
	};

	DrawInstruction timeQuadDrawInstructions[] = { DrawInstruction( PRIMITIVE_TYPE_TRIANGLES, 0, 6, true ) };
	Mesh* timeQuadMesh = new Mesh( BufferUsage::STATIC_DRAW, Vertex3D_PCT::DEFINITION, _countof( timeQuad ), timeQuad, _countof( timeQuadIndices ), timeQuadIndices, 1, timeQuadDrawInstructions );

	Material* timeQuadMaterial = Material::CreateOrGetMaterial( "TimeQuad_" + shaderProgramName, &renderState, &Vertex3D_PCT::DEFINITION, shaderProgramName.c_str() );

	return new MeshRenderer( timeQuadMesh, timeQuadMaterial );
}
MeshRenderer* BuildAxisTintedCubeRendererTBN( const AABB3& bounds, const std::string& shaderProgramName, const RenderState& renderState )
{
	Mesh* cubeMesh = BuildCubeMeshTBN( bounds );

	Material* cubeMaterial = Material::CreateOrGetMaterial( "CubeTBN_" + shaderProgramName, &renderState, &Vertex3D_PCUTB::DEFINITION, shaderProgramName.c_str() );

	return new MeshRenderer( cubeMesh, cubeMaterial );
}
TODO( "BuildSphere (and TBN ver) still need(s) to try and change that sphere theta/phi rotation mixup issue." );
MeshRenderer* BuildSphereRendererTBN( const Vector3& spherePos, float radiusSizeRho, float numCirclesVertical, float numCirclesHorizontal, const Rgba& tint, const std::string& shaderProgramName, const RenderState& renderState )
{
	Mesh* sphereMesh = BuildSphereMeshTBN( spherePos, radiusSizeRho, numCirclesVertical, numCirclesHorizontal, tint );

	Material* sphereMaterial = Material::CreateOrGetMaterial( "SphereTBN_" + shaderProgramName, &renderState, &Vertex3D_PCUTB::DEFINITION, shaderProgramName.c_str() );

	return new MeshRenderer( sphereMesh, sphereMaterial );
}
void BuildVBOs()
{
	RenderState universalRenderState = RenderState( CULL_MODE_BACK, BLEND_MODE_SOURCE_ALPHA, BLEND_MODE_ONE_MINUS_SOURCE_ALPHA );

	AABB2 blinnPhongQuadPlanarBounds;
	blinnPhongQuadPlanarBounds.mins = Vector2::ONE * -.5f;
	blinnPhongQuadPlanarBounds.maxs = Vector2::ONE * .5f;
	gMeshRenderers.push_back( BuildBlinnPhongQuadWithTangentsAndBitangents( blinnPhongQuadPlanarBounds, 9.f, Rgba::WHITE, "BlinnPhongTBN", universalRenderState ) );

	AABB2 timeQuadPlanarBounds;
	timeQuadPlanarBounds.mins = Vector2::ONE * -.25f;
	timeQuadPlanarBounds.maxs = Vector2::ONE * .25f;
	gMeshRenderers.push_back( BuildTemporalQuad( timeQuadPlanarBounds, 15.f, Rgba::CYAN, "TimeFX", universalRenderState ) );

	//Cube monster.
	AABB3 cubeBounds;
	cubeBounds.mins = Vector3( -1.f, 4.0f, -1.f );
	cubeBounds.maxs = Vector3( 1.f, 6.0f, 1.f );
	gMeshRenderers.push_back( BuildAxisTintedCubeRendererTBN( cubeBounds, "BlinnPhongTBN", universalRenderState ) );
	cubeBounds.mins = Vector3( -1.f, 8.0f, -1.f );
	cubeBounds.maxs = Vector3( 1.f, 10.0f, 1.f );
	gMeshRenderers.push_back( BuildAxisTintedCubeRendererTBN( cubeBounds, "BlinnPhongTBN", universalRenderState ) );
	cubeBounds.mins = Vector3( -3.f, 6.f, -1.f );
	cubeBounds.maxs = Vector3( -1.f, 8.f, 1.f );
	gMeshRenderers.push_back( BuildAxisTintedCubeRendererTBN( cubeBounds, "BlinnPhongTBN", universalRenderState ) );
	cubeBounds.mins = Vector3( 1.f, 6.f, -1.f );
	cubeBounds.maxs = Vector3( 3.f, 8.f, 1.f );
	gMeshRenderers.push_back( BuildAxisTintedCubeRendererTBN( cubeBounds, "BlinnPhongTBN", universalRenderState ) );
	cubeBounds.mins = Vector3( -1.f, 6.f, -3.f );
	cubeBounds.maxs = Vector3( 1.f, 8.f, -1.f );
	gMeshRenderers.push_back( BuildAxisTintedCubeRendererTBN( cubeBounds, "BlinnPhongTBN", universalRenderState ) );
	cubeBounds.mins = Vector3( -1.f, 6.f, 1.f );
	cubeBounds.maxs = Vector3( 1.f, 8.f, 3.f );
	gMeshRenderers.push_back( BuildAxisTintedCubeRendererTBN( cubeBounds, "BlinnPhongTBN", universalRenderState ) );

	//Custom A6 effect cube inside the shape.
	cubeBounds.mins = Vector3( -1.f, 6.0f, -1.f );
	cubeBounds.maxs = Vector3( 1.f, 8.0f, 1.f );
	gMeshRenderers.push_back( BuildAxisTintedCubeRendererTBN( cubeBounds, "Mandelbrot", universalRenderState ) );

	Vector3 spherePos = Vector3( 0.f, 0.f, 5.f );
	float radiusSizeRho = 1.f;
	gMeshRenderers.push_back( BuildSphereRendererTBN( spherePos, radiusSizeRho, 50.f, 50.f, Rgba::WHITE, "BlinnPhongTBN", universalRenderState ) );

	Vector3 triplanarSpherePos = Vector3( 0.f, 0.f, 7.f );
	radiusSizeRho = .75f;
	gMeshRenderers.push_back( BuildSphereRendererTBN( triplanarSpherePos, radiusSizeRho, 50.f, 50.f, Rgba::WHITE, "Triplanar", universalRenderState ) );

	Vector3 triplanarCubePos = triplanarSpherePos + WORLD_FORWARD;
	gMeshRenderers.push_back( BuildAxisTintedCubeRendererTBN( AABB3( triplanarCubePos, triplanarCubePos + Vector3::ONE ), "Triplanar", universalRenderState ) );

	Vector3 parallaxSpherePos = Vector3( 0.f, 0.f, -5.f );
	radiusSizeRho = .75f;
	gMeshRenderers.push_back( BuildSphereRendererTBN( parallaxSpherePos, radiusSizeRho, 50.f, 50.f, Rgba::WHITE, "Parallax", universalRenderState ) );

	Vector3 parallaxCubePos = parallaxSpherePos + WORLD_FORWARD;
	gMeshRenderers.push_back( BuildAxisTintedCubeRendererTBN( AABB3( parallaxCubePos, parallaxCubePos + Vector3::ONE ), "Parallax", universalRenderState ) );

	Vector3 dissolveSpherePos = Vector3( 0.f, -5.f, 0.f );
	radiusSizeRho = .75f;
	gMeshRenderers.push_back( BuildSphereRendererTBN( dissolveSpherePos, radiusSizeRho, 50.f, 50.f, Rgba::WHITE, "Dissolve", universalRenderState ) );

	Vector3 dissolveCubePos = dissolveSpherePos + WORLD_FORWARD;
	gMeshRenderers.push_back( BuildAxisTintedCubeRendererTBN( AABB3( dissolveCubePos, dissolveCubePos + Vector3::ONE ), "Dissolve", universalRenderState ) );

	Vector3 projectorSpherePos = Vector3( 0.f, 0.f, 3.f );
	radiusSizeRho = .75f;
	gMeshRenderers.push_back( BuildSphereRendererTBN( projectorSpherePos, radiusSizeRho, 50.f, 50.f, Rgba::WHITE, "Projector", universalRenderState ) );

	Vector3 projectorCubePos = projectorSpherePos + WORLD_FORWARD;
	gMeshRenderers.push_back( BuildAxisTintedCubeRendererTBN( AABB3( projectorCubePos, projectorCubePos + Vector3::ONE ), "Projector", universalRenderState ) );
}
void InitShaderPrograms()
{
	ShaderProgram::CreateOrGetShaderProgram( "FixedVertexFormat", "Data/Shaders/fvf.vert", "Data/Shaders/fvf.frag", &Vertex3D_PCT::DEFINITION );
	ShaderProgram::CreateOrGetShaderProgram( "TimeFX", "Data/Shaders/timeFx.vert", "Data/Shaders/timeFx.frag", &Vertex3D_PCT::DEFINITION );
	ShaderProgram::CreateOrGetShaderProgram( "BlinnPhongTBN", "Data/Shaders/blinnPhong.vert", "Data/Shaders/blinnPhongMultipleLights.frag", &Vertex3D_PCUTB::DEFINITION );
	ShaderProgram::CreateOrGetShaderProgram( "Triplanar", "Data/Shaders/triplanar.vert", "Data/Shaders/triplanar.frag", &Vertex3D_PCUTB::DEFINITION );
	ShaderProgram::CreateOrGetShaderProgram( "Parallax", "Data/Shaders/blinnPhong.vert", "Data/Shaders/parallax.frag", &Vertex3D_PCUTB::DEFINITION );
	ShaderProgram::CreateOrGetShaderProgram( "Dissolve", "Data/Shaders/blinnPhong.vert", "Data/Shaders/dissolve.frag", &Vertex3D_PCUTB::DEFINITION );
	ShaderProgram::CreateOrGetShaderProgram( "Projector", "Data/Shaders/blinnPhong.vert", "Data/Shaders/projector.frag", &Vertex3D_PCUTB::DEFINITION );
	ShaderProgram::CreateOrGetShaderProgram( "Passthrough", "Data/Shaders/passthrough.vert", "Data/Shaders/passthrough.frag", &Vertex3D_PCUTB::DEFINITION );
	ShaderProgram::CreateOrGetShaderProgram( "Mandelbrot", "Data/Shaders/passthrough.vert", "Data/Shaders/mandelbrot.frag", &Vertex3D_PCUTB::DEFINITION );
	//	ShaderProgram::CreateOrGetShaderProgram( "Skybox", "Data/Shaders/skybox.vert", "Data/Shaders/skybox.frag", &Vertex3D_PCUTB::DEFINITION );
	TODO( "Get noise working, and then see SD3 Todo Notes for implmenting clouds, wood, and marble shaders from the bunny book." );
	//	ShaderProgram::CreateOrGetShaderProgram( "Clouds", "Data/Shaders/clouds.vert", "Data/Shaders/clouds.frag", &Vertex3D_PCUTB::DEFINITION );

	ShaderProgram::CreateOrGetShaderProgram( "PostProcessGrayscale", "Data/Shaders/PostProcessing/post.vert", "Data/Shaders/PostProcessing/postGrayscale.frag", &Vertex3D_PCUTB::DEFINITION );
	//Grayscale == replace each pixel R, G, and B with the original color's sRGB luminance value.
	ShaderProgram::CreateOrGetShaderProgram( "PostProcessCMYK_C", "Data/Shaders/PostProcessing/post.vert", "Data/Shaders/PostProcessing/postCMYK_C.frag", &Vertex3D_PCUTB::DEFINITION );
	ShaderProgram::CreateOrGetShaderProgram( "PostProcessCMYK_M", "Data/Shaders/PostProcessing/post.vert", "Data/Shaders/PostProcessing/postCMYK_M.frag", &Vertex3D_PCUTB::DEFINITION );
	ShaderProgram::CreateOrGetShaderProgram( "PostProcessCMYK_Y", "Data/Shaders/PostProcessing/post.vert", "Data/Shaders/PostProcessing/postCMYK_Y.frag", &Vertex3D_PCUTB::DEFINITION );
	ShaderProgram::CreateOrGetShaderProgram( "PostProcessCMYK_K", "Data/Shaders/PostProcessing/post.vert", "Data/Shaders/PostProcessing/postCMYK_K.frag", &Vertex3D_PCUTB::DEFINITION );
	ShaderProgram::CreateOrGetShaderProgram( "PostProcessHueShift", "Data/Shaders/PostProcessing/post.vert", "Data/Shaders/PostProcessing/postHueShift.frag", &Vertex3D_PCUTB::DEFINITION );
	ShaderProgram::CreateOrGetShaderProgram( "PostProcessBunnyBlur", "Data/Shaders/PostProcessing/post.vert", "Data/Shaders/PostProcessing/postBunnyBlur.frag", &Vertex3D_PCUTB::DEFINITION );
	ShaderProgram::CreateOrGetShaderProgram( "PostProcessGaussianBlur", "Data/Shaders/PostProcessing/post.vert", "Data/Shaders/PostProcessing/postGaussianBlur.frag", &Vertex3D_PCUTB::DEFINITION );
	ShaderProgram::CreateOrGetShaderProgram( "PostProcessChromakey", "Data/Shaders/PostProcessing/post.vert", "Data/Shaders/PostProcessing/postChromakey.frag", &Vertex3D_PCUTB::DEFINITION );
	ShaderProgram::CreateOrGetShaderProgram( "PostProcessAnaglyph", "Data/Shaders/PostProcessing/post.vert", "Data/Shaders/PostProcessing/postAnaglyph.frag", &Vertex3D_PCUTB::DEFINITION );
	ShaderProgram::CreateOrGetShaderProgram( "PostProcessEdgeDetectByLuminance", "Data/Shaders/PostProcessing/post.vert", "Data/Shaders/PostProcessing/postEdgeDetectionByLuminance.frag", &Vertex3D_PCUTB::DEFINITION );
	ShaderProgram::CreateOrGetShaderProgram( "PostProcessEmboss", "Data/Shaders/PostProcessing/post.vert", "Data/Shaders/PostProcessing/postEmboss.frag", &Vertex3D_PCUTB::DEFINITION );
	ShaderProgram::CreateOrGetShaderProgram( "PostProcessToonShader", "Data/Shaders/PostProcessing/post.vert", "Data/Shaders/PostProcessing/postToonShader.frag", &Vertex3D_PCUTB::DEFINITION );
	ShaderProgram::CreateOrGetShaderProgram( "PostProcessZoom", "Data/Shaders/PostProcessing/post.vert", "Data/Shaders/PostProcessing/postZoom.frag", &Vertex3D_PCUTB::DEFINITION );
	ShaderProgram::CreateOrGetShaderProgram( "PostProcessTwirl", "Data/Shaders/PostProcessing/post.vert", "Data/Shaders/PostProcessing/postTwirl.frag", &Vertex3D_PCUTB::DEFINITION );

	ShaderProgram::CreateOrGetShaderProgram( "PostProcessNegative", "Data/Shaders/PostProcessing/post.vert", "Data/Shaders/PostProcessing/postNegative.frag", &Vertex3D_PCUTB::DEFINITION );
	ShaderProgram::CreateOrGetShaderProgram( "PostProcessBrightness", "Data/Shaders/PostProcessing/post.vert", "Data/Shaders/PostProcessing/postBrightness.frag", &Vertex3D_PCUTB::DEFINITION );
	ShaderProgram::CreateOrGetShaderProgram( "PostProcessContrast", "Data/Shaders/PostProcessing/post.vert", "Data/Shaders/PostProcessing/postContrast.frag", &Vertex3D_PCUTB::DEFINITION );
	ShaderProgram::CreateOrGetShaderProgram( "PostProcessSaturation", "Data/Shaders/PostProcessing/post.vert", "Data/Shaders/PostProcessing/postSaturation.frag", &Vertex3D_PCUTB::DEFINITION );
	ShaderProgram::CreateOrGetShaderProgram( "PostProcessSharpness", "Data/Shaders/PostProcessing/post.vert", "Data/Shaders/PostProcessing/postSharpness.frag", &Vertex3D_PCUTB::DEFINITION );

	ShaderProgram::CreateOrGetShaderProgram( "PostProcessColorMask", "Data/Shaders/PostProcessing/post.vert", "Data/Shaders/PostProcessing/postColorMask.frag", &Vertex3D_PCUTB::DEFINITION );
	ShaderProgram::CreateOrGetShaderProgram( "PostProcessDepthOnly", "Data/Shaders/PostProcessing/post.vert", "Data/Shaders/PostProcessing/postDepthOnly.frag", &Vertex3D_PCUTB::DEFINITION );
	ShaderProgram::CreateOrGetShaderProgram( "PostProcessEdgeDetectByDepthBuffer", "Data/Shaders/PostProcessing/post.vert", "Data/Shaders/PostProcessing/postEdgeDetectionByDepthBuffer.frag", &Vertex3D_PCUTB::DEFINITION );
	ShaderProgram::CreateOrGetShaderProgram( "PostProcessEdgeDetectQuantized", "Data/Shaders/PostProcessing/post.vert", "Data/Shaders/PostProcessing/postEdgeDetectionByDepthBufferWithQuantization.frag", &Vertex3D_PCUTB::DEFINITION );
	ShaderProgram::CreateOrGetShaderProgram( "PostProcessObama", "Data/Shaders/PostProcessing/post.vert", "Data/Shaders/PostProcessing/postObama.frag", &Vertex3D_PCUTB::DEFINITION );
	//Obama == switch on luminance values to assign pixels the Obama poster's colors. Does NOT make grayscale first, just calculates luminance.
	ShaderProgram::CreateOrGetShaderProgram( "PostProcessRipple", "Data/Shaders/PostProcessing/post.vert", "Data/Shaders/PostProcessing/postRipple.frag", &Vertex3D_PCUTB::DEFINITION );

}
void InitUniforms()
{
	Texture* texture = Texture::CreateOrGetTexture( "Data/Images/Paused.png" );
	//	Texture* diffuseAlienTex = Texture::CreateOrGetTexture( "Data/Images/alienfloor1_diffuse.png" );
	//	Texture* normalAlienTex = Texture::CreateOrGetTexture( "Data/Images/alienfloor1_normal.png" );
	//	Texture* specularAlienTex = Texture::CreateOrGetTexture( "Data/Images/alienfloor1_specular.png" );
	Texture* diffuseStoneTex = Texture::CreateOrGetTexture( "Data/Images/stone_diffuse.png" );
	Texture* normalStoneTex = Texture::CreateOrGetTexture( "Data/Images/stone_normal.png" );
	Texture* specularStoneTex = Texture::CreateOrGetTexture( "Data/Images/stone_spec.png" );
	Texture* emissiveStoneTex = Texture::CreateOrGetTexture( "Data/Images/stone_emissive.png" );
	Texture* ambientOcclusionStoneTex = Texture::CreateOrGetTexture( "Data/Images/stone_ao.png" );
	Texture* heightsStoneTex = Texture::CreateOrGetTexture( "Data/Images/stone_heights.png" );
	Texture* dissolveTex = Texture::CreateOrGetTexture( "Data/Images/dissolve.png" );
	Texture* projectedTex = Texture::CreateOrGetTexture( "Data/Images/projected.png" );
	Texture* diffuseStoneTexXY = Texture::CreateOrGetTexture( "Data/Images/Triplanar/stone_diffuseXY.png" );
	Texture* diffuseStoneTexXZ = Texture::CreateOrGetTexture( "Data/Images/Triplanar/stone_diffuseXZ.png" );
	Texture* diffuseStoneTexYZ = Texture::CreateOrGetTexture( "Data/Images/Triplanar/stone_diffuseYZ.png" );
	Texture* specularStoneTexXY = Texture::CreateOrGetTexture( "Data/Images/Triplanar/stone_specXY.png" );
	Texture* specularStoneTexXZ = Texture::CreateOrGetTexture( "Data/Images/Triplanar/stone_specXZ.png" );
	Texture* specularStoneTexYZ = Texture::CreateOrGetTexture( "Data/Images/Triplanar/stone_specYZ.png" );
	Texture* normalStoneTexXY = Texture::CreateOrGetTexture( "Data/Images/Triplanar/stone_normalXY.png" );
	Texture* normalStoneTexXZ = Texture::CreateOrGetTexture( "Data/Images/Triplanar/stone_normalXZ.png" );
	Texture* normalStoneTexYZ = Texture::CreateOrGetTexture( "Data/Images/Triplanar/stone_normalYZ.png" );

	Matrix4x4 projectorViewMatrix;
	projectorViewMatrix.ClearToRotationMatrix_MyBasis( 0.f, 90.f, 0.f, COLUMN_MAJOR );

	static Sampler noRepeat;

	//Won't take literals, have to make initial temps.
	float lightIntensity = .25f;
	float specularExponent = 4.f;
	float specularIntensity = 4.f;
	float fogMinDist = 4.f;
	float fogMaxDist = 16.f;
	float timerDurationSeconds = 1.f;

	for ( auto& sp : *ShaderProgram::GetRegistry() )
	{
		sp.second->SetMatrix4x4( "uProjectorView", false, &projectorViewMatrix );

		sp.second->SetColor( "uColor", &Rgba::WHITE );
		sp.second->SetColor( "uFogColor", &Rgba::DARK_GRAY ); //Be sure it matches the screen clear color, else will fade to different color!
		sp.second->SetColor( "uAmbientLight", &Rgba::GRAY );
		sp.second->SetColor( "uLightColor", &Rgba::WHITE );

		sp.second->SetFloat( "uWrappingTimerDuration", &timerDurationSeconds );
		sp.second->SetFloat( "uLightIntensity", &lightIntensity );
		sp.second->SetFloat( "uSpecularExponent", &specularExponent );
		sp.second->SetFloat( "uSpecularIntensity", &specularIntensity );
		sp.second->SetFloat( "uFogMinDist", &fogMinDist );
		sp.second->SetFloat( "uFogMaxDist", &fogMaxDist );

		sp.second->SetInt( "uUseTime", &gUseTimeEffect );
		sp.second->SetInt( "uUseFog", &gUseFog );
		sp.second->SetInt( "uUseDiffuse", &gUseDiffuse );
		sp.second->SetInt( "uUseSpecular", &gUseSpecular );
		sp.second->SetInt( "uUseAmbientAndDOT3", &gUseAmbientAndDOT3 );
		sp.second->SetInt( "uUseEmissive", &gUseEmissive );
		sp.second->SetInt( "uUseFalloffForSpecular", &gUseFalloffForSpecular );
		sp.second->SetInt( "uUseFalloffForAmbientAndDOT3", &gUseFalloffForAmbientAndDOT3 );

		sp.second->SetTexture( "uTexDiffuseXY", diffuseStoneTexXY->GetTextureID() );
		sp.second->SetTexture( "uTexDiffuseXZ", diffuseStoneTexXZ->GetTextureID() );
		sp.second->SetTexture( "uTexDiffuseYZ", diffuseStoneTexYZ->GetTextureID() );
		sp.second->SetTexture( "uTexNormalXY", normalStoneTexXY->GetTextureID() );
		sp.second->SetTexture( "uTexNormalXZ", normalStoneTexXZ->GetTextureID() );
		sp.second->SetTexture( "uTexNormalYZ", normalStoneTexYZ->GetTextureID() );
		sp.second->SetTexture( "uTexSpecularXY", specularStoneTexXY->GetTextureID() );
		sp.second->SetTexture( "uTexSpecularXZ", specularStoneTexXZ->GetTextureID() );
		sp.second->SetTexture( "uTexSpecularYZ", specularStoneTexYZ->GetTextureID() );

		sp.second->SetTexture( "uDiffuseTex", texture->GetTextureID() );
		sp.second->SetTexture( "uTexDiffuse", diffuseStoneTex->GetTextureID() );
		sp.second->SetTexture( "uTexNormal", normalStoneTex->GetTextureID() );
		sp.second->SetTexture( "uTexSpecular", specularStoneTex->GetTextureID() );
		sp.second->SetTexture( "uTexDiffuse", diffuseStoneTex->GetTextureID() );
		sp.second->SetTexture( "uTexNormal", normalStoneTex->GetTextureID() );
		sp.second->SetTexture( "uTexSpecular", specularStoneTex->GetTextureID() );
		sp.second->SetTexture( "uTexEmissive", emissiveStoneTex->GetTextureID() );
		sp.second->SetTexture( "uTexAmbientOcclusion", ambientOcclusionStoneTex->GetTextureID() );
		sp.second->SetTexture( "uTexHeights", heightsStoneTex->GetTextureID() );
		sp.second->SetTexture( "uTexDissolve", dissolveTex->GetTextureID() );
		sp.second->SetTexture( "uTexProjected", projectedTex->GetTextureID() );

		sp.second->SetSampler( "uDiffuseTex", TheRenderer::DEFAULT_SAMPLER_ID );
		sp.second->SetSampler( "uTexDiffuse", TheRenderer::DEFAULT_SAMPLER_ID );
		sp.second->SetSampler( "uTexNormal", TheRenderer::DEFAULT_SAMPLER_ID );
		sp.second->SetSampler( "uTexSpecular", TheRenderer::DEFAULT_SAMPLER_ID );
		sp.second->SetSampler( "uTexEmissive", TheRenderer::DEFAULT_SAMPLER_ID );
		sp.second->SetSampler( "uTexAmbientOcclusion", TheRenderer::DEFAULT_SAMPLER_ID );
		sp.second->SetSampler( "uTexHeights", TheRenderer::DEFAULT_SAMPLER_ID );
		sp.second->SetSampler( "uTexDissolve", TheRenderer::DEFAULT_SAMPLER_ID );
		sp.second->SetSampler( "uTexProjected", noRepeat.GetSamplerID() );
		sp.second->SetSampler( "uTexDiffuseXY", TheRenderer::DEFAULT_SAMPLER_ID );
		sp.second->SetSampler( "uTexDiffuseXZ", TheRenderer::DEFAULT_SAMPLER_ID );
		sp.second->SetSampler( "uTexDiffuseYZ", TheRenderer::DEFAULT_SAMPLER_ID );
		sp.second->SetSampler( "uTexNormalXY", TheRenderer::DEFAULT_SAMPLER_ID );
		sp.second->SetSampler( "uTexNormalXZ", TheRenderer::DEFAULT_SAMPLER_ID );
		sp.second->SetSampler( "uTexNormalYZ", TheRenderer::DEFAULT_SAMPLER_ID );
		sp.second->SetSampler( "uTexSpecularXY", TheRenderer::DEFAULT_SAMPLER_ID );
		sp.second->SetSampler( "uTexSpecularXZ", TheRenderer::DEFAULT_SAMPLER_ID );
		sp.second->SetSampler( "uTexSpecularYZ", TheRenderer::DEFAULT_SAMPLER_ID );
	}
}
void BuildLights()
{
	float scale = 1.f;
	//	float sqrt2 = sqrt( 2.f );
	Light::CreateLight( POINT_LIGHT, Vector3( scale * 1.f, 0.f, 0.f ), WORLD_LEFT, Rgba( 1.0f, 1.0f, 1.0f ) ); //the 1 global point light.
	Light::CreateLight( DIRECTIONAL_LIGHT, Vector3( scale * 2.f, 0.f, 0.f ), WORLD_BACKWARD, Rgba( 0.0f, 1.0f, 0.0f ) ); //the 1 global direct light.

	//7 local point lights.	
	Light::CreateLight( POINT_LIGHT, Vector3( scale * 3.f, 0.f, 0.f ), WORLD_FORWARD, Rgba( 0.1f, 0.f, 0.f ), 0.f, 1.f, 1.f, 0.f );
	Light::CreateLight( POINT_LIGHT, Vector3( scale * 4.f, 0.f, 0.f ), WORLD_BACKWARD, Rgba( 0.2f, 0.f, 0.f ), 0.f, 1.f, 1.f, 0.f );
	Light::CreateLight( POINT_LIGHT, Vector3( scale * 5.f, 0.f, 0.f ), WORLD_LEFT, Rgba( 0.3f, 0.f, 0.f ), 0.f, 1.f, 1.f, 0.f );
	Light::CreateLight( POINT_LIGHT, Vector3( scale * 6.f, 0.f, 0.f ), WORLD_RIGHT, Rgba( 0.4f, 0.f, 0.f ), 0.f, 1.f, 1.f, 0.f );
	Light::CreateLight( POINT_LIGHT, Vector3( scale * 7.f, 0.f, 0.f ), WORLD_UP, Rgba( 0.5f, 0.f, 0.f ), 0.f, 1.f, 1.f, 0.f );
	Light::CreateLight( POINT_LIGHT, Vector3( scale * 8.f, 0.f, 0.f ), WORLD_DOWN, Rgba( 0.6f, 0.f, 0.f ), 0.f, 1.f, 1.f, 0.f );
	Light::CreateLight( POINT_LIGHT, Vector3( scale * 9.f, 0.f, 0.f ), WORLD_DOWN + WORLD_FORWARD, Rgba( 0.7f, 0.f, 0.f ), 0.f, 1.f, 1.f, 0.f );

	//7 local spot lights. IF THESE ARE 1 to -1 or 1 to sqrt(2) etc. for the cosines, light gets subtracted and the quad is black?
	Light::CreateLight( SPOT_LIGHT, Vector3( scale * 10.f, 0.f, 0.f ), WORLD_FORWARD, Rgba( 0.0f, 0.1f, 0.f ), 0.f, 2.f, 1.f, 0.f, 1.f, 0.f, 1.f, 0.f );
	Light::CreateLight( SPOT_LIGHT, Vector3( scale * 11.f, 0.f, 0.f ), WORLD_BACKWARD, Rgba( 0.0f, 0.2f, 0.f ), 0.f, 2.f, 1.f, 0.f, 1.f, 0.f, 1.f, 0.f );
	Light::CreateLight( SPOT_LIGHT, Vector3( scale * 12.f, 0.f, 0.f ), WORLD_LEFT, Rgba( 0.0f, 0.3f, 0.f ), 0.f, 2.f, 1.f, 0.f, 1.f, 0.f, 1.f, 0.f );
	Light::CreateLight( SPOT_LIGHT, Vector3( scale * 13.f, 0.f, 0.f ), WORLD_RIGHT, Rgba( 0.0f, 0.4f, 0.f ), 0.f, 2.f, 1.f, 0.f, 1.f, 0.f, 1.f, 0.f );
	Light::CreateLight( SPOT_LIGHT, Vector3( scale * 14.f, 0.f, 0.f ), WORLD_UP, Rgba( 0.0f, 0.5f, 0.f ), 0.f, 2.f, 1.f, 0.f, 1.f, 0.f, 1.f, 0.f );
	Light::CreateLight( SPOT_LIGHT, Vector3( scale * 15.f, 0.f, 0.f ), WORLD_DOWN, Rgba( 0.0f, 0.6f, 0.f ), 0.f, 2.f, 1.f, 0.f, 1.f, 0.f, 1.f, 0.f );
	Light::CreateLight( SPOT_LIGHT, Vector3( scale * 16.f, 0.f, 0.f ), WORLD_DOWN + WORLD_FORWARD, Rgba( 0.0f, 0.7f, 0.f ), 0.f, 2.f, 1.f, 0.f, 1.f, 0.f, 1.f, 0.f );
}
void BuildFBOs()
{
	//Make FBO's quad's mesh.
	AABB2 quadBounds; //Full-screen in NDC!
	quadBounds.mins = Vector2( -1.f, -1.f );
	quadBounds.maxs = Vector2( 1.f, 1.f );

	Vertex3D_PCUTB quad[ 4 ] = //Needs to be in normalized device coordinates, default OpenGL, not my basis.
	{
		Vertex3D_PCUTB( Vector3( quadBounds.mins.x, quadBounds.mins.y, 0.f ), Vector3( 1.f,0.f,0.f ), Vector3( 0.f,1.f,0.f ), Rgba::WHITE, Vector2( 1.f, 1.f ) ), //Top-left.		//0
		Vertex3D_PCUTB( Vector3( quadBounds.maxs.x, quadBounds.maxs.y, 0.f ), Vector3( 1.f,0.f,0.f ), Vector3( 0.f,1.f,0.f ), Rgba::WHITE, Vector2( 0.f, 0.f ) ), //Bottom-right.	//1
		Vertex3D_PCUTB( Vector3( quadBounds.maxs.x, quadBounds.mins.y, 0.f ), Vector3( 1.f,0.f,0.f ), Vector3( 0.f,1.f,0.f ), Rgba::WHITE, Vector2( 0.f, 1.f ) ), //Bottom-left.	//2
		Vertex3D_PCUTB( Vector3( quadBounds.mins.x, quadBounds.maxs.y, 0.f ), Vector3( 1.f,0.f,0.f ), Vector3( 0.f,1.f,0.f ), Rgba::WHITE, Vector2( 1.f, 0.f ) ) //Top-right.	//3
	};
	unsigned int quadIndices[] =
	{
		2, 1, 0, //Counter-Clockwise, else renders to the back and the quad won't show with backface culling.
		0, 1, 3
	};

	DrawInstruction quadDrawInstructions[] = { DrawInstruction( PRIMITIVE_TYPE_TRIANGLES, 0, 6, true ) };
	Mesh* quadMesh = new Mesh( BufferUsage::STATIC_DRAW, Vertex3D_PCUTB::DEFINITION, _countof( quad ), quad, _countof( quadIndices ), quadIndices, 1, quadDrawInstructions );

	//Make each shader into a material, one per effect.
	RenderState renderState = RenderState( CULL_MODE_BACK, BLEND_MODE_SOURCE_ALPHA, BLEND_MODE_ONE_MINUS_SOURCE_ALPHA, DEPTH_COMPARE_MODE_LESS, true );
	Material* quadMaterial;

	quadMaterial = Material::CreateOrGetMaterial( "QuadTBN_PostProcessGrayscale", &renderState, &Vertex3D_PCUTB::DEFINITION, "PostProcessGrayscale" );
	gPostProcessingMeshRenderers.push_back( new MeshRenderer( quadMesh, quadMaterial ) );

	quadMaterial = Material::CreateOrGetMaterial( "QuadTBN_PostProcessCMYK_C", &renderState, &Vertex3D_PCUTB::DEFINITION, "PostProcessCMYK_C" );
	gPostProcessingMeshRenderers.push_back( new MeshRenderer( quadMesh, quadMaterial ) );

	quadMaterial = Material::CreateOrGetMaterial( "QuadTBN_PostProcessCMYK_M", &renderState, &Vertex3D_PCUTB::DEFINITION, "PostProcessCMYK_M" );
	gPostProcessingMeshRenderers.push_back( new MeshRenderer( quadMesh, quadMaterial ) );

	quadMaterial = Material::CreateOrGetMaterial( "QuadTBN_PostProcessCMYK_Y", &renderState, &Vertex3D_PCUTB::DEFINITION, "PostProcessCMYK_Y" );
	gPostProcessingMeshRenderers.push_back( new MeshRenderer( quadMesh, quadMaterial ) );

	quadMaterial = Material::CreateOrGetMaterial( "QuadTBN_PostProcessCMYK_K", &renderState, &Vertex3D_PCUTB::DEFINITION, "PostProcessCMYK_K" );
	gPostProcessingMeshRenderers.push_back( new MeshRenderer( quadMesh, quadMaterial ) );

	quadMaterial = Material::CreateOrGetMaterial( "QuadTBN_PostProcessHueShift", &renderState, &Vertex3D_PCUTB::DEFINITION, "PostProcessHueShift" );
	gPostProcessingMeshRenderers.push_back( new MeshRenderer( quadMesh, quadMaterial ) );

	quadMaterial = Material::CreateOrGetMaterial( "QuadTBN_PostProcessBunnyBlur", &renderState, &Vertex3D_PCUTB::DEFINITION, "PostProcessBunnyBlur" );
	gPostProcessingMeshRenderers.push_back( new MeshRenderer( quadMesh, quadMaterial ) );

	quadMaterial = Material::CreateOrGetMaterial( "QuadTBN_PostProcessGaussianBlur", &renderState, &Vertex3D_PCUTB::DEFINITION, "PostProcessGaussianBlur" );
	gPostProcessingMeshRenderers.push_back( new MeshRenderer( quadMesh, quadMaterial ) );

	quadMaterial = Material::CreateOrGetMaterial( "QuadTBN_PostProcessChromakey", &renderState, &Vertex3D_PCUTB::DEFINITION, "PostProcessChromakey" );
	gPostProcessingMeshRenderers.push_back( new MeshRenderer( quadMesh, quadMaterial ) );

	quadMaterial = Material::CreateOrGetMaterial( "QuadTBN_PostProcessAnaglyph", &renderState, &Vertex3D_PCUTB::DEFINITION, "PostProcessAnaglyph" );
	gPostProcessingMeshRenderers.push_back( new MeshRenderer( quadMesh, quadMaterial ) );

	quadMaterial = Material::CreateOrGetMaterial( "QuadTBN_PostProcessEdgeDetectByLuminance", &renderState, &Vertex3D_PCUTB::DEFINITION, "PostProcessEdgeDetectByLuminance" );
	gPostProcessingMeshRenderers.push_back( new MeshRenderer( quadMesh, quadMaterial ) );

	quadMaterial = Material::CreateOrGetMaterial( "QuadTBN_PostProcessEmboss", &renderState, &Vertex3D_PCUTB::DEFINITION, "PostProcessEmboss" );
	gPostProcessingMeshRenderers.push_back( new MeshRenderer( quadMesh, quadMaterial ) );

	quadMaterial = Material::CreateOrGetMaterial( "QuadTBN_PostProcessToonShader", &renderState, &Vertex3D_PCUTB::DEFINITION, "PostProcessToonShader" );
	gPostProcessingMeshRenderers.push_back( new MeshRenderer( quadMesh, quadMaterial ) );

	quadMaterial = Material::CreateOrGetMaterial( "QuadTBN_PostProcessZoom", &renderState, &Vertex3D_PCUTB::DEFINITION, "PostProcessZoom" );
	gPostProcessingMeshRenderers.push_back( new MeshRenderer( quadMesh, quadMaterial ) );

	quadMaterial = Material::CreateOrGetMaterial( "QuadTBN_PostProcessTwirl", &renderState, &Vertex3D_PCUTB::DEFINITION, "PostProcessTwirl" );
	gPostProcessingMeshRenderers.push_back( new MeshRenderer( quadMesh, quadMaterial ) );

	quadMaterial = Material::CreateOrGetMaterial( "QuadTBN_PostProcessNegative", &renderState, &Vertex3D_PCUTB::DEFINITION, "PostProcessNegative" );
	gPostProcessingMeshRenderers.push_back( new MeshRenderer( quadMesh, quadMaterial ) );

	quadMaterial = Material::CreateOrGetMaterial( "QuadTBN_PostProcessBrightness", &renderState, &Vertex3D_PCUTB::DEFINITION, "PostProcessBrightness" );
	gPostProcessingMeshRenderers.push_back( new MeshRenderer( quadMesh, quadMaterial ) );

	quadMaterial = Material::CreateOrGetMaterial( "QuadTBN_PostProcessContrast", &renderState, &Vertex3D_PCUTB::DEFINITION, "PostProcessContrast" );
	gPostProcessingMeshRenderers.push_back( new MeshRenderer( quadMesh, quadMaterial ) );

	quadMaterial = Material::CreateOrGetMaterial( "QuadTBN_PostProcessSaturation", &renderState, &Vertex3D_PCUTB::DEFINITION, "PostProcessSaturation" );
	gPostProcessingMeshRenderers.push_back( new MeshRenderer( quadMesh, quadMaterial ) );

	quadMaterial = Material::CreateOrGetMaterial( "QuadTBN_PostProcessSharpness", &renderState, &Vertex3D_PCUTB::DEFINITION, "PostProcessSharpness" );
	gPostProcessingMeshRenderers.push_back( new MeshRenderer( quadMesh, quadMaterial ) );

	quadMaterial = Material::CreateOrGetMaterial( "QuadTBN_PostProcessColorMask", &renderState, &Vertex3D_PCUTB::DEFINITION, "PostProcessColorMask" );
	gPostProcessingMeshRenderers.push_back( new MeshRenderer( quadMesh, quadMaterial ) );

	quadMaterial = Material::CreateOrGetMaterial( "QuadTBN_PostProcessDepthOnly", &renderState, &Vertex3D_PCUTB::DEFINITION, "PostProcessDepthOnly" );
	gPostProcessingMeshRenderers.push_back( new MeshRenderer( quadMesh, quadMaterial ) );

	quadMaterial = Material::CreateOrGetMaterial( "QuadTBN_PostProcessEdgeDetectByDepthBuffer", &renderState, &Vertex3D_PCUTB::DEFINITION, "PostProcessEdgeDetectByDepthBuffer" );
	gPostProcessingMeshRenderers.push_back( new MeshRenderer( quadMesh, quadMaterial ) );

	quadMaterial = Material::CreateOrGetMaterial( "QuadTBN_PostProcessEdgeDetectQuantized", &renderState, &Vertex3D_PCUTB::DEFINITION, "PostProcessEdgeDetectQuantized" );
	gPostProcessingMeshRenderers.push_back( new MeshRenderer( quadMesh, quadMaterial ) );

	quadMaterial = Material::CreateOrGetMaterial( "QuadTBN_PostProcessObama", &renderState, &Vertex3D_PCUTB::DEFINITION, "PostProcessObama" );
	gPostProcessingMeshRenderers.push_back( new MeshRenderer( quadMesh, quadMaterial ) );

	quadMaterial = Material::CreateOrGetMaterial( "QuadTBN_PostProcessRipple", &renderState, &Vertex3D_PCUTB::DEFINITION, "PostProcessRipple" );
	gPostProcessingMeshRenderers.push_back( new MeshRenderer( quadMesh, quadMaterial ) );

	//Make FBO.
	std::vector<TextureFormat> colorFormats;
	colorFormats.push_back( TextureFormat::TEXTURE_FORMAT_Rgba8 );
	const TextureFormat depthStencilFormat = TextureFormat::TEXTURE_FORMAT_Depth24_Stencil8;

	gFBO = new FrameBuffer( static_cast<int>( g_theApp->GetScreenWidth() ), static_cast<int>( g_theApp->GetScreenHeight() ), colorFormats, &depthStencilFormat );

	//Configure (each?) FBO's render target(s) as the shader program's input uniforms.
	Vector2 anaglyphOffset = Vector2( .005f, 0.f );
	float quantizeLevel = 3.f;
	Texture* chromakeyBackgroundTex = Texture::CreateOrGetTexture( "Data/Images/dissolve.png" );
	for ( MeshRenderer* mr : gPostProcessingMeshRenderers )
	{
		mr->SetTexture( "uTexDiffuse", gFBO->GetColorTextureID( 0 ) );
		mr->SetSampler( "uTexDiffuse", TheRenderer::DEFAULT_SAMPLER_ID );
		mr->SetTexture( "uTexDiffuseChromakeyBG", chromakeyBackgroundTex->GetTextureID() );
		mr->SetSampler( "uTexDiffuseChromakeyBG", TheRenderer::DEFAULT_SAMPLER_ID );
		mr->SetTexture( "uTexDepth", gFBO->GetDepthStencilTextureID() );
		mr->SetSampler( "uTexDepth", TheRenderer::DEFAULT_SAMPLER_ID );
		mr->SetColor( "uColorMask", &Rgba::RED );
		mr->SetColor( "uColor", &Rgba::WHITE );
		mr->SetVector2( "uOffset", &anaglyphOffset );
		mr->SetFloat( "uQuantizationLevel", &quantizeLevel );

		//If you output more things to FBOs like an out normal map, add them here.

		//Deferred rendering: you write shaders outputting TBN, color, depth as five "g-buffers".

		//Then all lighting is calculated per-pixel using those 5 g[raphics]-buffers.
	}
}

void TheGame::Initialize()
{
	RegisterConsoleCommands();

	InitShaderPrograms();

	InitUniforms();

	BuildVBOs();

	BuildFBOs();

	BuildLights();
}

//-----------------------------------------------------------------------------//SD3 Rendering Initialize Functions//-----------------------------------------------------------------------------
void UpdateAndRenderPostProcess( FrameBuffer* fbo )
{
	UNUSED( fbo );

	Matrix4x4 model, view, proj;
	model.ClearToIdentityMatrix();
	view.ClearToIdentityMatrix();
	proj.ClearToIdentityMatrix();
	//Identity because fullscreen. If we want it on-world on a quad, can pass in the updated MVP or the view of a projector.
	//Have one cam in scene render to the texture, and another that projects it on things.
	//Or have a drone that moves the view around, and you're standing still.

	//Only need to update the one that's actually being shown now, not loop entire container of MeshRenderers for post-process.
	MeshRenderer* mr = gPostProcessingMeshRenderers[ gCurrentEffectIndex ];
	mr->SetMatrix4x4( "uModel", false, &model );
	mr->SetMatrix4x4( "uView", false, &view );
	mr->SetMatrix4x4( "uProj", false, &proj );
	mr->Render();

	//MOVE ABOVE FUNCTIONS ONTO THERENDERER?
}
void UpdateMVP()
{
	Matrix4x4 model = Matrix4x4( COLUMN_MAJOR );
	Matrix4x4 view = Matrix4x4( COLUMN_MAJOR );
	Matrix4x4 proj = Matrix4x4( COLUMN_MAJOR );

	const Camera3D* activeCam = g_theGame->GetActiveCamera();
	const Vector3& camPos = activeCam->m_worldPosition;
	static double aspect = g_theApp->GetScreenWidth() / g_theApp->GetScreenHeight();

	//-------------------MODEL-------------------//

	//model.ClearToTranslationMatrix( gInputTrans ); //Currently translates off because I need to rotate the [x,y,z]_Forseth basis to [x,z,y]_Squirrel basis.
	//model.ClearToRotationMatrix_MyBasis( 0.f, 0.f, 0.f, COLUMN_MAJOR );
	TODO( "Implement LookAt" );
	//model.ClearToLookAtMatrix( Vector3::ONE, camPos, Vector3(0,1,0), COLUMN_MAJOR ); //0,1,0 == y or j-hat is world-up until we do a change of basis.

	//-------------------VIEW-------------------//

	//Detour Change of Basis Theorem: convert to Forseth's basis, apply his rotation matrix transform, convert back.

	view.ClearToRotationMatrix_MyBasis( //takes this order: yaw, pitch, roll.
										-activeCam->m_orientation.m_yawDegrees,		//static test shows this was around world up j-vector as desired.
										-activeCam->m_orientation.m_pitchDegrees,	//static test shows this was around world right i-vector as desired.
										-activeCam->m_orientation.m_rollDegrees,		//so this is around k.
										view.GetOrdering() ); //Note these and all transforms are the same order as TheGame::ApplyCamTransform, but there it's pushed on a stack, hence "reversed".
	view.SetTranslation( camPos, view.GetOrdering() );

	//Rather than using the view relative to the world, we want the world relative to the camera.	
	view.GetInverseAssumingOrthonormality( view );


	//-------------------PROJ-------------------//

	if ( gUseOrtho )
		proj.ClearToOrthogonalProjection( 2.f*static_cast<float>( aspect ), 2.f, -1.f, 1.f, proj.GetOrdering() );
	else
		proj.ClearToPerspectiveProjection( GetWorldChangeOfBasis( COLUMN_MAJOR ), 60.0f, static_cast<float>( aspect ), .5f, 100.f, proj.GetOrdering() );


	//-------------------UNIFORMS-------------------//

	TODO( "Use a uniform block over all shaders, for the MVP matrix uniforms." );

	for ( auto& sp : *ShaderProgram::GetRegistry() )
	{
		sp.second->SetMatrix4x4( "uModel", false, &model );
		sp.second->SetMatrix4x4( "uView", false, &view );
		sp.second->SetMatrix4x4( "uProj", false, &proj );
	}
}
void UpdateLights()
{
	//Parallel array setup.
	static Vector3* uLightPositions = new Vector3[ MAX_LIGHTS ];
	static Vector3* uLightDirections = new Vector3[ MAX_LIGHTS ];
	static Rgba* uLightColors = new Rgba[ MAX_LIGHTS ];

	static int* uLightIsDirectionalFlags = new int[ MAX_LIGHTS ];

	static float* uLightMinDistances = new float[ MAX_LIGHTS ];
	static float* uLightMaxDistances = new float[ MAX_LIGHTS ];
	static float* uLightPowersAtMinDists = new float[ MAX_LIGHTS ];
	static float* uLightPowersAtMaxDists = new float[ MAX_LIGHTS ];
	static float* uLightInnerAngleCosines = new float[ MAX_LIGHTS ];
	static float* uLightOuterAngleCosines = new float[ MAX_LIGHTS ];
	static float* uLightPowersInsideInnerAngles = new float[ MAX_LIGHTS ];
	static float* uLightPowersOutsideOuterAngles = new float[ MAX_LIGHTS ];

	Light::GetLightPositions( uLightPositions );
	Light::GetLightDirections( uLightDirections );
	Light::GetLightColors( uLightColors );

	Light::GetLightIsDirectionalFlags( uLightIsDirectionalFlags );

	Light::GetLightMinDistances( uLightMinDistances );
	Light::GetLightMaxDistances( uLightMaxDistances );
	Light::GetLightPowersAtMinDists( uLightPowersAtMinDists );
	Light::GetLightPowersAtMaxDists( uLightPowersAtMaxDists );
	Light::GetLightInnerAngleCosines( uLightInnerAngleCosines );
	Light::GetLightOuterAngleCosines( uLightOuterAngleCosines );
	Light::GetLightPowersInsideInnerAngles( uLightPowersInsideInnerAngles );
	Light::GetLightPowersOutsideOuterAngles( uLightPowersOutsideOuterAngles );

	//Set uniforms for any multi-light shaders. Right now we know only BlinnPhongTBN shader uses them.
	auto& sp = *ShaderProgram::CreateOrGetShaderProgram( "BlinnPhongTBN" ); //for ( auto& sp : ShaderProgram::GetRegistry() )
	{
		sp.SetVector3( "uLightPositions[0]", uLightPositions, 16 ); //Should it be *uLightPositions, since the function uses &val?
		sp.SetVector3( "uLightDirections[0]", uLightDirections, 16 );
		sp.SetColor( "uLightColors[0]", uLightColors, 16 );

		sp.SetFloat( "uLightMinDistances[0]", uLightMinDistances, 16 );
		sp.SetFloat( "uLightMaxDistances[0]", uLightMaxDistances, 16 );

		sp.SetFloat( "uLightPowersAtMinDists[0]", uLightPowersAtMinDists, 16 );
		sp.SetFloat( "uLightPowersAtMaxDists[0]", uLightPowersAtMaxDists, 16 );

		sp.SetFloat( "uLightInnerAngleCosines[0]", uLightInnerAngleCosines, 16 );
		sp.SetFloat( "uLightOuterAngleCosines[0]", uLightOuterAngleCosines, 16 );

		sp.SetFloat( "uLightPowersInsideInnerAngles[0]", uLightPowersInsideInnerAngles, 16 );
		sp.SetFloat( "uLightPowersOutsideOuterAngles[0]", uLightPowersOutsideOuterAngles, 16 );

		sp.SetInt( "uLightIsDirectionalFlags[0]", uLightIsDirectionalFlags, 16 );
	}
}
void RenderLight( const Vector3& position, float radius, const Rgba& tint)
{
	static Material* lightMaterial = Material::CreateOrGetMaterial( "LightMaterial", &gRenderState, &Vertex3D_PCUTB::DEFINITION, "Passthrough" );
	static MeshRenderer lightMeshRenderer = MeshRenderer( BuildSphereMeshTBN( Vector3::ZERO, radius, 50.f, 50.f ), lightMaterial );

	//uView is set at end of UpdateMVP already.

	lightMeshRenderer.SetColor( "uColor", &tint );

	Matrix4x4 newModelMatrix( COLUMN_MAJOR );
	newModelMatrix.ClearToTranslationMatrix( position, COLUMN_MAJOR );
	lightMeshRenderer.SetMatrix4x4( "uModel", false, &newModelMatrix );

	lightMeshRenderer.Render();
	//g_theDebugRenderCommands->push_back( new DebugRenderCommandSphere( position, radius, 0.f, DEPTH_TEST_ON, tint, 1.f ) ); //REPLACE? Or make them use above style?
}
void TheGame::Render()
{
	SetupView3D();
	Render3D();
	if ( g_renderDebugInfo ) RenderDebug3D();

//	g_theRenderer->EnableDepthTesting( true );

//	g_theRenderer->DrawTextProportional3D( Vector3::ONE, "Hello World", Vector3( 0, 1, 0 ), Vector3( 1, 0, 0 ), .1f, nullptr, Rgba(), false );

	SetupView2D();
	Render2D();
	if ( g_renderDebugInfo ) RenderDebug2D();


	//WARNING: currently renders on top of 2D!
	if ( gIsShaderWorldVisible ) //Corresponds to ShowShader dev console command.
	{
		UpdateMVP(); TODO( "Move into TheEngine::Render?" );
		UpdateLights();	TODO( "Move into Renderer" );

		TODO( "Move into Renderer, or make static singletons + functions on their classes." );
		for ( MeshRenderer* mr : gMeshRenderers )
			mr->Render();

		for ( Light* light : *Light::GetRegistry() )
			if ( light->GetIsLightOn() )
				RenderLight( light->GetPosition(), .1f, light->GetColor() );

		TODO( "Move into Renderer" );
		ShaderProgram::UnbindAnyPrograms();
	}

	g_theRenderer->DrawSphereGimbal( g_theRenderer->GetOpenGLVertexGroupingRule( TheRenderer::VertexGroupingRule::AS_TRIANGLES ), uLightPosition, 0.f, 10.f );
	AddDebugDebugRenderCommand( new DebugRenderCommandSphere( uLightPosition, .1f, 0.f, DEPTH_TEST_DUAL, Rgba::WHITE, 1.f ) );


	if ( gEnableCustomFBOs )
	{
		g_theRenderer->BindFBO( nullptr );
		UpdateAndRenderPostProcess( gFBO );
	}
}
#pragma endregion
