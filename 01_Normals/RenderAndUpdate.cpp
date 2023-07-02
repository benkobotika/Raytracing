//Path: 01_Normals\Source Files\RenderAndUpdate.cpp

#include <GL/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <iostream>
#include <sstream>
#include "GLDebugMessageCallback.h"
#include "Raytrace.h"
#include <thread>
#include <chrono>

void Raytrace::UpdateTextures()
{
	switch (current_scene)
	{
	case 0:
	{
		for (int i = 0; i < spheres.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, m_loadedTextureID[i]);
			std::stringstream uniformName;
			uniformName << "texImage[" << i << "]";
			glUniform1i(glGetUniformLocation(m_programID, uniformName.str().c_str()), i);
		}
		break;
	}
	case 1:
	{
		int offset = spheres0.size();
		for (int i = 0; i < spheres.size() - 1; i++)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, m_loadedTextureID[offset + i]);
			std::stringstream uniformName;
			uniformName << "texImage[" << i << "]";
			glUniform1i(glGetUniformLocation(m_programID, uniformName.str().c_str()), i);
		}
		// meteor
		int meteor_index = spheres.size();
		glActiveTexture(GL_TEXTURE0 + meteor_index);
		glBindTexture(GL_TEXTURE_2D, m_loadedTextureID[10]);
		std::stringstream uniformName;
		uniformName << "texImage[" << meteor_index << "]";
		glUniform1i(glGetUniformLocation(m_programID, uniformName.str().c_str()), meteor_index);
		break;
	}
	case 2:
	{
		int offset2 = spheres0.size() + spheres1.size();
		std::cout << offset2 << std::endl;
		for (int i = 0; i < spheres.size() - 1; i++)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, m_loadedTextureID[offset2 + i]);
			std::stringstream uniformName;
			uniformName << "texImage[" << i << "]";
			glUniform1i(glGetUniformLocation(m_programID, uniformName.str().c_str()), i);
		}
		// meteor
		int meteor_index = spheres.size();
		glActiveTexture(GL_TEXTURE0 + meteor_index);
		glBindTexture(GL_TEXTURE_2D, m_loadedTextureID[10]);
		std::stringstream uniformName;
		uniformName << "texImage[" << meteor_index << "]";
		glUniform1i(glGetUniformLocation(m_programID, uniformName.str().c_str()), meteor_index);
		break;
	}
	}
}

void SetRotationSpeed(float* rotationSpeed, int current_scene)
{
	if (current_scene == 0 || current_scene == 1)
	{
		rotationSpeed[0] = 0.45f;	// Mercury
		rotationSpeed[1] = 0.40f;	// Venus
		rotationSpeed[2] = 0.35f;	// Earth
		rotationSpeed[3] = 12.0f;	// Moon
		rotationSpeed[4] = 0.30f;	// Mars
		rotationSpeed[5] = 0.25f;	// Jupiter
		rotationSpeed[6] = 0.20f;	// Saturn
		rotationSpeed[7] = 0.15f;	// Uranus
		rotationSpeed[8] = 0.10f;	// Neptune
	}
	else
	{
		rotationSpeed[0] = 0.9f;
		rotationSpeed[1] = 0.8f;
		rotationSpeed[2] = 0.7f;
		rotationSpeed[3] = 0.8f;
		rotationSpeed[4] = 0.9f;
		rotationSpeed[5] = 1.0f;
		rotationSpeed[6] = 1.1f;
		rotationSpeed[7] = 1.2f;
		rotationSpeed[8] = 1.3f;
	}
}

