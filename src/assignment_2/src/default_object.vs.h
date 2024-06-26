const char* default_object_vs = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 Normal;
out vec3 FragPos;
out vec2 textureCoords;

void main()
{
	FragPos = vec3(model * vec4(aPos, 1.0));
	Normal = mat3(transpose(inverse(model))) * aNormal; // Inverse is costly for gpus, better to calc normal matrix on cpu where possible.
	textureCoords = aTexCoords;

	gl_Position = projection * view * model * vec4(aPos, 1.0);
}
)";