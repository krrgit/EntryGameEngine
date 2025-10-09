#include "etpch.h"
#include "N3DSAudioClip.h"

#include "Entry/Utils/StringUtils.h"

//#define DEBUG

namespace Entry 
{
	// Retrieve strings for libopusfile errors
// Sourced from David Gow's example code: https://davidgow.net/files/opusal.cpp
	const char* opusStrError(int error)
	{
		switch (error)
		{
		case OP_FALSE:
		return "OP_FALSE: A request did not succeed.";
		case OP_HOLE:
		return "OP_HOLE: There was a hole in the page sequence numbers.";
		case OP_EREAD:
		return "OP_EREAD: An underlying read, seek or tell operation "
			"failed.";
		case OP_EFAULT:
		return "OP_EFAULT: A NULL pointer was passed where none was "
			"expected, or an internal library error was encountered.";
		case OP_EIMPL:
		return "OP_EIMPL: The stream used a feature which is not "
			"implemented.";
		case OP_EINVAL:
		return "OP_EINVAL: One or more parameters to a function were "
			"invalid.";
		case OP_ENOTFORMAT:
		return "OP_ENOTFORMAT: This is not a valid Ogg Opus stream.";
		case OP_EBADHEADER:
		return "OP_EBADHEADER: A required header packet was not properly "
			"formatted.";
		case OP_EVERSION:
		return "OP_EVERSION: The ID header contained an unrecognised "
			"version number.";
		case OP_EBADPACKET:
		return "OP_EBADPACKET: An audio packet failed to decode properly.";
		case OP_EBADLINK:
		return "OP_EBADLINK: We failed to find data we had seen before or "
			"the stream was sufficiently corrupt that seeking is "
			"impossible.";
		case OP_ENOSEEK:
		return "OP_ENOSEEK: An operation that requires seeking was "
			"requested on an unseekable stream.";
		case OP_EBADTIMESTAMP:
		return "OP_EBADTIMESTAMP: The first or last granule position of a "
			"link failed basic validity checks.";
		default:
		return "Unknown error.";
		}
	}

	// NDSP audio frame callback
	// This signals the audioThread to decode more things
	// once NDSP has played a sound frame, meaning that there should be
	// one or more available waveBufs to fill with more data.
	void N3DSAudioClip::audioCallback(void* userData)
	{
		auto* self = static_cast<N3DSAudioClip*>(userData);
		self->OnAudioCallback();
	}

	// Audio de-initialisation code
	// Stops playback and frees the primary audio buffer
	void N3DSAudioClip::audioExit(void)
	{
		ndspChnReset(0);
		linearFree(m_AudioBuffer);
	}

	// Audio thread
	// This handles calling the decoder function to fill NDSP buffers as necessary
	void N3DSAudioClip::audioThread(void* userData)
	{
		auto* self = static_cast<N3DSAudioClip*>(userData);
		self->OnAudioThread();
	}

	// Main audio decoding logic
// This function pulls and decodes audio samples from opusFile_ to fill waveBuf_
	bool N3DSAudioClip::fillBuffer(OggOpusFile* opusFile_, ndspWaveBuf* waveBuf_)
	{
#ifdef DEBUG
		// Setup timer for performance stats
		TickCounter timer;
		osTickCounterStart(&timer);
#endif  // DEBUG

		// Decode samples until our waveBuf is full
		int totalSamples = 0;
		while (totalSamples < SAMPLES_PER_BUF)
		{
			int16_t* buffer = waveBuf_->data_pcm16 + (totalSamples *
				CHANNELS_PER_SAMPLE);
			const size_t bufferSize = (SAMPLES_PER_BUF - totalSamples) *
				CHANNELS_PER_SAMPLE;

			// Decode bufferSize samples from opusFile_ into buffer,
			// storing the number of samples that were decoded (or error)
			const int samples = op_read_stereo(opusFile_, buffer, bufferSize);
			if (samples <= 0)
			{
				if (samples == 0) break;  // No error here

				printf("op_read_stereo: error %d (%s)", samples,
					opusStrError(samples));
				break;
			}

			totalSamples += samples;
		}

		// If no samples were read in the last decode cycle, we're done
		if (totalSamples == 0)
		{
			printf("Playback complete, press Start to exit\n");
			return false;
		}

		// Pass samples to NDSP
		waveBuf_->nsamples = totalSamples;
		ndspChnWaveBufAdd(0, waveBuf_);
		DSP_FlushDataCache(waveBuf_->data_pcm16,
			totalSamples * CHANNELS_PER_SAMPLE * sizeof(int16_t));

#ifdef DEBUG
		// Print timing info
		osTickCounterUpdate(&timer);
		printf("fillBuffer %lfms in %lfms\n", totalSamples * 1000.0 / SAMPLE_RATE,
			osTickCounterRead(&timer));
#endif  // DEBUG

		return true;
	}

