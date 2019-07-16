#ifndef bufferobjectH
#define bufferobjectH

#include "structures/UVBuffer.h"

class MBufferObject
{
private:
	unsigned int TextureId;
	bool DirectionRight;
protected:
	stUVBuffer* pUVBuffer;
	unsigned int BufferOffset;
public:
	MBufferObject();
	~MBufferObject();
	void FlipTextureByX();
	void FlipTextureByY();
	void SetDirectionRight(bool Right);
	bool Create(stUVBuffer* inpUVBuffer, float pX0, float pY0, float pW, float pH, float tX0, float tY0, float tW, float tH, unsigned int inTextureId);
	bool Create(float pX0, float pY0, float pW, float pH, float tX0, float tY0, float tW, float tH);
	bool SetBuffer(stUVBuffer* inpUVBuffer);
	bool SetTexture(unsigned int inTextureId);
	
	bool SetVertex(stQuad Vertex);
	bool SetUV(stQuad UV);
	bool Set(stUVQuad UVQuad);
	bool Update();
	
	void Draw();
	stQuad GetVertex();
	stQuad GetUV();
};

#endif
