#version 330 core
#define DIFFUSE		0
#define POSITION	1
#define NORMAL		2
#define TEXCOORD	3
#define FRAG_COLOR	0

uniform sampler2D TEX_DIFF;
uniform sampler2D TEX_OCCLUSION;

uniform vec2 lightPosition_cs; //Clip Space

in block
{
	vec2 texcoord; //TexCoord
} Vertex;

layout(location = FRAG_COLOR, index = 0) out vec4 out_FragColor;

const int NUM_SAMPLES = 300;
const float exposure = 1.0;
const float decay    = 0.895;
const float density  = 1.75;
const float weight   = 0.075;

vec4 godraysCombine()
{
	vec2 deltaTextCoord = vec2( Vertex.texcoord - lightPosition_cs );
	vec2 textCoo = Vertex.texcoord;
	deltaTextCoord *= 1.0 /  float(NUM_SAMPLES) * density;
	float illuminationDecay = 1.0;
	vec4 scatter_diffuse = vec4(0.0,0.0,0.0,0.0);
	
	for(int i=0; i < NUM_SAMPLES ; i++)
	{
			textCoo -= deltaTextCoord;
			vec4 sample = texture(TEX_OCCLUSION, textCoo );
			if(sample.w > 0.0)
			{
				sample *= illuminationDecay * weight;
			
				scatter_diffuse += sample;
			}
			
			illuminationDecay *= decay;
	}
	
	
	scatter_diffuse *= exposure;
	return vec4(texture(TEX_DIFF, Vertex.texcoord).rgb + (scatter_diffuse.rgb*0.5), 1.0);
	//return vec4(scatter_diffuse.rgb, 1.0);
}

void main()
{
    out_FragColor = godraysCombine();
	//out_FragColor = texture(TEX_OCCLUSION, Vertex.texcoord);
}