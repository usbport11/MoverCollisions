#include "stdafx.h"
#include "Text.h"

MText::MText()
{
	TextureId = 0;
	BufferId = 0;
}

MText::~MText()
{
}

bool MText::Initialize(unsigned int inTextureId, unsigned int inFontSize)
{
	if(inFontSize < 8 || inFontSize > 32)
	{
		LogFile<<"Text: Font height too small or too big"<<endl;
		return false;
	}
	if(!inTextureId)
	{
		LogFile<<"Text: NULL font texture"<<endl;
		return false;
	}
	
	FontSize = inFontSize;
	TextureId = inTextureId;
	
	//gen buffer
	glGenBuffers(1, &BufferId);
	GLenum error = glGetError();
	if( error != GL_NO_ERROR )
    {
		LogFile<<"Text: "<<(char*)gluErrorString( error )<<" "<<error<<endl;
		return false;
    }
	
	return true;
}

void MText::Close()
{
	Symbol.clear();
	if(BufferId) glDeleteBuffers(1, &BufferId);
}

bool MText::SetString(float x, float y, char* inString)
{
	unsigned int Length = strlen(inString);
	if(!Length)
	{
		LogFile<<"Text: Empty string given"<<endl;
		return false;
	}
	if(!BufferId)
	{
		LogFile<<"Text: NULL buffer "<<endl;
		return false;
	}
	String = inString;
	
	stUVQuad UVQuad;
	stQuad Vertex, UV;

	float uv_x, uv_y;
	const float Offset = (float)1 / 16;
	const float FixX = 14;
	glm::vec2 point_down_left;
	glm::vec2 point_up_left;
	glm::vec2 point_up_right;
	glm::vec2 point_down_right;

	for(unsigned int i=0; i<Length; i++)
	{
		point_down_left = glm::vec2(x + i * FontSize - i * FixX, y);
		point_up_left = glm::vec2(x + i * FontSize - i * FixX, y + FontSize);
		point_up_right = glm::vec2(x + i * FontSize + FontSize - i * FixX, y + FontSize);
		point_down_right = glm::vec2(x + i * FontSize + FontSize - i * FixX, y);
		SetQuad(Vertex, point_down_left, point_up_left, point_up_right, point_down_right);

		uv_x = (float)(String[i] % 16) / 16;
		uv_y = (float)(1 - (float)(String[i] / 16) / 16 + (float)32 / 512);
		
		point_down_left = glm::vec2(uv_x, uv_y);
		point_up_left = glm::vec2(uv_x, uv_y + Offset);
		point_up_right = glm::vec2(uv_x + Offset, uv_y + Offset);
		point_down_right = glm::vec2(uv_x + Offset, uv_y);
		SetQuad(UV, point_down_left, point_up_left, point_up_right, point_down_right);
		
		SetUVQuad(UVQuad, Vertex, UV);
		Symbol.push_back(UVQuad);
	}

	glBindBuffer(GL_ARRAY_BUFFER, BufferId);
	glBufferData(GL_ARRAY_BUFFER, Symbol.size() * sizeof(stUVQuad), &Symbol[0].p[0][0].x, GL_STATIC_DRAW);
	LogFile<<"Text: Symbols count: "<<Symbol.size()<<endl;
	
	return true;
}

void MText::DrawString()
{
	if(!BufferId || !Symbol.size()) return;
	//vertex
	glBindBuffer(GL_ARRAY_BUFFER, BufferId);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);//4 * sizeof(float)
	//uv
	glBindBuffer(GL_ARRAY_BUFFER, BufferId);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));//4 * sizeof(float)
	//draw
	glBindTexture(GL_TEXTURE_2D, TextureId);
	glDrawArrays(GL_QUADS, 0, Symbol.size() * 4);
}
