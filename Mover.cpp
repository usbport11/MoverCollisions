#include "stdafx.h"
#include "Mover.h"

MMover::MMover()
{
	for(int j=0; j<4; j++)
	{
		Ray[j].Origin = glm::vec2(0, 0);
		Ray[j].Direction = glm::vec2(0, 0);
	}
	CanMove[0] = CanMove[1] = 0;
	pObject = NULL;
	pLevel = NULL;
}

MMover::~MMover()
{
	for(int j=0; j<4; j++)
	{
		Ray[j].Origin = glm::vec2(0, 0);
		Ray[j].Direction = glm::vec2(0, 0);
	}
	CanMove[0] = CanMove[1] = 0;
	pObject = NULL;
	pLevel = NULL;
}

bool MMover::SetLevel(MLevel2* inpLevel)
{
    if(!inpLevel)
	{
		LogFile<<"Mover: NULL level pointer"<<endl;
		return false;
	}
    if(inpLevel->GetGravity() < 0.5)
	{
		LogFile<<"Mover: Level zero gravity"<<endl;
		return false;
	}
    pLevel = inpLevel;
	 
    return true;
}

bool MMover::Move(MGameObject* inpObject)
{
	if(!inpObject)
	{
		LogFile<<"Mover: Null object"<<endl;
		return false;
	} 
	if(!pLevel)
	{
		LogFile<<"Mover: NUll level"<<endl;
		return false;
	}
	
	pObject = inpObject;
	if(pObject->GetCanJump()) UsualMove();
	
	return true;
}

void MMover::SetEnabled(bool inEnabled)
{
	Enabled = inEnabled;
}

bool MMover::GetEnabled()
{
	return Enabled;
}

bool MMover::SetRaysByVelocity()
{
	glm::vec2 Velocity = pObject->GetVelocity();
	if(Velocity.x == 0 && Velocity.y == 0) return false;
	
	//nulling rays, two rays on each direction
    for(int j=0; j<4; j++)
	{
		Ray[j].Origin = glm::vec2(0, 0);
		Ray[j].Direction = glm::vec2(0, 0);
	}
   	
   	stQuad Quad = pObject->GetVertex();
   	
	//get object x rays
	if(Velocity[0] > 0)
	{
		Ray[0].Origin = Quad.p[2];
		Ray[0].Direction = glm::vec2(Velocity[0], 0);
		Ray[1].Origin = Quad.p[3];
		Ray[1].Direction = glm::vec2(Velocity[0], 0);
	}
	if(Velocity[0] < 0)
	{
		Ray[0].Origin = Quad.p[1];
		Ray[0].Direction = glm::vec2(Velocity[0], 0);
		Ray[1].Origin = Quad.p[0];
		Ray[1].Direction = glm::vec2(Velocity[0], 0);
	}
	//get object y rays
	if(Velocity[1] > 0)
	{
		Ray[2].Origin = Quad.p[1];
		Ray[2].Direction = glm::vec2(0, Velocity[1]);
		Ray[3].Origin = Quad.p[2];
		Ray[3].Direction = glm::vec2(0, Velocity[1]);
	}
	if(Velocity[1] < 0)
	{
		Ray[2].Origin = Quad.p[0];
		Ray[2].Direction = glm::vec2(0, Velocity[1]);
		Ray[3].Origin = Quad.p[3];
		Ray[3].Direction = glm::vec2(0, Velocity[1]);
	}
	
	return true;
}

float MMover::RayTesting(stQuad Box)
{
	float DiffLimit = 1;
	glm::vec2 Test = glm::vec2(0, 0);
	float MinimalDiff = 999;//this is need for situation then no collision - need think in future
	
	for(int j=0; j<4; j++)
	{
		//skip null rays
		if(!Ray[j].Direction[0] && !Ray[j].Direction[1])
        {
            continue;
        }
		if(!QuadIntersectByRay(Box, Ray[j], Test))
        {
            continue;
        }
		//check differeneces and stop move on needed directions
		if(Test[0] <= DiffLimit || Test[1] <= DiffLimit)
		{
			if(MinimalDiff > Test[0]) MinimalDiff = Test[0];
			if(MinimalDiff > Test[1]) MinimalDiff = Test[1];
			if(j == 0 || j == 1)
			{
				CanMove[0] = 0;
				continue;
			}
			if(j == 2 || j == 3)
			{
				CanMove[1] = 0;
				continue;
			}
		}
	}
	return MinimalDiff;
}

