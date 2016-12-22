#pragma once
#include <string>


class Mesh;
class Material;
class Vector2;
class Vector3;
class Vector4;
class Matrix4x4;
struct Rgba;
class Sampler;
class Texture;


class MeshRenderer
{
public:
	MeshRenderer() {}
	MeshRenderer( Mesh* mesh, Material* material );
	~MeshRenderer();

	const std::string& GetMaterialName() const;

	void SetMesh( Mesh* mesh );
	void SetMaterial( Material* material );
	void SetMeshAndMaterial( Mesh* mesh, Material* material ) { SetMesh(mesh); SetMaterial(material); }

	void Render();
	void Render( Mesh* mesh, Material* material );

	//Just calls the Material equivalents.
	void SetInt( const std::string& uniformNameVerbatim, int* newValue );
	void SetFloat( const std::string& uniformNameVerbatim, float* newValue );
	void SetVector2( const std::string& uniformNameVerbatim, const Vector2* newValue );
	void SetVector3( const std::string& uniformNameVerbatim, const Vector3* newValue );
	void SetVector4( const std::string& uniformNameVerbatim, const Vector4* newValue );
	void SetMatrix4x4( const std::string& uniformNameVerbatim, bool shouldTranspose, const Matrix4x4* newValue );
	void SetColor( const std::string& uniformNameVerbatim, const Rgba* newValue );
	void SetSampler( const std::string& uniformNameVerbatim, unsigned int newSamplerID );
	void SetTexture( const std::string& uniformNameVerbatim, unsigned int newTextureID );


private:

	unsigned int m_vaoID;
	Mesh* m_mesh;
	Material* m_material;
};
