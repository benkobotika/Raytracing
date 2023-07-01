//Path: 01_Normals\Source Files\RenderAndUpdate.cpp

#include <GL/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <iostream>
#include <sstream>
#include "GLDebugMessageCallback.h"
#include "Raytrace.h"

//Newton gravitációs törvénye a következőképpen szól :
//
//F = G * (m1 * m2) / r ^ 2
//
//ahol :
//
//	F az erő,
//	G a gravitációs állandó(6.67430 * 10 ^ -11 m ^ 3 kg ^ -1 s ^ -2),
//	m1 és m2 a két test tömege,
//	r a távolságuk.

void Raytrace::Update()
{
	static Uint32 last_time = SDL_GetTicks();
	float delta_time = (SDL_GetTicks() - last_time) / 1000.0f;

	m_camera.Update(delta_time);

	// Rotations
	float rotationSpeed[] = {
		0.9f,	// Mercury
		0.8f,	// Venus
		0.7f,	// Earth
		12.0f,	// Moon
		0.5f,	// Mars
		0.4f,	// Jupiter
		0.3f,	// Saturn
		0.2f,	// Uranus
		0.1f };	// Neptune
	//float rotationSpeed[] = {
	//	0.0f,	// Mercury
	//	0.0f,	// Venus
	//	0.0f,	// Earth
	//	0.0f,	// Moon
	//	0.0f,	// Mars
	//	0.0f,	// Jupiter
	//	0.0f,	// Saturn
	//	0.0f,	// Uranus
	//	0.0f };	// Neptune

	for (int i = 1; i < spheres.size()-1; i++) {
		float& x = spheres[i][0];
		float& y = spheres[i][1];
		float& z = spheres[i][2];
		float radius = spheres[i][3];

		float angle = rotationSpeed[i - 1] * delta_time;

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
			// Polar coordinates
			float r = std::sqrt(x * x + z * z);
			float theta = std::atan2(z, x);

			// From polar to descartes
			float new_x = r * std::cos(theta + angle);
			float new_z = r * std::sin(theta + angle);
			x = new_x;
			z = new_z;
		}

		//// Gravity
		
		// direction of the force
		glm::vec3 forceDirection = glm::vec3(spheres[i]) - glm::vec3(spheres[10]);

		// distance between the two bodies
		float distance = glm::length(forceDirection);

		// mass = density * volume = density * (4/3 * pi * r^3) = k * r^3

		// gravitational force
		float forceMagnitude = G * masses[i] * masses[10] / std::pow(distance, 2);

		// force vector
		glm::vec3 force = forceMagnitude * glm::normalize(forceDirection);

		// acceleration = force / mass
		glm::vec3 acceleration = force / (float)masses[10];

		// update velocity
		meteorVelocity += acceleration * delta_time;

		glm::vec3& pos = *(glm::vec3*)&spheres[10];

		// update position
		pos += meteorVelocity * delta_time*1000.0f;

		std::cout << "pos: " << pos[0]<<", " << pos[1] <<", "<<pos[2]<<std::endl;
	}

	

	last_time = SDL_GetTicks();
}

void Raytrace::passLightAndMaterialProperties() {
	glUniform3fv(m_loc_light_sources, lightSources.size(), reinterpret_cast<const GLfloat*>(lightSources.data()));
	glUniform3fv(m_loc_light_properties, lightProperties.size(), reinterpret_cast<const GLfloat*>(lightProperties.data()));
	glUniform4fv(m_loc_material_properties, materialProperties.size(), reinterpret_cast<const GLfloat*>(materialProperties.data()));
}

void Raytrace::passSphereProperties() {
	glUniform1i(m_loc_spheres_count, spheres.size());
	glUniform4fv(m_loc_spheres, spheres.size(), reinterpret_cast<const GLfloat*>(spheres.data()));
}

void Raytrace::passEyeAtUp() {
	glm::vec3 eye = m_camera.GetEye();
	glUniform3fv(m_loc_eye, 1, &eye[0]);

	glm::vec3 at = m_camera.GetAt();
	glUniform3fv(m_loc_at, 1, &at[0]);

	glm::vec3 up = m_camera.GetUp();
	glUniform3fv(m_loc_up, 1, &up[0]);

	// std::cout << "========================================================" << std::endl;
	// std::cout << eye.x << " " << eye.y << " " << eye.z << std::endl;
	// std::cout << at.x << " " << at.y << " " << at.z << std::endl;
	// std::cout << up.x << " " << up.y << " " << up.z << std::endl;
	// td::cout << "========================================================" << std::endl;
}

void Raytrace::passMvpWorldWorldIT() {
	glm::mat4 viewProj = m_camera.GetViewProj();
	glm::mat4 world = glm::mat4(1.0f);
	glm::mat4 worldIT = glm::inverse(glm::transpose(world));
	glm::mat4 mvp = viewProj * world;
	glUniformMatrix4fv(m_loc_mvp, 1, GL_FALSE, &mvp[0][0]);
	glUniformMatrix4fv(m_loc_world, 1, GL_FALSE, &world[0][0]);
	glUniformMatrix4fv(m_loc_worldIT, 1, GL_FALSE, &worldIT[0][0]);
	glUniform1f(m_loc_screen_width, m_camera.GetScreenWidth());
	glUniform1f(m_loc_screen_height, m_camera.GetScreenHeight());
}

void Raytrace::Render()
{
	// Clear the frame buffer (GL_COLOR_BUFFER_BIT) and the depth buffer (GL_DEPTH_BUFFER_BIT)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Shader turn on
	glUseProgram(m_programID);

	// Pass light and material properties to the fragment shader
	passLightAndMaterialProperties();

	// Rendering
	passSphereProperties();
	passMvpWorldWorldIT();
	passEyeAtUp();

	// Texture
	for (int i = 0; i < spheres.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, m_loadedTextureID[i]);
		std::stringstream uniformName;
		uniformName << "texImage[" << i << "]";
		glUniform1i(glGetUniformLocation(m_programID, uniformName.str().c_str()), i);
	}

	// Skybox 
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

	// Adjusting the aspect ratios
	float screenWidth = 640.0f;
	float screenHeight = 480.0f;

	// Calculate inverseViewMatrix
	glm::mat4 inverseViewMatrix = glm::inverse(m_camera.GetViewMatrix()); // from camera to view (InverseViewMatrix)

	// Build and render the vertices
	std::vector<Vertex> vertexData2 = vertexData;

	for (auto& vertex : vertexData2) {
		vertex.p.x *= screenWidth / 2.0f;
		vertex.p.y *= screenHeight / 2.0f;
		vertex.p = glm::vec3(inverseViewMatrix * glm::vec4(vertex.p, 1.0f));
		vertex.n = glm::vec3(inverseViewMatrix * glm::vec4(vertex.n, 0.0f));
	}

	glBufferData(GL_ARRAY_BUFFER, vertexData2.size() * sizeof(Vertex), vertexData2.data(), GL_STATIC_DRAW);
	glBindVertexArray(vao);
	
	// Draw the geometry
	glDrawArrays((GLenum )GL_TRIANGLES,(GLint) 0,(GLsizei) vertexData2.size());

	glBindVertexArray(0);

}