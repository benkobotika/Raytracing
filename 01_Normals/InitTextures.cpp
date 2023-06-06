#include <GL/glew.h>

#include <SDL.h>
#include <SDL_opengl.h>

#include <iostream>
#include <sstream>

#include "GLDebugMessageCallback.h"

#include "Raytrace.h"

void Raytrace::InitTextures()
{
	// read from file
	m_loadedTextureID[0] = TextureFromFile("assets/sun.jpg");
	m_loadedTextureID[1] = TextureFromFile("assets/mercury.jpg");
	m_loadedTextureID[2] = TextureFromFile("assets/venus.jpg");
	m_loadedTextureID[3] = TextureFromFile("assets/moon.jpg");
	m_loadedTextureID[4] = TextureFromFile("assets/earth.jpg");
}