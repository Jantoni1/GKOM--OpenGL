#pragma once
#include <glm/mat4x4.hpp>
#include <string>
#include <functional>

class ShaderProgram
{
	GLuint program_id; 	// The program ID
public:
	// Constructor reads and builds the shader
	ShaderProgram(const GLchar* vertexPath, const GLchar* fragmentPath);

	// Use the program
	void Use() const
	{
		glUseProgram(get_programID());
	}
	void setInt(const std::string& name, float x)
	{
		glUniform1i(glGetUniformLocation(program_id, name.c_str()), x);
	}

	// returns program ID
	GLuint get_programID() const
	{
		return program_id;
	}

	void setVec2(const std::string &name, float x, float y)
	{
		glUniform2f(glGetUniformLocation(program_id, name.c_str()), x, y);

	}
	void setVec4(const std::string &name, float x, float y, float z, float w)
	{
		glUniform4f(glGetUniformLocation(program_id, name.c_str()), x, y, z, w);
	}

	void setVec3(const std::string &name, float x, float y, float z)
	{
		glUniform3f(glGetUniformLocation(program_id, name.c_str()), x, y, z);
	}

	void setFloat(const std::string& name, float x)
	{
		glUniform1f(glGetUniformLocation(program_id, name.c_str()), x);
	}

	void setMat4(const std::string &name, const glm::mat4 &mat) const
	{
		glUniformMatrix4fv(glGetUniformLocation(program_id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
};

//mix(texture(Texture0, TexCoord), texture(Texture1, TexCoord),0.4);
//in vec3 vecColor;
//