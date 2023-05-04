#include "mesh.h"

UVec3 Surface::TangentFromIndices(GLuint a, GLuint b, GLuint c)
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
	for(auto surf : m_Surfaces)
	{
		glBindVertexArray(surf->VAO);
		glDrawElements(surf->vertexFlag, surf->indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
}

void Mesh::AddSurface(Surface* surf)
{
	if(surf == nullptr) return;

	m_Surfaces.push_back(surf);
	//Clean and make sure everything is ready for buffer object creation
	glDeleteVertexArrays(1, &surf->VAO);
	glDeleteBuffers(1, &surf->VBO);
	glDeleteBuffers(1, &surf->EBO);
	glDeleteBuffers(1, &surf->VBOClr);
	glDeleteBuffers(1, &surf->VBONorm);
	glDeleteBuffers(1, &surf->VBOuv);
	glDeleteBuffers(1, &surf->VBOTangent);

	glGenVertexArrays(1, &surf->VAO);
	glGenBuffers(1, &surf->VBO);
	glGenBuffers(1, &surf->EBO);
	glGenBuffers(1, &surf->VBOClr);
	glGenBuffers(1, &surf->VBONorm);
	glGenBuffers(1, &surf->VBOuv);
	glGenBuffers(1, &surf->VBOTangent);

	glBindVertexArray(surf->VAO);

	if (surf->vertices.size() != 0) {
		glBindBuffer(GL_ARRAY_BUFFER, surf->VBO);
		glBufferData(GL_ARRAY_BUFFER, surf->vertices.size() * sizeof(UVec3), &(surf->vertices[0]), GL_DYNAMIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	}

	if (surf->colors.size() != 0) {
		glBindBuffer(GL_ARRAY_BUFFER, surf->VBOClr);
		glBufferData(GL_ARRAY_BUFFER, surf->colors.size() * sizeof(UVec3), &(surf->colors[0]), GL_DYNAMIC_DRAW);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	}

	if (surf->normals.size() != 0) {
		glBindBuffer(GL_ARRAY_BUFFER, surf->VBONorm);
		glBufferData(GL_ARRAY_BUFFER, surf->normals.size() * sizeof(UVec3), &(surf->normals[0]), GL_DYNAMIC_DRAW);

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	}

	if (surf->uvs.size() != 0) {
		glBindBuffer(GL_ARRAY_BUFFER, surf->VBOuv);
		glBufferData(GL_ARRAY_BUFFER, surf->uvs.size() * sizeof(UVec2), &surf->uvs[0], GL_DYNAMIC_DRAW);

		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GL_FLOAT), (GLvoid*)0);
	}

	if (surf->tangents.size() != 0) {
		glBindBuffer(GL_ARRAY_BUFFER, surf->VBOTangent);
		glBufferData(GL_ARRAY_BUFFER, surf->tangents.size() * sizeof(UVec2), &surf->tangents[0], GL_DYNAMIC_DRAW);

		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (GLvoid*)0);
	}

	if (surf->indices.size() != 0) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, surf->EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, surf->indices.size() * sizeof(GLuint), &(surf->indices[0]), GL_DYNAMIC_DRAW);
	}
	glBindVertexArray(0);
}

void Mesh::RecalculateNormals()
{
	for(auto surf : m_Surfaces)
	{
		if (surf->vertexFlag != GL_TRIANGLES) {
			throw "Cannot calculate normals with given flag!";
		}

		//normals.clear();
		surf->normals.resize(surf->vertices.size());

		for (size_t i = 0; i < surf->indices.size() / 3; i++)
		{
			GLuint triangleIndex = i * 3;
			GLuint pointA = surf->indices[triangleIndex];
			GLuint pointB = surf->indices[triangleIndex + 1];
			GLuint pointC = surf->indices[triangleIndex + 2];

			UVec3 normal = surf->NormalFromIndices(pointA, pointB, pointC);

			surf->normals[pointA] += normal;
			surf->normals[pointB] += normal;
			surf->normals[pointC] += normal;
		}

		for (size_t i = 0; i < surf->normals.size(); i++)
		{
			surf->normals[i] = Normalize(surf->normals[i]);
		}

		if (surf->uvs.size() > 0) {
			surf->tangents.resize(surf->vertices.size());
			for (size_t i = 0; i < surf->indices.size() / 3; i++)
			{
				GLuint triangleIndex = i * 3;
				GLuint pointA = surf->indices[triangleIndex];
				GLuint pointB = surf->indices[triangleIndex + 1];
				GLuint pointC = surf->indices[triangleIndex + 2];

				UVec3 tangent = surf->TangentFromIndices(pointA, pointB, pointC);

				surf->tangents[pointA] = tangent;
				surf->tangents[pointB] = tangent;
				surf->tangents[pointC] = tangent;

			}
		}
	}
}

Mesh::~Mesh()
{
	for(auto surf : m_Surfaces)
	{
		glDeleteVertexArrays(1, &surf->VAO);
		glDeleteBuffers(1, &surf->VBO);
		glDeleteBuffers(1, &surf->EBO);
		glDeleteBuffers(1, &surf->VBOClr);
		glDeleteBuffers(1, &surf->VBONorm);
		glDeleteBuffers(1, &surf->VBOuv);
		glDeleteBuffers(1, &surf->VBOTangent);
	}
}