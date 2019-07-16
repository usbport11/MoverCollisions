#include "stdafx.h"
#include "Game2.h"

MGame2::MGame2():MWindow()
{
	Pause = true;
    Key = new bool [256];
    memset(Key, 0, 256);
	TextureLoader = NULL;
	
	Level2 = NULL;
	Background2 = NULL;
	Mover = NULL;
	
	txQuad = NULL;
	txEdge = NULL;
	txDecor = NULL;
	txBack1 = NULL;
	txFont = NULL;
	HeroAnimation = NULL;
	WaitAnimation = NULL;
	
	programID = 0;
	VertexArrayID = 0;
	
	pDrawFunc = NULL;
	Text = NULL;
}

MGame2::~MGame2()
{
	TextureLoader = NULL;
	txQuad = NULL;
	txEdge = NULL;
	Level2 = NULL;
	Background2 = NULL;
	pDrawFunc = NULL;
}

bool MGame2::Initialize()
{	
    LogFile<<"Game2: Initialize"<<endl;
    GLenum Error;
    
    if(!WindowInitialized)
    {
    	LogFile<<"Game2: Window was not initialized"<<endl;
    	return 0;
	}
	
	//randomize
    LogFile<<"Game2: randomize rand by time"<<endl; 
    srand(time(NULL));
    
    LogFile<<"Game2: Load thread: started"<<endl;
	
	//texture prepare
    TextureLoader = new MTextureLoader;
	
	TextureLoader->LinearMode = false;
    TextureLoader->Wrap_Clamp = true;
    TextureLoader->Wrap_Repeat = false;

    txQuad = TextureLoader->LoadTexture("textures/tex02.png", 2, 1, 0, txQuad_cnt);
    if(!txQuad) return false;
    txDecor = TextureLoader->LoadTexture("textures/tex07.png", 3, 1, 0, txDecor_cnt);
    if(!txDecor) return false;
    txFont = TextureLoader->LoadTexture("font/font.png", 1, 1, 0, txQuad_cnt);
    if(!txFont) return false;
    
    HeroAnimation = new stAnimation[2];
    HeroAnimation[0].Texture = TextureLoader->LoadTexture("textures/tex04.png", 4, 1, 0,  HeroAnimation[0].Count);
    if(!HeroAnimation[0].Texture) return false;
    HeroAnimation[0].SetParameters(1, 0);
    HeroAnimation[1].Texture = TextureLoader->LoadTexture("textures/tex05.png", 4, 1, 0,  HeroAnimation[1].Count);
    if(!HeroAnimation[1].Texture) return false;
    HeroAnimation[1].SetParameters(2, 0);
    
    WaitAnimation = new stAnimation;
    WaitAnimation->Texture = TextureLoader->LoadTexture("textures/tex09.png", 4, 1, 0,  WaitAnimation->Count);
    if(!WaitAnimation->Texture) return false;
    WaitAnimation->SetParameters(1, 0);
    
    TextureLoader->Wrap_Clamp = false;
    TextureLoader->Wrap_Repeat = true; 
    txEdge = TextureLoader->LoadTexture("textures/tex15.png", 1, 1, 0, txEdge_cnt);
    if(!txEdge) return false;
    txBack1 = TextureLoader->LoadTexture("textures/tex11.png", 2, 1, 0, txBack1_cnt);
    if(!txBack1) return false;
    
    //shader prepare
    glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);
	Error = glGetError();
	if(Error != GL_NO_ERROR)
	{
		LogFile<<"Game2: "<<(char*)gluErrorString(Error)<<" "<<Error<<endl;
		return false;
	}
	
	programID = LoadShaders("shaders/simple.vertexshader.glsl", "shaders/simple.fragmentshader.glsl");
	MatrixID = glGetUniformLocation(programID, "MVP");
	Error = glGetError();
	if(Error != GL_NO_ERROR)
	{
		LogFile<<"Game2: "<<(char*)gluErrorString(Error)<<" "<<Error<<endl;
		return false;
	}
	CameraPosition = glm::vec3(0,0,1);
	CameraDirection = glm::vec3(0,0,0);
	Projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, -1.0f, 1.0f);
	Model = glm::mat4(1.0f);
	View = glm::lookAt(CameraPosition, CameraDirection, glm::vec3(0,1,0));
    MVP = Projection * View * Model;
    
    TextureID = glGetUniformLocation(programID, "myTextureSampler");
    Error = glGetError();
	if(Error != GL_NO_ERROR)
	{
		LogFile<<"Game2: "<<(char*)gluErrorString(Error)<<" "<<Error<<endl;
		return false;
	}
	
	//text
	Text = new MText;
	if(!Text->Initialize(txFont->Id, 24)) return false;
	if(!Text->SetString(50, 50, (char*)"Press [SPACE] to continue...")) return false;
	
	//level2
    Level2 = new MLevel2;
    Level2->SetZonesCount(1, 1);
    Level2->SetUnit(32, 32, 25, 18);
    Level2->SetBoxLimits(2, 4, 1, 2);
    Level2->SetHoleLimit(1, 1);
    Level2->SetGravity(0.5);
    Level2->SetTexture(txQuad, txQuad_cnt);
    Level2->SetEdgeTexture(txEdge);
    if(!Level2->GenerateLevel()) return false;
    
    //Background 2
    Background2 = new MBackground2[2];
    Background2[0].SetMainTexture(txEdge);
    Background2[0].SetMainUVSize(25, 18);
	Background2[0].SetDecorTextures(txDecor, txDecor_cnt);
	Background2[0].SetCellCount(3, 3);
	if(!Background2[0].Generate(800, 600)) return false;
	
	Background2[1].SetMainTexture(&txBack1[1]);
    Background2[1].SetMainUVSize(1, 1);
	if(!Background2[1].Generate(800, 600)) return false;
		
	//uv buffer and objects
	UVBuffer.Id = 0;
	if(!GameObject[0].Create(&UVBuffer, 50, 50, 30, 30, 0, 0, 1, 1, HeroAnimation)) return false;
	GameObject[0].SetDirectionRight(true);
    GameObject[0].SetVelocityValue(3, 0);
    GameObject[0].SetJumpValue(10);
    if(!WaitObject.Create(&UVBuffer, 400-32, 300-32, 64, 64, 0, 0, 1, 1, WaitAnimation)) return false;
	if(!UVBuffer.InitBuffer(GL_STREAM_DRAW)) return false;
	
	//mover
	Mover = new MMover;
	Mover->SetLevel(Level2);
	if(!Mover->SetObjectOnPlane(0, 1, &GameObject[0], 0.5, false)) return false;
	
    //VERY IMPORTANT! (Do not use game resources before textures loads)
    //pDrawFunc = &MGame2::DrawGame;
    //pFrameFunc = &MGame2::FrameGame;
    pDrawFunc = &MGame2::DrawWait;
    pFrameFunc = &MGame2::FrameWait;
    Pause = false;
    LogFile<<"Game2: pause is "<<Pause<<endl;

    
    return true;
}

