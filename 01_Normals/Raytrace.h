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

// Random
#include <random>

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

	//max depth to reflect
	int maxDepth = 100;
	int depth = 1;
	
	// current scene variable
	int current_scene = 0;

	// spheres (posX, posY, posZ, r)
	float mercury_size = 2.0f;
	std::vector<glm::vec4> spheres0 = {
	glm::vec4(0.0f, 0.0f, 0.0f, mercury_size * log2(277.0f)),				// sun
	glm::vec4(40.0f, 0.0f, 0.0f, mercury_size),								// mercury
	glm::vec4(60.0f, 0.0f, 0.0f, mercury_size * log2(277.0f / 115.0f)),	    // venus
	glm::vec4(80.0f, 0.0f, 0.0f, mercury_size * log2(277.0f / 109.0f)),		// earth
	glm::vec4(85.0f, 0.0f, 0.0f, mercury_size * log2(277.0f / 240.0f)),		// moon
	glm::vec4(105.0f, 0.0f, 0.0f, mercury_size * log2(277.0f / 207.0f)),	// mars
	glm::vec4(125.0f, 0.0f, 0.0f, mercury_size * log2(277.0f / 11.0f)),		// jupiter
	glm::vec4(155.0f, 0.0f, 0.0f, mercury_size * log2(277.0f / 12.0f)),		// saturn
	glm::vec4(185.0f, 0.0f, 0.0f, mercury_size * log2(277.0f / 27.7f)),		// uranus
	glm::vec4(215.0f, 0.0f, 0.0f, mercury_size * log2(277.0f / 27.4f)),		// neptune
	glm::vec4(0.0f, 0.0f, 0.0f, 10.0f)										// oversized asteroid
	};

	std::vector<glm::vec4> spheres1 = {
	glm::vec4(0.0f, 0.0f, 0.0f, mercury_size * log2(277.0f)),				// endor
	glm::vec4(40.0f, 0.0f, 0.0f, 2*mercury_size),							// death star
	glm::vec4(0.0f, 0.0f, 0.0f, 10.0f)										// oversized asteroid
	};

	float uranus_size = mercury_size * log2(277.0f / 27.7f);
	std::vector<glm::vec4> spheres2 = {
	glm::vec4(0.0f, 0.0f, 0.0f, uranus_size*2),
	glm::vec4(40.0f, 0.0f, 0.0f, uranus_size),
	glm::vec4(80.0f, 0.0f, 0.0f, uranus_size),
	glm::vec4(100.0f, 0.0f, 0.0f, uranus_size),
	glm::vec4(110.0f, 0.0f, 0.0f, uranus_size),
	glm::vec4(135.0f, 0.0f, 0.0f, uranus_size),
	glm::vec4(155.0f, 0.0f, 0.0f, uranus_size),
	glm::vec4(185.0f, 0.0f, 0.0f, uranus_size),
	glm::vec4(215.0f, 0.0f, 0.0f, uranus_size),
	glm::vec4(235.0f, 0.0f, 0.0f, uranus_size),
	glm::vec4(0.0f, 0.0f, 0.0f, 10.0f)										// oversized asteroid
	};

	std::vector<glm::vec4> spheres = {
	glm::vec4(0.0f, 0.0f, 0.0f, mercury_size * log2(277.0f)),				// sun
	glm::vec4(40.0f, 0.0f, 0.0f, mercury_size),								// mercury
	glm::vec4(60.0f, 0.0f, 0.0f, mercury_size * log2(277.0f / 115.0f)),	    // venus
	glm::vec4(80.0f, 0.0f, 0.0f, mercury_size * log2(277.0f / 109.0f)),		// earth
	glm::vec4(85.0f, 0.0f, 0.0f, mercury_size * log2(277.0f / 240.0f)),		// moon
	glm::vec4(105.0f, 0.0f, 0.0f, mercury_size * log2(277.0f / 207.0f)),	// mars
	glm::vec4(125.0f, 0.0f, 0.0f, mercury_size * log2(277.0f / 11.0f)),		// jupiter
	glm::vec4(155.0f, 0.0f, 0.0f, mercury_size * log2(277.0f / 12.0f)),		// saturn
	glm::vec4(185.0f, 0.0f, 0.0f, mercury_size * log2(277.0f / 27.7f)),		// uranus
	glm::vec4(215.0f, 0.0f, 0.0f, mercury_size * log2(277.0f / 27.4f)),		// neptune
	glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)										// oversized asteroid
	};

	glm::vec4 getRandomPosition();

	glm::vec3 meteorVelocity = { 0.0f,0.0f,0.0f };
	bool collisionOccurred = false;
	bool r_pressed = false;
	bool stop_animation = false;

