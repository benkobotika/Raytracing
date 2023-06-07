//Path: 01_Normals\Source Files\GLDebugMessageCallback.h

#pragma once

#include <GL/glew.h>

void GLAPIENTRY GLDebugMessageCallback(GLenum source, GLenum type, GLuint id,
    GLenum severity, GLsizei length,
    const GLchar* msg, const void* data);
