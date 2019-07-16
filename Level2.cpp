#include "stdafx.h"
#include "Level2.h"

MLevel2::MLevel2()
{
	UnitSize[0] = 0;
	UnitSize[1] = 0;
	TotalUnit.p[0] = 0;
	TotalUnit.p[1] = 0;
	BoxLimit[0].p[0] = 0;
	BoxLimit[0].p[1] = 0;
	BoxLimit[1].p[0] = 0;
	BoxLimit[1].p[1] = 0;
	HoleLimit.p[0] = 0;
	HoleLimit.p[1] = 0;
	Texture = NULL;
	TextureCount = 0;
	//for(unsigned short int i=0; i<1024; i++)
	//	Indicies[i] = i;
	SetQuad(StandardTexture, 0, 0, 0, 1, 1, 1, 1, 0);
	CurrentZoneNumber = 0;
	ZoneCount = 0;
	TotalZonesCount.p[0] = 0;
	TotalZonesCount.p[1] = 0;
	Gravity = 0;
}

MLevel2::~MLevel2()
{
}

void MLevel2::GenerateConnect()
{
	//LogFile<<"Level2: Start create connect"<<endl;
	//generate full size of box in units
	stUnitPoint RandSize;
	stUnitPoint LeftSize;
	//LeftSize.p[0] = CurrentLimit.p[0] - CurrentUnit.p[0];
	LeftSize.p[0] = CurrentLimit.p[0] - (HighestBox.Position.p[0] + HighestBox.Size.p[0]);
	LeftSize.p[1] = CurrentLimit.p[1] - CurrentUnit.p[1];
	if(HighestBox.Size.p[0] > 2) RandSize.p[0] = rand() % (HighestBox.Size.p[0] - 1) + 1;
	else RandSize.p[0] = 1;
	RandSize.p[1] = 1;
	if(LeftSize.p[0] < RandSize.p[0]) RandSize.p[0] = LeftSize.p[0];
	//y connect test
	if(HighestBox.Position.p[1] + HighestBox.Size.p[1] + RandSize.p[1] >= CurrentLimit.p[1]) return;
	//lastbox
	LastBox.Position.p[0] = HighestBox.Position.p[0] + 1;
	LastBox.Position.p[1] = HighestBox.Position.p[1] + HighestBox.Size.p[1] + 1;
	LastBox.Size.p[0] = RandSize.p[0];
	LastBox.Size.p[1] = RandSize.p[1];
	//one unit max element (let's shift to left)
	if(!LastBox.Size.p[0])
	{
		LastBox.Size.p[0] = 1;
		LastBox.Position.p[0] -= 1;
	}
	
	//LogFile<<"Level2: Connect box: POSx: "<<LastBox.Position.p[0]<<" POSy: "<<LastBox.Position.p[1]<<" SIZEx: "<<LastBox.Size.p[0]<<" SIZEy: "<<LastBox.Size.p[1]<<endl;
	
	//generate quads and add them into right array based on texture id
	//LogFile<<"Level2: Add zone data"<<endl;
	AddZoneData();
	
	HighestBox = LastBox;
	
	//let think there was a hole ???
	//WasHole = true;
}

