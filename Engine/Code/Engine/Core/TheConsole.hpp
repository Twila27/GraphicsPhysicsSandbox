#pragma once

#include <map>
#include <vector>
#include <utility>
#include <string.h>
#include "Engine/Renderer/Rgba.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Renderer/TheRenderer.hpp"


//--------------------------------------------------------------------------------------------------------------
class TheConsole;
class BitmapFont;
class Command;


//--------------------------------------------------------------------------------------------------------------
extern TheConsole* g_theConsole;


//--------------------------------------------------------------------------------------------------------------
typedef void( *ConsoleCommandCallback )( Command& );
static std::map < std::string, ConsoleCommandCallback > s_theConsoleCommands; //Only exists in this .cpp unit, hence below wrapper.


//--------------------------------------------------------------------------------------------------------------
//Command implementations. Register by calling g_theConsole->RegisterCommand().
void ShowHelp( Command& args );
void ClearLog( Command& args );
void CloseConsole( Command& args );
void SetColor( Command& args );
void FindString( Command& args );


//--------------------------------------------------------------------------------------------------------------
class TheConsole
{
public:

	TheConsole( double screenWidth, double screenHeight, const Rgba& textColor = Rgba(), bool isVisible = false, float textScale = .25f,
				double maxConsoleHeightCoverageNormalized = 0.3, BitmapFont* font = g_theRenderer->GetDefaultFont()  )
		: m_currentColor( textColor )
		, m_isVisible( isVisible )
		, m_currentScale( textScale )
		, m_screenWidth( screenWidth )
		, m_screenHeight( screenHeight )
		, m_maxConsoleHeight( screenHeight * maxConsoleHeightCoverageNormalized )
		, m_currentFont( font )
		, m_hasFontChanged( false )
		, m_caretAlphaCounter( 0.f )
		, m_replacerPos( 0 )
		, m_caretPosInInputString( 0 )
		, m_newestStoredTextIndexToRender( 0 )
		, m_storedLinesToShowCount( 0 )
		, m_shouldPulseSearchResult( false )
		, m_searchResultToPulse( "" )
	{
		RegisterCommand( "Help", ShowHelp );
		RegisterCommand( "Clear", ClearLog );
		RegisterCommand( "Clear2", ClearLog );
		RegisterCommand( "Close", CloseConsole );
		RegisterCommand( "SetConsoleColor", SetColor );
		RegisterCommand( "Find", FindString );
	}
	void RegisterCommand( const std::string& name, ConsoleCommandCallback cb );
	void RunCommand( const std::string& fullCommandString );
	void Printf( const char* format, ... ); //Trigger this on VK_ENTER from command prompt.
	void SetTextColor( const Rgba& newColor ) { m_currentColor = newColor; }
	void SetFont( BitmapFont* newFont ) { m_currentFont = newFont; m_hasFontChanged = true; }
	void Show() { m_isVisible = true; }
	void Hide() { m_isVisible = false; }
	bool IsVisible() const { return m_isVisible; }
	void Render();
	void Update( float deltaSeconds );
	void UpdatePromptForChar( unsigned char ch );
	void UpdatePromptForKeydown( unsigned char ch );
	void ClearConsoleLog() { m_storedText.clear(); m_replacerPos = 0; }
	void ShouldPulseSearchResults( bool newVal, const std::string& term );
	void AttemptAutocomplete( const std::map< std::string, ConsoleCommandCallback >::const_iterator* indexOfLastResult = nullptr );

private:

	std::vector< std::pair< std::string, Rgba > > m_storedText;

	Vector2 m_currentLogBoxTopLeft;
	Vector2 m_currentPromptBoxTopLeft;

	double m_screenWidth;
	double m_screenHeight;
	double m_maxConsoleHeight;


	std::string m_currentPromptString;
	BitmapFont* m_currentFont;
	bool m_hasFontChanged;
	Rgba m_currentColor;
	float m_currentScale;
	bool m_isVisible;
	bool m_shouldPulseSearchResult;
	std::string m_searchResultToPulse;
	std::string m_lastAutocompleteInput;

	float m_caretAlphaCounter;
	int m_replacerPos; //i.e. which line gets added to the command prompt if you hit up or down next.
	int m_caretPosInInputString;
	int m_newestStoredTextIndexToRender; //i.e. bottom-most.
	unsigned int m_storedLinesToShowCount;
};