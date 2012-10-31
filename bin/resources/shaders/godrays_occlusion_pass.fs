#version 330 core
#define DIFFUSE		0
#define POSITION	1
#define NORMAL		2
#define TEXCOORD	3
#define FRAG_COLOR	0

//
//THIS SHADER IS NOT IN USE!
//

uniform sampler2D TEX_DIFF; 

in block
{
	vec2 texcoord; //TexCoord
} Vertex;

layout(location = FRAG_COLOR, index = 0) out vec4 out_FragColor;

vec4 extractHighlights() 
{ 
	float fThreshold = 0.99;
	vec4 c = texture(TEX_DIFF, Vertex.texcoord);
	c = clamp((c - fThreshold), 0.0, 1.0);
	if(c.r > 0.0 && c.g > 0.0 && c.b > 0.0) //Light occluders are 0
		c = vec4(1.0,1.0,1.0,1.0); //Lights are 1
	return c;
}

void main()
{
    out_FragColor = extractHighlights();
}