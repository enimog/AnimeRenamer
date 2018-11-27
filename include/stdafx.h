// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//
#pragma once
#include <iostream>

// windows stuff
#if defined(_WIN32) || defined(WIN32)
    #include <SDKDDKVer.h>
    #include <windows.h>
#else
// linux stuff
#include <limits.h>
    #define INFINITE 0xFFFFFFFF
    #define MAX_PATH PATH_MAX
#endif

#ifndef UNICODE
    #define UNICODE
#endif
#ifndef _UNICODE
    #define _UNICODE
#endif

// common std includes
#include <cstdlib>
#include <cstddef>
#include <memory>
#include <vector>
#include <string>
#include <map>
#include <chrono>

// Personnal stuff
#include "StringToolbox.h"
#include "Logger.h"
