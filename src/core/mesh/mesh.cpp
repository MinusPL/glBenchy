#include "mesh.h"

UVec3 Mesh::TangentFromIndices(GLuint a, GLuint b, GLuint c)
{
	UVec3 vertA = vertices[a];
	UVec3 vertB = vertices[b];
	UVec3 vertC = vertices[c];

	UVec2 uv1 = uvs[a];
	UVec2  uv2 = uvs[b];
	UVec2  uv3 = uvs[c];

	UVec3 edge1 = vertB - vertA;
	UVec3 edge2 = vertC - vertA;

	UVec2  deltaUV1 = uv2 - uv1;
	UVec2  deltaUV2 = uv3 - uv1;

	float f = 1.0f / (deltaUV1.X * deltaUV2.Y - deltaUV2.X * deltaUV1.Y);

	UVec3 tangent;
	tangent.X = f * (deltaUV2.Y * edge1.X - deltaUV1.Y * edge2.X);
	tangent.Y = f * (deltaUV2.Y * edge1.Y - deltaUV1.Y * edge2.Y);
	tangent.Z = f * (deltaUV2.Y * edge1.Z - deltaUV1.Y * edge2.Z);
	tangent = Normalize(tangent);

	return tangent;
}

void Mesh::Draw()
{
	glBindVertexArray(VAO);
	glDrawElements(vertexFlag, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Mesh::CreateMesh()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteBuffers(1, &VBOClr);
	glDeleteBuffers(1, &VBONorm);
	glDeleteBuffers(1, &VBOuv);
	glDeleteBuffers(1, &VBOTangent);

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glGenBuffers(1, &VBOClr);
	glGenBuffers(1, &VBONorm);
	glGenBuffers(1, &VBOuv);
	glGenBuffers(1, &VBOTangent);

	glBindVertexArray(VAO);

	if (vertices.size() != 0) {
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(UVec3), &(vertices[0]), GL_DYNAMIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	}

	if (colors.size() != 0) {
		glBindBuffer(GL_ARRAY_BUFFER, VBOClr);
		glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(UVec3), &(colors[0]), GL_DYNAMIC_DRAW);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	}

	if (normals.size() != 0) {
		glBindBuffer(GL_ARRAY_BUFFER, VBONorm);
		glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(UVec3), &(normals[0]), GL_DYNAMIC_DRAW);

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	}

	if (uvs.size() != 0) {
		glBindBuffer(GL_ARRAY_BUFFER, VBOuv);
		glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(UVec2), &uvs[0], GL_DYNAMIC_DRAW);

		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GL_FLOAT), (GLvoid*)0);
	}

	if (tangents.size() != 0) {
		glBindBuffer(GL_ARRAY_BUFFER, VBOTangent);
		glBufferData(GL_ARRAY_BUFFER, tangents.size() * sizeof(UVec2), &tangents[0], GL_DYNAMIC_DRAW);

		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (GLvoid*)0);
	}

	if (indices.size() != 0) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &(indices[0]), GL_DYNAMIC_DRAW);
	}
	glBindVertexArray(0);
}

void Mesh::RecalculateNormals()
{
	if (vertexFlag != GL_TRIANGLES) {
		throw "Cannot calculate normals with given flag!";
	}

	//normals.clear();
	normals.resize(vertices.size());

	for (size_t i = 0; i < indices.size() / 3; i++)
	{
		GLuint triangleIndex = i * 3;
		GLuint pointA = indices[triangleIndex];
		GLuint pointB = indices[triangleIndex + 1];
		GLuint pointC = indices[triangleIndex + 2];

		UVec3 normal = NormalFromIndices(pointA, pointB, pointC);

		normals[pointA] += normal;
		normals[pointB] += normal;
		normals[pointC] += normal;
	}

	for (size_t i = 0; i < normals.size(); i++)
	{
		normals[i] = Normalize(normals[i]);
	}

	if (uvs.size() > 0) {
		tangents.resize(vertices.size());
		for (size_t i = 0; i < indices.size() / 3; i++)
		{
			GLuint triangleIndex = i * 3;
			GLuint pointA = indices[triangleIndex];
			GLuint pointB = indices[triangleIndex + 1];
			GLuint pointC = indices[triangleIndex + 2];

			UVec3 tangent = TangentFromIndices(pointA, pointB, pointC);

			tangents[pointA] = tangent;
			tangents[pointB] = tangent;
			tangents[pointC] = tangent;

		}
	}
}

Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteBuffers(1, &VBOClr);
	glDeleteBuffers(1, &VBONorm);
	glDeleteBuffers(1, &VBOuv);
	glDeleteBuffers(1, &VBOTangent);
}