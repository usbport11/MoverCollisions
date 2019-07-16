#include "stdafx.h"
#include "BufferObject.h"

MBufferObject::MBufferObject()
{
	pUVBuffer = NULL;
	BufferOffset = 0;
	TextureId = 0;
	DirectionRight = true;
}

MBufferObject::~MBufferObject()
{
	pUVBuffer = NULL;
	BufferOffset = 0;
	TextureId = 0;
}

void MBufferObject::SetDirectionRight(bool Right)
{
	if(Right != DirectionRight) FlipTextureByX();
	DirectionRight = Right;
}

void MBufferObject::FlipTextureByX()
{
	if(!pUVBuffer) return;
	swap(pUVBuffer->UVQuad[BufferOffset].p[0][1], pUVBuffer->UVQuad[BufferOffset].p[3][1]);
	swap(pUVBuffer->UVQuad[BufferOffset].p[1][1], pUVBuffer->UVQuad[BufferOffset].p[2][1]);
}

void MBufferObject::FlipTextureByY()
{
	if(!pUVBuffer) return;
	swap(pUVBuffer->UVQuad[BufferOffset].p[0][1], pUVBuffer->UVQuad[BufferOffset].p[1][1]);
	swap(pUVBuffer->UVQuad[BufferOffset].p[2][1], pUVBuffer->UVQuad[BufferOffset].p[3][1]);
}

bool MBufferObject::Create(stUVBuffer* inpUVBuffer, float pX0, float pY0, float pW, float pH, float tX0, float tY0, float tW, float tH, unsigned int inTextureId)
{
	if(!inpUVBuffer)
	{
		LogFile<<"Buffer object: NULL buffer"<<endl;
		return false;
	}
	if(!inTextureId)
	{
		LogFile<<"Buffer object: Null texture"<<endl;
		return false;
	}
	if(pW < 0.001 && pH > -0.001) return false;
	if(tW < 0.001 && tH > -0.001) return false;
	pUVBuffer = inpUVBuffer;
	TextureId = inTextureId;
	
	stQuad Vertex;
	stQuad UV;
	stUVQuad UVQuad;
	SetQuad(Vertex, pX0, pY0, pW, pH);
	SetQuad(UV, tX0, tY0, tW, tH);
	SetUVQuad(UVQuad, Vertex, UV);
	
	pUVBuffer->UVQuad.push_back(UVQuad);
	BufferOffset = pUVBuffer->UVQuad.size() - 1;
	
	LogFile<<"Buffer object create: Offset: "<<BufferOffset<<" Data: x: "<<pX0<<" y: "<<pY0<<" w: "<<pW<<" h: "<<pH<<" Buffer id: "<<pUVBuffer->Id<<endl;

	return true;
}

bool MBufferObject::Create(float pX0, float pY0, float pW, float pH, float tX0, float tY0, float tW, float tH)
{
	if(!pUVBuffer) 
	{
		LogFile<<"Buffer object: NULL buffer"<<endl;
		return false;
	}
	if(pW < 0.001 && pH > -0.001) return false;
	if(tW < 0.001 && tH > -0.001) return false;
	
	stQuad Vertex;
	stQuad UV;
	stUVQuad UVQuad;
	SetQuad(Vertex, pX0, pY0, pW, pH);
	SetQuad(UV, tX0, tY0, tW, tH);
	SetUVQuad(UVQuad, Vertex, UV);
	
	pUVBuffer->UVQuad.push_back(UVQuad);
	BufferOffset = pUVBuffer->UVQuad.size() - 1;
	
	LogFile<<"Buffer object create: Offset: "<<BufferOffset<<" Data: x: "<<pX0<<" y: "<<pY0<<" w: "<<pW<<" h: "<<pH<<" Buffer id: "<<pUVBuffer->Id<<endl;
	
	return true;
}

bool MBufferObject::SetBuffer(stUVBuffer* inpUVBuffer)
{
	if(!inpUVBuffer)
	{
		LogFile<<"Buffer object: NULL buffer"<<endl;
		return false;
	}
	pUVBuffer = inpUVBuffer;
	
	return true;
}

bool MBufferObject::SetTexture(unsigned int inTextureId)
{
	if(!inTextureId)
	{
		LogFile<<"Buffer object: Null texture"<<endl;
		return false;
	}
	TextureId = inTextureId;
	
	return true;
}

bool MBufferObject::SetVertex(stQuad Vertex)
{
	if(!pUVBuffer || !pUVBuffer->Id || BufferOffset > pUVBuffer->UVQuad.size()) return false;
	SetUVQuad(pUVBuffer->UVQuad[BufferOffset], Vertex, GetUVQuad(pUVBuffer->UVQuad[BufferOffset]));
	
	return true;
}

bool MBufferObject::SetUV(stQuad UV)
{
	if(!pUVBuffer || !pUVBuffer->Id || BufferOffset > pUVBuffer->UVQuad.size()) return false;
	SetUVQuad(pUVBuffer->UVQuad[BufferOffset], GetVertexQuad(pUVBuffer->UVQuad[BufferOffset]), UV);
	
	return true;
}

bool MBufferObject::Set(stUVQuad UVQuad)
{
	if(!pUVBuffer || !pUVBuffer->Id || BufferOffset > pUVBuffer->UVQuad.size()) return false;
	pUVBuffer->UVQuad[BufferOffset] = UVQuad;
	
	return true;
}

bool MBufferObject::Update()
{
	if(!pUVBuffer || !pUVBuffer->Id || BufferOffset > pUVBuffer->UVQuad.size()) return false;
	if(!pUVBuffer->UpdateElement(BufferOffset)) return false;
	
	return true;
}

void MBufferObject::Draw()
{
	if(!pUVBuffer || !pUVBuffer->Id || BufferOffset > pUVBuffer->UVQuad.size()) return;
	//vertex
	glBindBuffer(GL_ARRAY_BUFFER, pUVBuffer->Id);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), 0);
	//uv
	glBindBuffer(GL_ARRAY_BUFFER, pUVBuffer->Id);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)8);
	//draw
	glBindTexture(GL_TEXTURE_2D, TextureId);
	glDrawArrays(GL_QUADS, BufferOffset * 4, 4);
}


stQuad MBufferObject::GetVertex()
{
	stQuad Quad;
	NullQuad(Quad);
	if(!pUVBuffer || !pUVBuffer->Id || BufferOffset > pUVBuffer->UVQuad.size()) return Quad;
	
	return GetVertexQuad(pUVBuffer->UVQuad[BufferOffset]);
}

stQuad MBufferObject::GetUV()
{
	stQuad Quad;
	NullQuad(Quad);
	if(!pUVBuffer || !pUVBuffer->Id || BufferOffset > pUVBuffer->UVQuad.size()) return Quad;
	
	return GetUVQuad(pUVBuffer->UVQuad[BufferOffset]);
}
