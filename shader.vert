#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

uniform mat4 projection;
uniform mat4 modelview;

out vec3 FragPos;
out vec3 norm;
out vec2 tCoords;

void main()
{
    gl_Position = projection * modelview * vec4(position.x, position.y, position.z, 1.0);

	// once/if we add a modelview for objects
	FragPos = vec3(modelview * vec4(position, 1.0f));

	norm = normalize(mat3(transpose(inverse(modelview))) * normal);

	tCoords = texCoords;
}
