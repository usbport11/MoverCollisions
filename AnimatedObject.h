#ifndef animatedobjectH
#define animatedobjectH

#include "BufferObject.h"

class MAnimatedObject: public MBufferObject
{
protected:
	stAnimation* Animation;
	unsigned char CurrentAnimation;
	unsigned int CurrentFrame;
public:
	MAnimatedObject();
	~MAnimatedObject();
	bool Create(stUVBuffer* inpUVBuffer, float pX0, float pY0, float pW, float pH, float tX0, float tY0, float tW, float tH, stAnimation* inAnimation);
	bool SetAnimation(stAnimation* inAnimation);
	bool SetCurrentAnimation(unsigned char inCurrentAnimation);
	bool SetCurrentFrame(unsigned int inCurrentFrame);
	bool SetRandomFrame();
	void IncAnimationFrame();
	unsigned char GetCurrentAnimation();
	unsigned int GetCurrentFrame();
	unsigned int GetCurrentAnimationFrames();
};

#endif
