//Path: 01_Normals\Source Files\GLUtils.hpp

#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>

#include <GL/glew.h>

#include <SDL_image.h>

/* 

Based on the http://www.opengl-tutorial.org/ website.

*/
GLuint loadShader(GLenum _shaderType, const char* _fileName)
{
	// Creating a shader identifier
	GLuint loadedShader = glCreateShader(_shaderType);

	// If the creation failed, return an error message and -1
	if (loadedShader == 0)
	{
		std::cerr << "[glCreateShader] Error during the initialization of shader: " << _fileName << "!\n";
		return 0;
	}

	// Loading shader code from the _fileName file
	std::string shaderCode = "";

	// Opening the _fileName file
	std::ifstream shaderStream(_fileName);

	if (!shaderStream.is_open())
	{
		std::cerr << "[std::ifstream] Error during the reading of " << _fileName << " shaderfile's source!\n";
		return 0;
	}

	// Loading the contents of the file into the shaderCode string
	std::string line = "";
	while (std::getline(shaderStream, line))
	{
		shaderCode += line + "\n";
	}

	shaderStream.close();

	// Assigning the code loaded from the file to the shader
	const char* sourcePointer = shaderCode.c_str();
	glShaderSource(loadedShader, 1, &sourcePointer, nullptr);

	// Compile the shader
	glCompileShader(loadedShader);

	// Check if everything is fine
	GLint result = GL_FALSE;
	int infoLogLength;

	// Querying the compilation status
	glGetShaderiv(loadedShader, GL_COMPILE_STATUS, &result);
	glGetShaderiv(loadedShader, GL_INFO_LOG_LENGTH, &infoLogLength);

	if (GL_FALSE == result)
	{
		// Requesting and printing the error message
		std::vector<char> VertexShaderErrorMessage(infoLogLength);
		glGetShaderInfoLog(loadedShader, infoLogLength, nullptr, &VertexShaderErrorMessage[0]);

		std::cerr << "[glCompileShader] Shader compilation error in " << _fileName << ":\n" << &VertexShaderErrorMessage[0] << std::endl;
	}

	return loadedShader;
}

GLuint loadProgramVSGSFS(const char* _fileNameVS, const char* _fileNameGS, const char* _fileNameFS)
{
	// Loading the vertex, geometry, and fragment shaders
	GLuint vs_ID = loadShader(GL_VERTEX_SHADER,		_fileNameVS);
	GLuint gs_ID = loadShader(GL_GEOMETRY_SHADER,	_fileNameGS);
	GLuint fs_ID = loadShader(GL_FRAGMENT_SHADER,	_fileNameFS);

	// If there was any issue with loading any of the shaders, we cannot proceed with the program. Return 0
	if ( vs_ID == 0 || gs_ID == 0 || fs_ID == 0 )
	{
		return 0;
	}

	// Linking the components together
	GLuint program_ID = glCreateProgram();

	fprintf(stdout, "Linking program\n");
	glAttachShader(program_ID, vs_ID);
	glAttachShader(program_ID, gs_ID);
	glAttachShader(program_ID, fs_ID);

	glLinkProgram(program_ID);

	// Checking the linking status
	GLint infoLogLength = 0, result = 0;

	glGetProgramiv(program_ID, GL_LINK_STATUS, &result);
	glGetProgramiv(program_ID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if ( GL_FALSE == result)
	{
		std::vector<char> ProgramErrorMessage( infoLogLength );
		glGetProgramInfoLog(program_ID, infoLogLength, NULL, &ProgramErrorMessage[0]);
		fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);
	}

	// Then these are no longer needed
	glDeleteShader( vs_ID );
	glDeleteShader( gs_ID );
	glDeleteShader( fs_ID );

	// Returning the program identifier
	return program_ID;
}

int invert_image(int pitch, int height, void* image_pixels)
{
	int index;
	void* temp_row;
	int height_div_2;

	temp_row = (void *)malloc(pitch);
	if (NULL == temp_row)
	{
		SDL_SetError("Not enough memory for image inversion");
		return -1;
	}
	// If height is odd, don't need to swap middle row
	height_div_2 = (int)(height * .5);
	for (index = 0; index < height_div_2; index++) {
		//uses string.h
		memcpy((Uint8 *)temp_row,
			(Uint8 *)(image_pixels)+
			pitch * index,
			pitch);

		memcpy(
			(Uint8 *)(image_pixels)+
			pitch * index,
			(Uint8 *)(image_pixels)+
			pitch * (height - index - 1),
			pitch);
		memcpy(
			(Uint8 *)(image_pixels)+
			pitch * (height - index - 1),
			temp_row,
			pitch);
	}
	free(temp_row);
	return 0;
}

int SDL_InvertSurface(SDL_Surface* image)
{
	if (NULL == image)
	{
		SDL_SetError("Surface is NULL");
		return -1;
	}

	return invert_image(image->pitch, image->h, image->pixels);
}

GLuint TextureFromFile(const char* filename)
{
	// Load image
	SDL_Surface* loaded_img = IMG_Load(filename);
	if (loaded_img == nullptr)
	{
		std::cout << "[TextureFromFile] Error during loading file: " << filename << std::endl;
		return 0;
	}

	// SDL stores colors as Uint32 values, so byte order matters
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
	Uint32 format = SDL_PIXELFORMAT_ABGR8888;
#else
	Uint32 format = SDL_PIXELFORMAT_RGBA8888;
#endif

	// Converting to 32-bit RGBA format if it wasn't already
	SDL_Surface* formattedSurf = SDL_ConvertSurfaceFormat(loaded_img, format, 0);
	if (formattedSurf == nullptr)
	{
		std::cout << "[TextureFromFile] Error while processing image: " << SDL_GetError() << std::endl;
		SDL_FreeSurface(loaded_img);
		return 0;
	}

	// Switching from SDL coordinate system (with (0,0) in the top-left corner) to
	// OpenGL texture coordinate system (with (0,0) in the bottom-left corner)
	if (SDL_InvertSurface(formattedSurf) == -1) {
		std::cout << "[TextureFromFile] Error while processing image: " << SDL_GetError() << std::endl;
		SDL_FreeSurface(formattedSurf);
		SDL_FreeSurface(loaded_img);
		return 0;
	}

	// Generating an OpenGL texture
	GLuint tex;
	glGenTextures(1, &tex);

	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, formattedSurf->w, formattedSurf->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, formattedSurf->pixels);

	// Generating mipmaps
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);

	// Freeing the used SDL_Surfaces
	SDL_FreeSurface(formattedSurf);
	SDL_FreeSurface(loaded_img);

	return tex;
}

GLuint genSampler(GLint param_min = GL_LINEAR, GLint param_mag = GL_LINEAR_MIPMAP_LINEAR)
{
	GLuint samp;
	glGenSamplers(1, &samp);

	glSamplerParameteri(samp, GL_TEXTURE_MIN_FILTER, param_min);
	glSamplerParameteri(samp, GL_TEXTURE_MAG_FILTER, param_mag);

	return samp;
}