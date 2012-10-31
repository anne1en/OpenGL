#version 330 core

#define FRAG_COLOR	0

uniform struct SLight
{
	vec3 position_vs; //view space
} light[1];

uniform vec3 CamPos_vs; //view space

uniform sampler2D height_map;
uniform sampler2D rock_diffuse_map;
uniform sampler2D rock_normal_map;
uniform sampler2D sand_diffuse_map;
uniform sampler2D sand_normal_map;

in block
{
	vec4 position_ws;	//world space
	vec4 position_vs; 	//view space
	vec3 normal_os; 	//view space
	vec3 normal_vs; 	//view space
	vec3 tangent_vs;	//view space
	vec3 bitangent_vs;	//view space
	vec2 texcoord;
} Vertex;

layout(location = FRAG_COLOR, index = 0) out vec4 out_FragColor;

float compute_phong_term(in vec3 N, in vec3 L, in vec3 V, in float NdotL, in float shininess)
{
	vec3 R = reflect(N, -L);
	float term = max(clamp(dot(V,R), 0.0, 1.0), 0.0);
	term = pow(term, shininess);
	return term;
}

float compute_blinn_term(in vec3 N, in vec3 L, in vec3 V, in float NdotL, in float shininess)
{
	vec3 H = normalize(L + V);
	float term = dot(N,H);
	term = max(clamp(term, 0.0, 1.0), 0.0);
	term = pow(term, shininess);
	return term;
}

float compute_gauss_term(in vec3 N, in vec3 L, in vec3 V, in float NdotL, in float shininess)
{
	vec3 H = normalize(L+V);
	float ANH = acos(dot(N,H));
	float exponent = ANH / shininess;
	exponent = -(exponent*exponent);
	float term = max(exp(exponent), 0.0);
	return term;
}

void main()
{   
	vec3 Normal_ts;
	vec3 diffuse;
	if(Vertex.normal_os.y > 0.6 && Vertex.position_ws.y < 20.0)
	{
		Normal_ts = texture(sand_normal_map, Vertex.texcoord*512.0).rgb * 2.0 - 1.0; //Tangent Space
		diffuse = texture(sand_diffuse_map, Vertex.texcoord*512.0).rgb;
	}
	else
	{
		Normal_ts = texture(rock_normal_map, Vertex.texcoord*512.0).rgb * 2.0 - 1.0; //Tangent Space
		diffuse = texture(rock_diffuse_map, Vertex.texcoord*512.0).rgb;
	}
	
	vec3 frontNormal_vs = gl_FrontFacing ? Vertex.normal_vs : -Vertex.normal_vs;
	vec3 frontTangent_vs = gl_FrontFacing ? Vertex.tangent_vs : -Vertex.tangent_vs;
	vec3 frontBitangent_vs = gl_FrontFacing ? Vertex.bitangent_vs : -Vertex.bitangent_vs;
	
	vec3 Normal_vs = Normal_ts.x * frontTangent_vs - Normal_ts.y * frontBitangent_vs + Normal_ts.z * frontNormal_vs; //Tangent space to View space
	vec3 N = Vertex.normal_vs + Normal_vs;

	vec3 light_pos_vs = light[0].position_vs; //view space
	vec3 L = normalize(light_pos_vs - Vertex.position_vs.xyz); //view space
	vec3 V = normalize(CamPos_vs - Vertex.position_vs.xyz); //view space
	
	float NdotL = max(dot(N,L), 0.0);
	float shininess = 0.2;
	float term = compute_gauss_term(N, L, V, NdotL, shininess);

	const float LOG2 = 1.442695;
	float fog_density = 0.0085;
	vec3 fog_color = vec3(0.231372,0.258823,0.286274);
	float z = gl_FragCoord.z / gl_FragCoord.w;
	float fog_factor = exp2( -fog_density*fog_density * z*z * LOG2 );
	fog_factor = clamp(fog_factor, 0.0, 1.0);

	//diffuse + specular + ambient
	vec3 terrain_color =  (diffuse*(NdotL) + (vec3(0.2,0.2,0.2)*term) + (vec3(0.1, 0.1, 0.1)));
    
	out_FragColor = vec4( 
		mix(fog_color, terrain_color, fog_factor), 
		1.0);
}
