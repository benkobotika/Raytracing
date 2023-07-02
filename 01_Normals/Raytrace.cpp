//Path: 01_Normals\Source Files\Raytrace.cpp

#include "Raytrace.h"
#include "GLUtils.hpp"

#include <math.h>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


// Constructor
Raytrace::Raytrace()
{
	// Camera position (eye, center, up)
	glm::vec3 eye = glm::vec3(94.2055, -57.5629, 90.4523);
	glm::vec3 at = glm::vec3(0, 0, 0);
	glm::vec3 up = glm::vec3(0, 1, 0);
	m_camera.SetView(eye, at, up);
}

// Destructor
Raytrace::~Raytrace()
{
	glDeleteTextures(allSpheresSize, m_loadedTextureID);
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
}

// Init shaders
void Raytrace::InitShaders()
{
	GLuint vs_ID = loadShader(GL_VERTEX_SHADER, "vertexShader.vert");
	GLuint fs_ID = loadShader(GL_FRAGMENT_SHADER, "fragmentShader.frag");

	// Creating a shader program object
	m_programID = glCreateProgram();

	// Add to this program shaders
	glAttachShader(m_programID, vs_ID); // vertex shader
	glAttachShader(m_programID, fs_ID); // fragment shader

	// Assigning attributes in the VAO to shader variables
	glBindAttribLocation(m_programID,	// shader ID
		0,				// the identifier index in the VAO
		"vs_in_pos");	// variable name in the shader
	glBindAttribLocation(m_programID, 1, "vs_in_col");
	glBindAttribLocation(m_programID, 2, "vs_in_tex");

	// Connecting shaders (matching input-output variables, etc.), linking
	glLinkProgram(m_programID);

	// Linking check
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

// Init textures
void Raytrace::InitTextures()
{
	// Read from file
	// scene 0
	m_loadedTextureID[0] = TextureFromFile("assets/sun.jpg");
	m_loadedTextureID[1] = TextureFromFile("assets/mercury.jpg");
	m_loadedTextureID[2] = TextureFromFile("assets/venus.jpg");
	m_loadedTextureID[3] = TextureFromFile("assets/earth.jpg");
	m_loadedTextureID[4] = TextureFromFile("assets/moon.jpg");
	m_loadedTextureID[5] = TextureFromFile("assets/mars.jpg");
	m_loadedTextureID[6] = TextureFromFile("assets/jupiter.jpg");
	m_loadedTextureID[7] = TextureFromFile("assets/saturn.jpg");
	m_loadedTextureID[8] = TextureFromFile("assets/uranus.jpg");
	m_loadedTextureID[9] = TextureFromFile("assets/neptune.jpg");
	m_loadedTextureID[10] = TextureFromFile("assets/asteroid.jpg");
	
	// scene 1
	m_loadedTextureID[11] = TextureFromFile("assets/endor.jpg");
	m_loadedTextureID[12] = TextureFromFile("assets/deathstar.jpg");
	
	// scene 2
	m_loadedTextureID[13] = TextureFromFile("assets/Ball0.jpg");
	m_loadedTextureID[14] = TextureFromFile("assets/Ball1.jpg");
	m_loadedTextureID[15] = TextureFromFile("assets/Ball2.jpg");
	m_loadedTextureID[16] = TextureFromFile("assets/Ball3.jpg");
	m_loadedTextureID[17] = TextureFromFile("assets/Ball4.jpg");
	m_loadedTextureID[18] = TextureFromFile("assets/Ball5.jpg");
	m_loadedTextureID[19] = TextureFromFile("assets/Ball6.jpg");
	m_loadedTextureID[20] = TextureFromFile("assets/Ball7.jpg");
	m_loadedTextureID[21] = TextureFromFile("assets/Ball8.jpg");
	m_loadedTextureID[22] = TextureFromFile("assets/Ball9.jpg");
	m_loadedTextureID[23] = TextureFromFile("assets/Ball10.jpg");
	m_loadedTextureID[24] = TextureFromFile("assets/Ball11.jpg");
	m_loadedTextureID[25] = TextureFromFile("assets/Ball12.jpg");
	
}

void Raytrace::InitCubemap() 
{
	cubemapTextureID0 = LoadCubemapTexture(0);
	cubemapTextureID1 = LoadCubemapTexture(1);
	cubemapTextureID2 = LoadCubemapTexture(2);
}

GLuint Raytrace::LoadCubemapTexture(int scence = 0)
{
	std::vector<std::string> cubemapFilenames = {
	"assets/skybox/right"+std::to_string(scence) + ".png",
	"assets/skybox/left" + std::to_string(scence) + ".png",
	"assets/skybox/top" + std::to_string(scence) + ".png",
	"assets/skybox/bot" + std::to_string(scence) + ".png",
	"assets/skybox/front" + std::to_string(scence) + ".png",
	"assets/skybox/back" + std::to_string(scence) + ".png"
	};
	
	// Generate a texture object
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < cubemapFilenames.size(); i++)
	{
		unsigned char* data = stbi_load(cubemapFilenames[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap tex failed to load at path: " << cubemapFilenames[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}

void Raytrace::InitVaoVbo() {
	// Create and bind a VAO
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Create and bind a VBO
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	GLsizei stride = sizeof(Vertex);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, nullptr);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<const void*>(sizeof(glm::vec3)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<const void*>(2 * sizeof(glm::vec3)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
}

void Raytrace::InitMasses()
{
	// mass = density * volume = density * (4/3 * pi * r^3) = k * r^3
	// simplified to mass = r^3

	masses[0] = 1000000.0f;
	for (int i = 1; i < spheres.size()-1; i++)
	{
		masses[i]=std::pow(spheres[i][3], 2)*100;
		std::cout << "masses[" << i << "] = " << masses[i] << std::endl;
	}
	masses[10] = 100.0f;
}

bool Raytrace::Init()
{
	// Sky color
	glClearColor(0.0f, 0.19f, 0.25f, 1.0f);

	glEnable(GL_CULL_FACE); // enable backface culling
	glEnable(GL_DEPTH_TEST); // depth test enable (z-buffer)
	glCullFace(GL_BACK); // GL_BACK: polygons facing away from the camera, GL_FRONT: polygons facing towards the camera

	// Initialize spheres, shaders and textures
	InitVaoVbo();
	InitShaders();
	InitTextures();
	InitCubemap();
	InitMasses();

	for (int i = 0; i < spheres.size(); i++)
	{
		std::cout << "masses[" << i << "]" << masses[i]<<std::endl;
	}

	// Camera
	// Parameters: angle, aspect (ratio of the width to height), near clipping plane dist, far clipping plane dist 
	m_camera.SetProj(glm::radians(60.0f), 640.0f / 480.0f, 0.01f, 1000.0f);

	// Get the location of uniform variables in the shader
	m_loc_mvp = glGetUniformLocation(m_programID, "MVP");
	m_loc_world = glGetUniformLocation(m_programID, "world");
	m_loc_worldIT = glGetUniformLocation(m_programID, "worldIT");
	m_loc_tex = glGetUniformLocation(m_programID, "texImage");
	m_loc_spheres_count = glGetUniformLocation(m_programID, "spheresCount");
	m_loc_spheres = glGetUniformLocation(m_programID, "spheres");

	// Get the location of light and material uniform variables
	// Light properties
	m_loc_light_sources = glGetUniformLocation(m_programID, "light_sources");
	m_loc_light_properties = glGetUniformLocation(m_programID, "light_properties");

	// Material properties
	m_loc_material_properties = glGetUniformLocation(m_programID, "material_properties");

	// Camera
	m_loc_eye = glGetUniformLocation(m_programID, "eye");
	m_loc_at = glGetUniformLocation(m_programID, "at");
	m_loc_up = glGetUniformLocation(m_programID, "up");
	m_loc_screen_width = glGetUniformLocation(m_programID, "screen_width");
	m_loc_screen_height = glGetUniformLocation(m_programID, "screen_height");

	return true;
}

void Raytrace::Clean()
{
	// Deallocating memory
	for (int i = 0; i < allSpheresSize; i++)
	{
		glDeleteTextures(1, &m_loadedTextureID[i]);
	}

	glDeleteTextures(1, &cubemapTextureID0);
	glDeleteTextures(1, &cubemapTextureID1);
	glDeleteTextures(1, &cubemapTextureID2);

	glDeleteProgram(m_programID);
}

// New window size (width and height)
void Raytrace::Resize(int _w, int _h)
{
	glViewport(0, 0, _w, _h);
	m_camera.Resize(_w, _h);
}

glm::vec4 Raytrace::getRandomPosition() {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dis(-200.0f, 200.0f);
	return { dis(gen), dis(gen), dis(gen), 10.0f};
}

bool Raytrace::SpheresCollide(glm::vec4 sphere1, glm::vec4 sphere2)
{
	float distance = glm::distance(glm::vec3(sphere1), glm::vec3(sphere2));
	float radiusSum = sphere1[3] + sphere2[3];
	return distance < radiusSum;
}
