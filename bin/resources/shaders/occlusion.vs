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

layout(location = POSITION) 	in vec3 Position_os;	//object space

out gl_PerVertex
{
    vec4 gl_Position;
};

void main()
{
	gl_Position	= View_to_Clip  * World_to_View * Object_to_World * vec4(Position_os,1.0);
}