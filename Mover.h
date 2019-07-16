#ifndef moverH
#define moverH

#include "Level2.h"
#include "GameObject.h"

class MMover
{
private:
	MLevel2* pLevel;
	MGameObject* pObject;
	stRay Ray[4];
	bool Enabled;
	bool CanMove[2];
	stQuad* pGroundBox;
	stQuad* pNearestBox[2];
	vector<stZone*> CollisionZones;
	
	bool SetRaysByVelocity();
	float RayTesting(stQuad Box);
	bool PrepareMove();
    bool UsualMove();
	void Decision();
	bool CalculateCollisionZones(stQuad Quad);
	bool QuadIntersectByRay(stQuad Box, stRay Ray, glm::vec2& DiffSize);
	bool QuadCollision(stQuad Quad1, stQuad Quad2);
	
public:
	MMover();
	~MMover();
	bool SetLevel(MLevel2* inpLevel);
	bool Move(MGameObject* inpObject);
	void SetEnabled(bool inEnabled);
	bool GetEnabled();
	bool SetObjectOnPlane(unsigned int inZoneNumber, unsigned int inPlaneNumber, MGameObject* GameObject, float Offset, bool FixSize);
	bool RandomSetObjectOnPlane(MGameObject* GameObject, bool FixSize);
	unsigned char GetCanMove();
};

#endif
