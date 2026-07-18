#pragma once

#ifdef PS_PLATFORM_WINDOWS
#elif defined(PS_PLATFORM_LINUX)
#else
	#error Pulse Studio only supports Windows and Linux for now!
#endif

#ifdef PS_ENABLE_ASSERTS
	#define PS_ASSERT(x, ...) { if(!(x)) { PS_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#define PS_CORE_ASSERT(x, ...) { if(!(x)) { PS_CORE_ERROR(__VA_ARGS__); __debugbreak(); } }
#else
	#define PS_ASSERT(x, ...)
	#define PS_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)

#define PS_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }
