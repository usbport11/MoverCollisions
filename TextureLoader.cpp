#include "stdafx.h"
#include "TextureLoader.h"

MTextureLoader::MTextureLoader()
{
	LinearMode = false;
	Tga = new MTga;
	Png = new MPng;
	pImageData = NULL;
	ImageWidth = 0;
	ImageHeight = 0;
	BitsPerPixel = 0;
	Wrap_Clamp = true;
	Wrap_Repeat = false;
}

MTextureLoader::~MTextureLoader()
{
}

stTexture* MTextureLoader::LoadTexture(const char* inFileName, unsigned int CountX, unsigned int CountY, unsigned int CountToRead, unsigned int& ReturnCount)
{
	if(!CountX | !CountY) 
	{
		LogFile<<"Tx loader: CountX or CountY empty"<<endl;
		return NULL;
	}
	if(CountToRead > CountX * CountY)
	{
		LogFile<<"Tx loader: Count to read more than CountX * CountY"<<endl;
		return NULL;
	}
	if(!LoadFile(inFileName)) return NULL;
	
	stTexture* retTexture;
	
	if(CountX == 1 && CountY == 1 && (CountToRead == 0 || CountToRead == 1))
    {
    	LogFile<<"Tx loader: Single texture"<<endl;
    	try
	    {
			retTexture = new stTexture [1];
		}
		catch(bad_alloc& ba)
	    {
			LogFile<<"Tx loader: Cant allocate memory for retTexture"<<endl;
	        return NULL;
	    }
		if(!CreateTexture(retTexture[0].Id, ImageWidth, ImageHeight, pImageData)) return NULL;
	    FreeImage();
	    
	    ReturnCount = 1;
	    return retTexture;
	}
	
	LogFile<<"Tx loader: Multiply texture"<<endl;
	unsigned char* pSubImageData;
	unsigned int TotalCount;
	unsigned int CountReaded = 0;
	if(CountToRead) TotalCount = CountToRead;
	else TotalCount = CountX * CountY;
	unsigned int SubImageSize = ImageWidth * ImageHeight * (BitsPerPixel / 8 ) / (CountX * CountY);
	if(!SubImageSize)
	{
		LogFile<<"Tx loader: Cant allocate memory for pSubImageData"<<endl;
		return NULL;
	}
	unsigned int SubImageSizeX = ImageWidth * (BitsPerPixel / 8 ) / CountX;
	unsigned int SubImageSizeY = ImageHeight * (BitsPerPixel / 8 ) / CountY;
	unsigned int ImageSizeX = ImageWidth * (BitsPerPixel / 8 );
	unsigned int ImageSizeY = ImageHeight * (BitsPerPixel / 8 );
	unsigned int SubImageWidth = ImageWidth / CountX;
	unsigned int SubImageHeight = ImageHeight / CountY;
	if(!SubImageWidth || !SubImageHeight) return NULL;
	
	LogFile<<"Tx loader: SubImage size: "<<SubImageSize<<" bytes"<<endl;
	LogFile<<"Tx loader: SubImage width: "<<SubImageWidth<<endl;
	LogFile<<"Tx loader: SubImage height: "<<SubImageHeight<<endl;
	LogFile<<"Tx loader: SubImage count: "<<TotalCount<<endl;
	
	try
    {
		pSubImageData = new unsigned char [SubImageSize];
	}
	catch(bad_alloc& ba)
    {
		LogFile<<"Tx loader: Cant allocate memory for pSubImageData"<<endl;
        return NULL;
    }
	
	try
    {
		retTexture = new stTexture [TotalCount];
	}
	catch(bad_alloc& ba)
    {
    	LogFile<<"Tx loader: Cant allocate memory for retTexture"<<endl;
		if(pSubImageData) delete [] pSubImageData;
        return NULL;
    }
	
	for(unsigned int i=0; i<CountX; i++)
	{
		memset(pSubImageData, 0, SubImageSize);
		for(unsigned int j=0; j<CountY; j++)
		{
			for(unsigned int k=0; k<ImageHeight; k++)
			{
				memcpy(pSubImageData + (k * SubImageSizeX), pImageData + (i * SubImageSizeX + j * SubImageSizeY * ImageSizeX + k * ImageSizeX), SubImageSizeX);
			}
			if(!CreateTexture(retTexture[i].Id, SubImageWidth, SubImageHeight, pSubImageData))
			{
				if(pSubImageData) delete [] pSubImageData;
				if(retTexture) delete [] retTexture;
				return NULL;
			}
			if(CountToRead)
			{
				CountReaded ++;
				if(CountReaded >= CountToRead - 1) break;
			}
		}
	}
	
	if(pSubImageData) delete [] pSubImageData;
	FreeImage();
	
	ReturnCount = TotalCount;
	return retTexture;
}

