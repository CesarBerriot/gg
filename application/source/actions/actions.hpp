#include <array>
#include <vector>
#include <string>
#include <optional>
#include <functional>
#include <memory>

struct action
{	typedef std::function<void(action)> procedure_t;
	std::string label;
	struct
	{	enum { buffer_size = 250 };
		std::optional<std::array<char, buffer_size>> buffer;
		operator bool() { return buffer.has_value(); }
		std::string operator*() { return buffer->data(); }
	} input;
	procedure_t procedure;
	action(std::string label, procedure_t procedure, bool has_input = false)
	: label(label)
	, procedure(procedure)
	, input({ has_input ? decltype(input.buffer)(decltype(input.buffer)::value_type()) : std::nullopt })
	{}
	std::function<void()> make_procedure() { return std::bind(procedure, *this); }
};

typedef std::vector<action> actions_t;
typedef std::vector<std::pair<std::string, actions_t>> action_map_t;

extern action_map_t action_map;
