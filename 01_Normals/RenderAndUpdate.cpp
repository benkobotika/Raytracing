//Path: 01_Normals\Source Files\RenderAndUpdate.cpp

#include <GL/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <iostream>
#include <sstream>
#include "GLDebugMessageCallback.h"
#include "Raytrace.h"

void Raytrace::Update()
{
	static Uint32 last_time = SDL_GetTicks();
	float delta_time = (SDL_GetTicks() - last_time) / 1000.0f;

	m_camera.Update(delta_time);

	//float rotationSpeed[] = { 1.0f, 0.9f, 0.8f,
	//	0.7f, // earth
	//	12.0f, // moon
	//	0.5f, 0.4f, 0.3f, 0.2f, 0.1f };
	float rotationSpeed[] = { 0.0f, 0.0f, 0.0f,
		0.0f, // earth
		0.0f, // moon
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
	for (int i = 1; i < spheres.size(); i++) {


		float& x = spheres[i][0];
		float& y = spheres[i][1];
		float& z = spheres[i][2];
		float radius = spheres[i][3];

		float angle = rotationSpeed[i] * delta_time;


		// moon
		if (i == 4)
		{
			float& earth_x = spheres[3][0];
			float& earth_y = spheres[3][1];
			float& earth_z = spheres[3][2];
			float earth_radius = spheres[3][3];
			
			float& moon_x = spheres[4][0];
			float& moon_y = spheres[4][1];
			float& moon_z = spheres[4][2];
			
			 // Calculate the relative position of the moon from the earth
			float relative_x = moon_x - earth_x;
			float relative_z = moon_z - earth_z;

			// Apply rotation to the relative position
			float new_relative_x = relative_x * cos(angle) - relative_z * sin(angle);
			float new_relative_z = relative_x * sin(angle) + relative_z * cos(angle);

			// Update the absolute position of the moon
			moon_x = new_relative_x + earth_x;
			moon_z = new_relative_z + earth_z;
			
		}
		else 
		{
			//// polar coordinates
			float r = std::sqrt(x * x + z * z);
			float theta = std::atan2(z, x);

			//// from polar to descartes
			float new_x = r * std::cos(theta + angle);
			float new_z = r * std::sin(theta + angle);
			x = new_x;
			z = new_z;
		}

	}

	last_time = SDL_GetTicks();
}

void Raytrace::Render()
{
	// Clear the frame buffer (GL_COLOR_BUFFER_BIT) and the depth buffer (GL_DEPTH_BUFFER_BIT)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Shader turn on
	glUseProgram(m_programID);

	// Pass light and material properties to the fragment shader
	glUniform3fv(m_loc_light_sources, lightSources.size(), reinterpret_cast<const GLfloat*>(lightSources.data()));
	glUniform3fv(m_loc_light_properties, lightProperties.size(), reinterpret_cast<const GLfloat*>(lightProperties.data()));
	glUniform4fv(m_loc_material_properties, materialProperties.size(), reinterpret_cast<const GLfloat*>(materialProperties.data()));

	// Rendering
	glUniform1i(m_loc_spheres_count, spheres.size());
	glUniform4fv(m_loc_spheres, spheres.size(), reinterpret_cast<const GLfloat*>(spheres.data()));

	glm::mat4 viewProj = m_camera.GetViewProj();
	glm::mat4 world = glm::mat4(1.0f);
	glm::mat4 worldIT = glm::inverse(glm::transpose(world));
	glm::mat4 mvp = viewProj * world;
	glUniformMatrix4fv(m_loc_mvp, 1, GL_FALSE, &mvp[0][0]);
	glUniformMatrix4fv(m_loc_world, 1, GL_FALSE, &world[0][0]);
	glUniformMatrix4fv(m_loc_worldIT, 1, GL_FALSE, &worldIT[0][0]);

	glm::vec3 eye = m_camera.GetEye();
	glUniform3fv(m_loc_eye, 1, &eye[0]);

	glm::vec3 at = m_camera.GetAt();
	glUniform3fv(m_loc_at, 1, &at[0]);

	glm::vec3 up = m_camera.GetUp();
	glUniform3fv(m_loc_up, 1, &up[0]);

	// Texture
	for (int i = 0; i < spheres.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, m_loadedTextureID[i]);
		std::stringstream uniformName;
		uniformName << "texImage[" << i << "]";
		glUniform1i(glGetUniformLocation(m_programID, uniformName.str().c_str()), i);
	}

	std::vector<Vertex> vertexData = {
		{{-1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}},
		{{1.0f, -1.0f, -1.0f},  {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}},
		{{1.0f, 1.0f, -1.0f},   {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}},
		{{1.0f, 1.0f, -1.0f},   {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}},
		{{-1.0f, 1.0f, -1.0f},  {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}},
		{{-1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}}
	};

	//// skybox 
	glActiveTexture(GL_TEXTURE0 + spheres.size() + 1);
	switch (current_scene)
	{
	case 0:
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTextureID0);
		break;
	case 1:
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTextureID1);
		break;
	case 2:
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTextureID2);
		break;
	default:
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTextureID0);
		break;
	}

	GLint cubemapTextureLocation = glGetUniformLocation(m_programID, "cubemapTexture");
	glUniform1i(cubemapTextureLocation, spheres.size() + 1); // 0 corresponds to the texture unit used above (GL_TEXTURE0)

	// adjusting the aspect ratios
	float screenWidth = 640.0f;
	float screenHeight = 480.0f;

	glm::mat4 inverseViewMatrix = glm::inverse(m_camera.GetViewMatrix()); // from camera to view (InverseViewMatrix)

	for (auto& vertex : vertexData) {
		vertex.p.x *= screenWidth / 2.0f;
		vertex.p.y *= screenHeight / 2.0f;
		vertex.p = glm::vec3(inverseViewMatrix * glm::vec4(vertex.p, 1.0f));
		vertex.n = glm::vec3(inverseViewMatrix * glm::vec4(vertex.n, 0.0f));
	}

	// Create and bind a VAO
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Create and bind a VBO
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	GLsizei stride = sizeof(Vertex);
	glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(Vertex), vertexData.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, nullptr);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<const void*>(sizeof(glm::vec3)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<const void*>(2 * sizeof(glm::vec3)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glBindVertexArray(vao);	
	
	// Draw the geometry
	glDrawArrays((GLenum )GL_TRIANGLES,(GLint) 0,(GLsizei) vertexData.size());

	glBindVertexArray(0);
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);

}