#version 330 core
layout(location = 0) in vec3 aPos; // the position variable has attribute position 0
layout(location = 1) in vec3 aColor; // color variable has attrib pos 1
uniform vec3 offset;
out vec3 ourColor; // specify a color output to the fragment shader

void main()
{
    gl_Position = vec4(aPos + offset, 1.0); 
    ourColor = vec3(abs(aPos.x), abs(aPos.y), abs(aPos.z));
}
