#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
  
uniform mat4 view;
uniform mat4 model;
uniform mat4 projection;
uniform vec2 TexCoordShift;


void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f);
    TexCoords = texCoord + TexCoordShift;
    FragPos = (model * vec4(position, 1.0)).xyz;
    Normal = mat3(transpose(inverse(model))) * normal;
}