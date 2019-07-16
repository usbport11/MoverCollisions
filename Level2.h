#ifndef level2H
#define level2H

struct stUnitPoint
{
	int p[2];
};

struct stUnit
{
	stUnitPoint Position;
	stUnitPoint Size;
};

struct stZone
{
	vector<stTexturedQuadArray> TQA; //array based on texture ID (for good draw)
	vector<stQuad> Body; //collision plane array
	stQuad Border;
};

class MLevel2
{
private:
	float Gravity;

	vector<stZone> Zone;
	stTexture* Texture;
	unsigned int TextureCount;
	bool ShowBorder;
	bool Ready;
	glm::vec2 UnitSize; //points per unit
	glm::vec2 ZoneOffset;
	stUnitPoint BoxLimit[2]; //x,y size limits
	stUnitPoint HoleLimit; //hole size
	stUnitPoint TotalZonesCount; //total zones count
	stUnitPoint TotalUnit; //total units count in one zone
	
	bool WasHole;
	bool WasConnect;
	stUnit LastBox;
	stUnit HighestBox;
	stUnit LastZoneXLastBox;
	stUnitPoint CurrentUnit; //current unit pos
	stUnitPoint CurrentLimit; //limits used in conditions while line generating
	stUnitPoint BoxUnitSize; //generated box size
	unsigned int CurrentZoneNumber;
	unsigned int ZoneNumberX;
	unsigned int ZoneCount;
	unsigned int BodyCount;
	unsigned int TQACount;
	
	void GenerateConnect();
	void GenerateBox();
	void AddZoneData();
	void GenerateHole();
	void GenerateLine();
	bool GenerateZone(unsigned int NumberX, unsigned int NumberY);
	
	stTexture* ptxEdge;
	//vector<stTexturedQuad> Edge;//edges(left,right,bottom,top)
	//void SetEdges();
	
	//opengl data
	stQuad StandardTexture;
	//unsigned int TextureUniformId;
	
public:
	MLevel2();
	~MLevel2();
	bool SetUnit(float UnitSizeX, float UnitSizeY, unsigned int TotalUnitX, unsigned int TotalUnitY);
	bool SetBoxLimits(unsigned int UnitLimitX0, unsigned int UnitLimitX1, unsigned int UnitLimitY0, unsigned int UnitLimitY1);
	bool SetHoleLimit(unsigned int UnitLimitX0, unsigned int UnitLimitX1);
	bool SetTexture(stTexture* inTexture, unsigned int inTextureCount);
	bool SetEdgeTexture(stTexture* txEdge);
	bool SetZonesCount(unsigned int ZonesCountX, unsigned int ZonesCountY);
	//bool SetTextureUniformId(unsigned int inTextureUniformId);
	bool GenerateLevel();
	bool GetReady();
	void Draw();
	void Close();
	
	glm::vec2 GetUnitSize();
	unsigned int GetZonesCount();
	stZone* GetPZone(unsigned int Number);
	float GetGravity();
	void SetGravity(float inGravity);
};

#endif
