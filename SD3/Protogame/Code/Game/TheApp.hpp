#pragma once


//-----------------------------------------------------------------------------
class TheApp;
class Vector2;


//-----------------------------------------------------------------------------
extern TheApp* g_theApp;


//-----------------------------------------------------------------------------
class TheApp
{

public:

	TheApp( const double screenWidth, const double screenHeight );
	double GetScreenWidth() const { return m_screenWidth; }
	double GetScreenHeight() const { return m_screenHeight; }
	Vector2 GetScreenCenter() const;


private:

	double m_screenWidth;
	double m_screenHeight;
};
