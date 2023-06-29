// Path: 01_Normals\Shaders\fragmentShader.frag
#version 460

#extension GL_NV_shadow_samplers_cube : enable

#define M_PI 3.1415926535897932384626433832795

// attributes per fragment from the pipeline
in vec3 vs_out_pos;
in vec3 vs_out_norm;
in vec2 vs_out_tex;

// out parameter - color
out vec4 fs_out_col;

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

// light properties: ambient, diffuse, specular, attenuation
uniform vec3 light_properties[4]; // La, Ld, Ls, At
vec3 La = light_properties[0];
vec3 Ld = light_properties[1];
vec3 Ls = light_properties[2];
vec3 At = light_properties[3];

// material properties: ambient, diffuse, specular
uniform vec4 material_properties[3]; // Ka, Kd, Ks
vec3 Ka = material_properties[0].xyz;
vec3 Kd = material_properties[1].xyz;
vec3 Ks = material_properties[2].xyz;
float shininess = material_properties[2].w;

// spheres
uniform int spheresCount;
uniform vec4 spheres[10];

uniform sampler2D texImage[10];

// Cubemap texture
uniform samplerCube cubemapTexture;

vec3 setAmbientLight() {
    return La * Ka;
}

vec3 setDiffuseLight(vec3 to_light_dir_norm, vec3 to_point_light_norm, vec3 intersectionPoint, vec3 normal, vec3 Ld, vec3 Kd) {
    float di_dir = clamp(dot(to_light_dir_norm, normal), 0.0, 1.0);
    float di_point = clamp(dot(to_point_light_norm, normal), 0.0, 1.0);
    return (di_point * point_light_color + di_dir * light_dir_color) * Ld * Kd;
}

vec3 setSpecularLight(vec3 eye, vec3 intersectionPoint, vec3 to_point_light_norm, vec3 to_light_dir_norm, vec3 normal) {
    vec3 v_norm = normalize(eye - intersectionPoint);
    vec3 h_norm_1 = normalize(v_norm + to_point_light_norm);
    vec3 h_norm_2 = normalize(v_norm + to_light_dir_norm);
    float si_point = pow(clamp(dot(h_norm_1, normal), 0.0, 1.0), shininess);
    float si_dir = pow(clamp(dot(h_norm_2, normal), 0.0, 1.0), shininess);
    return (si_point * point_light_color + si_dir * light_dir_color) * Ls * Ks;
}

float setAttentuation(float distancee) {
    return (1 / (1 + distancee * 0.00009 + 0.000032 * distancee * distancee));
}

void main()
{

    vec3 fragmentColor = vec3(0.0);

    float fovx = radians(60.0);
    float aspect = 640.0 / 480.0;

    float alfa = tan(fovx / 2.0) * (gl_FragCoord.x - (640.0 / 2.0)) / (640.0 / 2.0);
    float beta = tan(fovx / 2.0) * ((480.0 / 2.0) - gl_FragCoord.y) / (480.0 / 2.0) / aspect;

    vec3 rayStart = eye;
    vec3 w = normalize(eye - at);
    vec3 u = normalize(cross(up, w));
    vec3 v = normalize(cross(w, u));

    vec3 rayDirection = normalize(alfa * u + beta * v - w);

    // Ray Sphere intersection
    // t^⟨v, v⟩ + 2t⟨v, p0 − c⟩ + ⟨p0 − c, p0 − c⟩ − r^2 = 0
    bool intersected = false;
    float closest_t = 1e20;

    for (int i = 0; i < spheresCount; i++)
    {
        vec3 center = spheres[i].xyz;
        float radius = spheres[i].w;

        vec3 poc = rayStart - center; // (p0 - c) vector
        float a = dot(rayDirection, rayDirection);
        float b = 2.0 * dot(poc, rayDirection);
        float c = dot(poc, poc) - radius * radius;
        float delta = b * b - 4.0 * a * c;

        if (delta >= 0.0)
        {
            // Ray intersects with the sphere
            float t1 = (-b + sqrt(delta)) / (2.0 * a);
            float t2 = (-b - sqrt(delta)) / (2.0 * a);

            float t = min(t1, t2); // closest intersection

            if (t > 0.0 && t < closest_t)
            {
                closest_t = t;
                intersected = true;

                // Intersection point
                vec3 intersectionPoint = rayStart + t * rayDirection;

                // Normal
                vec3 normal = normalize(intersectionPoint - center);

                // Calculate lights
                // Ambient
                vec3 ambient = setAmbientLight();

                // Diffuse
                vec3 to_light_dir_norm = normalize(to_light_dir);
                vec3 to_point_light_norm = normalize(to_point_light - intersectionPoint);
                vec3 diffuse = setDiffuseLight(to_light_dir_norm, to_point_light_norm, intersectionPoint, normal, Ld, Kd);

                // Specular (Phong Blinn)
                vec3 specular = setSpecularLight(eye, intersectionPoint, to_point_light_norm, to_light_dir_norm, normal);

                // Sphere texture
                vec3 sphereToIntersection = intersectionPoint - center;
                float u = 0.5 + atan(sphereToIntersection.z, sphereToIntersection.x) / (2.0 * M_PI);
                float v = 0.5 - asin(sphereToIntersection.y / radius) / M_PI;
                vec2 sphereTexCoords = vec2(u, v);
                
                float distancee = distance(intersectionPoint, eye);
                float attenuation = setAttentuation(distancee);

                ambient *= attenuation;
                diffuse *= attenuation;
                specular *= attenuation;

                fragmentColor = ambient + diffuse + specular;
                vec4 textureColor = texture(texImage[i], sphereTexCoords);
                fragmentColor *= textureColor.rgb;
            }
        }
    }

    if (intersected) {
        fs_out_col = vec4(fragmentColor, 1.0);
        return;
    }
    else {
        // Draw skybox using cubemap texture
        vec3 rayDirection = normalize(alfa * u + beta * v - w);
        vec3 skyboxColor = textureCube(cubemapTexture, rayDirection).rgb;
        fs_out_col = vec4(skyboxColor, 1.0);
        return;
    }

}
