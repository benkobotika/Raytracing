#include "Raytrace.h"
#include "GLUtils.hpp"

#include <math.h>
#include <vector>

void Raytrace::InitVAO(GLuint& m_vaoID) {
	// sphere
	glGenVertexArrays(1, &m_vaoID);
	// generate vao
	glBindVertexArray(m_vaoID);
}

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
		sizeof(Vertex),	// stride (0=egymas utan)
		0				// the 0th index attribute starts at the sizeof(Vertex)-sized area
	);

	// for normal
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(glm::vec3)));

	// for texture
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(2 * sizeof(glm::vec3)));
}

void Raytrace::InitIndexBuffer(GLuint& m_ibID, std::vector<GLushort>& indices) {
	glGenBuffers(1, &m_ibID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), indices.data(), GL_STATIC_DRAW);
}

void Raytrace::InitSphere()
{
	// We approximate our parametric surface with NxM quadrilaterals, so it needs to be evaluated at (N+1)x(M+1) points.
	std::vector<Vertex> vert((N + 1) * (M + 1));
	for (int j = 0; j <= M; ++j)
	{
		for (int i = 0; i <= N; ++i)
		{
			float u = i / (float)N; // normalization (0, 1)
			float v = j / (float)M; // normalization (0, 1)

			// it represents the index of the current point in a one dimensional array
			int index = i + j * (N + 1);
			vert[index].p = GetSphere(u, v);
			vert[index].n = GetNorm(u, v);
			vert[index].t = GetTex(u, v);
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
			// - az (i,j)-hez tartózó 1D-s index a VBO-ban: i+j*(N+1)
			// - az (i,j)-hez tartózó 1D-s index az IB-ben: i*6+j*6*N
			//		(mert minden négyszöghöz 2db háromszög = 6 index tartozik)
			int index = i * 6 + j * (6 * N);
			indices[index + 0] = (i)+(j) * (N + 1);
			indices[index + 1] = (i + 1) + (j) * (N + 1);
			indices[index + 2] = (i)+(j + 1) * (N + 1);
			indices[index + 3] = (i + 1) + (j) * (N + 1);
			indices[index + 4] = (i + 1) + (j + 1) * (N + 1);
			indices[index + 5] = (i)+(j + 1) * (N + 1);
		}
	}

	// Init VAOs
	for (int i = 0; i < spheresCount; i++) {
		InitVAO(m_vaoID[i]);
		InitVBO(vert, m_vboID[i]);
		InitIndexBuffer(m_ibID[i], indices);
	}

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

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
	glBindAttribLocation(m_programID,	// shader azonosítója, amiből egy változóhoz szeretnénk hozzárendelést csinálni
		0,				// a VAO-beli azonosító index
		"vs_in_pos");	// a shader-beli változónév
	glBindAttribLocation(m_programID, 1, "vs_in_col");
	glBindAttribLocation(m_programID, 2, "vs_in_tex0");

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

void Raytrace::InitTextures()
{
	// read from file
	m_loadedTextureID = TextureFromFile("earth.jpg");
	// sampling settings
	glBindTexture(GL_TEXTURE_2D, m_loadedTextureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	// mi legyen az eredmény, ha a textúrán kívülröl próbálunk mintát venni?
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // vízszintesen
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // függölegesen

	glBindTexture(GL_TEXTURE_2D, 0);
}