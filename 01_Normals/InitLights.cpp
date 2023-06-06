#include <GL/glew.h>

#include <SDL.h>
#include <SDL_opengl.h>

#include <iostream>
#include <sstream>

#include "GLDebugMessageCallback.h"

#include "Raytrace.h"

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