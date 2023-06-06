#include <GL/glew.h>

#include <SDL.h>
#include <SDL_opengl.h>

#include <iostream>
#include <sstream>

#include "GLDebugMessageCallback.h"

#include "Raytrace.h"

// INIT VAO
void Raytrace::InitVAO(GLuint& m_vaoID) {
	// sphere
	glGenVertexArrays(1, &m_vaoID);
	// generate vao
	glBindVertexArray(m_vaoID);
}

// INIT VBO
void Raytrace::InitVBO(std::vector<Vertex>& vert, GLuint& m_vboID) {

	// create new VBO
	glGenBuffers(1, &m_vboID);
	// set active created VBO
	glBindBuffer(GL_ARRAY_BUFFER, m_vboID);

	// fill VBO with data
	glBufferData(GL_ARRAY_BUFFER,
		vert.size() * sizeof(Vertex),	// size of data
		vert.data(),					// data with starting memory address
		GL_STATIC_DRAW);				// we don't want to write to our VBO anymore and use the data in it for every rendering


	// for position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,				// 0 index
		3,				// component number
		GL_FLOAT,		// data type
		GL_FALSE,		// should be normalized? NO
		sizeof(Vertex),	// stride (0=one after another)
		0				// the 0th index attribute starts at the sizeof(Vertex)-sized area
	);

	// for normal
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(glm::vec3)));

	// for texture
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(2 * sizeof(glm::vec3)));
}

// INIT Index Buffer
void Raytrace::InitIndexBuffer(GLuint& m_ibID, std::vector<GLushort>& indices) {
	glGenBuffers(1, &m_ibID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), indices.data(), GL_STATIC_DRAW);
}