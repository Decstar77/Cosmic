#include "SoundSystem.h"



namespace cm
{
	static inline irrklang::vec3df CreateIrrKlangVec3(const Vec3f &vec)
	{
		return irrklang::vec3df(vec.x, vec.y, vec.z);
	}

	void SoundSystem::SetListenerTransform(const Transform &transform)
	{
		engine->setListenerPosition(
			CreateIrrKlangVec3(transform.position),
			CreateIrrKlangVec3(transform.GetBasis().forward));
	}

	SoundSystem::SoundSystem()
	{
		engine = irrklang::createIrrKlangDevice();
		if (engine)
		{
			instance = this;

			//Footsteps_Casual_Metal_04
		}
		else
		{
			Platform::ErrorBox("Could not create sound engine");
		}
	}

	SoundSystem::~SoundSystem()
	{
		engine->drop();
	}


	Speaker::Speaker()
	{

	}

	Speaker::~Speaker()
	{

	}

	void Speaker::PlaySound(String sound_name)
	{
		String path = "../res/sounds/" + sound_name;
		auto track = SoundSystem::GetInstance()->engine->play2D(path.c_str(), false, false, true);

		ASSERT(track, "Speaker could not find sound !!");

		track->setVolume(0.75f);
	}

	void Speaker::PlayerSoundAtPoint(String sound_name, Vec3f pos)
	{
		auto track = SoundSystem::GetInstance()->engine->play3D("../res/sounds/ophelia.mp3",
			irrklang::vec3df(0, 0, 0), true, false, true);
		track->setMinDistance(10);
	}

}