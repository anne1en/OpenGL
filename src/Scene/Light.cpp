#include "Light.h"
#include "Camera.h"

#include <glm/ext.hpp>

#include <vector>
#include <sstream>

using namespace Scene;
using namespace glm;

Light::Light()
	: Node()
{
	auto lightId = lights.size();
	data = std::unique_ptr<Data>(new Data(lightId));

	data->position_vs = vec3(Camera::getSingleton()->getWorldToViewMatrix() * vec4(position, 1.0f));

	//This is how you work with uniform arrays in shaders
	std::stringstream ss;
	ss << "light[" << lightId << "].position_vs";
	uni_position_vs = std::make_shared<GM::Uniform>(ss.str());
}

void Light::bind(const GM::ShaderPtr &active_program, int location)
{
	//We need to move position to viewspace before we upload it to the uniform.
	//A light is bound from any other node's localRender function, and thus the
	//object_to_world matrix is already prepared. So we use the position in this
	//matrix to get position relative to parent + rotation.
	data->position_vs = vec3(Camera::getSingleton()->getWorldToViewMatrix() * vec4(object_to_world[3][0], object_to_world[3][1], object_to_world[3][2], 1.0));
	uni_position_vs->bind(data->position_vs, active_program->getFS(), location);
}

void Light::unbind()
{
}

int Light::findLocation(unsigned int program)
{
	return uni_position_vs->findLocation(program);
}
