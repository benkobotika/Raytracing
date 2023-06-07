#version 140

// attributes per fragment from the pipeline
in vec3 vs_out_pos;
in vec3 vs_out_norm;
in vec2 vs_out_tex;

// out parameter - color
out vec4 fs_out_col;

// light direction and point light (2 light sources)
uniform vec3 to_light_dir = vec3(1,1,1);
uniform vec3 to_point_light = vec3(0, 0, 0);

// light properties: ambient, diffuse, specular
uniform vec3 La;
uniform vec3 Ld;
uniform vec3 Ls;

// material properties: ambient, diffuse, specular
uniform vec3 Ka;
uniform vec3 Kd;
uniform vec3 Ks;

// light colors
uniform vec3 light_dir_color = vec3(1.0f, 1.0f, 1.0f); // white color
uniform vec3 point_light_color = vec3(1.0f, 0.6f, 0.0f); // orange color

// camera
uniform vec3 eye;
uniform vec3 at;
uniform vec3 up;

// spheres
uniform int spheresCount;
uniform vec4 spheres[5];

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
	vec3 to_light_point_norm = normalize(to_point_light - vs_out_pos);

	float di_dir = clamp(dot(to_light_dir_norm, vs_out_norm), 0.0, 1.0);
	float di_point = clamp(dot(to_light_point_norm, vs_out_norm), 0.0, 1.0);
	vec3 diffuse = (di_point * point_light_color + di_dir * light_dir_color) * Ld * Kd;

	/* help:
	    - normalization: http://www.opengl.org/sdk/docs/manglsl/xhtml/normalize.xml
	    - dot product: http://www.opengl.org/sdk/docs/manglsl/xhtml/dot.xml
	    - clamp: http://www.opengl.org/sdk/docs/manglsl/xhtml/clamp.xml
	*/

	// specular color (Phong-Blinn)
	// ==============
    vec3 v_norm = normalize(eye - vs_out_pos); // vector from vs_out_pos to eye (v)
	vec3 h_norm_1 = normalize(v_norm + to_light_point_norm); // half vector
	vec3 h_norm_2 = normalize(v_norm + to_light_dir_norm); // half vector

    vec3 r_point = normalize(reflect(-to_light_point_norm, vs_out_norm));
    vec3 r_dir = normalize(reflect(-to_light_dir_norm, vs_out_norm));
    float si_point = pow(clamp(dot(h_norm_1, vs_out_norm), 0.0, 1.0), 20);
    float si_dir = pow(clamp(dot(h_norm_2, vs_out_norm), 0.0, 1.0), 10);
    vec3 specular = (si_point * point_light_color + si_dir * light_dir_color) * Ls * Ks; // no specular light (blinking) for light direction

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
