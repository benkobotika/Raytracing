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

	last_time = SDL_GetTicks();
}

void Raytrace::Render()
{
	// clear the frame buffer (GL_COLOR_BUFFER_BIT) and the depth buffer (GL_DEPTH_BUFFER_BIT)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// shader turn on
	glUseProgram(m_programID);

	// pass light and material properties to fragment shader
	glUniform3fv(m_loc_light_sources, lightSources.size(), reinterpret_cast<const GLfloat*>(lightSources.data()));
	glUniform3fv(m_loc_light_properties, lightProperties.size(), reinterpret_cast<const GLfloat*>(lightProperties.data()));
	glUniform4fv(m_loc_material_properties, materialProperties.size(), reinterpret_cast<const GLfloat*>(materialProperties.data()));

	// draw spheres
	//============================================================================================================
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

	// texture
	for (int i = 0; i < spheres.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, m_loadedTextureID[i]);
		std::stringstream uniformName;
		uniformName << "texImage[" << i << "]";
		glUniform1i(glGetUniformLocation(m_programID, uniformName.str().c_str()), i);
	}


	glm::vec4 vertex1 = glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f); // we always look at -z direction
	glm::vec4 vertex2 = glm::vec4(1.0f, -1.0f, -1.0f, 1.0f);
	glm::vec4 vertex3 = glm::vec4(1.0f, 1.0f, -1.0f, 1.0f);
	glm::vec4 vertex4 = glm::vec4(-1.0f, 1.0f, -1.0f, 1.0f);

	// adjusting the aspect ratios
	float screenWidth = 480.0f;
	float screenHeight = 640.0f;
	vertex1.x *= screenWidth / 2.0f;
	vertex1.y *= screenHeight / 2.0f;
	vertex2.x *= screenWidth / 2.0f;
	vertex2.y *= screenHeight / 2.0f;
	vertex3.x *= screenWidth / 2.0f;
	vertex3.y *= screenHeight / 2.0f;
	vertex4.x *= screenWidth / 2.0f;
	vertex4.y *= screenHeight / 2.0f;

	glm::mat4 inverseViewMatrix = glm::inverse(m_camera.GetViewMatrix()); // from camera to view (InverseViewMatrix)
	vertex1 = inverseViewMatrix * vertex1;
	vertex2 = inverseViewMatrix * vertex2;
	vertex3 = inverseViewMatrix * vertex3;
	vertex4 = inverseViewMatrix * vertex4;

	glBegin(GL_TRIANGLES);
	glVertex4fv(&vertex1[0]);
	glVertex4fv(&vertex2[0]);
	glVertex4fv(&vertex3[0]);

	glVertex4fv(&vertex3[0]);
	glVertex4fv(&vertex4[0]);
	glVertex4fv(&vertex1[0]);
	glEnd();
}
