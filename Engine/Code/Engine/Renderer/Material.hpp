#pragma once

#include <string>
#include <map>
#include "Engine/Renderer/VertexDefinition.hpp"
#include "Engine/Renderer/RenderState.hpp"


class ShaderProgram;
class Mesh;
class Material;
class Vector2;
class Vector3;
class Vector4;
class Matrix4x4;
struct Rgba;
class Sampler;
class Texture;


/*
struct MaterialPass
{
MaterialPass() {};
MaterialPass( ShaderProgram* shaderProgram, RenderState* renderState ) : m_shaderProgram( shaderProgram ), m_renderState( renderState ) {}

ShaderProgram* m_shaderProgram;
RenderState* m_renderState;
}
*/


class Material //Has the uniforms, not VBO/IBOs. The "How" to render without the "What" (mesh) to render.
{
public:

	static Material* CreateOrGetMaterial( const std::string& materialName, const RenderState* renderState = nullptr, const VertexDefinition* vertexDefinition = nullptr, 
									 const char* shaderProgramName = nullptr, const char* vertexShaderFilePath = nullptr, const char* fragmentShaderFilePath = nullptr );

	//Material( const VertexDefinition& vertexDefinition, unsigned int passCount, std::vector<MaterialPass> passes );

	const std::string& GetName() const { return m_materialName; }
	void Bind();
	void Unbind();
	bool BindInputAttribute( const std::string& attributeNameVerbatim, unsigned int count, VertexFieldType engineFieldType, bool normalize, int strideInBytes, int offsetInBytes );
	
	const VertexDefinition* GetVertexDefinition() const { return m_vertexDefinition; }

	//Just calls the ShaderProgram equivalents.
	void SetInt( const std::string& uniformNameVerbatim, int* newValue, unsigned int arraySize = 1 );
	void SetFloat( const std::string& uniformNameVerbatim, float* newValue, unsigned int arraySize = 1 );
	void SetVector2( const std::string& uniformNameVerbatim, const Vector2* newValue, unsigned int arraySize = 1 );
	void SetVector3( const std::string& uniformNameVerbatim, const Vector3* newValue, unsigned int arraySize = 1 );
	void SetVector4( const std::string& uniformNameVerbatim, const Vector4* newValue, unsigned int arraySize = 1 );
	void SetMatrix4x4( const std::string& uniformNameVerbatim, bool shouldTranspose, const Matrix4x4* newValue, unsigned int arraySize = 1 );
	void SetColor( const std::string& uniformNameVerbatim, const Rgba* newValue, unsigned int arraySize = 1 );
	void SetSampler( const std::string& uniformNameVerbatim, unsigned int newSamplerID );
	void SetTexture( const std::string& uniformNameVerbatim, unsigned int newTextureID );


private:

	Material( const std::string& materialName, const RenderState& renderState, const VertexDefinition& vertexDefinition, ShaderProgram* shaderProgramName );
	static std::map< std::string, Material* > s_materialRegistry;

	const std::string m_materialName;
	const VertexDefinition* m_vertexDefinition;

	ShaderProgram* m_shaderProgram;
	RenderState m_renderState;
	//unsigned int m_passCount;
	//std::vector<MaterialPass> m_passes;
};
