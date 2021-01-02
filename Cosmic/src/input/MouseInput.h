#pragma once
#include "src/Cosmic.h"
#include "src/math/CosmicMath.h"

namespace cm
{
	enum class MouseCode
	{
		LEFT_MOUSE_BUTTON = 0x0,
		RIGHT_MOUSE_BUTTON = 0x1,
		MIDDLE_MOUSE_BUTTON = 0x2,
	};

	class MouseInput
	{
	public:
		static bool IsRawInput();
		static bool IsEnabled();

		static void EnableMouse();
		static void DisableMouse();
		static void EnableRawInput();
		static void DisableRawInput();

		static bool GetMouseJustDown(const MouseCode &code);
		static bool GetMouseHeldDown(const MouseCode &code);
		static bool GetMouseJustUp(const MouseCode &code);
		static bool GetMouseUp(const MouseCode &code);

		static void SetLastFromCurrent();
		static void SetMouseButtonState(const MouseCode &code, const bool &state);

		static Vec2f GetMouseDelta();
		static void SetMouseDelta(const Vec2f &d);

		static Vec2f GetLastPosition();
		static void SetLastPosition(const Vec2f &val);

		static Vec2f GetCurrentPosition();
		static void SetCurrentPosition(const Vec2f &val);

	public:
		MouseInput() = delete;
		MouseInput(const MouseInput &) = delete;
		MouseInput &operator=(const MouseInput &) = delete;
		~MouseInput() {};

	public:
		inline static bool enabled = false;
		inline static bool raw_input = false;
		inline static std::array<bool, 3> current_buttons = {};
		inline static std::array<bool, 3> last_buttons = {};

		inline static Vec2f current_position = Vec2f(0);
		inline static Vec2f last_position = Vec2f(0);
		inline static Vec2f delta = Vec2f(0);

		friend Platform;
	};
}