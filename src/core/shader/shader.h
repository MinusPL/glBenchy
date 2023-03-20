#ifndef SHADER_H
#define SHADER_H

#include "../../platform/platform.h"
#include "../../handmademath/math.h"

#include <string>

class Shader
{
public:
	//! ID of generated shader program
	GLuint ID;
	// Constructor
	Shader() { }
	//! Sets the current shader as active
	Shader  &Use();
	//! Compiles the shader from given source code

	/*!
	Compile and create shader.
	\param vertexSource - path to the file containing vertex shader.
	\param fragmentSource - path to the file containing fragment shader.
	\param geometrySource - path to the file containing geometry shader.
	*/
	void    Compile(const GLchar *vertexSource, const GLchar *fragmentSource, const GLchar *geometrySource = nullptr); // Note: geometry source code is optional 
	
	//! Set Float variable for shader.

	/*!
	\param name - name of variable.
	\param value - float value of variable that is beeing set.
	\param useShader - boolean that tells if shader should be set as current active shader.
	*/
	void    SetFloat(const GLchar *name, GLfloat value, GLboolean useShader = false);

	//! Set Integer variable for shader.

	/*!
	\param name - name of variable.
	\param value - integer value of variable that is beeing set.
	\param useShader - boolean that tells if shader should be set as current active shader.
	*/
	void    SetInteger(const GLchar *name, GLint value, GLboolean useShader = false);

	//! Set Vector2f variable for shader.

	/*!
	Sets 2 element vector using two separate float variables.
	\param name - name of variable.
	\param x - first vector value
	\param y - second vector value
	\param useShader - boolean that tells if shader should be set as current active shader.
	*/
	void    SetVector2f(const GLchar *name, GLfloat x, GLfloat y, GLboolean useShader = false);

	//! Set Vector2f variable for shader.

	/*!
	Sets 2 element vector using one UVec2 variable.
	\param name - name of variable.
	\param value - UVec2 that stores x and y values as one variable.
	\param useShader - boolean that tells if shader should be set as current active shader.
	*/
	void    SetVector2f(const GLchar *name, const UVec2 &value, GLboolean useShader = false);

	//! Set Vector2f variable for shader.

	/*!
	Sets 3 element vector using three separate float variables.
	\param name - name of variable.
	\param x - first vector value
	\param y - second vector value
	\param z - third vector value
	\param useShader - boolean that tells if shader should be set as current active shader.
	*/
	void    SetVector3f(const GLchar *name, GLfloat x, GLfloat y, GLfloat z, GLboolean useShader = false);

	//! Set Float variable for shader.

	/*!
	Sets 3 element vector using one UVec3 variable.
	\param name - name of variable.
	\param value - UVec3 that stores x, y and z values as one variable.
	\param useShader - boolean that tells if shader should be set as current active shader.
	*/
	void    SetVector3f(const GLchar *name, const UVec3 &value, GLboolean useShader = false);

	//! Set Vector2f variable for shader.

	/*!
	Sets 4 element vector using two separate float variables.
	\param name - name of variable.
	\param x - first vector value
	\param y - second vector value
	\param z - third vector value
	\param w - fourth vector value
	\param useShader - boolean that tells if shader should be set as current active shader.
	*/
	void    SetVector4f(const GLchar *name, GLfloat x, GLfloat y, GLfloat z, GLfloat w, GLboolean useShader = false);

	//! Set Float variable for shader.

	/*!
	Sets 4 element vector using one glm::vec2 variable.
	\param name - name of variable.
	\param value - glm::vec4 that stores x, y, z and w values as one variable.
	\param useShader - boolean that tells if shader should be set as current active shader.
	*/
	void    SetVector4f(const GLchar *name, const UVec4 &value, GLboolean useShader = false);

	//! Set Matrix4 variable for shader.

	/*!
	Sets 4x4 matrix as variable for shader.
	\param name - name of variable.
	\param matrix - UMat4 representing matrix with size 4x4.
	\param useShader - boolean that tells if shader should be set as current active shader.
	*/
	void    SetMatrix4(const GLchar *name, const UMat4 &matrix, GLboolean useShader = false);

	//! Set Matrix3 variable for shader.

	/*!
	Sets 3x3 matrix as variable for shader.
	\param name - name of variable.
	\param matrix - UMat3 representing matrix with size 3x3.
	\param useShader - boolean that tells if shader should be set as current active shader.
	*/
	//void	SetMatrix3(const GLchar *name, const UMat3 &matrix, GLboolean useShader = false);

private:
	// Checks if compilation or linking failed and if so, print the error logs
	void    checkCompileErrors(GLuint object, std::string type);
};

#endif