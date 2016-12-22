#include "Engine/Renderer/MeshRenderer.hpp"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <gl/gl.h>
#include "Engine/Renderer/OpenGLExtensions.hpp"

#include "Engine/Renderer/Mesh.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/Vertexes.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector4.hpp"
#include "Engine/Math/Matrix4x4.hpp"
#include "Engine/Renderer/Rgba.hpp"
#include "Engine/Renderer/Sampler.hpp"
#include "Engine/Renderer/Texture.hpp"


//--------------------------------------------------------------------------------------------------------------
MeshRenderer::MeshRenderer( Mesh* mesh, Material* material )
{
	glGenVertexArrays( 1, &m_vaoID );
	ASSERT_OR_DIE( m_vaoID != NULL, "glGenVertexArrays failed in MeshRenderer()" );

	SetMesh( mesh );
	SetMaterial( material );
}


//--------------------------------------------------------------------------------------------------------------
MeshRenderer::~MeshRenderer()
{
	if ( m_mesh != nullptr )
		delete m_mesh;

	glDeleteVertexArrays( 1, &m_vaoID );
}


//--------------------------------------------------------------------------------------------------------------
const std::string& MeshRenderer::GetMaterialName() const
{
	return m_material->GetName();
}


//--------------------------------------------------------------------------------------------------------------
void MeshRenderer::SetMesh( Mesh* mesh )
{
	glBindVertexArray( m_vaoID );
	glBindBuffer( GL_ARRAY_BUFFER, mesh->GetVertexBufferID() );

	if ( mesh->GetIndexBufferID() != NULL )
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mesh->GetIndexBufferID() );

	glBindVertexArray( NULL ); //Must come first, but why?

	glBindBuffer( GL_ARRAY_BUFFER, NULL );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, NULL ); //Why does this cause a crash if before VAO unbind?

	m_mesh = mesh;
}


//--------------------------------------------------------------------------------------------------------------
void MeshRenderer::SetMaterial( Material* material )
{
	if ( m_mesh == nullptr ) return;

	glBindVertexArray( m_vaoID );
	glBindBuffer( GL_ARRAY_BUFFER, m_mesh->GetVertexBufferID() );
	

	const VertexDefinition* vertexDefinition = material->GetVertexDefinition();
	for ( unsigned int attributeIndex = 0; attributeIndex < vertexDefinition->GetNumAttributes(); attributeIndex++ )
	{
		const VertexAttribute* attr = vertexDefinition->GetAttributeAtIndex( attributeIndex );

		TODO( "Add a function to return correct sizes for different Vertex types, but how to tell which Vertexes' type a definition corresponds to?" );
		material->BindInputAttribute( attr->m_attributeName.c_str(), attr->m_count, attr->m_fieldType,
							attr->m_normalized, vertexDefinition->GetVertexSize(), attr->m_offset );
	}

	glBindVertexArray( NULL ); //Must come first, but why?

	glBindBuffer( GL_ARRAY_BUFFER, NULL );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, NULL ); //Why does this cause a crash if before VAO unbind?

	m_material = material;
}


//--------------------------------------------------------------------------------------------------------------
void MeshRenderer::Render()
{
	Render( m_mesh, m_material );
}


//--------------------------------------------------------------------------------------------------------------
void MeshRenderer::Render( Mesh* mesh, Material* material )
{
	//Ideal: loop over all the instructions with the mesh, draw elements from startIndex to endIndex.


	glBindVertexArray( m_vaoID );

	material->Bind();

	const std::vector<DrawInstruction>& drawInstructions = mesh->GetDrawInstructions();
	for ( unsigned int instructionIndex = 0; instructionIndex < drawInstructions.size(); instructionIndex++ )
	{
		const DrawInstruction& currentInstruction = drawInstructions[ instructionIndex ];

		TODO( "Synthesize functionality with TheRenderer enum!" );
		unsigned int vertexGroupingRule = 0;
		switch ( currentInstruction.m_type )
		{
		case PRIMITIVE_TYPE_POINT:		vertexGroupingRule = GL_POINTS;  break;
		case PRIMITIVE_TYPE_LINE:		vertexGroupingRule = GL_LINES; break;
		case PRIMITIVE_TYPE_TRIANGLES:	vertexGroupingRule = GL_TRIANGLES; break;
		}

		if ( m_mesh->UsesIndexBuffer() )
			glDrawElements( GL_TRIANGLES, currentInstruction.m_count, GL_UNSIGNED_INT, (GLvoid*)currentInstruction.m_startIndex ); //Vertex grouping rule, # indices, uint, &loc[0] or 0 for bound.
		else
			glDrawArrays( GL_TRIANGLES, currentInstruction.m_startIndex, currentInstruction.m_count ); //Vertex grouping rule, start index into bound array, # vertexes to include.
	}

	material->Unbind();

	glBindVertexArray( NULL );
}


//--------------------------------------------------------------------------------------------------------------
void MeshRenderer::SetInt( const std::string& uniformNameVerbatim, int* newValue )
{
	m_material->SetInt( uniformNameVerbatim, newValue );
}


//--------------------------------------------------------------------------------------------------------------
void MeshRenderer::SetFloat( const std::string& uniformNameVerbatim, float* newValue )
{
	m_material->SetFloat( uniformNameVerbatim, newValue );
}


//--------------------------------------------------------------------------------------------------------------
void MeshRenderer::SetVector2( const std::string& uniformNameVerbatim, const Vector2* newValue )
{
	m_material->SetVector2( uniformNameVerbatim, newValue );
}


//--------------------------------------------------------------------------------------------------------------
void MeshRenderer::SetVector3( const std::string& uniformNameVerbatim, const Vector3* newValue )
{
	m_material->SetVector3( uniformNameVerbatim, newValue );
}


//--------------------------------------------------------------------------------------------------------------
void MeshRenderer::SetVector4( const std::string& uniformNameVerbatim, const Vector4* newValue )
{
	m_material->SetVector4( uniformNameVerbatim, newValue );
}


//--------------------------------------------------------------------------------------------------------------
void MeshRenderer::SetMatrix4x4( const std::string& uniformNameVerbatim, bool shouldTranspose, const Matrix4x4* newValue )
{
	m_material->SetMatrix4x4( uniformNameVerbatim, shouldTranspose, newValue );
}


//--------------------------------------------------------------------------------------------------------------
void MeshRenderer::SetColor( const std::string& uniformNameVerbatim, const Rgba* newValue )
{
	m_material->SetColor( uniformNameVerbatim, newValue );
}


//--------------------------------------------------------------------------------------------------------------
void MeshRenderer::SetSampler( const std::string& uniformNameVerbatim, unsigned int newSamplerID )
{
	m_material->SetSampler( uniformNameVerbatim, newSamplerID );
}


//--------------------------------------------------------------------------------------------------------------
void MeshRenderer::SetTexture( const std::string& uniformNameVerbatim, unsigned int newTextureID )
{
	m_material->SetTexture( uniformNameVerbatim, newTextureID );
}
