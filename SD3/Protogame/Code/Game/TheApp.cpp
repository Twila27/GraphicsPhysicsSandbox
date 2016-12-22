#include "Game/TheApp.hpp"


#include "Engine/Math/Vector2.hpp"


//---------------------------------------------------------------------
TheApp* g_theApp = nullptr;


//---------------------------------------------------------------------
TheApp::TheApp( const double screenWidth, const double screenHeight )
	: m_screenWidth( screenWidth )
	, m_screenHeight( screenHeight )
{
}


//---------------------------------------------------------------------
Vector2 TheApp::GetScreenCenter() const
{
	Vector2 screenCenter;

	screenCenter.x = static_cast<float>( m_screenWidth / 2.0 );
	screenCenter.y = static_cast<float>( m_screenHeight / 2.0 );

	return screenCenter;
}
