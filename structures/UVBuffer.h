#ifndef uvbufferH
#define uvbufferH

/*
UVBuffer holding data of UVQuads -> glm::vec2 p[4][2] (Vetex,UV)
Using:
1) Create variable stUVBuffer
2) Fill UVQuad array
3) Create buffer using InitBuffer
4) For relocate all buffer use RelocateBuffer(), for change one element use UpdateElement()
5) When close programm dont forget free buffer Close()
*/

struct stUVBuffer
{
	GLenum Type;
	unsigned int Id;
	vector<stUVQuad> UVQuad;
	
	bool InitBuffer(GLenum inType)
	{
		Type = inType;
		glGenBuffers(1, &Id);
		glBindBuffer(GL_ARRAY_BUFFER, Id);
		glBufferData(GL_ARRAY_BUFFER, UVQuad.size() * sizeof(stUVQuad), &UVQuad[0].p[0][0], Type);
		GLenum error = glGetError();
		if( error != GL_NO_ERROR )
		{
			LogFile<<"UVBuffer: "<<(char*)gluErrorString(error)<<" "<<error<<endl;
			return false;
		}
		LogFile<<"UVBuffer: Created. Id: "<<Id<<" UVQuads count: "<<UVQuad.size()<<endl;
		
		return true;
	}
	
	bool UpdateElement(unsigned int Offset)
	{
		glBindBuffer(GL_ARRAY_BUFFER, Id);
		glBufferSubData(GL_ARRAY_BUFFER, Offset * sizeof(stUVQuad), sizeof(stUVQuad), &UVQuad[Offset].p[0][0]);//16*float
		GLenum error = glGetError();
		if( error != GL_NO_ERROR )
		{
			LogFile<<"UVBuffer: "<<(char*)gluErrorString(error)<<" "<<error<<endl;
			return false;
		}
		return true;
	}
	
	//if some elemnts adds or removed
	void RelocateBuffer()
	{
		glBindBuffer(GL_ARRAY_BUFFER, Id);
		glBufferData(GL_ARRAY_BUFFER, UVQuad.size() * sizeof(stUVQuad), &UVQuad[0].p[0][0], Type);//16*float
		GLenum error = glGetError();
		if( error != GL_NO_ERROR )
		{
			LogFile<<"UVBuffer: "<<(char*)gluErrorString(error)<<" "<<error<<endl;
			return;
		}
	}
	
	//use this if only in case that UVQuad data static
	void FreeUVQuad()
	{
		UVQuad.clear();
	}
	
	void Close()
	{
		UVQuad.clear();
		glDeleteBuffers(1, &Id);
		LogFile<<"UVBuffer: Closed. Id: "<<Id<<endl;
	}
};

#endif
