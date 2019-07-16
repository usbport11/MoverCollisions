#ifndef timerH
#define timerH

struct stTimer
{
	HWND hWnd;
	unsigned int Id;
	unsigned int Value;
	TIMERPROC TimerProc;
	bool Enabled;
	
	bool Initialize(HWND inhWnd, unsigned int inId, unsigned int inValue, TIMERPROC inTimerProc=NULL)
	{
		if(!inhWnd && !inTimerProc)
		{
			LogFile<<"Can not create timer. Empty HWND and TimerProc"<<endl;
			return false;
		}
		if(inValue < 10 || inValue > 60000)
		{
			LogFile<<"Can not create timer. Wrong value (10 - 300)"<<endl;
			return false;
		}
		hWnd = inhWnd;
		TimerProc = inTimerProc;
		Value = inValue;
		//without HWND timer id is generating, even if id was sended it will be ignored
		if(!hWnd)
		{
			Id = SetTimer(NULL, 0, Value, TimerProc);
			if(!Id)
			{
				LogFile<<"Some error ocurred"<<endl;
				return false;
			}
		}
		else
		{
			Id = inId;
			if(!SetTimer(hWnd, Id, Value, TimerProc))
			{
				LogFile<<"Some error ocurred"<<endl;
				return false;
			}
		}
		return true;
	}
	
	bool Update(unsigned int inValue)
	{
		if(inValue < 10 || inValue > 300)
		{
			LogFile<<"Can not create timer. Wrong value (10 - 300)"<<endl;
			return false;
		}
		if(Value == inValue) return true;
		Value = inValue;
		LogFile<<"Trying update timer: "<<Id<<" value: "<<Value<<endl;
		return SetTimer(hWnd, Id, Value, TimerProc);
	}
	
	void Close()
	{
		KillTimer(hWnd, Id);
		LogFile<<"Killed timer: "<<Id<<endl;
	}
};

#endif