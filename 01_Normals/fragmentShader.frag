//Path: 01_Normals\Shaders\fragmentShader.frag
#version 140

// attributes per fragment from the pipeline
in vec3 vs_out_pos;
in vec3 vs_out_norm;
in vec2 vs_out_tex;

// out parameter - color
out vec4 fs_out_col;

// depth map texture for each light source
uniform sampler2D depthMapDir;
uniform sampler2D depthMapPoint;


// light space matrices for each light source
out vec4 FragPosLightSpaceDir;
out vec4 FragPosLightSpacePoint;

float shadowCalc(sampler2D depthMap, vec4 fragPosLightSpace)
{
	float shadow = 0.0;

	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	vec2 UVCoords;
	UVCoords.x = projCoords.x * 0.5 + 0.5;
	UVCoords.y = projCoords.y * 0.5 + 0.5;

	if (0.0 <= UVCoords.x && UVCoords.x <= 1.0 && 0.0 <= UVCoords.y && UVCoords.y <= 1.0)
	{
		float z = projCoords.z;
		float depth = texture(depthMap, UVCoords).r;

		shadow = z > depth ? 1.0 : 0.0;
	}

	return shadow;
}


// camera
uniform vec3 eye;
uniform vec3 at;
uniform vec3 up;

// point light and directional light (2 light sources)
uniform vec3 light_sources[4]; // [position, color, position, color]
vec3 to_point_light = light_sources[0];
vec3 point_light_color = light_sources[1]; // orange color
vec3 to_light_dir = light_sources[2];
vec3 light_dir_color = light_sources[3]; // white color

// light properties: ambient, diffuse, specular
uniform vec3 light_properties[3]; // La, Ld, Ls
vec3 La = light_properties[0];
vec3 Ld = light_properties[1];
vec3 Ls = light_properties[2];

// material properties: ambient, diffuse, specular
uniform vec4 material_properties[3]; // Ka, Kd, Ks
vec3 Ka = material_properties[0].xyz;
vec3 Kd = material_properties[1].xyz;
vec3 Ks = material_properties[2].xyz;
float shininess = material_properties[2].w;

// spheres
uniform int spheresCount;
uniform vec4 spheres[10];

uniform sampler2D texImage;

void main()
{	
	// lights
	// =============================================================================================

	// ambient color
	// ==============
	vec3 ambient = La * Ka;

	// diffuse color
	// ==============
	vec3 to_light_dir_norm = normalize(to_light_dir);
	vec3 to_point_light_norm = normalize(to_point_light - vs_out_pos);

	float di_dir = clamp(dot(to_light_dir_norm, vs_out_norm), 0.0, 1.0);
	float di_point = clamp(dot(to_point_light_norm, vs_out_norm), 0.0, 1.0);

	float shadowDir = shadowCalc(depthMapDir, FragPosLightSpaceDir);
	float shadowPoint = shadowCalc(depthMapPoint, FragPosLightSpacePoint);

	vec3 diffuse = ((1.0 - shadowPoint) * di_point * point_light_color + (1.0 - shadowDir) * di_dir * light_dir_color) * Ld * Kd;

	/* help:
	    - normalization: http://www.opengl.org/sdk/docs/manglsl/xhtml/normalize.xml
	    - dot product: http://www.opengl.org/sdk/docs/manglsl/xhtml/dot.xml
	    - clamp: http://www.opengl.org/sdk/docs/manglsl/xhtml/clamp.xml
	*/

	// specular color (Phong-Blinn)
	// ==============
    vec3 v_norm = normalize(eye - vs_out_pos); // vector from vs_out_pos to eye (v)
	vec3 h_norm_1 = normalize(v_norm + to_point_light_norm); // half vector (point light)
	vec3 h_norm_2 = normalize(v_norm + to_light_dir_norm); // half vector (directional light)

    vec3 r_point = normalize(reflect(-to_point_light_norm, vs_out_norm));
    vec3 r_dir = normalize(reflect(-to_light_dir_norm, vs_out_norm));
    float si_point = pow(clamp(dot(h_norm_1, vs_out_norm), 0.0, 1.0), shininess);
    float si_dir = pow(clamp(dot(h_norm_2, vs_out_norm), 0.0, 1.0), shininess);
    vec3 specular = (si_point * point_light_color + si_dir * light_dir_color) * Ls * Ks;



	/* help:
		- reflect: http://www.opengl.org/sdk/docs/manglsl/xhtml/reflect.xml
				reflect(beérkező_vektor, normálvektor);
		- pow: http://www.opengl.org/sdk/docs/manglsl/xhtml/pow.xml
				pow(alap, kitevő);
	*/
	
	// fragment color (calculate intersection with spheres)
	// =======================================================
	vec4 textureColor = texture(texImage, vs_out_tex);
	vec3 fragmentPosition = vs_out_pos;
	
	for (int i=0; i<spheresCount; i++) 
	{
		vec3 center = spheres[i].xyz;
		float radius = spheres[i].w;

		float dist = distance(center, fragmentPosition);

		if (dist <= radius)
		{
			fs_out_col = vec4(ambient + diffuse + specular, 1) * textureColor;
			return;
		}
	}

	discard;
}
