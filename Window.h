#ifndef windowH
#define windowH

#define MAIN_TIMER 1001
#define FRAME_TIMER 1002

class MWindow
{
private:
	const char* sWindowClass;
	const char* sTitle;
	
	float fOpenGlVersion;
	string OpenGLVersion;
	string OpenGLVendor;
	string OpenGLRenderer;
	
	bool Exit;
	HINSTANCE m_hInstance;
	SYSTEM_INFO sysinfo;
    int SetUpWindow(HWND hWnd);
	int InitGL();
	int CloseGL();
	void ResizeScene(int width, int height);
	static LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	
protected:
	HDC m_hDC;
	HGLRC m_hGLRC;
	HWND m_hWnd;
	bool WindowInitialized;
	
	int WindowWidth;
    int WindowHeight;
    stTimer MainTimer;
	stTimer FrameTimer;
	
	//virtual functions
	virtual void OnDraw();
	virtual void OnKeyUp(WPARAM wParam, LPARAM lParam);
	virtual void OnKeyDown(WPARAM wParam, LPARAM lParam);
	virtual void OnClose();
	virtual void OnMainTimer();
	virtual void OnFrameTimer();
	virtual void OnSize();
	
public:
	MWindow();
	~MWindow();
	int CreateMainWindow(HINSTANCE hInstance);
	int CreateMainWindow(HINSTANCE hInstance, int Width, int Height);
	bool ChangeWindowSize(int Width, int Height);
	LRESULT CALLBACK WndProc(UINT msg, WPARAM wParam, LPARAM lParam);
	
	void Run();
	HWND GetHWND();
};

#endif
