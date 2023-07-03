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
vec3 point_light_color = light_sources[3]; // orange color
vec3 to_light_dir = light_sources[2];
vec3 light_dir_color = light_sources[1]; // white color

// light properties: ambient, diffuse, specular, attenuation
uniform vec3 light_properties[4]; // La, Ld, Ls, At
vec3 La = light_properties[0];
vec3 Ld = light_properties[1];
vec3 Ls = light_properties[2];
vec3 At = light_properties[3];
// ratio between distanceEyeAndPlanet and distanceIntersectionPointAndPlanet
float distanceRatio = At[0];
float linearConst = At[1];
float quadraticConst = At[2];

// material properties: ambient, diffuse, specular, reflected color
uniform vec4 material_properties[4]; // Ka, Kd, Ks
vec3 Ka = material_properties[0].xyz;
vec3 Kd = material_properties[1].xyz;
vec3 Ks = material_properties[2].xyz;
float shininess = material_properties[2].w;
vec3 basicReflectedColor = material_properties[3].xyz;

// spheres
uniform int spheresCount;
uniform vec4 spheres[16];

uniform sampler2D texImage[16];

uniform float screen_width;
uniform float screen_height;

// Cubemap texture
uniform samplerCube cubemapTexture;

//max depth to reflected rays
uniform int maxDepth;

//current scene
uniform int currentScene;

struct Hit {
    float distance;
    vec3 position;
    vec3 normal;
    int indexOfSphere;
};

struct Ray {
    vec3 startPosition;
    vec3 direction;
};

Hit firstIntersection(Ray ray);

vec3 setAmbientLight() {
    return La * Ka;
}

vec3 setDiffuseLight(vec3 to_light_dir_norm, vec3 to_point_light_norm, Hit hit) {
    float di_dir = clamp(dot(to_light_dir_norm, hit.normal), 0.0, 1.0);
    float di_point = clamp(dot(to_point_light_norm, hit.normal), 0.0, 1.0);
    return (di_point * point_light_color + di_dir * light_dir_color) * Ld * Kd;
}

vec3 setSpecularLight(Hit hit, vec3 to_point_light_norm, vec3 to_light_dir_norm) {
    vec3 v_norm = normalize(eye - hit.position);
    vec3 h_norm_1 = normalize(v_norm + to_point_light_norm);
    vec3 h_norm_2 = normalize(v_norm + to_light_dir_norm);
    float si_point = pow(clamp(dot(h_norm_1, hit.normal), 0.0, 1.0), shininess);
    float si_dir = pow(clamp(dot(h_norm_2, hit.normal), 0.0, 1.0), shininess);
    return (si_point * point_light_color + si_dir * light_dir_color) * Ls * Ks;
}

float setAttentuation(vec3 sunCoordinate, Hit hit) {
    float distanceBetweenSunAndPlanet = distance(sunCoordinate, hit.position);
    float distanceBetweenEyeAndPlanet = distance(hit.position, eye);

    float allDistance = distanceRatio * distanceBetweenSunAndPlanet 
        + (1 - distanceRatio) *distanceBetweenEyeAndPlanet;
    //float attenuation = 1 + linearConst * allDistance + quadraticConst * allDistance * allDistance;
    float attenuation = 1 + quadraticConst * allDistance * allDistance;

    return 1 / attenuation;
}

bool thisIsShadow(Hit hit, vec3 sunCoordinate) {
    //calculate Rays
    Ray rayFromSunToHit;
    rayFromSunToHit.startPosition = hit.position + 0.01f * hit.normal;
    rayFromSunToHit.direction = normalize(sunCoordinate - hit.position);

    Ray rayFromTopOfSunToHit;
    rayFromTopOfSunToHit.startPosition = hit.position + 0.01f * hit.normal;
    rayFromTopOfSunToHit.direction = normalize(sunCoordinate + up * spheres[0].w - hit.position);

    Ray rayFromBottomOfSunToHit;
    rayFromBottomOfSunToHit.startPosition = hit.position + 0.01f * hit.normal;
    rayFromBottomOfSunToHit.direction = normalize(sunCoordinate - up * spheres[0].w - hit.position);

    Ray rayFromLeftSideOfSunToHit;
    rayFromLeftSideOfSunToHit.startPosition = hit.position + 0.01f * hit.normal;
    rayFromLeftSideOfSunToHit.direction = normalize(sunCoordinate + vec3(1,0,0) * spheres[0].w - hit.position);

    Ray rayFromRightSideOfSunToHit;
    rayFromRightSideOfSunToHit.startPosition = hit.position + 0.01f * hit.normal;
    rayFromRightSideOfSunToHit.direction = normalize(sunCoordinate - vec3(1,0,0) * spheres[0].w - hit.position);

    //calculate ray to shadow
    Hit firstHitFromHitToSun = firstIntersection(rayFromSunToHit);
    Hit firstHitFromHitToTopOfSun = firstIntersection(rayFromTopOfSunToHit);
    Hit firstHitFromHitToBottomOfSun = firstIntersection(rayFromBottomOfSunToHit);
    Hit firstHitFromHitToLeftSideOfSun = firstIntersection(rayFromBottomOfSunToHit);
    Hit firstHitFromHitToRightSideOfSun = firstIntersection(rayFromRightSideOfSunToHit);

    return !(hit.indexOfSphere == 0 || 
    (firstHitFromHitToSun.distance > 0.0 && firstHitFromHitToSun.indexOfSphere == 0) ||
    (firstHitFromHitToTopOfSun.distance > 0.0 && firstHitFromHitToTopOfSun.indexOfSphere == 0) ||
    (firstHitFromHitToBottomOfSun.distance > 0.0 && firstHitFromHitToBottomOfSun.indexOfSphere == 0) ||
    (firstHitFromHitToLeftSideOfSun.distance > 0.0 && firstHitFromHitToLeftSideOfSun.indexOfSphere == 0) ||
    (firstHitFromHitToRightSideOfSun.distance > 0.0 && firstHitFromHitToRightSideOfSun.indexOfSphere == 0));
}