void MLevel2::GenerateBox()
{	
	stUnitPoint RandSize;
	stUnitPoint LeftSize;
	
	//generate full size of box in units
	//box x limt
	if(BoxLimit[0].p[0] != BoxLimit[0].p[1]) RandSize.p[0] = (rand() % ((BoxLimit[0].p[1] - BoxLimit[0].p[0]) + 1)) + BoxLimit[0].p[0];
	else RandSize.p[0] = BoxLimit[0].p[0];
	//box y limit
	if(BoxLimit[1].p[0] != BoxLimit[1].p[1]) RandSize.p[1] = (rand() % ((BoxLimit[1].p[1] - BoxLimit[1].p[0]) +  1)) + BoxLimit[1].p[0];
	else RandSize.p[1] = BoxLimit[1].p[0];
	LeftSize.p[0] = CurrentLimit.p[0] - CurrentUnit.p[0];
	LeftSize.p[1] = CurrentLimit.p[1] - CurrentUnit.p[1];
	if(LeftSize.p[0] < RandSize.p[0]) RandSize.p[0] = LeftSize.p[0];
	if(LeftSize.p[1] < RandSize.p[1]) RandSize.p[1] = LeftSize.p[1];
	LastBox.Position.p[0] = CurrentUnit.p[0];
	LastBox.Position.p[1] = CurrentUnit.p[1];
	LastBox.Size.p[0] = RandSize.p[0];
	LastBox.Size.p[1] = RandSize.p[1];
	if(LastBox.Size.p[1] > HighestBox.Size.p[1]) HighestBox = LastBox;
	
	//LogFile<<"Level2: Generate box: POSx: "<<LastBox.Position.p[0]<<" POSy: "<<LastBox.Position.p[1]<<" SIZEx: "<<LastBox.Size.p[0]<<" SIZEy: "<<LastBox.Size.p[1]<<endl;
	
	//generate quads and add them into right array based on texture id
	AddZoneData();
	
	WasHole = false;
	
	//increse current unit position
	CurrentUnit.p[0] += LastBox.Size.p[0];
}

void MLevel2::AddZoneData()
{
	//stTexturedQuad TexturedQuad;
	stQuad Quad;
	//stPlane Plane;
	unsigned int RandValue;
	
	//generate quads and add them into right array based on texture id
	for(int i=0; i<LastBox.Size.p[1]; i++)
	{
		for(int j=0; j<LastBox.Size.p[0]; j++)
		{
			//SetPlaneByOffset(Plane, (LastBox.Position.p[0] + j) * UnitSize.p[0], (LastBox.Position.p[1] + i) * UnitSize.p[1], UnitSize.p[0], UnitSize.p[1]);
			//MovePlane(Plane, ZoneOffset);
			//SetQuad(Quad, Plane);
			SetQuad(Quad, (LastBox.Position.p[0] + j) * UnitSize[0], (LastBox.Position.p[1] + i) * UnitSize[1], UnitSize[0], UnitSize[1]);
			MoveQuad(Quad, ZoneOffset[0], ZoneOffset[1]);
			RandValue = rand() % TextureCount;
			Zone[CurrentZoneNumber].TQA[RandValue].Vertex.push_back(Quad);
			Zone[CurrentZoneNumber].TQA[RandValue].UV.push_back(StandardTexture);
		}
	}
	//add collision plane
	//SetPlaneByOffset(Plane, LastBox.Position.p[0] * UnitSize.p[0], LastBox.Position.p[1] * UnitSize.p[1], LastBox.Size.p[0] * UnitSize.p[0], LastBox.Size.p[1] * UnitSize.p[1]);
	//MovePlane(Plane, ZoneOffset);
	SetQuad(Quad, LastBox.Position.p[0] * UnitSize[0], LastBox.Position.p[1] * UnitSize[1], LastBox.Size.p[0] * UnitSize[0], LastBox.Size.p[1] * UnitSize[1]);
	MoveQuad(Quad, ZoneOffset[0], ZoneOffset[1]);
	//merge checkv (merging only collision planes) - need think about merging lastboxes
	if(BodyCount && !WasHole)
	{
		//if(Plane.Point[1].p[1] == Zone[CurrentZoneNumber].Body[BodyCount - 1].Point[1].p[1] &&
		//	Plane.Point[0].p[0] == Zone[CurrentZoneNumber].Body[BodyCount - 1].Point[1].p[0])
		//LogFile<<"Level2: Merge test: "<<Quad.p[1][1]<<" "<<Zone[CurrentZoneNumber].Body[BodyCount - 1].p[1][1]<<" "<<Quad.p[0][0]<<" "<<Zone[CurrentZoneNumber].Body[BodyCount - 1].p[3][0]<<endl;
		if(Quad.p[1][1] == Zone[CurrentZoneNumber].Body[BodyCount - 1].p[1][1] && Quad.p[0][0] == Zone[CurrentZoneNumber].Body[BodyCount - 1].p[3][0])
		{
			//Zone[CurrentZoneNumber].Body[BodyCount - 1].Point[1].p[0] = Plane.Point[1].p[0];
			Zone[CurrentZoneNumber].Body[BodyCount - 1].p[2][0] = Quad.p[3][0];
			Zone[CurrentZoneNumber].Body[BodyCount - 1].p[3][0] = Quad.p[3][0];
			if(HighestBox.Position.p[0] + HighestBox.Size.p[0] == LastBox.Position.p[0] && HighestBox.Size.p[1] == LastBox.Size.p[1]) HighestBox.Size.p[0] += LastBox.Size.p[0];
			LogFile<<"Level2: Merged!"<<endl;
			return;
		}
	}
	///add
	Zone[CurrentZoneNumber].Body.push_back(Quad);
	LogFile<<"Level2: Body: "<<BodyCount<<" Data: x0: "<<Zone[CurrentZoneNumber].Body[BodyCount].p[0][0]<<" y0: "<<Zone[CurrentZoneNumber].Body[BodyCount].p[0][1]<<" x1: "<<
		Zone[CurrentZoneNumber].Body[BodyCount].p[2][0]<<" y1: "<<Zone[CurrentZoneNumber].Body[BodyCount].p[2][1]<<endl;
	BodyCount ++;
}