void MGame2::Start()
{
    Pause = false;
}

void MGame2::Stop()
{
    Pause = true;
}

void MGame2::DrawGame()
{
	Mover->Move(&GameObject[0]);
	if(Mover->GetCanMove() == 1 || Mover->GetCanMove() == 3) Background2[1].Move();
	
	Background2[0].Draw();
	Background2[1].Draw();
	Level2->Draw();
	GameObject[0].Draw();
}

void MGame2::DrawWait()
{
	WaitObject.Draw();
	Text->DrawString();
}

void MGame2::FrameGame()
{
	GameObject[0].IncAnimationFrame();
}

void MGame2::FrameWait()
{
	WaitObject.IncAnimationFrame();
}

void MGame2::OnDraw()
{
	static DWORD lastTime = GetTickCount();
	currentTime = GetTickCount();
	deltaTime = (float)(currentTime - lastTime) / 1000 + 1;
		
	//shader programm
	glUseProgram(programID);
	
	//matrix uniform parameter
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
	
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
		
	//shader texture parameter
	glUniform1i(TextureID, 0);
		
	if(pDrawFunc) ((*this).*(pDrawFunc))();
	
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	
	lastTime = currentTime;
}

void MGame2::OnKeyDown(WPARAM wParam, LPARAM lParam)
{
	Key[wParam] = 1;
	if(pDrawFunc == &MGame2::DrawGame)
	{
		if(Key[VK_ESCAPE])
		{
			pDrawFunc = &MGame2::DrawWait;
			pFrameFunc = &MGame2::FrameWait;
			return;
		}
		//if(!Pause)
		//{
			if(wParam == VK_SPACE && !(lParam  & 0x40000000)) GameObject[0].StartJump();
			if(Key[VK_LEFT])
			{
				GameObject[0].SetMove(0);
				Background2[1].SetMoveVelocity(-0.005, 0);
			}
			if(Key[VK_RIGHT])
			{
				GameObject[0].SetMove(1);
				Background2[1].SetMoveVelocity(0.005, 0);
			}
		//}
	}
	if(pDrawFunc == &MGame2::DrawWait)
	{
		if(Key[VK_ESCAPE])
		{
			SendMessage(m_hWnd, WM_DESTROY, 0, 0);
			return;
		}
		if(Key[VK_SPACE])
		{
			pDrawFunc = &MGame2::DrawGame;
			pFrameFunc = &MGame2::FrameGame;
		}
	}
}

