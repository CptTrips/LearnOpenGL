#version 330 core
layout(location = 0) in vec3 aPos; // the position variable has attribute position 0
//layout(location = 1) in vec3 aColor; // color variable has attrib pos 1
layout(location = 1) in vec2 aTexCoord; // tex coord variable has attrib pos 2

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


out vec3 ourColor; // specify a color output to the fragment shader
out vec2 texCoord;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0f); 
    texCoord = aTexCoord;
}
