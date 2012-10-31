#pragma once


#include <memory>
#include <vector>

namespace GM
{
	class VAO;
	typedef std::shared_ptr<VAO> VAOPtr;
	
	class VAO
	{
	public:
		VAO();
		~VAO();

		void bind();
		void unbind();

	private:
		unsigned int handle;
	};
}
