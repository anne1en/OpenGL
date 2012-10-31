#version 330 core

#define FRAG_COLOR	0

uniform sampler2D TEX_DIFF;

in block
{
	vec2 texcoord; //TexCoord
} Vertex;

layout(location = FRAG_COLOR, index = 0) out vec4 out_FragColor;

void main( void )
{
	vec3 diffuse = texture( TEX_DIFF, Vertex.texcoord ).rgb;
	out_FragColor = vec4(diffuse, 1.0);
}
