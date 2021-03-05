#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices=3) out;

in VS_OUT {
	vec3 normal;
	vec3 world_pos;
	vec2 tex_coord;
} gs_in[];

/*
out GS_OUT {
	vec2 tex_coord;
	vec3 normal;
} gs_out;
*/

out vec2 tex_coord;
out vec3 normal;
out vec3 world_pos;

uniform float time;

vec3 screenspace_normal()
{
   vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
   vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
   return normalize(cross(a, b));
} 

vec4 explode(vec4 pos, vec3 normal)
{
	vec3 displacement = 0.1*(sin(time)+1.)*normal;
	return pos + vec4(displacement, 0.);
}

void main()
{
	vec3 normal = screenspace_normal();
	for (int i=0; i<3; i++)
	{
		gl_Position = explode(gl_in[i].gl_Position, normal);
		tex_coord = gs_in[i].tex_coord;
		normal = gs_in[i].normal;
		world_pos = gs_in[i].world_pos;
		EmitVertex();
	}
	EndPrimitive();
}
