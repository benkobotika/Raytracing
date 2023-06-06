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
	std::vector<glm::vec4> spheres = {
	glm::vec4(0.0f, 0.0f, 0.0f, 10.0f),		// sun
	glm::vec4(-25.0f, 0.0f, 0.0f, 2.0f),	// mercury
	glm::vec4(-35.0f, 0.0f, 0.0f, 2.5f),	// venus
	glm::vec4(25.0f, 0.0f, 0.0f, 1.0f),		// moon
	glm::vec4(35.0f, 0.0f, 0.0f, 2.0f)		// earth
	};

	// light properties
	glm::vec3 La = glm::vec3(0.8f, 0.9f, 0.9f);
	glm::vec3 Ld = glm::vec3(0.4f, 0.6f, 0.6f);
	glm::vec3 Ls = glm::vec3(0.9f, 0.9f, 0.9f);

	// material properties
	glm::vec3 Ka = glm::vec3(0.7f, 0.8f, 0.9f);
	glm::vec3 Kd = glm::vec3(0.2f, 0.4f, 0.6f);
	glm::vec3 Ks = glm::vec3(0.4f, 0.8f, 1.0f);

	void InitVAO(GLuint&);
	void InitVBO(std::vector<Vertex>&, GLuint&);
	void InitIndexBuffer(GLuint&, std::vector<GLushort>&);

	// helper functions
	glm::vec3 GetSphere(float u, float v, float r);
	glm::vec3 GetNorm(float u, float v);
	glm::vec2 GetTex(float u, float v);

	// variables to shader
	GLuint m_programID = 0; // shader program

	// OpenGL things
	GLuint* m_vaoID = new GLuint[5]; // vertex array object resource identifier
	GLuint* m_vboID = new GLuint[5]; // vertex buffer object resource identifier
	GLuint* m_ibID = new GLuint[5];  // index buffer object resource identifier
	GLuint* m_loadedTextureID = new GLuint[5]; // loaded texture identifier

	gCamera	m_camera;

	// uniform variable locations in shader
	GLuint m_loc_mvp = 0;
	GLuint m_loc_world = 0;
	GLuint m_loc_worldIT = 0;
	GLuint m_loc_eye = 0;
	GLuint m_loc_tex = 0;
	GLuint m_loc_spheres = 0;
	GLuint m_loc_spheres_count = 0;

	// lights uniform variables
	GLuint m_loc_La = 0;
	GLuint m_loc_Ld = 0;
	GLuint m_loc_Ls = 0;

	GLuint m_loc_Ka = 0;
	GLuint m_loc_Kd = 0;
	GLuint m_loc_Ks = 0;

	// we approximate our parametric surface with NxM quadrilaterals, so it needs to be evaluated at (N+1)x(M+1) points
	static const int N = 80;
	static const int M = 40;

	void InitSphere();
	void InitShaders();
	void InitTextures();
	void InitLightProperties();
	void InitMaterialProperties();
};

