#include "Shader.h"

unsigned int Shader::loadSubShader(const char* sub_shader_path, GLuint shader_type)
{

	// 1. retrieve the vertex/fragment source code from filePath
	std::string code;
	std::ifstream shader_file;
	// ensure ifstream objects can throw exceptions:
	shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		// open file
		shader_file.open(sub_shader_path);
		std::stringstream shader_stream;
		// read file's buffer contents into streams
		shader_stream << shader_file.rdbuf();
		// close file handlers
		shader_file.close();
		// convert stream into string
		code = shader_stream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
	}

	const char* shader_code = code.c_str();

	// 2. compile shaders
	unsigned int shader_id;
	int success;
	char infoLog[512];

	shader_id = glCreateShader(shader_type);

	glShaderSource(shader_id, 1, &shader_code, NULL);
	glCompileShader(shader_id);

	// Check for compile errors
	glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(shader_id, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	return shader_id;
}

Shader::Shader(const char* vertexPath, const char* fragmentPath, const char* geometry_path)
{
	//std::cout << "Loading shader \n" << vertexPath << "\n" << fragmentPath << std::endl;
	unsigned int vertex_shader = loadSubShader(vertexPath, GL_VERTEX_SHADER);
	unsigned int fragment_shader = loadSubShader(fragmentPath, GL_FRAGMENT_SHADER);
	std::vector<unsigned int> shader_list =
	{
		vertex_shader,
		fragment_shader
	};

	unsigned int geometry_shader;
	if (geometry_path)
	{
		geometry_shader = loadSubShader(geometry_path, GL_GEOMETRY_SHADER);
		shader_list.push_back(geometry_shader);
	}
		
	// combine into shader program
	ID = glCreateProgram();
	for (unsigned int shader : shader_list)
	{
		glAttachShader(ID, shader);
	}
	glLinkProgram(ID);

	// print any linking errors
	char infoLog[512];
	int success;
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	// delete shaders now that they're linked
	for (unsigned int shader : shader_list)
	{
		glDeleteShader(shader);
	}
}


void Shader::use()
{
	glUseProgram(ID);
}


void Shader::setBool(const std::string& name, bool value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}
void Shader::setInt(const std::string& name, int value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}
void Shader::setFloat(const std::string& name, float value) const
{
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setMat4(const std::string& name, glm::mat4* value) const
{
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(*value));
}

void Shader::setVec3(const std::string& name, float v0, float v1, float v2) const
{
	glUniform3f(glGetUniformLocation(ID, name.c_str()), v0, v1, v2);
}

void Shader::setVec3(const std::string& name, glm::vec3* value) const
{
	glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(*value));
}

void Shader::setVec2(const std::string& name, glm::vec2* value) const
{
	glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(*value));
}

