#pragma once


#include <memory>
#include <vector>

namespace GM
{
	class GPUTimer;
	typedef std::shared_ptr<GPUTimer> GPUTimerPtr;
	
	class GPUTimer
	{
	public:
		GPUTimer();
		~GPUTimer();

		void begin();
		void end();
		int getTimeElapsed();
		bool isResultAvailable();

	private:
		unsigned int handle;
	};
}
