const char* default_object_fs = R"(
#version 330 core
struct Material {
//	vec3 ambient; // These vectors are colour vectors, defining the colour under those types of lighting.
//	vec3 diffuse;
//	vec3 specular;

	// Replace vec3s with sampler2Ds to take in a texture as the material.
	sampler2D diffuse;
	sampler2D specular;
	
	float shininess;
};

uniform Material material; // Take in the material struct.

struct Light {
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	// For attenuation.
	float constant;
	float linear;
	float quadratic;
};

uniform Light light;

out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 textureCoords;

//uniform vec3 objectColor;
//uniform vec3 lightColor;
//uniform vec3 lightPosition;
uniform vec3 viewPosition;

uniform sampler2D myTexture;

void main()
{
// Ambient lighting.
	//float ambientStrength = 0.1;
	//vec3 ambient = light.ambient * material.ambient;
	vec3 ambient = light.ambient * texture(material.diffuse, textureCoords).rgb;

// Diffuse lighting.
	vec3 norm = normalize(Normal); // Only direction is really needed in lighting, remember to normalize!
	vec3 lightDir = normalize(light.position - FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * texture(material.diffuse, textureCoords).rgb;

// Specular lighting.
	//float specularStrength = 0.5;
	vec3 viewDir = normalize(viewPosition - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm); // Negate lightdir because reflect expects it to point from the light to the fragmen, but it is calculated the other way arorund for the diffuse.

	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess); // Last constant is the shininess.
	vec3 specular = light.specular * spec * texture(material.specular, textureCoords).rgb;

	// Simple attenuation, from the chapter on light casters.
	float distance = length(light.position - FragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	// Don't affect ambient with attenuation since it shouldn always be there and not be affected by distance.
	diffuse *= attenuation;
	specular *= attenuation;

	vec3 result = (ambient + diffuse + specular); //* objectColor;
	FragColor = vec4(result, 1.0);
}
)";
