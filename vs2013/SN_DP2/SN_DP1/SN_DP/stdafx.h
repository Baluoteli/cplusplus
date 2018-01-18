// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料
// Windows 头文件: 
#include <windows.h>

// C 运行时头文件
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <stdint.h>


#include <IAgoraRtcEngine.h>
#include <IAgoraMediaEngine.h>
#pragma comment(lib, "agora_rtc_sdk.lib")
using namespace agora::util;
using namespace agora::media;
#pragma comment(lib,"../debug/PlayerHookerV6.lib");

// TODO:  在此处引用程序需要的其他头文件
// #include "IAgoraMediaEngine.h"
// #include "IAgoraRtcEngine.h"
#include "AGEngineEventHandler.h"
//#define APP_ID "570465840e604903811de2f3a72d174b"
#define APP_ID "aab8b8f5a8cd4469a63042fcfafe7063"

#include <iostream>
#include <atlstr.h>