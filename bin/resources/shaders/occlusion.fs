#version 330 core

#define FRAG_COLOR	0

layout(location = FRAG_COLOR, index = 0) out vec4 out_FragColor;

void main()
{   
	out_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
}