bool MTextureLoader::CreateTexture(unsigned int& Id, unsigned long Width, unsigned long Height, unsigned char* Data)
{
	glGenTextures(1, &Id);
    glBindTexture(GL_TEXTURE_2D, Id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, Data);
    if(LinearMode)
	{
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
    else
	{
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
	if(Wrap_Clamp)
	{
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
	if(Wrap_Repeat)
	{
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
    LogFile<<"Tx loader: Texture id: "<<Id<<endl;
    GLenum error = glGetError();
    if( error != GL_NO_ERROR )
    {
        LogFile<<error<<" "<<(char*)gluErrorString(error)<<endl;
        return false;
    }

	return true;	
}

bool MTextureLoader::LoadFile(const char* inFileName)
{
	if(!inFileName)
    {
        LogFile<<"Tx loader: Empty texture filename given"<<endl;
        return false;
    }
	LogFile<<"Tx loader: Trying load file: "<<inFileName<<endl;
	
	FILE* pFile = fopen(inFileName, "rb");
	if(pFile == NULL)
	{
		LogFile<<"Tx loader: Can not open"<<endl;
		return false;
	}
	fread((void*)FileType, 1, 1, pFile);
	fclose(pFile);
	switch(FileType[0])
	{
		case 0x89:
			LogFile<<"Tx loader: PNG file"<<endl;
			if(!Png->LoadPNG(inFileName))
			{
				Png->FreePng();
				return false;
			}
			pImageData = Png->GetImageData();
			ImageWidth = Png->GetWidth();
			ImageHeight = Png->GetHeight();
			BitsPerPixel = Png->GetBitsPerPixel();
			break;
		case 0x00:
			LogFile<<"Tx loader: TGA file"<<endl;
			if(!Tga->LoadTGA(inFileName))
			{
				Tga->FreeTga();
				return false;
			}
			pImageData = Tga->GetImageData();
			ImageWidth = Tga->GetWidth();
			ImageHeight = Tga->GetHeight();
			BitsPerPixel = Tga->GetBitsPerPixel();
			break;
		default:
			LogFile<<"Tx loader: Unsupported file format"<<endl;
			return false;
	}
	
	return true;
}

void MTextureLoader::FreeImage()
{
	switch(FileType[0])
	{
		case 0x89:
			Png->FreePng();
			break;
		case 0x00:
			Tga->FreeTga();
			break;
	}
}

void MTextureLoader::Close()
{
	if(Tga) delete Tga;
	if(Png) delete Png;
}

bool MTextureLoader::DeleteTexture(stTexture* Texture, unsigned int Count)
{
	if(!Count)
	{
		LogFile<<"Tx loader: Zero textures count given"<<endl;
		return false;
	}
	if(!Texture)
	{
		LogFile<<"Tx loader: Texture(s) is NULL"<<endl;
		return false;
	}
	//unsigned int Address = Texture;
	for(int i=0; i<Count; i++)
	{
		LogFile<<Texture[i].Id<<" ";
		glDeleteTextures(1, &Texture[i].Id);
	}
	LogFile<<endl;
	GLenum error = glGetError();
    if( error != GL_NO_ERROR )
    {
        LogFile<<"Tx loader: Opengl error: "<<error<<" "<<(char*)gluErrorString(error)<<endl;
        delete [] Texture;
        return false;
    }
    delete [] Texture;
    LogFile<<"Tx loader: Texture(s) successefully deleted"<<endl;
    
    return true;
}