bool MMover::UsualMove()
{
    //prepare
    if(!PrepareMove()) return false;
    SetRaysByVelocity();
	if(!CalculateCollisionZones(pObject->GetVelocityArea()))
	{
		return false;
	}
	//init values
    CanMove[0] = CanMove[1] = 1;
    pNearestBox[0] = pNearestBox[1] = NULL;
    float Diff;
    unsigned int ZonesCount = CollisionZones.size();
	unsigned int PlanesCount;
	for(unsigned int i=0; i<ZonesCount; i++)
	{
		PlanesCount = CollisionZones[i]->Body.size();
		for(unsigned int j=0; j<PlanesCount; j++)
		{
			//exit from cycle if we can not move by both axes
			if(!CanMove[0] && !CanMove[1]) break;
			//skip ground check
			if(pGroundBox == &CollisionZones[i]->Body[j]) continue;
			//checking
			Diff = RayTesting(CollisionZones[i]->Body[j]);
			if(!CanMove[0] && Diff <= 1 && !pNearestBox[0]) pNearestBox[0] = &CollisionZones[i]->Body[j];
			if(!CanMove[1] && Diff <= 1 && !pObject->GetGroundBox()) //IMPORTANT
			{
				pNearestBox[1] = &CollisionZones[i]->Body[j];
				if(pObject->GetVelocity()[1] <= 0) pObject->SetGroundBox(&CollisionZones[i]->Body[j]);
			}
		}
	}
	Decision();
	
	return true;
}

bool MMover::PrepareMove()
{
	//get ground box
	pGroundBox = pObject->GetGroundBox();
	//if we stay on ground and not move return
	if(!pObject->GetVelocityX() && !pObject->GetVelocityY() && pGroundBox) return false;
	//calculate pursuit velocity in case that object follow another object
    pObject->PursuitVelocity();
	//ground checks
	stQuad ObjectQuad = pObject->GetVertex();
	if(pGroundBox)
	{	
		if(ObjectQuad.p[0][0] > pGroundBox->p[2][0] || ObjectQuad.p[2][0] < pGroundBox->p[0][0])
        {
		    //start fall
			pObject->StartJump(0);
			LogFile<<"Start Fall"<<endl;
        	return false;
        }
		//collision test with ground
		if(QuadCollision(ObjectQuad, *pGroundBox))
		{
			return false;
		}
	}
	//decrese velocity by Y axis if we jumping or falling
    if(pObject->GetCanJump() && pObject->GetJump()) pObject->SetVelocityY(pObject->GetVelocityY() - pLevel->GetGravity());
    //LogFile<<"Mover: Quad pos: x0: "<<ObjectQuad.p[0][0]<<" y0: "<<ObjectQuad.p[0][1]<<" x1: "<<ObjectQuad.p[2][0]<<" y1: "<<ObjectQuad.p[2][1]<<endl;
    
	return true;
}