void MLevel2::GenerateHole()
{
	unsigned int RandSize;
	unsigned int LeftSize;
	if(HoleLimit.p[0] != HoleLimit.p[1]) RandSize = (rand() % ((HoleLimit.p[1] - HoleLimit.p[0]) + 1)) + HoleLimit.p[0];
	else RandSize = HoleLimit.p[0];
	LeftSize = CurrentLimit.p[0] - CurrentUnit.p[0];
	if(LeftSize < RandSize) RandSize = LeftSize;
	CurrentUnit.p[0] += RandSize;
	
	LogFile<<"Level2: Hole size: "<<RandSize<<endl;
	WasHole = true;
}

void MLevel2::GenerateLine()
{
	if(WasConnect)
	{
		CurrentUnit.p[0] = 0;
		CurrentLimit.p[0] = TotalUnit.p[0];
		//HighestBox = LastBox;
		HighestBox.Position.p[0] = 0;
		HighestBox.Position.p[1] = 0;
		HighestBox.Size.p[0] = 0;
		HighestBox.Size.p[1] = 0;
		if(LastBox.Position.p[0] > (TotalUnit.p[0] >> 1))
		{
			CurrentUnit.p[0] = 0 + (bool)ZoneNumberX;
			CurrentLimit.p[0] = LastBox.Position.p[0];
		}
		else
		{
			CurrentUnit.p[0] = LastBox.Position.p[0] + LastBox.Size.p[0];
			CurrentLimit.p[0] = TotalUnit.p[0];
		}
		while(CurrentUnit.p[0] < CurrentLimit.p[0])
		{
			if(!(rand() % 2) && !WasHole) GenerateHole();
			else GenerateBox();
		}
		CurrentUnit.p[1] += HighestBox.Size.p[1];
	}
	else
	{
		GenerateConnect();
		CurrentUnit.p[1] += HighestBox.Size.p[1] + 1;
	}
	WasConnect = !WasConnect;
	LogFile<<"Level2: Line add "<<WasConnect<<endl;
}

