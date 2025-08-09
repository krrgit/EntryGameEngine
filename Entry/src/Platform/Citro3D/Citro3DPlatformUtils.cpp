#include "etpch.h"
#include "Entry/Utils/PlatformUtils.h"
#include "Citro3DWindow.h"

#include <3ds/os.h>


namespace Entry
{
	
	TickCounter C3D_RuntimeTickCounter;
	float C3D_Runtime = 0;
	
	float Time::GetTime()
	{
		osTickCounterUpdate(&C3D_RuntimeTickCounter);
		C3D_Runtime += osTickCounterRead(&C3D_RuntimeTickCounter) * 0.001f;
		return C3D_Runtime;
	}
}