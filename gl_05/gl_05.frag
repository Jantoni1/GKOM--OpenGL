#version 330 core


in vec3 FragPos;  
in vec3 Normal;  
in vec2 TexCoords;

out vec4 color;

uniform sampler2D Texture;
uniform float ambientIntensity;

void main()
{
    color = ambientIntensity * texture(Texture, TexCoords);
}
