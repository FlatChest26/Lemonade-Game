#include "game_states.h"
#include "game.h"

GameSettings& GameState::get_game_settings() const
{
	return m_game->get_settings();
}

void GameState::update(double delta_time)
{
	if (auto input_handler = get_current_input_handler())
	{
		input_handler->update(delta_time);
	}
}

void GameState::render() const
{
	if (auto input_handler = get_current_input_handler())
	{
		input_handler->render({});
	}

	if (m_renderer)
	{
		m_renderer->render({});
	}
}

#include <ranges>

InputResult GameState::handle_event(SDL_Event event)
{
	for (auto& input_handler : std::views::reverse(m_input_handlers))
	{
		auto result = input_handler->handle_event(event);

		if (result.type != InputResult::Type::None)
		{
			return result;
		}

		if (!input_handler->allow_input_pass_through())
		{
			break;
		}
	}

	return InputResult();
}

void GameState::handle_input_result(InputResult result)
{
	switch (result.type)
	{
	case InputResult::Type::None:
	{
		return;
	}
	case InputResult::Type::Pop_InputHandler:
	{
		pop_input_handler();
		return;
	}
	case InputResult::Type::Push_InputHandler:
	{
		push_input_handler(result.next_input_handler);
		return;
	}
	case InputResult::Type::Reset_InputHandler:
	{
		reset_input_handlers(result.next_input_handler);
		return;
	}
	default:
		return;
	}
}

void GameState::pop_input_handler()
{
	if (m_input_handlers.empty())
	{
		return;
	}

	get_current_input_handler()->remove();
	get_current_input_handler()->set_game_state(nullptr);
	m_input_handlers.pop_back();
}

void GameState::push_input_handler(InputHandler* input_handler)
{
	if (!input_handler)
	{
		return;
	}

	auto new_input_handler = std::unique_ptr<InputHandler>(input_handler);
	m_input_handlers.push_back(std::move(new_input_handler));

	get_current_input_handler()->set_game_state(this);
	get_current_input_handler()->init();
}

void GameState::reset_input_handlers(InputHandler* input_handler)
{
	m_input_handlers.clear();

	if (input_handler)
	{
		push_input_handler(input_handler);
	}
}