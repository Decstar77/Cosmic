#pragma once
#include "src/Cosmic.h"
#include "src/math/CosmicMath.h"
#include "vendor/irrKlang/include/irrKlang.h"

namespace cm
{
	class Speaker
	{
	public:
		Speaker();
		~Speaker();

	public:
		void PlaySound(String sound_name);
		void PlayerSoundAtPoint(String sound_name, Vec3f pos);
	};

	class SoundSystem
	{
	public:
		void RegisterSpeaker(Speaker *speaker);
		void DeregisterSpeaker(Speaker *speaker);

		void SetListenerTransform(const Transform &transform);

		inline static SoundSystem *GetInstance() { return instance; }

	public:
		SoundSystem();
		~SoundSystem();

		irrklang::ISoundEngine* engine;
	private:
		//std::vector<Speaker*> speaker;
		inline static SoundSystem *instance = nullptr;

	};


}