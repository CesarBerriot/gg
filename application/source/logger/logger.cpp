#include "logger.hpp"
#include <mutex>

namespace logger
{	static log_t log;
	static std::mutex mutex;

	static void push(item item)
	{	mutex.lock();
		log.push_back(item);
		mutex.unlock();
	}

	void message(std::string message) { push({ false, message }); }

	void warn(std::string message) { push({ true, message }); }

	log_t pop()
	{	mutex.lock();
		log_t copy = log;
		log.clear();
		mutex.unlock();
		return copy;
	}
}
