#include "stdafx.h"
#include "AgoraAudInputManager.h"
#include <string>

CAgoraAudInputManager::CAgoraAudInputManager()
	: m_ptrDeviceManager(NULL)
	, m_lpCollection(NULL)
	, m_bTestingOn(FALSE)
{
}


CAgoraAudInputManager::~CAgoraAudInputManager()
{
	Close();
}

BOOL CAgoraAudInputManager::Create(IRtcEngine *lpRtcEngine)
{
	m_ptrDeviceManager = new AAudioDeviceManager(lpRtcEngine);
	if (m_ptrDeviceManager == NULL || m_ptrDeviceManager->get() == NULL)
		return FALSE;

	m_lpCollection = (*m_ptrDeviceManager)->enumerateRecordingDevices();
	if (m_lpCollection == NULL) {
		delete m_ptrDeviceManager;
		m_ptrDeviceManager = NULL;
	}

	
	return m_lpCollection != NULL ? TRUE : FALSE;
}

void CAgoraAudInputManager::Close()
{
	if (m_lpCollection != NULL){
		m_lpCollection->release();
		m_lpCollection = NULL;
	}

	if (m_ptrDeviceManager != NULL) {
		delete m_ptrDeviceManager;
		m_ptrDeviceManager = NULL;
	}
}

UINT CAgoraAudInputManager::GetVolume()
{
	int nVol = 0;

	if (m_ptrDeviceManager == NULL || m_ptrDeviceManager->get() == NULL)
		return 0;

	(*m_ptrDeviceManager)->getRecordingDeviceVolume(&nVol);

	return (UINT)nVol;
}

BOOL CAgoraAudInputManager::SetVolume(UINT nVol)
{
	if (m_ptrDeviceManager == NULL || m_ptrDeviceManager->get() == NULL)
		return FALSE;

	int nRet = (*m_ptrDeviceManager)->setRecordingDeviceVolume((int)nVol);

	return nRet == 0 ? TRUE : FALSE;
}

UINT CAgoraAudInputManager::GetDeviceCount()
{
	if (m_lpCollection != NULL)
		return (UINT)m_lpCollection->getCount();

	return 0;
}

BOOL CAgoraAudInputManager::GetDevice(std::string rDeviceName, char * rDeviceID)
{
	CHAR szDeviceName[MAX_DEVICE_ID_LENGTH];
	CHAR szDeviceID[MAX_DEVICE_ID_LENGTH];

	if (m_lpCollection == NULL)
		return FALSE;

	//	ASSERT(nIndex < GetDeviceCount());
	//if (nIndex >= GetDeviceCount())
	//	return FALSE;

	int nDeviceCount = GetDeviceCount();
	for (size_t i = 0; i < nDeviceCount; i++)
	{
		memset(szDeviceName, 0, MAX_DEVICE_ID_LENGTH);
		int nRet = m_lpCollection->getDevice(i, szDeviceName, szDeviceID);
		if (nRet == 0)
		{
			int len = MultiByteToWideChar(CP_UTF8, 0, szDeviceName, -1, NULL, 0);
			wchar_t wstr[MAX_DEVICE_ID_LENGTH] = { 0 };
			MultiByteToWideChar(CP_UTF8, 0, szDeviceName, -1, wstr, len);
			len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
			WideCharToMultiByte(CP_ACP, 0, wstr, -1, szDeviceName, len, NULL, NULL);
			std::string a = szDeviceName;
			if (a == rDeviceName)
			{
				len = MultiByteToWideChar(CP_UTF8, 0, szDeviceID, -1, NULL, 0);
				memset(wstr, 0, MAX_DEVICE_ID_LENGTH);
				MultiByteToWideChar(CP_UTF8, 0, szDeviceID, -1, wstr, len);
				len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
				WideCharToMultiByte(CP_ACP, 0, wstr, -1, rDeviceID, len, NULL, NULL);
				return TRUE;
			}
		}
	}


	int nRet = m_lpCollection->getDevice(0, szDeviceName, szDeviceID);
	if (nRet != 0)
		return FALSE;
// 
// 	int len = MultiByteToWideChar(CP_UTF8, 0, szDeviceName, -1, NULL, 0);
// 	wchar_t wstr[MAX_DEVICE_ID_LENGTH] = { 0 };
// 	MultiByteToWideChar(CP_UTF8, 0, szDeviceName, -1, wstr, len);
// 	len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
// 	WideCharToMultiByte(CP_ACP, 0, wstr, -1, rDeviceName, len, NULL, NULL
	int len = MultiByteToWideChar(CP_UTF8, 0, szDeviceID, -1, NULL, 0);
	wchar_t wstr[MAX_DEVICE_ID_LENGTH] = { 0 };
	MultiByteToWideChar(CP_UTF8, 0, szDeviceID, -1, wstr, len);
	len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, wstr, -1, rDeviceID, len, NULL, NULL);
