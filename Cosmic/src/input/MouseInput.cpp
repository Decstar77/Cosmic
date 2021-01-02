#include "MouseInput.h"


namespace cm
{

	bool MouseInput::IsRawInput()
	{
		return raw_input;
	}

	bool MouseInput::IsEnabled()
	{
		return enabled;
	}

	void MouseInput::EnableMouse()
	{
		enabled = true;
	}

	void MouseInput::DisableMouse()
	{
		enabled = false;
	}

	void MouseInput::EnableRawInput()
	{
		raw_input = true;
	}

	void MouseInput::DisableRawInput()
	{
		raw_input = false;
	}

	bool MouseInput::GetMouseJustDown(const MouseCode &code)
	{
		int32 index = static_cast<int32>(code);

		bool result = current_buttons[index] && !last_buttons[index];

		return result;
	}

	bool MouseInput::GetMouseHeldDown(const MouseCode &code)
	{
		bool result = current_buttons[static_cast<int32>(code)];

		return result;
	}

	bool MouseInput::GetMouseJustUp(const MouseCode &code)
	{
		int32 index = static_cast<int32>(code);

		bool result = !current_buttons[index] && last_buttons[index];

		return result;
	}

	bool MouseInput::GetMouseUp(const MouseCode &code)
	{
		bool result = !current_buttons[static_cast<int32>(code)];

		return result;
	}

	void MouseInput::SetLastFromCurrent()
	{
		for (int32 i = 0; i < last_buttons.size(); i++)
		{
			last_buttons.at(i) = current_buttons.at(i);
		}
	}

	void MouseInput::SetMouseButtonState(const MouseCode &code, const bool &state)
	{
		int32 index = static_cast<int32>(code);

		current_buttons[index] = state;
	}

	Vec2f MouseInput::GetMouseDelta()
	{
		return delta;
	}

	void MouseInput::SetMouseDelta(const Vec2f &d)
	{
		delta = d;
	}

	Vec2f MouseInput::GetLastPosition()
	{
		return last_position;
	}

	void MouseInput::SetLastPosition(const Vec2f &val)
	{
		last_position = val;
	}

	Vec2f MouseInput::GetCurrentPosition()
	{
		return current_position;
	}

	void MouseInput::SetCurrentPosition(const Vec2f &val)
	{
		current_position = val;
	}

}