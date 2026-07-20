#pragma once

#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <filesystem>
#include <map>
#include <set>
#include <thread>
#include <chrono>
#include <cstdint>
#include <array>
#include <stack>
#include <queue>
#include <list>
#include <fstream>
#include <mutex>
#include <iomanip>
#include <format>

#include "PulseCode/Core.h"
#include "PulseCode/Log.h"
#include "PulseCode/Theme.h"
#include "PulseCode/Channel.h"
#include "PulseCode/Settings.h"

#include "PulseCode/KeyCodes.h"
#include "PulseCode/MouseButtonCodes.h"

#ifdef PS_PLATFORM_WINDOWS
#include <Windows.h>
#endif