void MGame2::OnKeyUp(WPARAM wParam, LPARAM lParam)
{
	Key[wParam] = 0;
	if(pDrawFunc == &MGame2::DrawGame)
	{
		//if(!Pause)
		//{
			if(!Key[VK_LEFT] && !Key[VK_RIGHT])
			{
				GameObject[0].SetVelocityX(0);
				Background2[1].SetMoveVelocity(0, 0);
			}
		//}
	}
}

void MGame2::OnClose()
{
	Stop();
	LogFile<<"Game2: Stopped."<<endl;

	if(Level2)
	{
		Level2->Close();
		delete Level2;
	}
	LogFile<<"Game2: Level2 free"<<endl;
	
	if(Background2)
	{
		Background2[0].Close();
		Background2[1].Close();
		delete [] Background2;
	}
	LogFile<<"Game2: Background2 free"<<endl;
	
	UVBuffer.Close();
	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &VertexArrayID);
	LogFile<<"Game2: Shader free"<<endl;
	
	if(Key) delete [] Key;
	LogFile<<"Game2: Keys free"<<endl;
	
    if(TextureLoader)
	{
		TextureLoader->DeleteTexture(txQuad, txQuad_cnt);
		TextureLoader->DeleteTexture(txDecor, txDecor_cnt);
		TextureLoader->DeleteTexture(txEdge, txEdge_cnt);
		TextureLoader->DeleteTexture(txBack1, txBack1_cnt);
		TextureLoader->DeleteTexture(txFont, txFont_cnt);
		TextureLoader->DeleteTexture(HeroAnimation[0].Texture, HeroAnimation[0].Count);
		TextureLoader->DeleteTexture(HeroAnimation[1].Texture, HeroAnimation[1].Count);
		TextureLoader->DeleteTexture(WaitAnimation->Texture, WaitAnimation->Count);
		TextureLoader->Close();
		delete TextureLoader;
	}
	if(HeroAnimation) delete [] HeroAnimation;
	if(WaitAnimation) delete WaitAnimation;
	LogFile<<"Game: Textures, Animations and TextureLoader free"<<endl;
	
	if(Mover) delete Mover;
	LogFile<<"Game: Mover free"<<endl;
	
	if(Text)
	{
		Text->Close();
		delete Text;
	}
	LogFile<<"Game: Text free"<<endl;
}

void MGame2::OnMainTimer()
{
}

void MGame2::OnFrameTimer()
{
	if(Pause) return;
	if(pFrameFunc) ((*this).*(pFrameFunc))();
}

void MGame2::OnSize()
{
	Projection = glm::ortho(0.0f, (float)WindowWidth, 0.0f, (float)WindowHeight, -1.0f, 1.0f);
}
