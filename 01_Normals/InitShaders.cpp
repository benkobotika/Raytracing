#include <GL/glew.h>

#include <SDL.h>
#include <SDL_opengl.h>

#include <iostream>
#include <sstream>

#include "GLDebugMessageCallback.h"

#include "Raytrace.h"
#include "GLUtils.hpp"

void Raytrace::InitShaders()
{
	GLuint vs_ID = loadShader(GL_VERTEX_SHADER, "myVert.vert");
	GLuint fs_ID = loadShader(GL_FRAGMENT_SHADER, "myFrag.frag");

	// creating a shader program object
	m_programID = glCreateProgram();

	// add to this program shaders
	glAttachShader(m_programID, vs_ID); // vertex shader
	glAttachShader(m_programID, fs_ID); // fragment shader

	// assigning attributes in the VAO to shader variables
	glBindAttribLocation(m_programID,	// shader ID
		0,				// the identifier index in the VAO
		"vs_in_pos");	// variable name in the shader
	glBindAttribLocation(m_programID, 1, "vs_in_col");
	glBindAttribLocation(m_programID, 2, "vs_in_tex");

	// connecting shaders (matching input-output variables, etc.), linking
	glLinkProgram(m_programID);

	// linking check
	GLint infoLogLength = 0, result = 0;

	glGetProgramiv(m_programID, GL_LINK_STATUS, &result);
	glGetProgramiv(m_programID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (GL_FALSE == result || infoLogLength != 0)
	{
		std::vector<char> VertexShaderErrorMessage(infoLogLength);
		glGetProgramInfoLog(m_programID, infoLogLength, nullptr, VertexShaderErrorMessage.data());
		std::cerr << "[glLinkProgram] Shader linking error:\n" << &VertexShaderErrorMessage[0] << std::endl;
	}

	glDeleteShader(vs_ID);
	glDeleteShader(fs_ID);
}