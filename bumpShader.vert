#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;

uniform mat4 projection;
uniform mat4 modelview;

out vec3 FragPos;
out vec3 norm;
out vec2 tCoords;
out vec3 Tangent;
out vec3 Bitangent0;

void main()
{
    gl_Position = projection * modelview * vec4(position.x, position.y, position.z, 1.0);

	// once/if we add a modelview for objects
	//FragPos = vec3(modelview * vec4(position, 1.0f));

	FragPos = position;
	norm = normal;
	tCoords = texCoords;

	Tangent = tangent;
	Bitangent0 = bitangent;
}
