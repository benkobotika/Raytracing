#version 140

// attributes per fragment from the pipeline
in vec3 vs_out_pos;
in vec3 vs_out_norm;
in vec2 vs_out_tex;

// out parameter - color
out vec4 fs_out_col;

// light direction and point light
uniform vec3 light_dir = vec3(-1,-1,-1);
uniform vec3 point_light = vec3(20, 20, 50);

// light properties: ambient, diffuse, specular
uniform vec3 La;
uniform vec3 Ld;
uniform vec3 Ls;

// material properties: ambient, diffuse, specular
uniform vec3 Ka;
uniform vec3 Kd;
uniform vec3 Ks;

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
	vec3 to_light = normalize(-light_dir);
	float di = clamp(dot(to_light, vs_out_norm), 0.0, 1.0);

	vec3 to_light_point = normalize(point_light - vs_out_pos);
	float di_point = clamp(dot(to_light_point, vs_out_norm), 0.0, 1.0);
	vec3 diffuse = di_point * Ld * Ld;
	/* help:
	    - normalization: http://www.opengl.org/sdk/docs/manglsl/xhtml/normalize.xml
	    - dot product: http://www.opengl.org/sdk/docs/manglsl/xhtml/dot.xml
	    - clamp: http://www.opengl.org/sdk/docs/manglsl/xhtml/clamp.xml
	*/

	// specular color
	// ==============
	vec3 e = normalize(eye - vs_out_pos);
	vec3 r = normalize(reflect(light_dir, vs_out_norm));
	float si = pow(clamp(dot(e, r), 0.0, 1.0), 20);

	vec3 r_point = normalize(reflect(-to_light_point, vs_out_norm));
	float si_point = pow(clamp(dot(e, r_point), 0.0, 1.0), 20);
	vec3 specular = si_point * Ls * Ks;
	/* help:
		- reflect: http://www.opengl.org/sdk/docs/manglsl/xhtml/reflect.xml
				reflect(beérkező_vektor, normálvektor);
		- pow: http://www.opengl.org/sdk/docs/manglsl/xhtml/pow.xml
				pow(alap, kitevő);
	*/
	
	// surface normal
	// fs_out_col = vec4(vs_out_norm, 1);
	// fs_out_col = vec4(ambient + diffuse + specular, 1);

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
