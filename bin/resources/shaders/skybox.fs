#version 330 core

#define FRAG_COLOR	0

uniform samplerCube cube_map;

in block
{
	vec3 cube_texcoord;
} Vertex;

layout(location = FRAG_COLOR, index = 0) out vec4 out_FragColor;

void main()
{   
	vec3 diffuse = texture(cube_map, Vertex.cube_texcoord).rgb;
	out_FragColor = vec4(diffuse, 1.0);
}