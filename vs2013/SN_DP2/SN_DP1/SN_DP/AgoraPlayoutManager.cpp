#include "stdafx.h"
#include "AgoraPlayoutManager.h"
#include "AGResourceVisitor.h"
#include <string>

CAgoraPlayoutManager::CAgoraPlayoutManager()
	: m_ptrDeviceManager(NULL)
	, m_lpCollection(NULL)
	, m_bTestingOn(FALSE)
{
}


CAgoraPlayoutManager::~CAgoraPlayoutManager()
{
	Close();
}

BOOL CAgoraPlayoutManager::Create(IRtcEngine *lpRtcEngine)
{
	m_ptrDeviceManager = new AAudioDeviceManager(lpRtcEngine);
	if (m_ptrDeviceManager == NULL || m_ptrDeviceManager->get() == NULL)
		return FALSE;

	m_lpCollection = (*m_ptrDeviceManager)->enumeratePlaybackDevices();
	if (m_lpCollection == NULL) {
		delete m_ptrDeviceManager;
		m_ptrDeviceManager = NULL;
	}

	return m_lpCollection != NULL ? TRUE : FALSE;
}

void CAgoraPlayoutManager::Close()
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

UINT CAgoraPlayoutManager::GetVolume()
{
	int nVol = 0;

	if (m_ptrDeviceManager == NULL || m_ptrDeviceManager->get() == NULL)
		return 0;

	(*m_ptrDeviceManager)->getPlaybackDeviceVolume(&nVol);

	return (UINT)nVol;
}

BOOL CAgoraPlayoutManager::SetVolume(UINT nVol)
{
	if (m_ptrDeviceManager == NULL || m_ptrDeviceManager->get() == NULL)
		return 0;

	int nRet = (*m_ptrDeviceManager)->setPlaybackDeviceVolume((int)nVol);

	return nRet == 0 ? TRUE : FALSE;
}

UINT CAgoraPlayoutManager::GetDeviceCount()
{
	if (m_lpCollection == NULL)
		return 0;

	return (UINT)m_lpCollection->getCount();
}

BOOL CAgoraPlayoutManager::GetDevice(std::string rDeviceName, char * rDeviceID)
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

BOOL CAgoraPlayoutManager::GetCurDeviceID(char * szDeviceID)
{
//	CString		str;
//	CHAR		szDeviceID[MAX_DEVICE_ID_LENGTH];
	
	if (m_ptrDeviceManager == NULL || m_ptrDeviceManager->get() == NULL)
		return FALSE;

	(*m_ptrDeviceManager)->getPlaybackDevice(szDeviceID);
// 
// #ifdef UNICODE
// 	::MultiByteToWideChar(CP_UTF8, 0, szDeviceID, -1, str.GetBuffer(MAX_DEVICE_ID_LENGTH), MAX_DEVICE_ID_LENGTH);
// 	str.ReleaseBuffer();
// #else
// 	strDeviceName = szDeviceID;
// #endif

	return TRUE;
}

BOOL CAgoraPlayoutManager::SetCurDevice(char* lpDeviceID)
{
	if (m_ptrDeviceManager == NULL || m_ptrDeviceManager->get() == NULL)
		return FALSE;

// #ifdef UNICODE
// 	CHAR szDeviceID[128];
// 	::WideCharToMultiByte(CP_ACP, 0, lpDeviceID, -1, szDeviceID, 128, NULL, NULL);
// 	int nRet = (*m_ptrDeviceManager)->setPlaybackDevice(szDeviceID);
// #else
// #endif
	int nRet = (*m_ptrDeviceManager)->setPlaybackDevice(lpDeviceID);

	return nRet == 0 ? TRUE : FALSE;
}

void CAgoraPlayoutManager::TestPlaybackDevice(UINT nWavID, BOOL bTestOn)
{
	TCHAR	szWavPath[MAX_PATH];

	if (m_ptrDeviceManager == NULL || m_ptrDeviceManager->get() == NULL)
		return;

	::GetModuleFileName(NULL, szWavPath, MAX_PATH);
	LPTSTR lpLastSlash = (LPTSTR)_tcsrchr(szWavPath, _T('\\')) + 1;
	_tcscpy_s(lpLastSlash, 16, _T("test.wav"));

	if (bTestOn && !m_bTestingOn) {
		CAGResourceVisitor::SaveResourceToFile(_T("WAVE"), nWavID, szWavPath);

#ifdef UNICODE
		CHAR szWavPathA[MAX_PATH];

		::WideCharToMultiByte(CP_ACP, 0, szWavPath, -1,szWavPathA , MAX_PATH, NULL, NULL);
		(*m_ptrDeviceManager)->startPlaybackDeviceTest(szWavPathA);
#else
		(*m_ptrDeviceManager)->startPlaybackDeviceTest(szWavPathA);
#endif
	}
	else if (!bTestOn && m_bTestingOn)
		(*m_ptrDeviceManager)->stopPlaybackDeviceTest();

	m_bTestingOn = bTestOn;

}