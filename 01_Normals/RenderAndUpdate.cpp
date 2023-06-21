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
	for (int i = 0; i < spheres.size(); i++)
	{
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
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_loadedTextureID[i]);
		glUniform1i(m_loc_tex, 0);

		// bind the VAO (VBO comes with VAO)
		glBindVertexArray(m_vaoID[i]);

		// the VAO and the program need to be bounded for draw calls (glUseProgram() and glBindVertexArray())
		glDrawElements(GL_TRIANGLES, 3 * 2, GL_UNSIGNED_SHORT, 0);
	}
}