	N3DSAudioClip::N3DSAudioClip(const std::string& path)
	{
		std::string romfsPath = "romfs:/" + path;


		// Allocate audio buffer
		const size_t bufferSize = WAVEBUF_SIZE * ARRAY_SIZE(m_WaveBufs);
		m_AudioBuffer = (int16_t*)linearAlloc(bufferSize);
		if (!m_AudioBuffer)
		{
			printf("Failed to allocate audio buffer\n");
			return;
		}

		// Setup waveBufs for NDSP
		memset(&m_WaveBufs, 0, sizeof(m_WaveBufs));
		int16_t* buffer = m_AudioBuffer;

		for (size_t i = 0; i < ARRAY_SIZE(m_WaveBufs); ++i)
		{
			m_WaveBufs[i].data_vaddr = buffer;
			m_WaveBufs[i].status = NDSP_WBUF_DONE;

			buffer += WAVEBUF_SIZE / sizeof(buffer[0]);
		}

		// Setup LightEvent for synchronisation of audioThread
		LightEvent_Init(&m_Event, RESET_ONESHOT);

		int error = 0;
		m_OpusFile = op_open_file(romfsPath.c_str(), &error);
		if (error)
		{
			printf("Failed to open file: error %d (%s)\n", error,
				opusStrError(error));
		}

		// Set the ndsp sound frame callback which signals our audioThread
		ndspSetCallback(audioCallback, this);

		fillBuffer(m_OpusFile, &m_WaveBufs[0]);
		fillBuffer(m_OpusFile, &m_WaveBufs[1]);
		fillBuffer(m_OpusFile, &m_WaveBufs[2]);
	}

	N3DSAudioClip::~N3DSAudioClip()
	{
		if (m_OpusFile)
			op_free(m_OpusFile);

		// Cleanup audio things and de-init platform features
		//audioExit();
		//ndspExit();
	}

	void N3DSAudioClip::Play()
	{
		// Set the thread priority to the main thread's priority ...
		int32_t priority = 0x30;
		svcGetThreadPriority(&priority, CUR_THREAD_HANDLE);
		// ... then subtract 1, as lower number => higher actual priority ...
		priority -= 1;
		// ... finally, clamp it between 0x18 and 0x3F to guarantee that it's valid.
		priority = priority < 0x18 ? 0x18 : priority;
		priority = priority > 0x3F ? 0x3F : priority;

		// Start the thread, passing our opusFile as an argument.
		m_ThreadId = threadCreate(audioThread, this,
			THREAD_STACK_SZ, priority,
			THREAD_AFFINITY, false);

	}
	void N3DSAudioClip::Pause()
	{

	}

	void N3DSAudioClip::Stop()
	{
		// Signal audio thread to quit
		m_Quit = true;
		LightEvent_Signal(&m_Event);

		// Free the audio thread
		threadJoin(m_ThreadId, UINT64_MAX);
		threadFree(m_ThreadId);

		m_Quit = false;
	}

	void N3DSAudioClip::OnAudioCallback()
	{
		if (m_Quit)
		{ // Quit flag
			return;
		}

		LightEvent_Signal(&m_Event);
	}

	void N3DSAudioClip::OnAudioThread()
	{
		while (!m_Quit)
		{  // Whilst the quit flag is unset,
		  // search our waveBufs and fill any that aren't currently
		  // queued for playback (i.e, those that are 'done')
			for (size_t i = 0; i < ARRAY_SIZE(m_WaveBufs); ++i)
			{
				if (m_WaveBufs[i].status != NDSP_WBUF_DONE)
				{
					continue;
				}

				if (!fillBuffer(m_OpusFile, &m_WaveBufs[i]))
				{   // Playback complete
					return;
				}
			}

			// Wait for a signal that we're needed again before continuing,
			// so that we can yield to other things that want to run
			// (Note that the 3DS uses cooperative threading)
			LightEvent_Wait(&m_Event);
		}
	}
}