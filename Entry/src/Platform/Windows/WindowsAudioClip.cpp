#include "etpch.h"
#include "WindowsAudioClip.h"

#include <extras/decoders/libopus/miniaudio_libopus.c>
//#include <extras/decoders/libvorbis/miniaudio_libvorbis.c>

namespace Entry
{
    void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
    {
        ma_data_source* pDataSource = (ma_data_source*)pDevice->pUserData;
        if (pDataSource == NULL)
        {
            return;
        }

        ma_data_source_read_pcm_frames(pDataSource, pOutput, frameCount, NULL);

        (void)pInput;
    }

	WindowsAudioClip::WindowsAudioClip(const std::string& path)
	{
		m_Name = path;
		m_Path = path;

		ma_decoding_backend_vtable* pCustomBackendVTables[] = {
			ma_decoding_backend_libopus
		};

        /* Initialize the decoder. */
        decoderConfig = ma_decoder_config_init_default();
        decoderConfig.pCustomBackendUserData = NULL;  /* None of our decoders require user data, so this can be set to null. */
        decoderConfig.ppCustomBackendVTables = pCustomBackendVTables;
        decoderConfig.customBackendCount = sizeof(pCustomBackendVTables) / sizeof(pCustomBackendVTables[0]);

        result = ma_decoder_init_file(path.c_str(), &decoderConfig, &decoder);
        if (result != MA_SUCCESS)
        {
            printf("Failed to initialize decoder.");
            return;
        }

        ma_data_source_set_looping(&decoder, MA_FALSE);

        /* Initialize the device. */
        result = ma_data_source_get_data_format(&decoder, &format, &channels, &sampleRate, NULL, 0);
        if (result != MA_SUCCESS)
        {
            printf("Failed to retrieve decoder data format.");
            ma_decoder_uninit(&decoder);
            return;
        }

        deviceConfig = ma_device_config_init(ma_device_type_playback);
        deviceConfig.playback.format = format;
        deviceConfig.playback.channels = channels;
        deviceConfig.sampleRate = sampleRate;
        deviceConfig.dataCallback = data_callback;
        deviceConfig.pUserData = &decoder;

        if (ma_device_init(NULL, &deviceConfig, &device) != MA_SUCCESS)
        {
            printf("Failed to open playback device.\n");
            ma_decoder_uninit(&decoder);
            return;
        }
	}

	WindowsAudioClip::~WindowsAudioClip()
	{
        ET_CORE_INFO("STOP AUDIO");
        ma_device_uninit(&device);
        ma_decoder_uninit(&decoder);
	}

	void WindowsAudioClip::Play()
	{
		if (ma_device_start(&device) != MA_SUCCESS)
		{
			printf("Failed to start playback device.\n");
			ma_device_uninit(&device);
			ma_decoder_uninit(&decoder);
			return;
		}
	}

	void WindowsAudioClip::Pause()
	{
        ma_device_stop(&device);
	}
	void WindowsAudioClip::Stop()
	{
        ma_device_stop(&device);
        ma_decoder_seek_to_pcm_frame(&decoder, 0);
	}
}