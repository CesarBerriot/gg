#include <filesystem>
#include <fstream>
#include <dial/form_base.hpp>
#include <imgui.h>
#include <cmrc/cmrc.hpp>
#include <shlobj.h>
#include <windows.h>
#include <string_theory/string>
#include <string_theory/format>

CMRC_DECLARE(resources);

static class form : dial::form_base
{	private:
		std::vector<std::string> directory = { "C:\\Program Files", "CesarBerriot", "GG" };
		std::string file = "gg.exe";
		struct
		{	HKEY key = HKEY_LOCAL_MACHINE;
			char
				* sub_key = "SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment",
				* value = "Path";
			DWORD
				read_flags = RRF_RT_REG_SZ | RRF_RT_REG_EXPAND_SZ | RRF_NOEXPAND,
				write_type = REG_EXPAND_SZ;
		} system_path_reg_data;
	private:
		std::string get_name() override { return "master"; }
		bool should_draw_log() override { return false; }
		void draw_contents() override;
		void draw_status();
		void draw_action_buttons();
		bool check_installation();
		std::filesystem::path compute_directory_path();
		std::filesystem::path compute_complete_path();
		void install();
		void uninstall();
		std::string read_system_path();
		void write_system_path(std::string);
} instance;

void form::draw_contents()
{	if(IsUserAnAdmin())
	{	draw_status();
		draw_action_buttons();
	}
	else
		ImGui::TextColored({ 1, 0, 0, 1 }, MODULE_NAME " requires\nadministrator privileges.");
}

void form::draw_status()
{	bool installed = check_installation();
	ImGui::Text("Status :");
	ImGui::SameLine();
	ImGui::TextColored
	(	{ (float)!installed, (float)installed, 0, 1 },
		installed ? "Installed" : "Not Installed"
	);
}

void form::draw_action_buttons()
{	if(check_installation())
	{	if(ImGui::Button("Update"))
			install();
		ImGui::SameLine();
		if(ImGui::Button("Uninstall"))
			uninstall();
		ImGui::SameLine();
		if(ImGui::Button("Run"))
		{	STARTUPINFOA startup_information;
			GetStartupInfoA(&startup_information);
			PROCESS_INFORMATION process_information;
			if(!CreateProcessA(compute_complete_path().string().c_str(), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &startup_information, &process_information))
				throw std::runtime_error("Process creation failure.");
			CloseHandle(process_information.hThread);
			CloseHandle(process_information.hProcess);
		}
	}
	else
		if(ImGui::Button("Install"))
			install();
}

bool form::check_installation()
{	return std::filesystem::is_regular_file(compute_complete_path());
}

std::filesystem::path form::compute_directory_path()
{	std::filesystem::path path = directory[0];
	for(int i = 1; i < directory.size(); i++)
		path /= directory[i];
	return path;
}

std::filesystem::path form::compute_complete_path()
{	return compute_directory_path() / file;
}

void form::install()
{	if(check_installation())
		uninstall();
	std::filesystem::path path = directory[0];
	for(int i = 1; i < directory.size(); i++)
	{	path /= directory[i];
		if(!std::filesystem::is_directory(path))
			std::filesystem::create_directory(path);
	}
	path /= file;
	cmrc::file data = cmrc::resources::get_filesystem().open("gg.exe");
	std::ofstream(path.string(), std::ios::binary).write(data.begin(), data.size());

	write_system_path
	(	ST::format("{};{};", read_system_path(), compute_directory_path().string().c_str())
		.replace(";;", ";")
		.to_std_string()
	);
}

void form::uninstall()
{	std::filesystem::remove(compute_complete_path());
	std::filesystem::remove(compute_directory_path());
	write_system_path
	(	ST::string(read_system_path())
		.replace(compute_directory_path().string(), "")
		.replace(";;", ";")
		.to_std_string()
	);
}

std::string form::read_system_path()
{	DWORD buffer_size = 0;
	if
	(	RegGetValueA
		(	system_path_reg_data.key,
			system_path_reg_data.sub_key,
			system_path_reg_data.value,
			system_path_reg_data.read_flags,
			NULL,
			NULL,
			&buffer_size
		) != ERROR_SUCCESS
	)
		throw std::runtime_error("PATH key size obtention failure.");
	std::unique_ptr<char[]> buffer = std::make_unique<char[]>(buffer_size);
	if
	(	RegGetValueA
		(	system_path_reg_data.key,
			system_path_reg_data.sub_key,
			system_path_reg_data.value,
			system_path_reg_data.read_flags,
			NULL,
			buffer.get(),
			&buffer_size
		) != ERROR_SUCCESS
	)
		throw std::runtime_error("PATH obtention failure.");
	return buffer.get();
}

void form::write_system_path(std::string new_value)
{	if
	(	RegSetKeyValueA
		(	system_path_reg_data.key,
			system_path_reg_data.sub_key,
			system_path_reg_data.value,
			system_path_reg_data.write_type,
			new_value.c_str(),
			new_value.length() + 1
		) != ERROR_SUCCESS
	)
		throw std::runtime_error("PATH modification failure.");
}
