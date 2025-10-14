#pragma once
#include "Entry/Audio/AudioClip.h"

extern "C" {
#include "miniaudio.h"
}

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
		std::string m_Path;
	private:
		ma_result result;
		ma_decoder_config decoderConfig;
		ma_decoder decoder;
		ma_device_config deviceConfig;
		ma_device device;
		ma_format format;
		ma_uint32 channels;
		ma_uint32 sampleRate;

		//ma_decoding_backend_vtable* pCustomBackendVTables[2];
	};
}