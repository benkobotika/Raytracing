//Path: 01_Normals\Source Files\InitSphere.cpp

#include <GL/glew.h>

#include <SDL.h>
#include <SDL_opengl.h>

#include <iostream>
#include <sstream>

#include "GLDebugMessageCallback.h"

#include "Raytrace.h"

// get sphere
glm::vec3 Raytrace::GetSphere(float u, float v, float r)
{
	u *= float(2 * M_PI);
	v *= float(M_PI);

	// parametric equation of sphere
	return glm::vec3(r * sin(v) * cos(u),
		r * cos(v),
		r * sin(v) * sin(u));
}

// the normal vector associated with the parameter values (u, v) of a parametric surface
glm::vec3 Raytrace::GetNorm(float u, float v)
{
	u *= float(2 * M_PI);
	v *= float(M_PI);
	return glm::vec3(sin(v) * cos(u), cos(v), sin(v) * sin(u));

	//glm::vec3 du = GetPos(u+0.01, v)-GetPos(u-0.01, v);
	//glm::vec3 dv = GetPos(u, v+0.01)-GetPos(u, v-0.01);
	//return glm::normalize(glm::cross(du, dv));*/
}

glm::vec2 Raytrace::GetTex(float u, float v)
{
	return glm::vec2(1 - u, 1 - v);
}

void Raytrace::InitTriangles()
{
    std::vector<Vertex> vertices(4); // Create four vertices

    // Define the position, normal, and texture coordinates for each vertex
    vertices[0].p = glm::vec3(-10000.0f, -10000.0f, 0.0f);
    vertices[0].n = glm::vec3(0.0f, 0.0f, 1.0f);
    vertices[0].t = glm::vec2(0.0f, 0.0f);

    vertices[1].p = glm::vec3(10000.0f, -10000.0f, 0.0f);
    vertices[1].n = glm::vec3(0.0f, 0.0f, 1.0f);
    vertices[1].t = glm::vec2(1.0f, 0.0f);

    vertices[2].p = glm::vec3(-10000.0f, 10000.0f, 0.0f);
    vertices[2].n = glm::vec3(0.0f, 0.0f, 1.0f);
    vertices[2].t = glm::vec2(0.0f, 1.0f);

    vertices[3].p = glm::vec3(10000.0f, 10000.0f, 0.0f);
    vertices[3].n = glm::vec3(0.0f, 0.0f, 1.0f);
    vertices[3].t = glm::vec2(1.0f, 1.0f);

    std::vector<GLushort> indices = { 0, 1, 2, 2, 1, 3 }; // Define the indices for the two triangles

    // Initialize the VAO, VBO, and IndexBuffer
    for (int i = 0; i < spheres.size(); i++)
    {
        InitVAO(m_vaoID[i]);
        InitVBO(vertices, m_vboID[i]);
        InitIndexBuffer(m_ibID[i], indices);
    }

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
