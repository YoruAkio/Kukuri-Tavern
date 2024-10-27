#pragma once

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>
#include <Windows.h>
#endif

#include <Utils/MiscUtils.hpp>
#include <Utils/TextParse.hpp>
#include <Utils/TimingClock.hpp>