#include "stdafx.h"
#include "Background2.h"

MBackground2::MBackground2()
{
	SetQuad(StandardUV, 0,0,0,1,1,1,1,0);
	CellsCount[0] = 0;
	CellsCount[1] = 0;
	ptxMain = NULL;
	ptxDecor = NULL;
	ptxDecor_cnt = 0;
	BufferId = 0;
	Ready = false;
	BackGroundCount = 0;
	
	DecorScaleLimit[0] = DecorScaleLimit[1] = 1;
	Velocity = glm::vec2(0, 0);
	MoveLimit[0] = glm::vec2(0, 1);
	MoveLimit[1] = glm::vec2(0, 1);
	ReturnValue[0] = glm::vec2(0, 1);
	ReturnValue[1] = glm::vec2(0, 1);
	BasicDecorSize = glm::vec2(32, 32);
}

MBackground2::~MBackground2()
{
	NullQuad(StandardUV);
}

bool MBackground2::SetMainTexture(stTexture* Texture)
{
	if(!Texture) return false;
	ptxMain = Texture;
	
	return true;
}

bool MBackground2::SetDecorTextures(stTexture* Texture, unsigned int Count)
{
	if(!Texture || !Count) return false;
	ptxDecor = Texture;
	ptxDecor_cnt = Count;
	
	return true;
}

bool MBackground2::SetCellCount(unsigned int CountX, unsigned int CountY)
{
	CellsCount[0] = CountX;
	CellsCount[1] = CountY;
	
	return true;
}

bool MBackground2::SetMainUVSize(float X, float Y)
{
	if(X < 0.001 && X > -0.001) X = 0;
	if(Y < 0.001 && Y > -0.001) Y = 0;
	if(!X) X = 1;
	if(!Y) Y = 1;
	SetQuad(MainUV, 0, 0, X, Y);
	
	return true;
}

bool MBackground2::Generate(unsigned int Width, unsigned int Height)
{
	if(!Width || !Height)
	{
		LogFile<<"BackGround: Null width/height"<<endl;
		return false;	
	} 
	if(!ptxMain)
	{
		LogFile<<"BackGround: Null main textures"<<endl;
		return false;
	}
	if(!DecorScaleLimit[0] && !DecorScaleLimit[1])
	{
		DecorScaleLimit[0] = 1;
		DecorScaleLimit[1] = 1;
	}
	
	if(!ptxDecor || !ptxDecor_cnt) CellsCount[0] = CellsCount[1] = 0;
	
	Ready = false;
	
	//skip first time clear
	if(BackGroundCount) Close();
		
	stQuad Quad;
	stUVQuad UVQuad;
	SetQuad(Quad, 0, 0, Width, Height);
	SetUVQuad(UVQuad, Quad, MainUV);
	uvqBackGround.push_back(UVQuad);
	txBackGround.push_back(ptxMain[0].Id);
	
	if(!CellsCount[0] || !CellsCount[1])
	{
		LogFile<<"Background: Null cells count. Skipping"<<endl;
		
		glGenBuffers(1, &BufferId);
		glBindBuffer(GL_ARRAY_BUFFER, BufferId);
		if(BufferId <= 0)
		{
			LogFile<<"Background2: Can not create buffer"<<endl;
			return false;
		}
		BackGroundCount = 1;
		glBufferData(GL_ARRAY_BUFFER, BackGroundCount * sizeof(float) * 4 * 4, &uvqBackGround[0].p[0][0].x, GL_STATIC_DRAW);
		
		Ready = true;
		
		return true;
	}
	
	glm::vec2 CellSize = glm::vec2((int)uvqBackGround[0].p[2][0].x / CellsCount[0], (int)uvqBackGround[0].p[2][0].y / CellsCount[1]);
	LogFile<<"Background2: CellSize: X: "<<CellSize[0]<<" Y: "<<CellSize[1]<<endl;
	
	unsigned int CurrentScale;
	for(unsigned int i=0; i<CellsCount[1]; i++)
	{
		for(unsigned int j=0; j<CellsCount[0]; j++)
		{
			if(!(rand()%2)) continue;
			//SetQuadRand(Quad, CellSize.x * i, CellSize.y * j, CellSize.y, 32);//?
			if(DecorScaleLimit[0] != DecorScaleLimit[1]) CurrentScale = rand() % (DecorScaleLimit[1] - DecorScaleLimit[0]) + DecorScaleLimit[0];
			else CurrentScale = DecorScaleLimit[0];
			SetQuad(Quad, CellSize.x * i, CellSize.y * j, BasicDecorSize[0] * CurrentScale, BasicDecorSize[1] * CurrentScale);
			SetUVQuad(UVQuad, Quad, StandardUV);
			uvqBackGround.push_back(UVQuad);
			txBackGround.push_back(ptxDecor[rand() % ptxDecor_cnt].Id);
		}
	}
	LogFile<<"Background2: Cells generated and added "<<endl;
	
	glGenBuffers(1, &BufferId);
	glBindBuffer(GL_ARRAY_BUFFER, BufferId);
	if(BufferId <= 0)
	{
		LogFile<<"Background2: Can not create buffer"<<endl;
		return false;
	}
	BackGroundCount = uvqBackGround.size();
	glBufferData(GL_ARRAY_BUFFER, BackGroundCount * sizeof(float) * 4 * 4, &uvqBackGround[0].p[0][0].x, GL_STATIC_DRAW);
	
	Ready = true;
	
	return true;
}