vec3 lights(Hit hit, vec3 sunCoordinate, bool shadow) {
    // Calculate lights
    // ambient
    vec3 ambient = setAmbientLight();

    // diffuse
    vec3 to_light_dir_norm = normalize(to_light_dir);
    vec3 to_point_light_norm = normalize(to_point_light - hit.position);

    vec3 diffuse = setDiffuseLight(to_light_dir_norm, to_point_light_norm, hit);

    // specular (Phong Blinn)
    vec3 specular = setSpecularLight(hit, to_light_dir_norm, to_point_light_norm);

    float attenuation = setAttentuation(sunCoordinate, hit);
    
    if (shadow) {
        return attenuation * ambient * 0.45f;
    } else {
        return attenuation * (ambient + diffuse + specular);
    }
}

Hit intersect(Ray ray, int indexOfSphere) {
    Hit hit;
    hit.distance = -1.0f;

    vec3 center = spheres[indexOfSphere].xyz;
    float radius = spheres[indexOfSphere].w;

    vec3 poc = ray.startPosition - center; // (p0 - c) vector
    float a = dot(ray.direction, ray.direction);
    float b = 2.0 * dot(poc, ray.direction);
    float c = dot(poc, poc) - radius * radius;
    float delta = b * b - 4.0 * a * c;

    if (delta >= 0.0)
    {
        // Ray intersects with the sphere
        float t1 = (-b + sqrt(delta)) / (2.0 * a);
        float t2 = (-b - sqrt(delta)) / (2.0 * a);

        float t = min(t1, t2); // closest intersection

        if (t > 0.0) {
            // Intersection point
            hit.position = ray.startPosition + t * ray.direction;

            // Normal
            hit.normal = normalize(hit.position - center);

            hit.indexOfSphere = indexOfSphere;
            hit.distance = t;
        }
    }
    return hit;
}

Hit firstIntersection(Ray ray) {
    Hit bestHit;
    bestHit.distance = -1;

    for (int i = 0; i < spheresCount; i++)
    {
        Hit hit = intersect(ray, i);
        if (hit.distance > 0 && (bestHit.distance < 0 || bestHit.distance > hit.distance)) {
            bestHit = hit;
        }
    }
    return bestHit;
}

vec4 getTextureColor(Hit hit,vec2 sphereTexCoords)
{
    vec4 textureColor;
    if(hit.indexOfSphere == 0) 
    {
        textureColor = texture(texImage[0], sphereTexCoords);
    }
    else if(hit.indexOfSphere == 1)
    { 
        textureColor = texture(texImage[1], sphereTexCoords);
    }
    else if(hit.indexOfSphere == 2)
    { 
        textureColor = texture(texImage[2], sphereTexCoords);
    }
    else if(hit.indexOfSphere == 3)
    { 
        textureColor = texture(texImage[3], sphereTexCoords);
    }
    else if(hit.indexOfSphere == 4)
    { 
        textureColor = texture(texImage[4], sphereTexCoords);
    }
    else if(hit.indexOfSphere == 5)
    { 
        textureColor = texture(texImage[5], sphereTexCoords);
    }
    else if(hit.indexOfSphere == 6)
    { 
        textureColor = texture(texImage[6], sphereTexCoords);
    }
    else if(hit.indexOfSphere == 7)
    { 
        textureColor = texture(texImage[7], sphereTexCoords);
    }
    else if(hit.indexOfSphere == 8)
    { 
        textureColor = texture(texImage[8], sphereTexCoords);
    }
    else if(hit.indexOfSphere == 9)
    { 
        textureColor = texture(texImage[9], sphereTexCoords);
    }
    else if(hit.indexOfSphere == 10)
    {
        textureColor = texture(texImage[10], sphereTexCoords);
    }
    return textureColor;
}

