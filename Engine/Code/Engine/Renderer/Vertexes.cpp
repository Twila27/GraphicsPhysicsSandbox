#include "Engine/Renderer/Vertexes.hpp"


//--------------------------------------------------------------------------------------------------------------
const VertexAttribute Vertex3D_PCT::ATTRIBUTES[] =
{
	VertexAttribute( "inPosition", 3, VERTEX_FIELD_TYPE_FLOAT, false, offsetof( Vertex3D_PCT, m_position ) ),
	VertexAttribute( "inColor", 4, VERTEX_FIELD_TYPE_UNSIGNED_BYTE, true, offsetof( Vertex3D_PCT, m_color ) ),
	VertexAttribute( "inUV0", 2, VERTEX_FIELD_TYPE_FLOAT, false, offsetof( Vertex3D_PCT, m_texCoords ) )
};
const VertexDefinition Vertex3D_PCT::DEFINITION = VertexDefinition( sizeof( Vertex3D_PCT ), 3, ATTRIBUTES ); //Must be below attribute definition.


//--------------------------------------------------------------------------------------------------------------
const VertexAttribute Vertex3D_PCUTB::ATTRIBUTES[] =
{
	VertexAttribute( "inPosition", 3, VERTEX_FIELD_TYPE_FLOAT, false, offsetof( Vertex3D_PCUTB, m_position ) ),
	VertexAttribute( "inColor", 4, VERTEX_FIELD_TYPE_UNSIGNED_BYTE, true, offsetof( Vertex3D_PCUTB, m_color ) ),
	VertexAttribute( "inUV0", 2, VERTEX_FIELD_TYPE_FLOAT, false, offsetof( Vertex3D_PCUTB, m_texCoords ) ),
	VertexAttribute( "inTangent", 3, VERTEX_FIELD_TYPE_FLOAT, false, offsetof( Vertex3D_PCUTB, m_tangent ) ),
	VertexAttribute( "inBitangent", 3, VERTEX_FIELD_TYPE_FLOAT, false, offsetof( Vertex3D_PCUTB, m_bitangent ) )
};
const VertexDefinition Vertex3D_PCUTB::DEFINITION = VertexDefinition( sizeof( Vertex3D_PCUTB ), 5, ATTRIBUTES ); //Must be below attribute definition.


//--------------------------------------------------------------------------------------------------------------
Vertex3D_PCT::Vertex3D_PCT( const Vector3& position, const Vector2& texCoords /*= Vector2::ZERO*/, const Rgba& color /*= Rgba::WHITE */ )
	: m_position( position )
	, m_color( color )
	, m_texCoords( texCoords )
{
}


//--------------------------------------------------------------------------------------------------------------
Vertex3D_PCT::Vertex3D_PCT( const Vector3& position, const Rgba& color /*= Rgba::WHITE */, const Vector2& texCoords /*= Vector2::ZERO*/ )
	: m_position( position )
	, m_color( color )
	, m_texCoords( texCoords )
{
}


//--------------------------------------------------------------------------------------------------------------
Vertex3D_PCT::Vertex3D_PCT()
	: m_position()
	, m_color()
	, m_texCoords()
{
}


//--------------------------------------------------------------------------------------------------------------
Vertex3D_PCUTB::Vertex3D_PCUTB( const Vector3& position, const Vector3& tangent, const Vector3& bitangent, 
								const Vector2& texCoords /*= Vector2::ZERO*/, const Rgba& color /*= Rgba::WHITE */ )
	: m_position( position )
	, m_color( color )
	, m_texCoords( texCoords )
	, m_tangent( tangent )
	, m_bitangent( bitangent )
{
}


//--------------------------------------------------------------------------------------------------------------
Vertex3D_PCUTB::Vertex3D_PCUTB( const Vector3& position, const Vector3& tangent, const Vector3& bitangent, 
								const Rgba& color /*= Rgba::WHITE */, const Vector2& texCoords /*= Vector2::ZERO*/ )
	: m_position( position )
	, m_color( color )
	, m_texCoords( texCoords )
	, m_tangent( tangent )
	, m_bitangent( bitangent )
{
}

//--------------------------------------------------------------------------------------------------------------
Vertex3D_PCUTB::Vertex3D_PCUTB()
	: m_position()
	, m_color()
	, m_texCoords()
	, m_tangent()
	, m_bitangent()
{
}