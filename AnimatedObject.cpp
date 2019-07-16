#include "stdafx.h"
#include "AnimatedObject.h"

MAnimatedObject::MAnimatedObject():MBufferObject()
{
}

MAnimatedObject::~MAnimatedObject()
{
}

bool MAnimatedObject::Create(stUVBuffer* inpUVBuffer, float pX0, float pY0, float pW, float pH, float tX0, float tY0, float tW, float tH, stAnimation* inAnimation)
{
	if(!SetBuffer(inpUVBuffer)) return false;
	if(!MBufferObject::Create(pX0, pY0, pW, pH, tX0, tY0, tW, tH)) return false;
	if(!SetAnimation(inAnimation)) return false;
	
	return true;
}

bool MAnimatedObject::SetAnimation(stAnimation* inAnimation)
{
	if(!inAnimation) return false;
	Animation = inAnimation;
	SetCurrentAnimation(0);
	
	return true;
}

bool MAnimatedObject::SetCurrentAnimation(unsigned char inCurrentAnimation)
{
	if(inCurrentAnimation >= sizeof(Animation))
	{
		LogFile<<"Animation: "<<inCurrentAnimation<<" not exist"<<endl;
		return false;
	}
	if(Animation[inCurrentAnimation].Count > 25)
	{
		LogFile<<"To many animation frames"<<endl;
		return false;
	}
	CurrentAnimation = inCurrentAnimation;
	if(CurrentAnimation != 1)
	{
		CurrentFrame = 0;
		SetTexture(Animation[CurrentAnimation].Texture[CurrentFrame].Id);
	}
	
	return true;
}

bool MAnimatedObject::SetCurrentFrame(unsigned int inCurrentFrame)
{
	if(inCurrentFrame >= Animation[CurrentAnimation].Count)
	{
		LogFile<<"Frame: "<<inCurrentFrame<<" not exist"<<endl;
		return false;
	}
	CurrentFrame = inCurrentFrame;
	
	return true;
}

bool MAnimatedObject::SetRandomFrame()
{
	CurrentFrame = rand() % Animation[CurrentAnimation].Count;
	return true;
}

void MAnimatedObject::IncAnimationFrame()
{
	switch(Animation[CurrentAnimation].Type)
	{
		case 1:
			CurrentFrame ++;
			if(CurrentFrame >= Animation[CurrentAnimation].Count) CurrentFrame = 0;
			SetTexture(Animation[CurrentAnimation].Texture[CurrentFrame].Id);
			break;
		case 2:
			if(CurrentFrame < Animation[CurrentAnimation].Count - 1)
			{
				CurrentFrame ++;
				SetTexture(Animation[CurrentAnimation].Texture[CurrentFrame].Id);
			}
			else
			{
				if(Animation[CurrentAnimation].Next != CurrentAnimation)
				{
					SetCurrentAnimation(Animation[CurrentAnimation].Next);
					SetCurrentFrame(0);
				}
			}
			break;
		default:
			break;
	}
	
}
unsigned char MAnimatedObject::GetCurrentAnimation()
{
	return CurrentAnimation;
}

unsigned int MAnimatedObject::GetCurrentFrame()
{
	return CurrentFrame;
}

unsigned int MAnimatedObject::GetCurrentAnimationFrames()
{
	return Animation[CurrentAnimation].Count;
}