protected:

	// Gravity implementation
	void InitMasses();
	bool SpheresCollide(glm::vec4 sphere1, glm::vec4 sphere2);
	float collisionTimer = 0.0f;
	float collisionDelay = 1.0f;  // delay in seconds
	float delta_time;

	// Gravity
	const float G = 6.67430e-11;  // Gravitational constant
	int masses[11];

	// Vertex initialization
	struct Vertex
	{
		glm::vec3 p; // position
		glm::vec3 n; // normal
		glm::vec2 t; // texture
	};

	std::vector<Vertex> vertexData = {
	{{-1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}},
	{{1.0f, -1.0f, -1.0f},  {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}},
	{{1.0f, 1.0f, -1.0f},   {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}},
	{{1.0f, 1.0f, -1.0f},   {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}},
	{{-1.0f, 1.0f, -1.0f},  {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}},
	{{-1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}}
	};
	

	// light sources and their color
	std::vector<glm::vec3> lightSources = {
		glm::vec3(0.0f, 0.0f, 0.0f), // point light position
		glm::vec3(1.0f, 0.6f, 0.0f), // point light color
		glm::vec3(1.0f, 1.0f, 1.0f), // directional light position
		glm::vec3(1.0f, 1.0f, 1.0f) // directional light color
	};

	// ambient, diffuse, specular light properties
	// and attenuation propreties (distanceRatio, linear, quadratic)
	glm::vec3 La = glm::vec3(0.8f, 0.9f, 0.9f);
	glm::vec3 Ld = glm::vec3(0.4f, 0.6f, 0.6f);
	glm::vec3 Ls = glm::vec3(0.9f, 0.9f, 0.9f);
	glm::vec3 At = glm::vec3(0.5f, 0.00009f, 0.000032f);
	std::vector<glm::vec3> lightProperties = { La, Ld, Ls, At };

	// ambient, diffuse, specular material properties
	glm::vec3 Ka = glm::vec3(0.7f, 0.8f, 0.9f);
	glm::vec3 Kd = glm::vec3(0.8f, 0.8f, 0.8f);
	glm::vec3 Ks = glm::vec3(0.4f, 0.8f, 1.0f);
	float shininess = 20.0f;
	std::vector<glm::vec4> materialProperties = {
		glm::vec4(Ka, 0.0f),
		glm::vec4(Kd, 0.0f),
		glm::vec4(Ks, shininess),
		glm::vec4(255, 253, 55, 0)
	};


	// variables to shader
	GLuint m_programID = 0; // shader program

	// OpenGL things
	GLuint vao = 0;
	GLuint vbo = 0;
	int allSpheresSize = 3 + spheres0.size() + spheres1.size() + spheres2.size();
	GLuint* m_loadedTextureID = new GLuint[allSpheresSize]; // loaded texture identifier
	GLuint cubemapTextureID0 = 0;
	GLuint cubemapTextureID1 = 0;
	GLuint cubemapTextureID2 = 0;
	
	gCamera	m_camera;
	GLuint m_loc_screen_width = 0;
	GLuint m_loc_screen_height = 0;

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

	// Init functions
	void InitVaoVbo();
	void InitShaders();
	void InitTextures();

	// Render functions
	void PassLightAndMaterialProperties();
	void PassSphereProperties();
	void PassEyeAtUp();
	void PassMvpWorldWorldIT();

	// skybox
	void InitCubemap();
	GLuint  LoadCubemapTexture(int scence);

	void UpdateTextures();
	void UpdateSpheres();

	// last scene
	int last_scene = 0;
};