bool MLevel2::GenerateZone(unsigned int NumberX, unsigned int NumberY)
{
	LogFile<<"Level2: ZoneX: "<<NumberX<<" ZoneY: "<<NumberY<<endl;
	if(NumberX >= TotalZonesCount.p[0] || NumberY >= TotalZonesCount.p[1]) return false;
	
	//set zone offset
	//SetPoint(ZoneOffset, NumberX * TotalUnit.p[0] * UnitSize.p[0], NumberY * TotalUnit.p[1] * UnitSize.p[1]);
	ZoneOffset = glm::vec2(NumberX * TotalUnit.p[0] * UnitSize[0], NumberY * TotalUnit.p[1] * UnitSize[1]);
	LogFile<<"Level2: Zone offset: X:"<<ZoneOffset[0]<<" Y: "<<ZoneOffset[1]<<endl;
	
	//prepare zone arrays
	stZone AddZone;
	stTexturedQuadArray AddTQA;
	for(unsigned int i=0; i<TextureCount; i++)
	{
		AddTQA.VertexBufferId = 0;
		AddTQA.UVBufferId = 0;
		AddTQA.TextureId = Texture[i].Id;
		AddZone.TQA.push_back(AddTQA);
	}
	//SetPlane(AddZone.Border, ZoneOffset.p[0], ZoneOffset.p[1], ZoneOffset.p[0] + TotalUnit.p[0] * UnitSize.p[0], ZoneOffset.p[1] + TotalUnit.p[1] * UnitSize.p[1]);
	SetQuad(AddZone.Border, ZoneOffset[0], ZoneOffset[1], ZoneOffset[0] + TotalUnit.p[0] * UnitSize[0], ZoneOffset[1] + TotalUnit.p[1] * UnitSize[1]);
	Zone.push_back(AddZone);
	//generate
	HighestBox.Position.p[0] = 0;
	HighestBox.Position.p[1] = 0;
	HighestBox.Size.p[0] = 0;
	HighestBox.Size.p[1] = 0;
	LastBox = HighestBox;
	WasConnect = true;
	CurrentUnit.p[0] = 0;
	CurrentUnit.p[1] = 0;
	CurrentLimit.p[0] = TotalUnit.p[0];
	CurrentLimit.p[1] = TotalUnit.p[1];
	BodyCount = 0;
	LogFile<<"Level2: CLx: "<<CurrentLimit.p[0]<<" CLy: "<<CurrentLimit.p[1]<<endl;
	//save zone X number (used on line 168)
	ZoneNumberX = NumberX;
	//shift Y zone
	if(TotalZonesCount.p[1] > 1 && NumberX == 0 && NumberY > 0)
	{
		LastBox = LastZoneXLastBox;
		LogFile<<"Level2: From "<<CurrentUnit.p[0]<<" to "<<CurrentLimit.p[0]<<endl;
		LogFile<<"Level2: LastXZoneBox POSy: "<<LastBox.Position.p[1]<<" SIZEy: "<<LastBox.Size.p[1]<<endl;
		if(LastBox.Position.p[1] + LastBox.Size.p[1] + 1 >= TotalUnit.p[1]) CurrentUnit.p[1] = 1;
	}
	//cycle
	while(CurrentUnit.p[1] < CurrentLimit.p[1])
	{
		WasHole = true;
		GenerateLine(); //line may be full or just connect element
		LogFile<<"Level2: CUy: "<<CurrentUnit.p[1]<<" HighestBox SIZEy: "<<HighestBox.Size.p[1]<<endl;
	}
	CurrentZoneNumber ++;
	LogFile<<"Level2: Zone add"<<endl;
	
	//check indicies limit !!!
	//if (array_size() << 2) > (Indicies_size() << 2) some elements no will be draw
	
	return true;
}

