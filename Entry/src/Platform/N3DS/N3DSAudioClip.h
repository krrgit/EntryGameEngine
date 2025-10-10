#pragma once

#include "Entry/Audio/AudioClip.h"

#include <opusfile.h>
#include <3ds.h>

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

static const int SAMPLE_RATE = 48000;            // Opus is fixed at 48kHz
static const int SAMPLES_PER_BUF = SAMPLE_RATE * 120 / 1000;  // 120ms buffer
static const int CHANNELS_PER_SAMPLE = 2;        // We ask libopusfile for
// stereo output; it will down
// -mix for us as necessary.

static const int THREAD_AFFINITY = -1;           // Execute thread on any core
static const int THREAD_STACK_SZ = 32 * 1024;    // 32kB stack for audio thread

static const size_t WAVEBUF_SIZE = SAMPLES_PER_BUF * CHANNELS_PER_SAMPLE
* sizeof(int16_t);                           // Size of NDSP wavebufs

namespace Entry
{
	class N3DSAudioClip : public AudioClip
	{
	public:
		N3DSAudioClip(const std::string& path);
		~N3DSAudioClip();

		virtual void Play() override;
		virtual void Pause() override;
		virtual void Stop() override;

		virtual std::string& GetName() override { return m_Name; }

	private:
		static void audioCallback(void* userData);
		static void audioThread(void* userData);
		void audioExit(void);
		bool fillBuffer(OggOpusFile* opusFile_, ndspWaveBuf* waveBuf_);

		// Actual instance logic
		void OnAudioCallback();
		void OnAudioThread();

	private:
		std::string m_Name;

		OggOpusFile* m_OpusFile = nullptr;
		Thread m_ThreadId = nullptr;

		ndspWaveBuf m_WaveBufs[3];
		int16_t* m_AudioBuffer = NULL;
		LightEvent m_Event;
		bool m_Quit = false;  // Quit flag
	};
}