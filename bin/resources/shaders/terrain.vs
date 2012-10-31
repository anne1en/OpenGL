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
layout(location = NORMAL) 		in vec3 Normal_os;	//object space
layout(location = TEXCOORD) 	in vec2 TexCoord;

out gl_PerVertex
{
    vec4 gl_Position;
};

out block
{
	vec4 position_ws;	//world space
	vec4 position_vs; 	//view space
	vec3 normal_os; 	//view space
	vec3 normal_vs; 	//view space
	vec3 tangent_vs;	//view space
	vec3 bitangent_vs;	//view space
	vec2 texcoord;
} Vertex;

vec3 approximateTangent(in vec3 normal)
{
	vec3 tangent;
	vec3 c1 = cross(normal, vec3(0.0, 0.0, 1.0)); 
	vec3 c2 = cross(normal, vec3(0.0, 1.0, 0.0)); 
	if(length(c1) > length(c2))
	{
		tangent = c1;	
	}
	else
	{
		tangent = c2;	
	}
	return tangent;
}

void main()
{
	mat3 Object_to_View = mat3(World_to_View * Object_to_World);			//Object space to View space matrix
	
	Vertex.texcoord 		= TexCoord;
	
	//So we do some cheap tricks here to get some things going for us here...
	vec3 Tangent_os			= approximateTangent(Normal_os);				//object space
	vec3 Bitangent_os		= cross(Normal_os, Tangent_os);					//object space

	Vertex.normal_os		= normalize(Normal_os);
	Vertex.normal_vs		= normalize(Normal_to_View * Normal_os);		//Object space to View space
	Vertex.tangent_vs		= normalize(Object_to_View * Tangent_os);		//Object space to View space
	Vertex.bitangent_vs		= normalize(Object_to_View * Bitangent_os);		//Object space to View space
	
	Vertex.position_ws		= Object_to_World * vec4(Position_os, 1.0);		//Object space to World space
	Vertex.position_vs		= World_to_View * Vertex.position_ws;			//World  space to View  space
	gl_Position				= View_to_Clip  * Vertex.position_vs;			//View   space to Clip  space
}