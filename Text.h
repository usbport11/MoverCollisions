#ifndef textH
#define textH

#include "stb_truetype.h"	

class MText
{
private:
	char* String;
	unsigned int FontSize;
	unsigned int TextureId;
	unsigned int BufferId;
	vector<stUVQuad> Symbol;
public:
	MText();
	~MText();
	bool Initialize(unsigned int inTextureId, unsigned int inFontSize);
	bool SetString(float x, float y, char* inString);
	void DrawString();
	void Close();
};

#endif
