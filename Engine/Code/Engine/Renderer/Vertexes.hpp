#pragma once


#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Renderer/Rgba.hpp"
#include "Engine/Renderer/VertexDefinition.hpp"


struct Vertex3D_PCT
{
public:

	Vector3 m_position;
	Rgba m_color;
	Vector2 m_texCoords;

	Vertex3D_PCT();
	Vertex3D_PCT( const Vector3& position, const Vector2& texCoords = Vector2::ZERO, const Rgba& color = Rgba::WHITE );
	Vertex3D_PCT( const Vector3& position, const Rgba& color = Rgba::WHITE, const Vector2& texCoords = Vector2::ZERO );

	static const VertexDefinition DEFINITION;


private:

	static const VertexAttribute ATTRIBUTES[];
};


struct Vertex3D_PCUTB
{
public:

	Vector3 m_position;
	Rgba m_color;
	Vector2 m_texCoords;
	Vector3 m_tangent; //Along u-direction (1,0), think derivative/rate of change of texture coordinates over mesh surface.
	Vector3 m_bitangent; //Along v-direction (0,1). But in OpenGL due to the flip this is really -v. Just vertical in general.

	Vertex3D_PCUTB();
	Vertex3D_PCUTB( const Vector3& position, const Vector3& tangent, const Vector3& bitangent, const Vector2& texCoords = Vector2::ZERO, const Rgba& color = Rgba::WHITE );
	Vertex3D_PCUTB( const Vector3& position, const Vector3& tangent, const Vector3& bitangent, const Rgba& color = Rgba::WHITE, const Vector2& texCoords = Vector2::ZERO );

	static const VertexDefinition DEFINITION;


private:

	static const VertexAttribute ATTRIBUTES[];
};