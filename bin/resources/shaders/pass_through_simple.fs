#version 330 core

#define FRAG_COLOR	0

uniform struct SLight
{
	vec3 position_vs; //view space
} light[1];

uniform vec3 CamPos_vs; //view space

uniform sampler2D diffuse_map;
uniform sampler2D normal_map;

in block
{
	vec4 position_ws;	//world space
	vec4 position_vs; 	//view space
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
	vec3 Normal_ts = texture(normal_map, Vertex.texcoord).rgb * 2.0 - 1.0; //Tangent Space
	
	vec3 frontNormal_vs = gl_FrontFacing ? Vertex.normal_vs : -Vertex.normal_vs;
	vec3 frontTangent_vs = gl_FrontFacing ? Vertex.tangent_vs : -Vertex.tangent_vs;
	vec3 frontBitangent_vs = gl_FrontFacing ? Vertex.bitangent_vs : -Vertex.bitangent_vs;
	
	vec3 Normal_vs = Normal_ts.x * frontTangent_vs - Normal_ts.y * frontBitangent_vs + Normal_ts.z * frontNormal_vs; //Tangent space to View space
	vec3 N = Normal_vs;

	vec3 light_pos_vs = light[0].position_vs; //view space
	vec3 L = normalize(light_pos_vs - Vertex.position_vs.xyz); //view space
	vec3 V = normalize(CamPos_vs - Vertex.position_vs.xyz); //view space
	
	float NdotL = max(dot(N,L), 0.0);
	float shininess = 0.5;
	float term = compute_gauss_term(N, L, V, NdotL, shininess);

	vec3 diffuse = texture(diffuse_map, Vertex.texcoord).rgb;
	if(diffuse == vec3(0.0,0.0,0.0))
		diffuse = vec3(1.0,1.0,1.0);

	const float LOG2 = 1.442695;
	float fog_density = 0.0085;
	vec3 fog_color = vec3(0.231372,0.258823,0.286274);
	float z = gl_FragCoord.z / gl_FragCoord.w;
	float fog_factor = exp2( -fog_density*fog_density * z*z * LOG2 );
	fog_factor = clamp(fog_factor, 0.0, 1.0);
    
	vec3 final_color = vec3( 
		(diffuse*(NdotL) + 
		 (term) + 
		 (vec3(0.1, 0.1, 0.1))));

	out_FragColor = vec4( 
		mix(fog_color, final_color, fog_factor), 
		1.0);
}
