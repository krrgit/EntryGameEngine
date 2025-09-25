#include "etpch.h"
#include "LightLut.h"

namespace Entry
{
	void IntArrayToIvec4Array(uint32_t* in, glm::ivec4* out)
	{
		printf("int array: \n");
		for (int i = 0; i < 256; i++)
		{
			int group = i / 4;
			int comp = i % 4;
			out[group][comp] = in[i];
			printf("%d ", in[i]);
		}
		printf("\n");

	}

	void ET_LightLut_FromArray(LightLut* lut, float* data)
	{
		int i;
		for (i = 0; i < 256; i++)
		{
			float in = data[i], diff = data[i + 256];

			uint32_t val = 0;
			if (in > 0.0f)
			{
				in *= 0x1000;
				val = (in < 0x1000) ? (uint32_t)in : 0xFFF;
			}

			uint32_t val2 = 0;
			if (diff != 0.0f)
			{
				if (diff < 0)
				{
					diff = -diff;
					val2 = 0x800;
				}
				diff *= 0x800;
				val2 |= (diff < 0x800) ? (uint32_t)diff : 0x7FF;
			}

			lut->data[i] = val | (val2 << 12);
		}
	}

	void ET_LightLut_FromFunc(LightLut* lut, LightLutFunc func, float param, bool negative)
	{
		printf("float array:\n");
		int i;
		float data[512];
		memset(data, 0, sizeof(data));
		int min = negative ? (-128) : 0;
		int max = negative ? 128 : 256;
		for (i = min; i <= max; i++)
		{
			float x = (float)i / max;
			float val = func(x, param);
			int   idx = negative ? (i & 0xFF) : i;
			if (i < max)
				data[idx] = val;
			if (i > min)
				data[idx + 255] = val - data[idx - 1];

			printf("%.2f ", val);
		}
		printf("\n");
		ET_LightLut_FromArray(lut, data);
	}

	void ET_LightLutDA_Create(LightLutDA* lut, LightLutFuncDA func, float from, float to, float arg0, float arg1)
	{
		int i;
		float data[512];

		float range = to - from;
		lut->scale = 1.0f / range;
		lut->bias = -from * lut->scale;

		for (i = 0; i <= 256; i++)
		{
			float x = from + range * i / 256.0f;
			float val = func(x, arg0, arg1);
			if (i < 256)
				data[i] = val;
			if (i > 0)
				data[i + 255] = val - data[i - 1];

		}

		ET_LightLut_FromArray(&lut->lut, data);
	}
}