/*
void MLevel2::SetEdges()
{
	stPoint TexMaxSize;
	stPlane TotalEdge;
	stQuad Quad;
	stQuad Texture;
	stTexturedQuad AddTexturedQuad;
	
	SetPlane(TotalEdge, Zone[0].Border.Point[0].p[0], Zone[0].Border.Point[0].p[1], Zone[ZoneCount - 1].Border.Point[1].p[0], Zone[ZoneCount - 1].Border.Point[1].p[1]);
	SetPoint(TexMaxSize, (float)GetPlaneWidth(TotalEdge) / UnitSize.p[0], (float)GetPlaneHeight(TotalEdge) / UnitSize.p[1]);
	LogFile<<"Level2: TexMaxSize: X: "<<TexMaxSize.p[0]<<" Y: "<<TexMaxSize.p[1]<<endl;
	//left
	SetQuadByPlane(Quad, (stPlane){TotalEdge.Point[0].p[0] - UnitSize.p[0], TotalEdge.Point[0].p[1] - UnitSize.p[1], TotalEdge.Point[0].p[0], TotalEdge.Point[1].p[1] + UnitSize.p[1]});
	SetQuadByPlane(Texture, (stPlane){0, 0, 1, TexMaxSize.p[1]});
	SetTexturedQuad(AddTexturedQuad, Quad, Texture);
	Edge.push_back(AddTexturedQuad);
	//right
	SetQuadByPlane(Quad, (stPlane){TotalEdge.Point[1].p[0], TotalEdge.Point[0].p[1] - UnitSize.p[1], TotalEdge.Point[1].p[0] + UnitSize.p[0], TotalEdge.Point[1].p[1] + UnitSize.p[1]});
	SetQuadByPlane(Texture, (stPlane){0, 0, 1, TexMaxSize.p[1]});
	SetTexturedQuad(AddTexturedQuad, Quad, Texture);
	Edge.push_back(AddTexturedQuad);
	//bottom
	SetQuadByPlane(Quad, (stPlane){TotalEdge.Point[0].p[0], TotalEdge.Point[0].p[1] - UnitSize.p[1], TotalEdge.Point[1].p[0], TotalEdge.Point[0].p[1]});
	SetQuadByPlane(Texture, (stPlane){0, 0, TexMaxSize.p[0], 1});
	SetTexturedQuad(AddTexturedQuad, Quad, Texture);
	Edge.push_back(AddTexturedQuad);
	//top
	SetQuadByPlane(Quad, (stPlane){TotalEdge.Point[0].p[0], TotalEdge.Point[1].p[1], TotalEdge.Point[1].p[0], TotalEdge.Point[1].p[1] + UnitSize.p[1]});
	SetQuadByPlane(Texture, (stPlane){0, 0, TexMaxSize.p[0], 1});
	SetTexturedQuad(AddTexturedQuad, Quad, Texture);
	Edge.push_back(AddTexturedQuad);
}
*/

bool MLevel2::GenerateLevel()
{
	if(!Texture || !TextureCount) //ERROR
	{
		LogFile<<"Leve2: NULL texture"<<endl;
		return false;
	}
	if(!BoxLimit[0].p[0] || !BoxLimit[1].p[0] || !BoxLimit[1].p[0] || !BoxLimit[1].p[1])
	{
		LogFile<<"Level2: NULL box limits"<<endl;
		return false;
	}
	if(!UnitSize[0] || !UnitSize[1])
	{
		LogFile<<"Level2: NULL unit size"<<endl;
		return false;
	}
	if(!TotalUnit.p[0] || !TotalUnit.p[1])
	{
		LogFile<<"Level2: NULL unit count"<<endl;
		return false;
	}
	if(!TotalZonesCount.p[0] || !TotalZonesCount.p[1])
	{
		LogFile<<"Level2: NULL zones count"<<endl;
		return false;
	}
	LogFile<<"Level2: Total zones count X:"<<TotalZonesCount.p[0]<<" Y: "<<TotalZonesCount.p[1]<<endl;
	
	Ready = false;
	
	//skip first time clear
	if(ZoneCount) Close();
	
	CurrentZoneNumber = 0;
	ZoneCount = 0;
	
	LastZoneXLastBox.Position.p[0] = 0;
	LastZoneXLastBox.Position.p[1] = 0;
	LastZoneXLastBox.Size.p[0] = 0;
	LastZoneXLastBox.Size.p[1] = 0;
	for(unsigned int i=0; i<TotalZonesCount.p[1]; i++)
	{
		for(unsigned int j=0; j<TotalZonesCount.p[0]; j++)
		{
			GenerateZone(j, i); //ERROR
		}
		LastZoneXLastBox = HighestBox;
	}
	ZoneCount = Zone.size(); //ERROR
	//SetEdges();
	
	LogFile<<"Level2: Zones count: "<<ZoneCount<<endl;
	LogFile<<"Level2: Planes count: "<<Zone[0].Body.size()<<endl;
	LogFile<<"Level2: Zone 0: TQA 0: Vertex count: "<<Zone[0].TQA[0].Vertex.size()<<" UV count: "<<Zone[0].TQA[0].UV.size()<<endl;
	LogFile<<"Level2: Zone 0: TQA 1: Vertex count: "<<Zone[0].TQA[1].Vertex.size()<<" UV count: "<<Zone[0].TQA[1].UV.size()<<endl;
	
	//init shader data
	for(unsigned int i=0; i<ZoneCount; i++)
	{
		for(unsigned int j=0; j<Zone[i].TQA.size(); j++)
		{
			LogFile<<"Level2: Buffers init: Zone: "<<i<<" TQA: "<<j<<endl;
			if(!InitTexturedQuadArray(Zone[i].TQA[j]))
			{
				LogFile<<"Level2: InitTexturedQuadArray error! Vertex or UV empty. i: "<<i<<" j: "<<j<<" size: "<<Zone[i].TQA[j].Vertex.size()<<endl;
				return false;
			}
		}
	}
	
	Ready = true;
	
	return true;
}

