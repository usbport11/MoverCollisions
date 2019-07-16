#include "stdafx.h"
#include "Window.h"

LRESULT CALLBACK ChildWProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return 0;
}

MWindow::MWindow()
{   
    //set default width and height
    WindowWidth = 800;
    WindowHeight = 600;
    GetSystemInfo( &sysinfo );
    Exit = false;
    sWindowClass = "win32app";
	sTitle = "Win32 2D Engine";
	WindowInitialized = false;
}

MWindow::~MWindow()
{
}

int MWindow::CreateMainWindow(HINSTANCE hInstance, int Width, int Height)
{
	WindowWidth = Width;
    WindowHeight = Height;
    
    return CreateMainWindow(hInstance);
}

int MWindow::CreateMainWindow(HINSTANCE hInstance)
{
    LogFile<<"Window: initialize"<<endl;
    
	if(!hInstance) return 0;
	LogFile<<"Window: hInstance: "<<hInstance<<endl;

	WNDCLASS wc;

	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.lpszClassName = sWindowClass;
	wc.style = CS_OWNDC;
	wc.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
	wc.lpfnWndProc = StaticWndProc;
	wc.lpszMenuName = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);

	if(!RegisterClass(&wc))
    {
		MessageBox(NULL, "Can't reg window", "Error", 0);
		return 0;
	}

	m_hInstance = hInstance;

	m_hWnd = CreateWindow(sWindowClass, 
		sTitle, 
		//WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		CW_USEDEFAULT, CW_USEDEFAULT,
		WindowWidth, WindowHeight,
		NULL, 
		NULL, 
		hInstance, 
		this);// VERY IMPORTANT!

	if(!m_hWnd)
	{
		MessageBox(NULL, "Can't create window", "Error", 0);
		return 0;
	}
	
	LogFile<<"Windows: Main: HWND: "<<m_hWnd<<endl;

	//in windowed mode
	RECT wndRect;
	RECT cltRect;
	GetWindowRect(m_hWnd, &wndRect);
	GetClientRect(m_hWnd, &cltRect);
	unsigned int Offset[2] = {WindowWidth - cltRect.right, WindowHeight - cltRect.bottom };
	MoveWindow(m_hWnd, wndRect.left, wndRect.top, (wndRect.right - wndRect.left) + Offset[0], (wndRect.bottom - wndRect.top) + Offset[1], true);

	if(!SetUpWindow(m_hWnd)) return 0;
	
	//glew
	GLenum Error = glewInit();
	if(GLEW_OK != Error)
	{
		LogFile<<"Game2: Loader: "<<glewGetErrorString(Error)<<endl;
		return 0;
	}
	
	//init opengl
	if(!InitGL()) return 0;
	
	//get opengl data
	OpenGLVersion = (char*)glGetString(GL_VERSION);
	OpenGLVendor = (char*)glGetString(GL_VENDOR);
	OpenGLRenderer = (char*)glGetString(GL_RENDERER);
	LogFile<<"Window: OpenGL version: "<<OpenGLVersion<<endl;
	LogFile<<"Window: OpenGL vendor: "<<OpenGLVendor<<endl;
	LogFile<<"Window: OpenGL renderer: "<<OpenGLRenderer<<endl;
	sscanf(OpenGLVersion.c_str(), "%f", &fOpenGlVersion);
	
	if(fOpenGlVersion < 4)
	{
		LogFile<<"Window: Old version of OpenGL installed. Sorry"<<endl;
		return 0;
	}

	//Init timers
	MainTimer.Initialize(m_hWnd, MAIN_TIMER, 14);
	FrameTimer.Initialize(m_hWnd, FRAME_TIMER, 250);

	return 1;
}

