#pragma once

#include "Entry/Core/Core.h"
#include <string>

namespace Entry
{
	class AudioClip
	{
	public:
		virtual ~AudioClip() = default;

		virtual void Play() = 0;
		virtual void Pause() = 0;
		virtual void Stop() = 0;

		virtual std::string& GetName() = 0;

		static Ref<AudioClip> Create(const std::string& path);
	private:

	};
}