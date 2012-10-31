#version 330 core
#define DIFFUSE		0
#define POSITION	1
#define NORMAL		2
#define TEXCOORD	3
#define FRAG_COLOR	0

const float blurSize = 2.05/1024.0;

uniform sampler2D TEX_BLOOM;

in block
{
	vec2 texcoord; //TexCoord
} Vertex;

layout(location = FRAG_COLOR, index = 0) out vec4 out_FragColor;

vec4 gaussianFilter() 
{ 
	vec4 sum = vec4(0.0);
	vec2 t = Vertex.texcoord;

	// blur in s (horizontal)
	// take nine samples, with the distance blurSize between them
	sum += texture(TEX_BLOOM, vec2(t.s - 4.0*blurSize,		t.t)) * 0.05;
	sum += texture(TEX_BLOOM, vec2(t.s - 3.0*blurSize,		t.t)) * 0.09;
	sum += texture(TEX_BLOOM, vec2(t.s - 2.0*blurSize,		t.t)) * 0.12;
	sum += texture(TEX_BLOOM, vec2(t.s - 	   blurSize,	t.t)) * 0.15;
	sum += texture(TEX_BLOOM, vec2(t.s, 					t.t)) * 0.16;
	sum += texture(TEX_BLOOM, vec2(t.s + 	   blurSize,	t.t)) * 0.15;
	sum += texture(TEX_BLOOM, vec2(t.s + 2.0*blurSize,		t.t)) * 0.12;
	sum += texture(TEX_BLOOM, vec2(t.s + 3.0*blurSize,		t.t)) * 0.09;
	sum += texture(TEX_BLOOM, vec2(t.s + 4.0*blurSize,		t.t)) * 0.05;
	
	return sum;
}

void main()
{
    out_FragColor = gaussianFilter();
}