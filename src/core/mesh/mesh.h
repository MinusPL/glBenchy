#ifndef MESH_H
#define MESH_H

#include "../../platform/platform.h"
#include "../../handmademath/math.h"

#include <vector>

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
        return Cross(vertices[b] - vertices[a], vertices[c] - vertices[a]);
    }

	UVec3 TangentFromIndices(GLuint a, GLuint b, GLuint c);
};


class Mesh
{
public:
	//Use this vector to store all surfaces in mesh (vertex subgroups divided by materials). Then MeshRenderer component will assign proper material to each surface element.
	std::vector<Surface*> m_Surfaces;

	void Draw();

	void AddSurface(Surface* surf);

	void RecalculateNormals();

	//! Destructor.
	~Mesh();
};

#endif