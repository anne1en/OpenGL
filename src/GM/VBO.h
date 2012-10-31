#pragma once

#include <GL3/gl3w.h>

#include <memory>
#include <vector>

namespace GM
{
	class VBO;
	typedef std::shared_ptr<VBO> VBOPtr;
	
	class VBO
	{
	public:
		VBO(const unsigned int &size, const unsigned int &draw_type);
		~VBO();

		void bind();
		void unbind();
		unsigned int getHandle() const { return handle; }
		unsigned int getSize() const { return size; }

		template<class T>
		unsigned int buffer(const std::vector<T> &data)
		{
			// http://www.opengl.org/sdk/docs/man4/xhtml/glBufferSubData.xml
			glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(T)*data.size(), &data[0]);
			unsigned int return_offset = offset;
			offset += sizeof(T)*data.size();
			return return_offset;
		}
	
	private:
		unsigned int handle;
		unsigned int offset;
		unsigned int size;
	};
}
