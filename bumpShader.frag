#version 330 core

struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 FragPos;
in vec3 norm;
in vec2 tCoords;
in vec3 Tangent;
in vec3 Bitangent0;

// For the object
uniform sampler2D tex;
uniform sampler2D normalMap;
uniform float matShininess;

// For the scene
uniform vec3 viewPos;
uniform DirLight dirLight;

out vec4 color;

// Function prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcBumpedNormal();

void main()
{
	// Properties
    vec3 viewDir = normalize(viewPos - FragPos);

	vec3 n = CalcBumpedNormal();

	vec3 result = CalcDirLight(dirLight, n, viewDir);

	color = vec4(result, 1.0);            // texture with lighting
	
    //color = texture(tex, tCoords);        // texture no lighting
	//color = vec4(1.0f, 1.0f, 0.0f, 1.0f); // yellow
	//color = vec4(1.0f, norm);             // normal coloring
}

// Calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);

    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), matShininess);

    // Combine results
    vec3 ambient = light.ambient * vec3(texture(tex, tCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(tex, tCoords));
    vec3 specular = light.specular * spec * vec3(texture(tex, tCoords));

    return (ambient + diffuse + specular);
}

vec3 CalcBumpedNormal()
{
    vec3 Normal = normalize(norm);
    vec3 Tangent = normalize(Tangent);
    Tangent = normalize(Tangent - dot(Tangent, Normal) * Normal);
    vec3 Bitangent = cross(Tangent, Normal);
    vec3 BumpMapNormal = texture(normalMap, tCoords).xyz;
    BumpMapNormal = 2.0 * BumpMapNormal - vec3(1.0, 1.0, 1.0);
    vec3 NewNormal;
    mat3 TBN = mat3(Tangent, Bitangent, Normal);
    NewNormal = TBN * BumpMapNormal;
    NewNormal = normalize(NewNormal);
    return NewNormal;
}