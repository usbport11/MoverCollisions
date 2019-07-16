#ifndef background2H
#define background2H

class MBackground2
{
private:
	vector<stUVQuad> uvqBackGround;
	vector<unsigned int> txBackGround;//0 - main always
	unsigned int BufferId;
	stTexture* ptxMain;
	stTexture* ptxDecor;
	unsigned int ptxDecor_cnt;
	stQuad StandardUV;
	stQuad MainUV;
	unsigned int CellsCount[2];
	bool Ready;
	unsigned int BackGroundCount;
	
	unsigned int DecorScaleLimit[2];
	glm::vec2 BasicDecorSize;
	glm::vec2 Velocity;
	glm::vec2 MoveLimit[2];
	glm::vec2 ReturnValue[2];
	
public:
	MBackground2();
	~MBackground2();
	bool SetMainTexture(stTexture* Texture);
	bool SetDecorTextures(stTexture* Texture, unsigned int Count);
	bool SetCellCount(unsigned int CountX, unsigned int CountY);
	bool SetMainUVSize(float X, float Y);
	bool Generate(unsigned int Width, unsigned int Height);
	void Draw();
	void Close();
	
	void SetDecorScaleLimit(unsigned int Start, unsigned int End);
	void SetMoveVelocity(float X, float Y);
	void Move();
};

#endif