void MBackground2::Draw()
{
	if(!Ready) return;
	
	//vertex
	glBindBuffer(GL_ARRAY_BUFFER, BufferId);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), 0);
	//uv
	glBindBuffer(GL_ARRAY_BUFFER, BufferId);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)8);
	//draw
	for(unsigned int i=0; i<uvqBackGround.size(); i++)
	{
		glBindTexture(GL_TEXTURE_2D, txBackGround[i]);
		glDrawArrays(GL_QUADS, i*4, 4);
	}
}

void MBackground2::Close()
{
	uvqBackGround.clear();
	txBackGround.clear();
	if(BufferId) glDeleteBuffers(1, &BufferId);
}

void MBackground2::SetDecorScaleLimit(unsigned int Start, unsigned int End)
{
	if(!Start) DecorScaleLimit[0] = 1;
	else DecorScaleLimit[0] = Start;
	if(!End) DecorScaleLimit[1] = 1;
	else DecorScaleLimit[1] = End;
}

void MBackground2::SetMoveVelocity(float X, float Y)
{
	Velocity = glm::vec2(X, Y);
}

void MBackground2::Move()
{
	if(uvqBackGround.size() > 1) return; //we can not move background with decor
	if(Velocity[0] || Velocity[1])
	{
		//change UV data and update buffer
		stQuad UV;
		UV = GetUVQuad(uvqBackGround[0]);
		MoveQuad(UV, Velocity[0], Velocity[1]);
		//check
		if((Velocity[0] > 0 && UV.p[0][0] >= MoveLimit[0][1]) || (Velocity[0] < 0 && UV.p[2][0] <= MoveLimit[0][0]))
		{
			UV.p[1][0] = UV.p[0][0] = ReturnValue[0][0];
			UV.p[3][0] = UV.p[2][0] = ReturnValue[0][1];
		}
		if((Velocity[1] > 0 && UV.p[0][0] >= MoveLimit[1][1]) || (Velocity[1] < 0 && UV.p[2][0] <= MoveLimit[1][0]))
		{
			UV.p[3][1] = UV.p[0][1] = ReturnValue[0][0];
			UV.p[2][1] = UV.p[1][1] = ReturnValue[0][1];
		}
		//set
		SetUVQuad(uvqBackGround[0], GetVertexQuad(uvqBackGround[0]), UV);
		glBindBuffer(GL_ARRAY_BUFFER, BufferId);
		glBufferSubData(GL_ARRAY_BUFFER, 0, 16 * sizeof(float), &uvqBackGround[0].p[0][0].x);
	}
}
