#include "Engine/Renderer/Mesh.hpp"


#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <gl/gl.h>
#include "Engine/Renderer/OpenGLExtensions.hpp"

#include "Engine/Renderer/VertexDefinition.hpp"


//--------------------------------------------------------------------------------------------------------------
Mesh::Mesh( BufferUsage usage, const VertexDefinition& vertexDefinition,
			unsigned int numVertices, void* vertexData,
			unsigned int numDrawInstructions, DrawInstruction* drawInstructions )
	: m_vertices( new VertexBuffer( numVertices, vertexDefinition.GetVertexSize(), usage, vertexData ) )
	, m_vertexDefinition( &vertexDefinition )
	, m_usingIndexBuffer( false )
	, m_indices( nullptr )
{
	m_drawCommands.reserve( numDrawInstructions );
	for ( unsigned int instructionIndex = 0; instructionIndex < numDrawInstructions; instructionIndex++ )
		m_drawCommands.push_back( drawInstructions[ instructionIndex ] );
}


//--------------------------------------------------------------------------------------------------------------
Mesh::Mesh( BufferUsage usage, const VertexDefinition& vertexDefinition,
			unsigned int numVertices, void* vertexData, 
			unsigned int numIndices, void* indexData,
			unsigned int numDrawInstructions, DrawInstruction* drawInstructions )
	: Mesh( usage, vertexDefinition, numVertices, vertexData, numDrawInstructions, drawInstructions )
{
	m_indices = new IndexBuffer( numIndices, sizeof( unsigned int ), BufferUsage::STATIC_DRAW, indexData );
	m_usingIndexBuffer = true;
}


//--------------------------------------------------------------------------------------------------------------
void Mesh::AddDrawInstruction( PrimitiveType prim, unsigned int startIndex, unsigned int count, bool useIndexBuffer )
{
	m_drawCommands.push_back( DrawInstruction( prim, startIndex, count, useIndexBuffer ) );
}
