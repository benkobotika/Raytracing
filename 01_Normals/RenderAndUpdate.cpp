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

void Raytrace::UpdateTextures()
{
	switch (current_scene)
	{
		case 0:
		{
			for (int i = 0; i < spheres.size(); i++) {
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
			for (int i = 0; i < spheres.size() - 1; i++) {
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
			for (int i = 0; i < spheres.size() - 1; i++) {
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

void Raytrace::Update()
{
	static Uint32 last_time = SDL_GetTicks();
	delta_time = (SDL_GetTicks() - last_time) / 1000.0f;
	
	// resetting collision
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


	if (current_scene == 0 || current_scene == 1)
	{

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
		
		for (int i = 1; i < spheres.size() - 1; i++) {
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
			glm::vec3 forceDirection = glm::vec3(spheres[i]) - glm::vec3(spheres[spheres.size() - 1]);

			// distance between the two bodies
			float distance = glm::length(forceDirection)*1.0f;

			// gravitational force
			float forceMagnitude = G * masses[i] * masses[10] / std::pow(distance, 2) * 1000.0f;

			// force vector
			glm::vec3 force = forceMagnitude * glm::normalize(forceDirection) * 1000.0f;

			// acceleration = force / mass
			glm::vec3 acceleration = force / (float)masses[10]*100000.0f;

			// update velocity
			meteorVelocity += acceleration * delta_time;

			glm::vec3& pos = *(glm::vec3*)&spheres[spheres.size() - 1];

			std::cout << "pos: " << pos[0] << ", " << pos[1] << ", " << pos[2] << std::endl;

			// update position
			pos += meteorVelocity * delta_time * 1000.0f;

			// detect collision
			if (SpheresCollide(spheres[i], spheres[spheres.size() - 1]))
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

			}
		}
	}
	else
	{

		// Rotations
		float rotationSpeed[] = {
			0.9f,	// Mercury
			0.8f,	// Venus
			0.7f,	// Earth
			0.8f,	// Moon
			0.9f,	// Mars
			1.0f,	// Jupiter
			1.1f,	// Saturn
			1.2f,	// Uranus
			1.3f };	// Neptune
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
		
		for (int i = 1; i < spheres.size() - 1; i++) {
			float& x = spheres[i][0];
			float& y = spheres[i][1];
			float& z = spheres[i][2];
			float radius = spheres[i][3];

			float angle = rotationSpeed[i - 1] * delta_time;

			// Polar coordinates
			float r = std::sqrt(x * x + z * z);
			float theta = std::atan2(z, x);

			// From polar to descartes
			float new_x = r * std::cos(theta + angle);
			float new_z = r * std::sin(theta + angle);
			x = new_x;
			z = new_z;

			//// Gravity

			// direction of the force
			glm::vec3 forceDirection = glm::vec3(spheres[i]) - glm::vec3(spheres[spheres.size() - 1]);

			// distance between the two bodies
			float distance = glm::length(forceDirection) * 1.0f;

			// gravitational force
			float forceMagnitude = G * masses[i] * masses[10] / std::pow(distance, 2) * 10.0f;

			// force vector
			glm::vec3 force = forceMagnitude * glm::normalize(forceDirection) * 10.0f;

			// acceleration = force / mass
			glm::vec3 acceleration = force / (float)masses[10] * 100000.0f;

			// update velocity
			meteorVelocity += acceleration * delta_time;

			glm::vec3& pos = *(glm::vec3*)&spheres[spheres.size() - 1];

			std::cout << "pos: " << pos[0] << ", " << pos[1] << ", " << pos[2] << std::endl;

			// update position
			pos += meteorVelocity * delta_time * 1000.0f;
			
			// detect collision
			if (SpheresCollide(spheres[i], spheres[spheres.size() - 1]))
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

			}
		}
	}
	

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

	// std::cout << "========================================================" << std::endl;
	// std::cout << eye.x << " " << eye.y << " " << eye.z << std::endl;
	// std::cout << at.x << " " << at.y << " " << at.z << std::endl;
	// std::cout << up.x << " " << up.y << " " << up.z << std::endl;
	// std::cout << "========================================================" << std::endl;
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
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);  // white background
		collisionTimer += delta_time;
		if (collisionTimer > collisionDelay) {
			collisionOccurred = false;
			collisionTimer = 0.0f;
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
	glDrawArrays((GLenum )GL_TRIANGLES,(GLint) 0,(GLsizei) vertexData2.size());

	glBindVertexArray(0);

}