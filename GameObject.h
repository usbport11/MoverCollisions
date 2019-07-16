#ifndef gameobjectH
#define gameobjectH

#include "AnimatedObject.h"

class MGameObject: public MAnimatedObject
{
private:
	glm::vec2 VelocityValue;
	glm::vec2 Velocity;
	float JumpValue;
	bool Jump;
	bool CanMove;
	bool CanJump; //1 - jumping, 0- flying
	bool CanClip;
	
protected:
	MGameObject* pTarget;
	stQuad* pGroundBox;
	
public:
	MGameObject();
	~MGameObject();

	//move
	void Move();
	void MoveByX();
	void MoveByY();
	//jump
	void StartJump();
	void StartJump(float StartVelocityY);
	void StopJump(stQuad* pQuad = NULL);
	//movement
	void SetCanMove(bool inCanMove);
	void SetCanJump(bool inCanJump);
	void SetCanClip(bool inCanClip);
	bool GetCanMove();
	bool GetCanJump();
	bool GetCanClip();
	//velocity
	void SetVelocityX(float Value);
	void SetVelocityY(float Value);
	glm::vec2 GetVelocity();
	float GetVelocityX();
	float GetVelocityY();
	//default values
	void SetVelocityValue(float Vx, float Vy);
	void SetJumpValue(float Value);
	float GetJumpValue();
	//level
	stQuad* GetGroundBox();
	void SetGroundBox(stQuad* pQuad);	
	//direction
	void SetMove(unsigned char Direction);
	bool GetJump();
	//body (this function must change name)
	stQuad GetVelocityArea();
	//target
	bool PursuitVelocity();
	void SetTarget(MGameObject* inTarget);
	MGameObject* GetTarget();
};

#endif
