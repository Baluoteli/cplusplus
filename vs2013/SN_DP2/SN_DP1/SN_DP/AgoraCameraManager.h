#pragma once
class CAgoraCameraManager
{
public:
	CAgoraCameraManager();
	~CAgoraCameraManager();

	BOOL Create(IRtcEngine *lpRtcEngine);
	void Close();

	UINT GetDeviceCount();

	BOOL GetCurDeviceID(char* szDeviceID);
	BOOL SetCurDevice(char* lpDeviceID);

	BOOL GetDevice(UINT nIndex, char *rDeviceName, char *rDeviceID);
	void TestCameraDevice(HWND hVideoWnd, BOOL bTestOn = TRUE);

	BOOL IsTesting() { return m_bTestingOn; };

private:
	AVideoDeviceManager			*m_ptrDeviceManager;
	IVideoDeviceCollection		*m_lpCollection;
	BOOL						m_bTestingOn;
};