void MMover::Decision()
{
	//make decisions
    if(CanMove[0])
	{
		pObject->MoveByX();
	}
	else
	{
		if(pNearestBox[0])
		{
			stQuad ObjectQuad = pObject->GetVertex();
			float Width = ObjectQuad.p[3][0] - ObjectQuad.p[0][0];
			if(pObject->GetVelocity()[0] > 0)
			{
				//moving from left to right
				ObjectQuad.p[3][0] = ObjectQuad.p[2][0] = pNearestBox[0]->p[0][0] - 1;
				ObjectQuad.p[0][0] = ObjectQuad.p[1][0] = ObjectQuad.p[2][0] - Width;
				pObject->SetVertex(ObjectQuad);
				pObject->Update();
			}
			else
			{
				//moving from right to left
				ObjectQuad.p[0][0] = ObjectQuad.p[1][0] = pNearestBox[0]->p[2][0] + 1;
				ObjectQuad.p[3][0] = ObjectQuad.p[2][0] = ObjectQuad.p[0][0] + Width;
				pObject->SetVertex(ObjectQuad);
				pObject->Update();
			}
			//LogFile<<"Mover: NB 0: "<<pNearestBox[0]->p[0][0]<<" y0: "<<pNearestBox[0]->p[0][1]<<" x1: "<<pNearestBox[0]->p[2][0]<<" y1: "<<pNearestBox[0]->p[2][1]<<endl;
			
		}
	}
    if(CanMove[1])
	{
		pObject->MoveByY();
	}
    else
    {
    	if(pNearestBox[1])
    	{
    		stQuad ObjectQuad = pObject->GetVertex();
			float Height = ObjectQuad.p[1][1] - ObjectQuad.p[0][1];
			if(pObject->GetVelocity()[1] > 0)
			{
				//moving from bottom to up
				ObjectQuad.p[1][1] = ObjectQuad.p[2][1] = pNearestBox[1]->p[0][1] - 1;
				ObjectQuad.p[0][1] = ObjectQuad.p[3][1] = ObjectQuad.p[1][1] - Height;
				pObject->SetVertex(ObjectQuad);
				pObject->Update();
				if(pObject->GetCanJump()) pObject->SetVelocityY(0);
			}
			else
			{
				//moving from up to bottom
				ObjectQuad.p[0][1] = ObjectQuad.p[3][1] = pNearestBox[1]->p[1][1] + 1;
				ObjectQuad.p[1][1] = ObjectQuad.p[2][1] = ObjectQuad.p[0][1] + Height;
				pObject->SetVertex(ObjectQuad);
				pObject->Update();
				if(pObject->GetCanJump()) pObject->StopJump();
			}
			//LogFile<<"Mover: NB 1: "<<pNearestBox[1]->p[0][0]<<" y0: "<<pNearestBox[1]->p[0][1]<<" x1: "<<pNearestBox[1]->p[2][0]<<" y1: "<<pNearestBox[1]->p[2][1]<<endl;
		}
    }
}

bool MMover::CalculateCollisionZones(stQuad Quad)
{
	int NearZonesCount = 0;
	//clear array
	CollisionZones.clear();
	//check segments and fill array
	unsigned int ZonesCount = pLevel->GetZonesCount();
    for(unsigned int i=0; i<ZonesCount; i++)
    {
		if(QuadCollision(Quad, pLevel->GetPZone(i)->Border))
		{
			CollisionZones.push_back(pLevel->GetPZone(i));
			NearZonesCount ++;
		}
	}
	if(NearZonesCount > 4) NearZonesCount = 0;
	return (bool)NearZonesCount;
}

