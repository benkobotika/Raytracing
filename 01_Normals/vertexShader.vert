//Path: 01_Normals\Shaders\vertex_shader.vert
#version 460 core

// Variable from VBO
in vec3 vs_in_pos;
in vec3 vs_in_norm;
in vec2 vs_in_tex;

// Values to be passed further in the pipeline
out vec3 vs_out_pos;
out vec3 vs_out_norm;
out vec2 vs_out_tex;

// External parameters of the shader
uniform mat4 MVP;
uniform mat4 world;
uniform mat4 worldIT; // inverse transpose of the world

void main()
{
	gl_Position = MVP * vec4( vs_in_pos, 1 );

	vs_out_pos = (world * vec4(vs_in_pos, 1)).xyz; // homogeneous coordinate = 1
	vs_out_norm = (worldIT * vec4(vs_in_norm, 0)).xyz; // homogeneous coordinate = 0
	vs_out_tex = vs_in_tex;
}
