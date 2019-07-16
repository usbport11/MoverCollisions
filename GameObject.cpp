#include "stdafx.h"
#include "GameObject.h"

MGameObject::MGameObject():MAnimatedObject()
{
	Velocity = glm::vec2(0, 0);
	VelocityValue = glm::vec2(0, 0);
	JumpValue = 0;
	Jump = false;
	pTarget = NULL;
	CanMove = true;
	CanJump = true;
	CanClip = false;
	pGroundBox = NULL;
}

MGameObject::~MGameObject()
{
}

void MGameObject::SetCanMove(bool inCanMove)
{
    CanMove = inCanMove;
}
void MGameObject::SetCanJump(bool inCanJump)
{
    CanJump = inCanJump;
}
void MGameObject::SetCanClip(bool inCanClip)
{
    CanClip = inCanClip;
}
bool MGameObject::GetCanMove()
{
    return CanMove;
}
bool MGameObject::GetCanJump()
{
    return CanJump;
}
bool MGameObject::GetCanClip()
{
    return CanClip;
}

void MGameObject::Move()
{
	if(!pUVBuffer) return;
	MoveUVQuad(pUVBuffer->UVQuad[BufferOffset], Velocity.x, Velocity.y);
	Update();
}

void MGameObject::MoveByX()
{
	if(!pUVBuffer) return;
	MoveUVQuad(pUVBuffer->UVQuad[BufferOffset], Velocity.x, 0);
	Update();
}

void MGameObject::MoveByY()
{
	if(!pUVBuffer) return;
	MoveUVQuad(pUVBuffer->UVQuad[BufferOffset], 0, Velocity.y);
	Update();
}

bool MGameObject::PursuitVelocity()
{
	if(!pTarget) return false;
	if(!pUVBuffer) return false;
	
	glm::vec2 Center = GetQuadCenter(GetVertexQuad(pUVBuffer->UVQuad[BufferOffset]));
	glm::vec2 CenterTarget = GetQuadCenter(GetVertexQuad(pTarget->pUVBuffer->UVQuad[BufferOffset]));
	glm::vec2 Offset = Center - CenterTarget;
	if(Offset[0] > 0) Velocity[0] = -VelocityValue[0];
	else Velocity[0] = VelocityValue[0];
	//here must check flying
	
	return true;
}

void MGameObject::SetVelocityX(float Value)
{
	Velocity[0] = Value;
}

void MGameObject::SetVelocityY(float Value)
{
	Velocity[1] = Value;
}

void MGameObject::SetVelocityValue(float Vx, float Vy)
{
	VelocityValue = glm::vec2(Vx, Vy);
}

void MGameObject::SetJumpValue(float Value)
{
	JumpValue = Value;
}

float MGameObject::GetJumpValue()
{
	return JumpValue;
}

stQuad* MGameObject::GetGroundBox()
{
	return pGroundBox;
}

void MGameObject::SetGroundBox(stQuad* pQuad)
{
	pGroundBox = pQuad;
}

glm::vec2 MGameObject::GetVelocity()
{
	return Velocity;
}

void MGameObject::SetMove(unsigned char Direction)
{
    if(Direction > 3 ) return;
    switch(Direction)
    {
        case 0://left
    	    if(CanMove) SetVelocityX(-VelocityValue[0]);
            SetDirectionRight(false);
			if(CurrentAnimation != 0) break;
			SetCurrentAnimation(1);
			SetCurrentFrame(0);
            break;
        case 1://right
            if(CanMove) SetVelocityX(VelocityValue[0]);
        	SetDirectionRight(true);
			if(CurrentAnimation != 0) break;
			SetCurrentAnimation(1);
			SetCurrentFrame(0);
            break;
        case 2://bottom fly
        	if(!CanJump) SetVelocityY(VelocityValue[1]);
            break;
        case 3://top fly
            if(!CanJump) SetVelocityY(-VelocityValue[1]);
            break;
    }
}

void MGameObject::StartJump()
{
	StartJump(JumpValue);
}

void MGameObject::StartJump(float StartVelocityY)
{
	if(CanJump && !Jump)
	{
		Velocity[1] = StartVelocityY;
		pGroundBox = NULL;
		Jump = true;
		//LogFile<<"GameObject: Start jump"<<endl;
		//SetCurrentAnimation(3);
		//SetCurrentFrame(0);
	}
}

void MGameObject::StopJump(stQuad* pQuad)
{
	Velocity[1] = 0;
	if(pQuad) pGroundBox = pQuad;
	Jump = false;
	SetCurrentAnimation(1);
	SetCurrentFrame(0);
}

bool MGameObject::GetJump()
{
	return Jump;
}

stQuad MGameObject::GetVelocityArea()
{
	stQuad VelocityArea;
	glm::vec2 Edge = glm::vec2(1, 1);
	SetQuadBy2Points(VelocityArea, pUVBuffer->UVQuad[BufferOffset].p[0][0].x - fabs(Velocity[0]) - Edge[0], pUVBuffer->UVQuad[BufferOffset].p[0][0].y - fabs(Velocity[1] - Edge[1]),
		pUVBuffer->UVQuad[BufferOffset].p[2][0].x + fabs(Velocity[0]) + Edge[0], pUVBuffer->UVQuad[BufferOffset].p[2][0].y + fabs(Velocity[1]) + Edge[1]);
	return VelocityArea;
}

float MGameObject::GetVelocityX()
{
	return Velocity[0];
}

float MGameObject::GetVelocityY()
{
	return Velocity[1];
}

void MGameObject::SetTarget(MGameObject* inTarget)
{
	pTarget = inTarget;
}

MGameObject* MGameObject::GetTarget()
{
	return pTarget;
}
