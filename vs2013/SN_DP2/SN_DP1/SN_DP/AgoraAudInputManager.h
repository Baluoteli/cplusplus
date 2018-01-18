#pragma once
#include <string>
class CAgoraAudInputManager
{
public:
	CAgoraAudInputManager();
	~CAgoraAudInputManager();

	BOOL Create(IRtcEngine *lpRtcEngine);
	void Close();

	UINT GetVolume();
	BOOL SetVolume(UINT nVol);
	UINT GetDeviceCount();

	BOOL GetCurDeviceID(char* szDeviceID);
	BOOL SetCurDevice(char* lpDeviceID);

	BOOL GetDevice(std::string rDeviceName, char * rDeviceID);
	
	void TestAudInputDevice(HWND hMsgWnd, BOOL bTestOn);

	BOOL IsTesting() { return m_bTestingOn; };
private:
	BOOL						m_bTestingOn;
	HWND						m_hOldMsgWnd;
	AAudioDeviceManager			*m_ptrDeviceManager;
	IAudioDeviceCollection		*m_lpCollection;
};

