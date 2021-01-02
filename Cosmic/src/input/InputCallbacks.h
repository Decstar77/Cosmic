#pragma once
#include "src/Cosmic.h"


namespace cm
{
	class InputCallbacks
	{
	public:
		virtual void MouseCallback() = 0;
		virtual void KeyCallback() = 0;

	public:
		inline static void PostMouseCallbacks()
		{
			for (InputCallbacks *call : callbacks)
			{
				call->MouseCallback();
			}
		}

		inline static void PostKeyCallbacks()
		{
			for (InputCallbacks *call : callbacks)
			{
				call->KeyCallback();
			}
		}

	public:
		InputCallbacks()
		{
			bool added = false;
			for (int32 i = 0; i < callbacks.size(); i++)
			{
				if (callbacks.at(i) == nullptr)
				{
					index = i;
					callbacks.at(i) = this;
					added = true;
					break;
				}
			}
			if (!added)
			{
				index = static_cast<int32>(callbacks.size());
				callbacks.push_back(this);
			}
		}

		virtual ~InputCallbacks()
		{
			callbacks.at(index) = nullptr;
		}

		// @NOTE: I'm sure if this will cause an issue that's why I deleted them
		//		: If needed remove and check nothing bad happens
		InputCallbacks(const InputCallbacks &) = delete;
		InputCallbacks &operator=(const InputCallbacks &) = delete;

	private:
		int32 index;

	private:
		inline static std::vector <InputCallbacks*> callbacks = {};
	};
}