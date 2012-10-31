#version 330 core

#define DIFFUSE		0
#define POSITION	1
#define NORMAL		2
#define TEXCOORD	3
#define TANGENT		4
#define BITANGENT	5

uniform mat4 Object_to_World;
uniform mat4 World_to_View;
uniform mat4 View_to_Clip;
uniform mat3 Normal_to_View;

layout(location = POSITION) 	in vec3 Position_os;	//object space
layout(location = NORMAL) 		in vec3 Normal_os;		//object space
layout(location = TANGENT) 		in vec3 Tangent_os;		//object space
layout(location = BITANGENT) 	in vec3 Bitangent_os;	//object space
layout(location = TEXCOORD) 	in vec2 TexCoord;
layout(location = DIFFUSE) 		in vec3 Color;

out gl_PerVertex
{
    vec4 gl_Position;
};

out block
{
	vec4 position_ws;	//world space
	vec4 position_vs; 	//view space
	vec3 normal_vs; 	//view space
	vec3 tangent_vs;	//view space
	vec3 bitangent_vs;	//view space
	vec2 texcoord;
} Vertex;

void main()
{
	mat3 Object_to_View = mat3(World_to_View * Object_to_World);			//Object space to View space matrix
	
	Vertex.texcoord 		= TexCoord;
	
	Vertex.normal_vs		= normalize(Normal_to_View * Normal_os);		//Object space to View space
	Vertex.tangent_vs		= normalize(Object_to_View * Tangent_os);		//Object space to View space
	Vertex.bitangent_vs		= normalize(Object_to_View * Bitangent_os);		//Object space to View space
	
	Vertex.position_ws		= Object_to_World * vec4(Position_os, 1.0);		//Object space to World space
	Vertex.position_vs		= World_to_View * Vertex.position_ws;			//World  space to View  space
	gl_Position				= View_to_Clip  * Vertex.position_vs;			//View   space to Clip  space
}