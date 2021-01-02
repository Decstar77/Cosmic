#include "KeyInput.h"


namespace cm
{
	bool KeyInput::GetKeyHeldDown(const int32 &key)
	{
		return current_keys.at(key);
	}

	bool KeyInput::GetKeyHeldDown(const KeyCode &code)
	{
		return GetKeyHeldDown(static_cast<int32>(code));
	}

	bool KeyInput::GetKeyJustDown(const int32 &key)
	{
		return current_keys.at(key) && !last_keys.at(key);
	}

	bool KeyInput::GetKeyJustDown(const KeyCode &code)
	{
		return GetKeyJustDown(static_cast<int32>(code));
	}

	bool KeyInput::GetKeyJustUp(const int32 &key)
	{
		return !current_keys.at(key) && last_keys.at(key);
	}

	bool KeyInput::GetKeyJustUp(const KeyCode &code)
	{
		return GetKeyJustUp(static_cast<int32>(code));
	}

	void KeyInput::SetKeyState(const int32 &key, const bool &state)
	{
		current_keys.at(key) = state;
	}

	void KeyInput::SetLastFromCurrent()
	{
		for (int32 i = 0; i < current_keys.size(); i++)
		{
			last_keys.at(i) = current_keys.at(i);
		}
	}

}