#pragma once
#include <iostream>
#include <cstdint>
#include <cctype>
#include <memory>
#include <array>
#include <vector>
#include <stack>
#include <queue>
#include <string>
#include <sstream>
#include <atomic>
#include <thread>

#define Kilobytes(val) (val * 1024LL)
#define Megabytes(val) (Kilobytes(val) * 1024LL)
#define Gigabytes(val) (Megabytes(val) * 1024LL)

#define PURELOG(msg) std::cout << msg << std::endl;
#define PURELOGTS(msg) std::cout << ToString(msg) << std::endl;

#ifdef _DEBUG
#define ASSERT(val, msg)       \
	if (!(val))                \
	{                          \
		Platform::ErrorBox(msg); \
		*(int *)NULL = 0;      \
	}

#if EDITOR
#define LOG(msg) Debug::Log(msg);
#define LOGE(msg) Debug::LogError(msg);
#else
#define LOG(msg) std::cout << msg << std::endl;
#define LOGTS(msg) std::cout << ToString(msg) << std::endl;
#define LOGE(msg) std::cout << "Error: " << msg << std::endl;
#endif

#else
#define LOG(msg)
#define ASSERT(val, msg);
#endif

#ifndef __FUNCTION_NAME__
#ifdef _WIN32   //WINDOWS
#define __FUNCTION_NAME__   __FUNCTION__  
#endif
#endif

#define INTROSPECTED_ENUM(name) enum class name
#define INTROSPECTED_ENUM_FUNCTIONS(name) extern inline String ToString(const name &entry); extern inline name StringTo##name(const String &entry);

#define SET_BIT(x) (1 << x)

#ifndef COSMIC_TYPES
#define COSMIC_TYPES

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef bool bool32;
typedef float real32;
typedef double real64;

typedef std::string String;
typedef std::stringstream StringStream;

typedef std::thread Thread;
typedef std::atomic<bool> AtomicBool;

#include "src/platforms/Platform.h"

#endif