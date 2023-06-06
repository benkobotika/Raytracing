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

void Raytrace::InitSphere()
{
	// we approximate our parametric surface with NxM quadrilaterals, so it needs to be evaluated at (N+1)x(M+1) points
	std::vector<std::vector<Vertex>> vertices(spheres.size());
	for (int k = 0; k < spheres.size(); k++)
	{
		std::vector<Vertex>& vert = vertices[k];
		vert.resize((N + 1) * (M + 1));
		for (int j = 0; j <= M; ++j)
		{
			for (int i = 0; i <= N; ++i)
			{
				float u = i / (float)N; // normalization (0, 1)
				float v = j / (float)M; // normalization (0, 1)

				// it represents the index of the current point in a one dimensional array
				int index = i + j * (N + 1);
				vert[index].p = GetSphere(u, v, spheres[k].w);
				vert[index].n = GetNorm(u, v);
				vert[index].t = GetTex(u, v);
			}
		}
	}

	// index buffer data: 
	std::vector<GLushort> indices(3 * 2 * (N) * (M));
	for (int j = 0; j < M; ++j)
	{
		for (int i = 0; i < N; ++i)
		{
			// (i,j+1) C-----D (i+1,j+1)
			//         |\    |				A = p(u_i, v_j)
			//         | \   |				B = p(u_{i+1}, v_j)
			//         |  \  |				C = p(u_i, v_{j+1})
			//         |   \ |				D = p(u_{i+1}, v_{j+1})
			//         |    \|
			//   (i,j) A-----B (i+1,j)
			//
			// - 1D index in the VBO for (i, j): i + j * (N + 1)
			// - 1D index in the IB for (i, j): i * 6 + j * 6 * N
			//     (because each quad has 2 triangles = 6 indices)
			int index = i * 6 + j * (6 * N);
			indices[index + 0] = i + j * (N + 1);
			indices[index + 1] = (i + 1) + j * (N + 1);
			indices[index + 2] = i + (j + 1) * (N + 1);
			indices[index + 3] = (i + 1) + j * (N + 1);
			indices[index + 4] = (i + 1) + (j + 1) * (N + 1);
			indices[index + 5] = i + (j + 1) * (N + 1);
		}
	}

	// Init VAO, VBO, IndexBuffer
	for (int i = 0; i < spheres.size(); i++)
	{
		InitVAO(m_vaoID[i]);
		InitVBO(vertices[i], m_vboID[i]);
		InitIndexBuffer(m_ibID[i], indices);
	}

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}