int MWindow::SetUpWindow(HWND hWnd)
{
	//prepare window
	static PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW |
		PFD_SUPPORT_OPENGL |
		PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		16,
		0, 0, 0, 0, 0, 0,
		0,
		0,
		0,
		0, 0, 0, 0,
		16, 
		0,
		0,
		PFD_MAIN_PLANE,	
		0,
		0, 0, 0
	};

	m_hDC = GetDC(hWnd);
	if(!m_hDC)
	{
		MessageBox(NULL, "Can't get DC", "Error", 0);
		return 0;
	}
	unsigned short int PixelFormat = ChoosePixelFormat(m_hDC, &pfd);
	if(!PixelFormat)
	{
		MessageBox(NULL, "Can't chose pixelformat", "Error", 0);
		return 0;
	}
	if(!SetPixelFormat(m_hDC, PixelFormat, &pfd))
	{
		MessageBox(NULL, "Can't set pixelformat", "Error", 0);
		return 0;
	}
	DescribePixelFormat(m_hDC, PixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
	m_hGLRC = wglCreateContext(m_hDC);
	if(!m_hGLRC)
	{
		MessageBox(NULL, "Can't create context", "Error", 0);
		return 0;
	}
	if(!wglMakeCurrent(m_hDC, m_hGLRC))
	{
		MessageBox(NULL, "Can't make context current", "Error", 0);
		return 0;
	}
	
	WindowInitialized = true;

	return 1;
}

int MWindow::InitGL()
{
    LogFile<<"Window: initialize OpenGl"<<endl;
	//prepare viport
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//enable textures
	//glEnable(GL_TEXTURE_2D);
	//set blending function
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//enable blend
	glEnable(GL_BLEND);
	
	gluOrtho2D(0, WindowWidth, 0, WindowHeight);
	glMatrixMode(GL_MODELVIEW);

	return 1;
}

int MWindow::CloseGL()
{
	if(m_hGLRC)
	{
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(m_hGLRC);
	}
	return 1;
}

void MWindow::ResizeScene(int width, int height)
{
    if(width == 0) width = 1;
	if(height == 0) height = 1;
	WindowWidth = width;
	WindowHeight = height;
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, width, 0, height);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

LRESULT CALLBACK MWindow::StaticWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	MWindow* pWindow;
	if(msg == WM_CREATE)
	{
		pWindow = (MWindow*)((LPCREATESTRUCT)lParam)->lpCreateParams;
		SetWindowLongPtr(hWnd, GWL_USERDATA, (LONG_PTR)pWindow);
		LogFile<<"Window: connect static window proc"<<endl;
	}
	else
	{
		pWindow = (MWindow*)GetWindowLongPtr(hWnd, GWL_USERDATA);
		if(!pWindow) return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	pWindow->m_hWnd = hWnd;
	return pWindow->WndProc(msg, wParam, lParam);
}

LRESULT CALLBACK MWindow::WndProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
	if(Exit) return 0;
	
	switch (msg)
	{
		case WM_KEYDOWN:
			/*
			switch(wParam)
			{
				case VK_ESCAPE:
					SendMessage(m_hWnd, WM_DESTROY, 0, 0);
					break;
			}
			*/
			OnKeyDown(wParam, lParam);
			break;
		case WM_KEYUP:
            OnKeyUp(wParam, lParam);
			break;
		case WM_SIZE:
			ResizeScene(LOWORD(lParam), HIWORD(lParam));
			OnSize();
			PostMessage(m_hWnd, WM_PAINT, 0, 0);
			break;
		case WM_PAINT:
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glLoadIdentity();
			OnDraw();
			SwapBuffers(m_hDC);
            ValidateRect(m_hWnd, NULL);
            break;
		case WM_TIMER:
            switch(wParam)
            {
                case MAIN_TIMER:
                	OnMainTimer();
                    InvalidateRect(m_hWnd, NULL, FALSE);
                    break;
                case FRAME_TIMER:
                	OnFrameTimer();
                    break;
            }
            break;
		case WM_CLOSE:
		case WM_DESTROY:
			Exit = true;
			MainTimer.Close();
			FrameTimer.Close();
            OnClose();
			CloseGL();
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(m_hWnd, msg, wParam, lParam);
			break;
	}
	return 0;
}

void MWindow::Run()
{
	//show window
	ShowWindow(m_hWnd, SW_SHOW);
	UpdateWindow(m_hWnd);
	
	//run
	MSG msg;
    while(GetMessage(&msg, m_hWnd, 0, 0) && !Exit)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

HWND MWindow::GetHWND()
{
	return m_hWnd;
}

void MWindow::OnDraw(){}
void MWindow::OnKeyUp(WPARAM wParam, LPARAM lParam){}
void MWindow::OnKeyDown(WPARAM wParam, LPARAM lParam){}
void MWindow::OnClose(){}
void MWindow::OnMainTimer(){}
void MWindow::OnFrameTimer(){}
void MWindow::OnSize(){}

