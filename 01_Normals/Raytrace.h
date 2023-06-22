//Path: 01_Normals\Header Files\Raytrace.h
#pragma once

// GLEW
#include <GL/glew.h>

// SDL
#include <SDL.h>
#include <SDL_opengl.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>
#include <vector>

#include "gCamera.h"

class Raytrace
{
public:
	Raytrace();
	~Raytrace();

	bool Init();
	void Clean();

	void Update();
	void Render();

	void KeyboardDown(SDL_KeyboardEvent&);
	void KeyboardUp(SDL_KeyboardEvent&);
	void MouseMove(SDL_MouseMotionEvent&);
	void MouseDown(SDL_MouseButtonEvent&);
	void MouseUp(SDL_MouseButtonEvent&);
	void MouseWheel(SDL_MouseWheelEvent&);
	void Resize(int, int);

protected:
	struct Vertex
	{
		glm::vec3 p; // position
		glm::vec3 n; // normal
		glm::vec2 t; // texture
	};

	// spheres (posX, posY, posZ, r)
	float mercury_size = 2.0f;
	std::vector<glm::vec4> spheres = {
	glm::vec4(0.0f, 0.0f, 0.0f, mercury_size * log2(277.0f)),				// sun
	glm::vec4(20.0f, 0.0f, 0.0f, mercury_size),								// mercury
	glm::vec4(40.0f, 0.0f, 0.0f, mercury_size * log2(277.0f / 115.0f)),	    // venus
	glm::vec4(60.0f, 0.0f, 0.0f, mercury_size * log2(277.0f / 109.0f)),		// earth
	glm::vec4(70.0f, 0.0f, 0.0f, mercury_size * log2(277.0f / 240.0f)),		// moon
	glm::vec4(85.0f, 0.0f, 0.0f, mercury_size * log2(277.0f / 207.0f)),		// mars
	glm::vec4(105.0f, 0.0f, 0.0f, mercury_size * log2(277.0f / 11.0f)),		// jupiter
	glm::vec4(135.0f, 0.0f, 0.0f, mercury_size * log2(277.0f / 12.0f)),		// saturn
	glm::vec4(165.0f, 0.0f, 0.0f, mercury_size * log2(277.0f / 27.7f)),		// uranus
	glm::vec4(195.0f, 0.0f, 0.0f, mercury_size * log2(277.0f / 27.4f)),		// neptune
	};

	// light sources and their color
	std::vector<glm::vec3> lightSources = {
		glm::vec3(0.0f, 0.0f, 0.0f), // point light position
		glm::vec3(1.0f, 0.6f, 0.0f), // point light color
		glm::vec3(1.0f, 1.0f, 1.0f), // directional light position
		glm::vec3(1.0f, 1.0f, 1.0f) // directional light color
	};

	// ambient, diffuse, specular light properties
	// and attenuation propreties (constant, linear, quadratic)
	glm::vec3 La = glm::vec3(0.8f, 0.9f, 0.9f);
	glm::vec3 Ld = glm::vec3(0.4f, 0.6f, 0.6f);
	glm::vec3 Ls = glm::vec3(0.9f, 0.9f, 0.9f);
	glm::vec3 At = glm::vec3(1.0f, 0.00009f, 0.000032f);
	std::vector<glm::vec3> lightProperties = { La, Ld, Ls, At };

	// ambient, diffuse, specular material properties
	glm::vec3 Ka = glm::vec3(0.7f, 0.8f, 0.9f);
	glm::vec3 Kd = glm::vec3(0.2f, 0.4f, 0.6f);
	glm::vec3 Ks = glm::vec3(0.4f, 0.8f, 1.0f);
	float shininess = 20.0f;
	std::vector<glm::vec4> materialProperties = {
		glm::vec4(Ka, 0.0f),
		glm::vec4(Kd, 0.0f),
		glm::vec4(Ks, shininess)
	};

	// variables to shader
	GLuint m_programID = 0; // shader program

	// OpenGL things
	GLuint vao;
	GLuint vbo;
	GLuint* m_loadedTextureID = new GLuint[spheres.size()]; // loaded texture identifier
	GLuint cubemapTextureID = 0;
	
	gCamera	m_camera;

	// uniform variable locations in shader
	GLuint m_loc_mvp = 0;
	GLuint m_loc_world = 0;
	GLuint m_loc_worldIT = 0;
	GLuint m_loc_tex = 0;

	GLuint m_loc_spheres = 0;
	GLuint m_loc_spheres_count = 0;

	// lights uniform variables
	GLuint m_loc_light_sources = 0;
	GLuint m_loc_light_properties = 0;
	GLuint m_loc_material_properties = 0;

	// camera
	GLuint m_loc_eye = 0;
	GLuint m_loc_at = 0;
	GLuint m_loc_up = 0;

	void InitShaders();
	void InitTextures();

	// skybox
	void InitCubemap();
	GLuint  LoadCubemapTexture();
};

