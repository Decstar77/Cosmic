#pragma once
#include "src/Cosmic.h"
#include "src/math/CosmicMath.h"

namespace cm
{
	enum class KeyCode
	{
		LEFT_SHIFT = 0xA0,
		RIGHT_SHIFT = 0xA1,
		LEFT_CONTROL = 0xA2,
		RIGHT_CONTROL = 0xA3,
		SPACE = 0x20,
		A = 0x41,
		C = 0x43,
		D = 0x44,
		E = 0x45,
		R = 0x52,
		T = 0x54,
		S = 0x53,
		V = 0x56,
		W = 0x57,
		X = 0x58,
		Q = 0x51,
		Y = 0x59,
		Z = 0x5A,
		F1 = 0x70,
		F2 = 0x7,
		F3 = 0x72,
		F4 = 0x73,
		F5 = 0x74,
		F6 = 0x75,
		F7 = 0x76,
		F8 = 0x77,
		F9 = 0x78,
		F10 = 0x79,
		F11 = 0x7A,
		F12 = 0x7B,
		TILDE = 0xC0
	};

#define COSMIC_KEY_COUNT 256

	class KeyInput
	{
	public:
		static bool GetKeyHeldDown(const int32 &key);
		static bool GetKeyJustDown(const int32 &key);
		static bool GetKeyJustUp(const int32 &key);
		static bool GetKeyHeldDown(const KeyCode &code);
		static bool GetKeyJustDown(const KeyCode &code);
		static bool GetKeyJustUp(const KeyCode &code);

		static void SetKeyState(const int32 &key, const bool &state);

		static void SetLastFromCurrent();

	public:
		KeyInput() = delete;
		KeyInput(const KeyInput &) = delete;
		KeyInput &operator=(const KeyInput &) = delete;
		~KeyInput() {};

	private:
		inline static std::array<bool, COSMIC_KEY_COUNT> current_keys = {};
		inline static std::array<bool, COSMIC_KEY_COUNT> last_keys = {};

	};
}