void Raytrace::UpdateSpheres()
{
	float rotationSpeed[9];
	SetRotationSpeed(rotationSpeed, current_scene);

	// Gravity for the Sun
	// ===================
	// Newton's law of gravity is as follows:
	//
	// F = G * (m1 * m2) / r ^ 2
	//
	// where:
	//
	// F is the force,
	// G is the gravitational constant (6.67430 * 10^-11 m^3 kg^-1 s^-2),
	// m1 and m2 are the masses of the two bodies,
	// r is the distance between them.

	// Direction of the force
	glm::vec3 forceDirection = glm::vec3(spheres[0]) - glm::vec3(spheres[spheres.size() - 1]);

	// distance between the two bodies
	float distance = glm::length(forceDirection) * 2.0f;

	// Gravitational force
	float forceMagnitude = G * masses[0] * masses[10] / std::pow(distance, 2) * 1.0f;

	// Force vector
	glm::vec3 force = forceMagnitude * glm::normalize(forceDirection) * 1.0f;

	// acceleration = force / mass
	glm::vec3 acceleration = force / (float)masses[10] * 1000.0f;

	// Update velocity
	meteorVelocity += acceleration * delta_time;

	glm::vec3& pos = *(glm::vec3*)&spheres[spheres.size() - 1];

	// std::cout << "pos: " << pos[0] << ", " << pos[1] << ", " << pos[2] << std::endl;

	// Update position
	pos += meteorVelocity * delta_time * 1000.0f;
	for (int i = 1; i < spheres.size() - 1; i++)
	{
		float& x = spheres[i][0];
		float& y = spheres[i][1];
		float& z = spheres[i][2];
		float radius = spheres[i][3];

		float angle = rotationSpeed[i - 1] * delta_time;

		// Moon
		if (i == 4 && current_scene == 0)
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

		// Gravity
		// =======

		// Direction of the force
		glm::vec3 forceDirection = glm::vec3(spheres[i]) - glm::vec3(spheres[spheres.size() - 1]);

		// distance between the two bodies
		float distance = glm::length(forceDirection) * 2.0f;

		// Gravitational force
		float forceMagnitude = G * masses[i] * masses[10] / std::pow(distance, 2) * 1.0f;

		// Force vector
		glm::vec3 force = forceMagnitude * glm::normalize(forceDirection) * 1.0f;

		// acceleration = force / mass
		glm::vec3 acceleration = force / (float)masses[10] * 1000.0f;

		// Update velocity
		meteorVelocity += acceleration * delta_time;

		glm::vec3& pos = *(glm::vec3*)&spheres[spheres.size() - 1];

		std::cout << "pos: " << pos[0] << ", " << pos[1] << ", " << pos[2] << std::endl;

		// Update position
		pos += meteorVelocity * delta_time * 1000.0f;

		// Detect collision
		if ( r_pressed && (SpheresCollide(spheres[i], spheres[spheres.size() - 1])  || SpheresCollide(spheres[0],spheres[spheres.size()-1])))
		{
			collisionOccurred = true;
			/*std::cout << "collision" << std::endl;
			std::cout << "pos: " << pos[0] << ", " << pos[1] << ", " << pos[2] << std::endl;
			std::cout << "vel: " << meteorVelocity[0] << ", " << meteorVelocity[1] << ", " << meteorVelocity[2] << std::endl;
			std::cout << "acc: " << acceleration[0] << ", " << acceleration[1] << ", " << acceleration[2] << std::endl;
			std::cout << "force: " << force[0] << ", " << force[1] << ", " << force[2] << std::endl;
			std::cout << "forceDir: " << forceDirection[0] << ", " << forceDirection[1] << ", " << forceDirection[2] << std::endl;
			std::cout << "forceMag: " << forceMagnitude << std::endl;
			std::cout << "distance: " << distance << std::endl;
			std::cout << "mass: " << masses[i] << std::endl;
			std::cout << "acceleration: " << acceleration[0] << ", " << acceleration[1] << ", " << acceleration[2] << std::endl;
			std::cout << "velocity: " << meteorVelocity[0] << ", " << meteorVelocity[1] << ", " << meteorVelocity[2] << std::endl;
			std::cout << "position: " << pos[0] << ", " << pos[1] << ", " << pos[2] << std::endl;
			std::cout << "radius: " << radius << std::endl;*/

			pos = getRandomPosition();
			meteorVelocity = { 0.0f,0.0f,0.0f };

		}
	}
}

