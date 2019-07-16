#ifndef game2H
#define game2H

#include "Window.h"
#include "TextureLoader.h"
#include "Quad.h"
#include "Level2.h"
#include "Background2.h"
#include "GameObject.h"
#include "Mover.h"
#include "Text.h"

class MGame2;

typedef void (MGame2::*VoidFunc) ();

class MGame2: public MWindow
{
private:
	VoidFunc pDrawFunc;
	VoidFunc pFrameFunc;
	
	//statuses
	bool Pause;
	bool* Key;
	
	//clases
	MTextureLoader* TextureLoader;
	MLevel2* Level2;
	MBackground2* Background2;
	MMover* Mover;
	MText* Text;
	
	DWORD currentTime;
	float deltaTime;
	
	//shader data
	GLuint programID;
	GLuint VertexArrayID;
	GLuint MatrixID;
	GLuint TextureID;
	
	//matrixes and view
	glm::mat4 Projection;
	glm::mat4 View;
	glm::mat4 Model;
	glm::mat4 MVP;
	glm::vec3 CameraPosition;
	glm::vec3 CameraDirection;
	
	//Texture
	stTexture* txQuad;
	unsigned int txQuad_cnt;
	stTexture* txDecor;
	unsigned int txDecor_cnt;
	stTexture* txEdge;
	unsigned int txEdge_cnt;
	stTexture* txBack1;
	unsigned int txBack1_cnt;
	stTexture* txFont;
	unsigned int txFont_cnt;
	
	//simple hero
	stAnimation* HeroAnimation;
	stAnimation* WaitAnimation;
	stUVBuffer UVBuffer;
	MGameObject GameObject[1];
	MAnimatedObject WaitObject;
	
	//draw functions
	void DrawGame();
	void DrawWait();
	void FrameGame();
	void FrameWait();
	
	//overload virtual functions
	void OnDraw();
	void OnKeyUp(WPARAM wParam, LPARAM lParam);
	void OnKeyDown(WPARAM wParam, LPARAM lParam);
	void OnMainTimer();
	void OnFrameTimer();
	void OnSize();
	
	//local
	void Start();
	void Stop();
	
public:
	MGame2();
	~MGame2();
	bool Initialize();
	bool LoadConfig();
	void OnClose();
};

#endif
