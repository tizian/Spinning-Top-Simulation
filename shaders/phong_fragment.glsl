#version 150

uniform mat4 model;
uniform vec3 cameraPosition;

uniform struct Material {
	vec3 Ks;
	vec3 Kd;
	vec3 Ka;
	float shininess;
} material;

uniform struct Light {
	vec3 position;
	vec3 Ls;
	vec3 Ld;
	vec3 La;
	float attenuation;
} light;

//uniform sampler2D tex;

// In: per fragment information
in vec3 fPosition;
in vec3 fNormal;
//in vec2 fTexCoord;

// Out: final color of this fragmant
out vec4 fragColor;	

void main() {
	vec3 normal = normalize(transpose(inverse(mat3(model))) * fNormal);
	vec3 surfacePosition = vec3(model * vec4(fPosition, 1));
	//vec4 surfaceColor = texture(tex, fTexCoord);
	vec3 surfaceToLight = normalize(light.position - surfacePosition);
	vec3 surfaceToCamera = normalize(cameraPosition - surfacePosition);
    vec3 halfway = normalize(surfaceToCamera + surfaceToLight);

	// Ambient intensity
	vec3 Ia = light.La * material.Ka;

	// Diffuse intensity
	float NL = max(0.0, dot(normal, surfaceToLight));
	vec3 Id = light.Ld * material.Kd * NL;

	// specular intensity
	float RVexp = 0.0;
	if (NL > 0)
		RVexp = pow(max(0.0, dot(halfway, normal)), material.shininess);
	vec3 Is = light.Ls * material.Ks * RVexp;

	// Attenuation
//	float distanceToLight = length(light.position - surfacePosition);
//	float att = 1.0 / (1.0 + light.attenuation * pow(distanceToLight, 2));
    float att = 1.0;

	// Linear color (color before gamma correction)
	vec3 linearColor = Ia + att*(Id + Is);

	fragColor = vec4(linearColor, 1.0);
}