bool MMover::SetObjectOnPlane(unsigned int inZoneNumber, unsigned int inPlaneNumber, MGameObject* GameObject, float Offset, bool FixSize)
{
	if(!pLevel)
	{
		LogFile<<"Mover: Can not place object. Level is NULL"<<endl;
		return false;
	}
	if(inZoneNumber >= pLevel->GetZonesCount())
	{
		LogFile<<"Mover: ZoneNumber more than total zones count"<<endl;
		return false;
	}
	if(inPlaneNumber >= pLevel->GetPZone(inZoneNumber)->Body.size())
	{
		LogFile<<"Mover: Plane more than total planes count in given zone"<<endl;
		return false;
	}
	
	//get plane
	stQuad* pQuad = &pLevel->GetPZone(inZoneNumber)->Body[inPlaneNumber];
	//set as ground box
	GameObject->SetGroundBox(pQuad);
	
	//check center offset
	if(Offset <= 0.3) Offset = 0.3;
	if(Offset >= 0.7) Offset = 0.7;
	
	//get width
	float Width = pQuad->p[3][0] - pQuad->p[0][0];
	glm::vec2 Position;
	Position = glm::vec2(pQuad->p[0][0] + Width * Offset, pQuad->p[1][1] + 1);
	
	stQuad ResultQuad;
	//lower percent
	//object height always must be a bit smaller than unit height size
	//dont forget that pLevel->UnitSize.p[1] may change while level generate boxes
	if(FixSize)
	{
		float Percent = 0.9;
		glm::vec2 Size;
		Size = glm::vec2(pLevel->GetUnitSize()[0] * Percent, pLevel->GetUnitSize()[0] * Percent);
		SetQuad(ResultQuad, Position[0], Position[1], Size[0], Size[1]);
		//GameObject->SetVertex(Position[0], Position[1], Size[0], Size[1]);
	}
	//else GameObject->SetVertex(Position[0], Position[1], GameObject->GetVertex().p[3][0] - GameObject->GetVertex().p[0][0], GameObject->GetVertex().p[2][1] - GameObject->GetVertex().p[0][1]);
	else SetQuad(ResultQuad, Position[0], Position[1], GameObject->GetVertex().p[3][0] - GameObject->GetVertex().p[0][0], GameObject->GetVertex().p[2][1] - GameObject->GetVertex().p[0][1]);
	GameObject->SetVertex(ResultQuad);
	GameObject->Update();
	
	LogFile<<"Mover: Object placed in zone: "<<inZoneNumber<<" on plane: "<<inPlaneNumber<<endl;
	
	return true;
}

bool MMover::RandomSetObjectOnPlane(MGameObject* GameObject, bool FixSize)
{
	if(!pLevel)
	{
		LogFile<<"Mover: Can not place object. Level is NULL"<<endl;
		return false;
	}
	
	int RndNum[2] = {0,0};
	RndNum[0] = rand() % pLevel->GetZonesCount();
	RndNum[1] = rand() % pLevel->GetPZone(RndNum[1])->Body.size();
	if(!SetObjectOnPlane(RndNum[0], RndNum[1], GameObject, (rand() % 5 + 3) * 0.1, FixSize)) return false;
	return true;
}

unsigned char MMover::GetCanMove()
{
	return (int)CanMove[0] * 1 + (int)CanMove[1] * 2;
}

bool MMover::QuadIntersectByRay(stQuad Box, stRay Ray, glm::vec2& DiffSize)
{   
    glm::vec2 Test1, Test2;
    float Near = -FLT_MAX;
    float Far = FLT_MAX;
    for(int i=0; i<2; i++)
    {
        if(Ray.Direction[i] == 0)
        {
            if(Ray.Origin[i] < Box.p[0][i] || Ray.Origin[i] > Box.p[2][i])
            {
                return false;
            }
        }
        Test1[i] = (Box.p[0][i] - Ray.Origin[i]) / Ray.Direction[i];
		Test2[i] = (Box.p[2][i] - Ray.Origin[i]) / Ray.Direction[i];
        if(Test1[i] > Test2[i]) swap(Test1, Test2);
        if(Test1[i] > Near) Near = Test1[i];
        if(Test2[i] < Far) Far = Test2[i];
        if(Near > Far || Far < 0)
        {
            return false;
        }
    }
    DiffSize = glm::vec2(Near, Far);
    return true;
}

bool MMover::QuadCollision(stQuad Quad1, stQuad Quad2)
{
	if(Quad1.p[0][1] < Quad2.p[0][1] && Quad1.p[2][1] < Quad2.p[0][1]) return false; //plane over us
	if(Quad1.p[0][1] > Quad2.p[2][1] && Quad1.p[2][1] > Quad2.p[2][1]) return false; //plane under us
	if(Quad1.p[0][0] < Quad1.p[0][0] && Quad1.p[2][0] < Quad1.p[0][0]) return false; //plane right of us
	if(Quad1.p[0][0] > Quad1.p[2][0] && Quad1.p[2][0] > Quad1.p[2][0]) return false; //plane left of us
	return true;
}
