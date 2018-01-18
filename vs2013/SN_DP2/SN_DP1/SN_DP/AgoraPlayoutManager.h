#pragma once
#include <string>
class CAgoraPlayoutManager
{
public:
	CAgoraPlayoutManager();
	~CAgoraPlayoutManager();

	BOOL Create(IRtcEngine *lpRtcEngine);
	void Close();

	UINT GetVolume();
	BOOL SetVolume(UINT nVol);
	UINT GetDeviceCount();

	BOOL GetCurDeviceID(char * szDeviceID);
	BOOL SetCurDevice(char* lpDeviceID);

	BOOL GetDevice(std::string rDeviceName, char* rDeviceID);
	void TestPlaybackDevice(UINT nWavID, BOOL bTestOn = TRUE);

	BOOL IsTesting() { return m_bTestingOn; };
private:
	AAudioDeviceManager			*m_ptrDeviceManager;
	IAudioDeviceCollection		*m_lpCollection;

private:
	BOOL m_bTestingOn;
};

