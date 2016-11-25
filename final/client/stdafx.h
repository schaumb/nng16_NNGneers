// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

#ifdef WIN32
#include <winsock2.h>
#undef min
#undef max
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <ctime>
#include <cstring>
#include <climits>


inline uint32_t GetTickCount() {
    struct timespec ts;
    unsigned theTick = 0U;
    clock_gettime( CLOCK_REALTIME, &ts );
    theTick  = ts.tv_nsec / 1000000;
    theTick += ts.tv_sec * 1000;
    return theTick;
}
#endif

#include <cassert>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <ctime>
#include <map>

