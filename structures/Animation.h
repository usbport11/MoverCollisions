#ifndef animationH
#define animationH

struct stAnimation
{
	stTexture* Texture;
	unsigned int Count;
	unsigned char Type;//1 - repeat animation (default), 2 - finite animation
	unsigned int Next;//if next == current - skip, if next != current and type == 2 - change animation
	void SetParameters(unsigned int inType, unsigned int inNext)
	{
		if(inType < 1 || inType > 2) Type = 1;
		else Type = inType;
		if(inNext >= Count ) Next = Count - 1;
		else Next = inNext;
	}
};

#endif