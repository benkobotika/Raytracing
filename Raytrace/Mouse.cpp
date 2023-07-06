//Path: Raytrace\Source Files\Mouse.cpp

#include <GL/glew.h>

#include <SDL.h>
#include <SDL_opengl.h>

#include <iostream>
#include <sstream>

#include "GLDebugMessageCallback.h"

#include "Raytrace.h"

// Key and mouse events
void Raytrace::KeyboardDown(SDL_KeyboardEvent& key)
{
	m_camera.KeyboardDown(key);
}

void Raytrace::KeyboardUp(SDL_KeyboardEvent& key)
{
	m_camera.KeyboardUp(key);
}

void Raytrace::MouseMove(SDL_MouseMotionEvent& mouse)
{
	m_camera.MouseMove(mouse);
}

void Raytrace::MouseDown(SDL_MouseButtonEvent& mouse)
{
}

void Raytrace::MouseUp(SDL_MouseButtonEvent& mouse)
{
	m_camera.MouseUp(mouse);
}

void Raytrace::MouseWheel(SDL_MouseWheelEvent& wheel)
{
	m_camera.MouseWheel(wheel);
}