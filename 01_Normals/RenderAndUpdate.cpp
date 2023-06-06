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

void Raytrace::InitLightProperties()
{
	glUniform3f(m_loc_La, La.x, La.y, La.z);
	glUniform3f(m_loc_Ld, Ld.x, Ld.y, Ld.z);
	glUniform3f(m_loc_Ls, Ls.x, Ls.y, Ls.z);
}

void Raytrace::InitMaterialProperties()
{
	glUniform3f(m_loc_Ka, Ka.x, Ka.y, Ka.z);
	glUniform3f(m_loc_Kd, Kd.x, Kd.y, Kd.z);
	glUniform3f(m_loc_Ks, Ks.x, Ks.y, Ks.z);
}

void Raytrace::Render()
{
	// clear the frame buffer (GL_COLOR_BUFFER_BIT) and the depth buffer (GL_DEPTH_BUFFER_BIT)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// shader turn on
	glUseProgram(m_programID);

	// pass lights to fragment shader
	InitLightProperties();
	InitMaterialProperties();

	// draw spheres
	//============================================================================================================
	glUniform1i(m_loc_spheres_count, spheres.size());
	glUniform4fv(m_loc_spheres, spheres.size(), reinterpret_cast<const GLfloat*>(spheres.data()));
	for (int i = 0; i < spheres.size(); i++)
	{
		glm::mat4 viewProj = m_camera.GetViewProj();
		glm::mat4 world = glm::translate(glm::mat4(1.0f), glm::vec3(spheres[i].x, spheres[i].y, spheres[i].z));
		glm::mat4 worldIT = glm::inverse(glm::transpose(world));
		glm::mat4 mvp = viewProj * world;
		glUniformMatrix4fv(m_loc_mvp, 1, GL_FALSE, &mvp[0][0]);
		glUniformMatrix4fv(m_loc_world, 1, GL_FALSE, &world[0][0]);
		glUniformMatrix4fv(m_loc_worldIT, 1, GL_FALSE, &worldIT[0][0]);

		glm::vec3 eye = m_camera.GetEye();
		glUniform3fv(m_loc_eye, 1, &eye.x);

		// texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_loadedTextureID[i]);
		glUniform1i(m_loc_tex, 0);

		// bind the VAO (VBO comes with VAO)
		glBindVertexArray(m_vaoID[i]);

		// the VAO and the program need to be bounded for draw calls (glUseProgram() and glBindVertexArray())
		glDrawElements(GL_TRIANGLES, 3 * 2 * (N) * (M), GL_UNSIGNED_SHORT, 0);
	}
}