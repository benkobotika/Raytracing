#include "Raytrace.h"
#include "GLUtils.hpp"

#include <math.h>
#include <vector>


// constructor
Raytrace::Raytrace()
{
	// camera position (eye, center, up)
	glm::vec3 eye = glm::vec3(0, 50, 150);
	glm::vec3 at = glm::vec3(0, 0, 0);
	glm::vec3 up = glm::vec3(0, 1, 0);
	m_camera.SetView(eye, at, up);
}

// destructor
Raytrace::~Raytrace()
{
	glDeleteVertexArrays(spheres.size(), m_vaoID);
	glDeleteBuffers(spheres.size(), m_vboID);
	glDeleteBuffers(spheres.size(), m_ibID);
	glDeleteTextures(spheres.size(), m_loadedTextureID);

	delete[] m_vaoID;
	delete[] m_vboID;
	delete[] m_ibID;
	delete[] m_loadedTextureID;
}

// init shaders
void Raytrace::InitShaders()
{
	GLuint vs_ID = loadShader(GL_VERTEX_SHADER, "vertexShader.vert");
	GLuint fs_ID = loadShader(GL_FRAGMENT_SHADER, "fragmentShader.frag");

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

// init textures
void Raytrace::InitTextures()
{
	// read from file
	m_loadedTextureID[0] = TextureFromFile("assets/sun.jpg");
	m_loadedTextureID[1] = TextureFromFile("assets/mercury.jpg");
	m_loadedTextureID[2] = TextureFromFile("assets/venus.jpg");
	m_loadedTextureID[3] = TextureFromFile("assets/earth.jpg");
	m_loadedTextureID[4] = TextureFromFile("assets/moon.jpg");
	m_loadedTextureID[5] = TextureFromFile("assets/mars.jpg");
	m_loadedTextureID[6] = TextureFromFile("assets/jupiter.jpg");
	m_loadedTextureID[7] = TextureFromFile("assets/saturn.jpg");
	m_loadedTextureID[8] = TextureFromFile("assets/uranus.jpg");
	m_loadedTextureID[9] = TextureFromFile("assets/neptune.jpg");
}

bool Raytrace::Init()
{
	// sky color
	glClearColor(0.0f, 0.19f, 0.25f, 1.0f);

	glEnable(GL_CULL_FACE); // enable backface culling
	glEnable(GL_DEPTH_TEST); // depth test enable (z-buffer)
	glCullFace(GL_BACK); // GL_BACK: polygons facing away from the camera, GL_FRONT: polygons facing towards the camera

	// initialize spheres, shaders and textures
	InitSphere();
	InitShaders();
	InitTextures();

	// camera
	// parameters: angle, aspect (ratio of the width to height), near clipping plane dist, far clipping plane dist 
	m_camera.SetProj(glm::radians(60.0f), 640.0f / 480.0f, 0.01f, 1000.0f);
	
	// get the location of uniform variables in the shader
	m_loc_mvp = glGetUniformLocation(m_programID, "MVP");
	m_loc_world = glGetUniformLocation(m_programID, "world");
	m_loc_worldIT = glGetUniformLocation(m_programID, "worldIT");
	m_loc_tex = glGetUniformLocation(m_programID, "texImage");
	m_loc_spheres_count = glGetUniformLocation(m_programID, "spheresCount");
	m_loc_spheres = glGetUniformLocation(m_programID, "spheres");

	// get the location of light and material uniform variables
	// light properties
	m_loc_La = glGetUniformLocation(m_programID, "La");
	m_loc_Ld = glGetUniformLocation(m_programID, "Ld");
	m_loc_Ls = glGetUniformLocation(m_programID, "Ls");
	
	// material properties
	m_loc_Ka = glGetUniformLocation(m_programID, "Ka");
	m_loc_Kd = glGetUniformLocation(m_programID, "Kd");
	m_loc_Ks = glGetUniformLocation(m_programID, "Ks");

	// camera
	m_loc_eye = glGetUniformLocation(m_programID, "eye");
	m_loc_at = glGetUniformLocation(m_programID, "at");
	m_loc_up = glGetUniformLocation(m_programID, "up");
	
	return true;
}

void Raytrace::Clean()
{
	// deallocating memory
	for (int i = 0; i < spheres.size(); i++) 
	{
		glDeleteTextures(1, &m_loadedTextureID[i]);
		glDeleteBuffers(1, &m_vboID[i]);
		glDeleteBuffers(1, &m_ibID[i]);
		glDeleteVertexArrays(1, &m_vaoID[i]);
	}

	glDeleteProgram(m_programID);
}

// new window size (width and height)
void Raytrace::Resize(int _w, int _h)
{
	glViewport(0, 0, _w, _h);
	m_camera.Resize(_w, _h);
}
