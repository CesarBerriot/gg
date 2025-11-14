#include <functional>
#include <filesystem>
#include <format>
#include <dial/form_base.hpp>
#include <imgui.h>
#include "logger/logger.hpp"
#include "actions/actions.hpp"
#include "command_runner/command_runner.hpp"

static class form : dial::form_base
{	public:
		std::string get_name() override { return "master"; }
		void draw_contents() override;
		void process_logger();
		void draw_working_directory();
		void draw_actions();
		void draw_project_tree();
} instance;

void form::draw_contents()
{	process_logger();
	draw_working_directory();
	draw_actions();
	draw_project_tree();
}

void form::process_logger()
{	for(logger::item item : logger::pop())
		log(item.severe ? log_level::WARNING : log_level::INFO, item.message);
}

void form::draw_working_directory()
{	ImGui::Text("Working Directory :");
	ImGui::SameLine();
	ImGui::TextColored({ 0, 1, 0, 1 }, "%s", std::filesystem::current_path().string().c_str());
	spacer();
}

void form::draw_actions()
{	for(auto & pair : action_map)
		category
		(	(char*)pair.first.c_str(),
			[this, &pair]
			{	bool preceded_by_button = false;
				for(action & action : pair.second)
					if(action.input)
					{	ImGui::InputText(("##" + action.label).c_str(), action.input.buffer->data(), action.input.buffer_size);
						ImGui::SameLine();
						ImGui::BeginDisabled((*action.input).empty() || is_running_asynchronous_task());
						if(ImGui::Button(action.label.c_str()))
						{	run_asynchronous_task(action.make_procedure());
							action.input.buffer->data()[0] = '\0';
						}
						ImGui::EndDisabled();
						preceded_by_button = false;
					}
					else
					{	if(preceded_by_button)
							ImGui::SameLine();
						ImGui::BeginDisabled(is_running_asynchronous_task());
						if(ImGui::Button(action.label.c_str()))
							run_asynchronous_task(action.make_procedure());
						ImGui::EndDisabled();
						preceded_by_button = true;
					}
			}
		);
}

void form::draw_project_tree()
{	category
	(	"Project Tree",
		[this]
		{	if(ImGui::BeginChild("project tree", {}, ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY))
			{	std::function<void(std::filesystem::path)> draw_tree =
					[this, &draw_tree](std::filesystem::path path)
					{	std::vector<std::filesystem::path>
							directories,
							files;
						for(std::filesystem::directory_entry entry : std::filesystem::directory_iterator(path))
							(	entry.is_directory()
								? directories
								: files
							).push_back(entry.path());
						auto draw_paths =
							[this]
							(	std::vector<std::filesystem::path> paths,
								std::function<void(std::filesystem::path)> procedure
							)
							{	for(std::filesystem::path path : paths)
								{	auto button =
										[this, path](std::string label, std::string command)
										{	ImGui::BeginDisabled(is_running_asynchronous_task());
											if(ImGui::Button(label.c_str()))
												run_asynchronous_task
												(	[command, path]
													{	run_command
														(	std::format
															(	"{} \"{}\"",
																command,
																path.string()
															)
														);
													}
												);
											ImGui::EndDisabled();
										};
									ImGui::PushID(path.string().c_str());
									button("Add", "add");
									ImGui::SameLine();
									button("Remove", "rm --cached -r");
									ImGui::SameLine();
									button("Diff", "diff");
									ImGui::SameLine();
									procedure(path);
									ImGui::PopID();
								}

							};
						draw_paths
						(	directories,
							[draw_tree](std::filesystem::path path)
							{	if(ImGui::TreeNode(path.filename().string().c_str()))
								{	draw_tree(path);
									ImGui::TreePop();
								}
							}
						);
						draw_paths
						(	files,
							[](std::filesystem::path path)
							{	ImGui::TextUnformatted(path.filename().string().c_str());
							}
						);
					};
				draw_tree(std::filesystem::current_path());
				ImGui::EndChild();
			}
		}
	);
}
