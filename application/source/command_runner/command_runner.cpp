#include "command_runner.hpp"
#include <sub_process.hpp>
#include "logger/logger.hpp"

void run_command(std::string command)
{	try
	{	sub_process::result result = sub_process::run("git", command);
		auto log_stream =
			[](decltype(logger::message) procedure, std::string stream)
			{	if(!stream.empty())
					procedure('\n' + stream);
			};
		log_stream(logger::message, result.cout);
		log_stream(logger::warn, result.cerr);
	}
	catch(const std::exception exception)
	{	logger::warn(std::string("exception : ") + exception.what());
	}
}
