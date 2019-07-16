#ifndef framecontrolH
#define framecontrolH

struct stFrameControl
{
    //usual time
    DWORD FrameStart; //time of start draw one frame (ms)
    DWORD FrameEnd; //time of end draw one frame (ms)
    DWORD liStartTime; //time for start count frames (ms count)
	DWORD liEndTime; //time for end count frames (ms count)
	DWORD liDiffTime; //time of draw one frame (ms count)
	int Frames;
	float FPS;
	void Initialize()
	{
        Frames = 0;
        liStartTime = GetTickCount();
		liEndTime = liStartTime;
	}
	void Calculate()
	{
        Frames ++;
		liEndTime = GetTickCount();
		liDiffTime = liEndTime - liStartTime;
		if(liDiffTime >= 1000)
		{
            FPS = (float)Frames / liDiffTime * 1000;
            liStartTime = liEndTime;
            Frames = 0;
        }
	}
};

#endif