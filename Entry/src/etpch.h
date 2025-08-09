#pragma once

#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>
#include <time.h>

#include <string>
#include <array>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "Entry/Core/Log.h"

#include "Entry/Debug/Instrumentor.h"

#ifdef ET_PLATFORM_WINDOWS
	#include <Windows.h>
#endif