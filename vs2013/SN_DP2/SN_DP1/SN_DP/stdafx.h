// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // �� Windows ͷ���ų�����ʹ�õ�����
// Windows ͷ�ļ�: 
#include <windows.h>

// C ����ʱͷ�ļ�
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

// TODO:  �ڴ˴����ó�����Ҫ������ͷ�ļ�
// #include "IAgoraMediaEngine.h"
// #include "IAgoraRtcEngine.h"
#include "AGEngineEventHandler.h"
//#define APP_ID "570465840e604903811de2f3a72d174b"
#define APP_ID "aab8b8f5a8cd4469a63042fcfafe7063"

#include <iostream>
#include <atlstr.h>