#ifndef viewboxH
#define viewboxH

struct stViewBox
{
	bool Enabled;
	stPoint Size;
	stPoint Offset;
	void SetSize(float x, float y)
	{
		SetPoint(Size, x, y);
		NullPoint(Offset);
	}
	void SetOffsetByPlane(stPlane Plane)
	{
		SetPoint(Offset, 
			(Plane.Point[1].p[0] + Plane.Point[0].p[0])/2 - Size.p[0]/2, 
			(Plane.Point[1].p[1] + Plane.Point[0].p[1])/2 - Size.p[1]/2);
	}
	stPlane GetPlane()
	{
		stPlane retPlane;
		SetPlane(retPlane, Offset.p[0], Offset.p[1],
			Offset.p[0] + Size.p[0], Offset.p[1] + Size.p[1]);
		return retPlane;
	}
};

#endif