// #ifdef UNICODE
// 	::MultiByteToWideChar(CP_UTF8, 0, szDeviceName, -1, rDeviceName.GetBuffer(MAX_DEVICE_ID_LENGTH), MAX_DEVICE_ID_LENGTH);
// 	::MultiByteToWideChar(CP_UTF8, 0, szDeviceID, -1, rDeviceID.GetBuffer(MAX_DEVICE_ID_LENGTH), MAX_DEVICE_ID_LENGTH);
// 
// 	rDeviceName.ReleaseBuffer();
// 	rDeviceID.ReleaseBuffer();
// #else
// 	strDeviceName = szDeviceName;
// 	strDeviceID = szDeviceID;
// #endif

	return TRUE;
}

BOOL CAgoraAudInputManager::GetCurDeviceID(char* szDeviceID)
{
//	CString		str;
//	CHAR		szDeviceID[MAX_DEVICE_ID_LENGTH];
	
	if (m_ptrDeviceManager == NULL || m_ptrDeviceManager->get() == NULL)
		return FALSE;

	(*m_ptrDeviceManager)->getRecordingDevice(szDeviceID);

// #ifdef UNICODE
// 	::MultiByteToWideChar(CP_UTF8, 0, szDeviceID, -1, str.GetBuffer(MAX_DEVICE_ID_LENGTH), MAX_DEVICE_ID_LENGTH);
// 	str.ReleaseBuffer();
// #else
// 	strDeviceName = szDeviceID;
// #endif

	return TRUE;
}

BOOL CAgoraAudInputManager::SetCurDevice(char* lpDeviceID)
{
	if (m_ptrDeviceManager == NULL || m_ptrDeviceManager->get() == NULL)
		return FALSE;

// #ifdef UNICODE
// 	CHAR szDeviceID[128];
// 	::WideCharToMultiByte(CP_ACP, 0, lpDeviceID, -1, szDeviceID, 128, NULL, NULL);
// 	int nRet = (*m_ptrDeviceManager)->setRecordingDevice(szDeviceID);
// #else
// 	int nRet = (*m_ptrDeviceManager)->setRecordingDevice(lpDeviceID);
// #endif
	int nRet = (*m_ptrDeviceManager)->setRecordingDevice(lpDeviceID);

	return nRet == 0 ? TRUE : FALSE;
}



//For test audio input device
void CAgoraAudInputManager::TestAudInputDevice(HWND hMsgWnd, BOOL bTestOn)
{
// 	if (m_ptrDeviceManager == NULL || m_ptrDeviceManager->get() == NULL)
// 		return;
// 
// 	if (bTestOn && !m_bTestingOn) {
// 		m_hOldMsgWnd = CAgoraObject::GetAgoraObject()->GetMsgHandlerWnd();
// 		CAgoraObject::GetAgoraObject()->SetMsgHandlerWnd(hMsgWnd);
// 
// 		IRtcEngine *lpRtcEngine = CAgoraObject::GetEngine();
// 		RtcEngineParameters rep(*lpRtcEngine);
// 		rep.enableAudioVolumeIndication(1000, 10);
// 		(*m_ptrDeviceManager)->startRecordingDeviceTest(1000);
// 	}
// 	else if (!bTestOn && m_bTestingOn){
// 		CAgoraObject::GetAgoraObject()->SetMsgHandlerWnd(m_hOldMsgWnd);
// 		(*m_ptrDeviceManager)->stopRecordingDeviceTest();
// 	}

	m_bTestingOn = bTestOn;

}