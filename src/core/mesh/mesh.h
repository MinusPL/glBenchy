#ifndef MESH_H
#define MESH_H

#include "../../platform/platform.h"
#include "../../handmademath/math.h"

#include <vector>

//INFO: This is implementation for OpenGL. In other circumstances there would be additional abstraction layer needed for multiple rendering backends.
struct Surface
{
	GLenum vertexFlag = GL_TRIANGLES;

	std::vector<UVec3> vertices;

	std::vector<UVec3> colors;

	std::vector<GLuint> indices;

	std::vector<UVec3> normals;

	std::vector<UVec2> uvs;

	std::vector<UVec3> tangents;

	GLuint VBO, EBO, VAO;
	GLuint VBOClr, VBONorm;
	GLuint VBOTangent;
	GLuint VBOuv;

	inline UVec3 NormalFromIndices(GLuint a, GLuint b, GLuint c)
    {
        return HMM_Cross(vertices[b] - vertices[a], vertices[c] - vertices[a]);
    }

	void TangentFromIndices(GLuint a, GLuint b, GLuint c, UVec3& tangent);

	void CalculateTangents();
};


class Mesh
{
public:
	//Use this vector to store all surfaces in mesh (vertex subgroups divided by materials). Then MeshRenderer component will assign proper material to each surface element.
	std::vector<Surface*> m_Surfaces;

	void AddSurface(Surface* surf);

	void RecalculateNormals();


	//! Destructor.
	~Mesh();
};

#endif