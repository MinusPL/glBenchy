#ifndef MESH_H
#define MESH_H

#include "../../platform/platform.h"
#include "../../handmademath/math.h"

#include <vector>

class Mesh
{
public:

	GLenum vertexFlag = GL_TRIANGLES;

	std::vector<UVec3> vertices;

	std::vector<UVec3> colors;

	std::vector<GLuint> indices;

	std::vector<UVec3> normals;

	std::vector<UVec2> uvs;

	std::vector<UVec3> tangents;

	GLuint GetVAO() {
		return VAO;
	}

	void Draw();

	void CreateMesh();

	void RecalculateNormals();

	//! Destructor.
	~Mesh();

private:
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

#endif