void MLevel2::Draw()
{
	if(!Ready) return;
	
	for(int i=0; i<ZoneCount; i++) //zone count
	{
		TQACount = Zone[i].TQA.size();
		for(int j=0; j<TQACount; j++) //vertex array count
		{
			DrawTexturedQuadArray(Zone[i].TQA[j]);
		}
	}
}

bool MLevel2::GetReady()
{
	return Ready;
}

void MLevel2::Close()
{
	//init shader data
	for(unsigned int i=0; i<ZoneCount; i++)
	{
		for(unsigned int j=0; j<Zone[i].TQA.size(); j++)
			ClearTexturedQuadArray(Zone[i].TQA[i]);
	}
	for(int i=0; i<ZoneCount; i++) //zone count
	{
		TQACount = Zone[i].TQA.size();
		for(int j=0; j<TQACount; j++)
		{
			Zone[i].TQA[j].Vertex.clear();
			Zone[i].TQA[j].UV.clear();
		}
		Zone[i].TQA.clear();
		Zone[i].Body.clear();
	}
	Zone.clear();
}

bool MLevel2::SetTexture(stTexture* inTexture, unsigned int inTextureCount)
{
	if(!inTexture || !inTextureCount)
	{
		LogFile<<"Leve2: NULL texture"<<endl;
		return false;
	}
	Texture = inTexture;
	TextureCount = inTextureCount;
	
	return true;
}

bool MLevel2::SetEdgeTexture(stTexture* txEdge)
{
	if(!txEdge)
	{
		LogFile<<"Level: No edge texture"<<endl;
		return false;
	}
	ptxEdge = txEdge;
	
	return true;
}

bool MLevel2::SetUnit(float UnitSizeX, float UnitSizeY, unsigned int TotalUnitX, unsigned int TotalUnitY)
{
	UnitSize = glm::vec2(UnitSizeX, UnitSizeY);
	TotalUnit.p[0] = TotalUnitX;
	TotalUnit.p[1] = TotalUnitY;
}

bool MLevel2::SetBoxLimits(unsigned int UnitLimitX0, unsigned int UnitLimitX1, unsigned int UnitLimitY0, unsigned int UnitLimitY1)
{
	BoxLimit[0].p[0] = UnitLimitX0;
	BoxLimit[0].p[1] = UnitLimitX1;
	BoxLimit[1].p[0] = UnitLimitY0;
	BoxLimit[1].p[1] = UnitLimitY1;
}

bool MLevel2::SetHoleLimit(unsigned int UnitLimitX0, unsigned int UnitLimitX1)
{
	HoleLimit.p[0] = UnitLimitX0;
	HoleLimit.p[1] = UnitLimitX1;
}

bool MLevel2::SetZonesCount(unsigned int ZonesCountX, unsigned int ZonesCountY)
{
	TotalZonesCount.p[0] = ZonesCountX;
	TotalZonesCount.p[1] = ZonesCountY;
	
	return true;
}

/*
bool MLevel2::SetTextureUniformId(unsigned int inTextureUniformId)
{
	TextureUniformId = inTextureUniformId;
	return true;
}
*/

glm::vec2 MLevel2::GetUnitSize()
{
	return UnitSize;
}

unsigned int MLevel2::GetZonesCount()
{
	return Zone.size();
}

stZone* MLevel2::GetPZone(unsigned int Number)
{
	return &Zone[Number];
}

float MLevel2::GetGravity()
{
	return Gravity;
}

void MLevel2::SetGravity(float inGravity)
{
	Gravity = inGravity;
}
