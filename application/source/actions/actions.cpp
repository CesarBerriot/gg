#include "actions.hpp"
#include <filesystem>
#include <format>
#include <fstream>
#include <string_theory/string>
#include <cmrc/cmrc.hpp>
#include "command_runner/command_runner.hpp"
#include "logger/logger.hpp"

CMRC_DECLARE(resources);

static action::procedure_t command_single(std::string command)
{	return
		[command](...)
		{	run_command(command);
		};
}

static action::procedure_t command_multi(std::vector<std::string> commands)
{	return
		[commands](...)
		{	for(std::string command : commands)
				run_command(command);
		};
}

static action::procedure_t commit(bool amend)
{	return
		[amend](action action)
		{	if(ST::string(*action.input).contains('"'))
				logger::warn("Commit messages can't contain quotes.");
			else
				run_command
				(	std::format
					(	"commit{} -m \"{}\"",
						amend ? " --amend" : "",
						*action.input
					)
				);
		};
}

static action::procedure_t resource(std::string resource_name, std::optional<std::string> file_name = {})
{	if(!file_name)
		file_name = resource_name;
	return
		[resource_name, file_name](...)
		{	if(std::filesystem::exists(*file_name))
				logger::warn("File already exists.");
			else
			{	cmrc::file data = cmrc::resources::get_filesystem().open("resource/" + resource_name);
				std::ofstream(*file_name, std::ios::binary).write(data.begin(), data.end() - data.begin());
			}
		};
}

static action::procedure_t directory(std::string name)
{	return
		[name](...)
		{	if(std::filesystem::exists(name))
				logger::warn
				(	std::filesystem::is_directory(name)
						? "Directory already exists."
						: "Directory location contains a file."
				);
			else
				std::filesystem::create_directory(name);
		};
}

action_map_t action_map =
	{	{	"Git Actions",
			{	{ "Init", command_single("init") },
				{ "Status", command_single("status") },
				{ "Version", command_single("--version") },
				{ "Push", command_single("push origin master") },
				{	"Reset Master And Force Push",
					command_multi
					(	{	"checkout --orphan new",
							"branch -D master",
							"branch -m master",
							"commit -m -",
							"push origin master -f"
						}
					)
				},
				{ "Commit", commit(false), true },
				{ "Amend", commit(true), true },
				{ "Log", command_single("log") },
				{ "Log Latest", command_single("log -n 1") },
				{ "Log 5", command_single("log -n 5") },
				{ "Soft Reset Latest", command_single("reset HEAD~1") },
				{ "Hard Reset", command_single("reset --hard") }
			}
		},
		{	"File Template Actions",
			{	{ "Read Me", resource("readme.md") },
				{ "Apache License 2.0", resource("Apache License 2.0", "LICENSE") },
				{ "Executable CMakeLists", resource("cmake/executable.txt", "CMakeLists.txt") },
				{ "Library CMakeLists", resource("cmake/library.txt", "CMakeLists.txt") },
			}
		},
		{	"Directory Template Actions",
			{	{ "Include", directory("include") },
				{ "Source", directory("source") },
				{ "Resource", directory("resource") },
				{ "Screenshot", directory("screenshot") },
			}
		},
	};

