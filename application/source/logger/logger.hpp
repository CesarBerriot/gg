#include <string>
#include <vector>

namespace logger
{	void message(std::string);
	void warn(std::string);
	struct item { bool severe; std::string message; };
	typedef std::vector<item> log_t;
	log_t pop();
}