vec3 rayTrace(Ray ray, float alfa, float beta, vec3 u, vec3 v, vec3 w) {
    vec3 resultColor = vec3(0, 0, 0);
    vec3 resultColor2 = vec3(0, 0, 0);
    vec3 reflectedColor = vec3(0, 0, 0);
    float intensity = 0.25;
    const float epsilon = 0.0000001f;

    bool isSkyBox = false;

    // sun coordinate
    vec3 sunCoordinate = spheres[0].xyz;
    // ratio between distanceEyeAndPlanet and distanceIntersectionPointAndPlanet
    float distanceRatio = 0.75f;

    bool shadow = false;
    bool water = false;
    for (int d = 0; d <= maxDepth && !isSkyBox; d++) {
        Hit hit = firstIntersection(ray);

        if(hit.distance <= 0.0 && d == 0) {
            // Draw skybox using cubemap texture
            vec3 rayDirection = normalize(alfa * u + beta * v - w);
            vec3 skyboxColor = textureCube(cubemapTexture, rayDirection).rgb;
            resultColor = skyboxColor;
            isSkyBox = true;
            break;
        } else if (hit.distance <= 0.0)
            break; // reflected ray to skybox

        if (d == 0) {
            vec3 center = spheres[hit.indexOfSphere].xyz;
            float radius = spheres[hit.indexOfSphere].w;

            vec3 sphereToIntersection = hit.position - center;
            float u = 0.5 + atan(-sphereToIntersection.z, sphereToIntersection.x) / (2.0 * M_PI);
            float v = 0.5 - asin(sphereToIntersection.y / radius) / M_PI;
            vec2 sphereTexCoords = vec2(u, v);
            
            // Check that the fragment is in shadow or not
            shadow = thisIsShadow(hit, sunCoordinate);

            // Add lights
            resultColor = lights(hit, sunCoordinate, shadow);
            vec4 textureColor = getTextureColor(hit, sphereTexCoords);
            
            // Water reflection
            if (((textureColor.r > 0.12 && textureColor.r < 0.35) || (textureColor.r > 0.62 && textureColor.r < 0.74) || (textureColor.r > 0.90)) &&
            ((textureColor.g > 0.22 && textureColor.g < 0.48) || (textureColor.g > 0.82)) &&
            (textureColor.b > 0.34) || (textureColor.r > 0.78 && textureColor.g > 0.78 && textureColor.b > 0.78)) 
            {
                water = true;
            }

            resultColor *= textureColor.rgb;
            
            // Scale up sun light intensity 
            if (hit.indexOfSphere == 0) {
                resultColor *= 2.5;
                break;
            }
        } else {

            if (shadow) {
                break; // don't calculate reflected lights
            }

            vec3 center = spheres[hit.indexOfSphere].xyz;
            float radius = spheres[hit.indexOfSphere].w;

            vec3 sphereToIntersection = hit.position - center;
            float u = 0.5 + atan(-sphereToIntersection.z, sphereToIntersection.x) / (2.0 * M_PI);
            float v = 0.5 - asin(sphereToIntersection.y / radius) / M_PI;
            vec2 sphereTexCoords = vec2(u, v);
            
            // Check that the fragment is in shadow or not
            shadow = thisIsShadow(hit, sunCoordinate);

            // Add lights
            resultColor2 = lights(hit, sunCoordinate, shadow);
            vec4 textureColor = getTextureColor(hit, sphereTexCoords);
            
            resultColor2 *= textureColor.rgb;

            if (currentScene == 2) {
                reflectedColor += intensity * resultColor2;
            } else {
                reflectedColor += intensity * basicReflectedColor;
            }
            intensity *= intensity;
        }

        ray.startPosition = hit.position + epsilon * hit.normal;
        ray.direction = reflect(ray.direction, hit.normal);
    }

    if (!isSkyBox && (water || currentScene != 0))
        resultColor += reflectedColor;
    return resultColor;
}

void main()
{
    float fovx = radians(60.0);
    float aspect = screen_width / screen_height;

    float alfa = tan(fovx / 2.0) * (gl_FragCoord.x - (screen_width / 2.0)) / (screen_width / 2.0);
    float beta = tan(fovx / 2.0) * ((screen_height / 2.0) - gl_FragCoord.y) / (screen_height / 2.0) / aspect;

    vec3 w = normalize(eye - at);
    vec3 u = normalize(cross(up, w));
    vec3 v = normalize(cross(w, u));

    Ray ray;
    ray.startPosition = eye;
    ray.direction = normalize(alfa * u + beta * v - w);

    vec3 result = rayTrace(ray, alfa, beta, u, v, w);

    fs_out_col = vec4(result, 1.0);
}
