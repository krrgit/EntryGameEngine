#pragma once
#include "Entry/Audio/AudioClip.h"

#include <opusfile.h>

namespace Entry
{
	class WindowsAudioClip : public AudioClip
	{
	public:
		WindowsAudioClip(const std::string& path);
		~WindowsAudioClip();

		virtual void Play() override;
		virtual void Pause() override;
		virtual void Stop() override;

		virtual std::string& GetName() override { return m_Name; }

	private:
		std::string m_Name;
	};
}