void Raytrace::Update()
{
	static Uint32 last_time = SDL_GetTicks();
	delta_time = (SDL_GetTicks() - last_time) / 1000.0f;

	// Resetting collision
	collisionOccurred = false;

	m_camera.Update(delta_time);

	if (last_scene != current_scene)
	{
		switch (current_scene)
		{
		case 0:
			spheres = spheres0;
			last_scene = current_scene;
			break;
		case 1:
			spheres = spheres1;
			last_scene = current_scene;
			break;
		case 2:
			spheres = spheres2;
			last_scene = current_scene;
			break;
		default:
			spheres = spheres0;
			last_scene = current_scene;
			break;
		}
	}

	UpdateSpheres();
	last_time = SDL_GetTicks();
}

void Raytrace::PassLightAndMaterialProperties() {
	glUniform3fv(m_loc_light_sources, lightSources.size(), reinterpret_cast<const GLfloat*>(lightSources.data()));
	glUniform3fv(m_loc_light_properties, lightProperties.size(), reinterpret_cast<const GLfloat*>(lightProperties.data()));
	glUniform4fv(m_loc_material_properties, materialProperties.size(), reinterpret_cast<const GLfloat*>(materialProperties.data()));
}

void Raytrace::PassSphereProperties() {
	int current_spheres_size = spheres.size();
	glUniform1i(m_loc_spheres_count, current_spheres_size);
	glUniform4fv(m_loc_spheres, current_spheres_size, reinterpret_cast<const GLfloat*>(spheres.data()));
}

void Raytrace::PassEyeAtUp() {
	glm::vec3 eye = m_camera.GetEye();
	glUniform3fv(m_loc_eye, 1, &eye[0]);

	glm::vec3 at = m_camera.GetAt();
	glUniform3fv(m_loc_at, 1, &at[0]);

	glm::vec3 up = m_camera.GetUp();
	glUniform3fv(m_loc_up, 1, &up[0]);
}

void Raytrace::PassMvpWorldWorldIT() {
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
	if (collisionOccurred) {
		collisionTimer += delta_time;
		if (collisionTimer > collisionDelay) {
			glClearColor(1.0f, 1.0f, 1.0f, 1.0f);  // white background
			collisionOccurred = false;
			collisionTimer = 0.0f;
			std::this_thread::sleep_for(std::chrono::milliseconds(2000)); // Sleep to let the flash be visible
		}
	}

	// Clear the frame buffer (GL_COLOR_BUFFER_BIT) and the depth buffer (GL_DEPTH_BUFFER_BIT)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Shader turn on
	glUseProgram(m_programID);

	// Pass light and material properties to the fragment shader
	PassLightAndMaterialProperties();

	// Rendering
	PassSphereProperties();
	PassMvpWorldWorldIT();
	PassEyeAtUp();

	// Texture
	UpdateTextures();

	// Skybox 
	glActiveTexture(GL_TEXTURE0 + allSpheresSize + 1);
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
	glUniform1i(cubemapTextureLocation, allSpheresSize + 1); // 0 corresponds to the texture unit used above (GL_TEXTURE0)

	GLint maxDepthLocation = glGetUniformLocation(m_programID, "maxDepth");
	glUniform1i(maxDepthLocation, depth);

	GLint current_scene_Location = glGetUniformLocation(m_programID, "currentScene");
	glUniform1i(current_scene_Location, current_scene);

	// Adjusting the aspect ratios
	float screenWidth = m_camera.GetScreenWidth();
	float screenHeight = m_camera.GetScreenHeight();

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
	glDrawArrays((GLenum)GL_TRIANGLES, (GLint)0, (GLsizei)vertexData2.size());

	glBindVertexArray(0);

}