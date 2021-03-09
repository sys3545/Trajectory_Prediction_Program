#pragma once
#include <afxwin.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/GLAUX.H>
#include <math.h>

#pragma comment(lib,"GLAUX.LIB")
#pragma comment(lib,"legacy_stdio_definitions.lib")

typedef struct {
	GLfloat xpos = 0.0f, ypos = 0.0f, zpos = 0.0f;
	GLfloat xvel = 0.0f, yvel = 0.0f, zvel = 0.0f;
	GLfloat C1 = 0.0f, C2 = 0.0f, C3 = 0.0f;
	GLfloat h = 0.0f;
	GLfloat angleSpeed = 36.0f; // angle per t
	GLfloat angle = 0.0f;
	GLfloat radius = 7.0f;
	GLUquadricObj* craft = NULL;
}vecInfo_t;


class OPenGLRenderer : public CWnd
{
public:
	OPenGLRenderer();
	virtual ~OPenGLRenderer();
	bool CreateGLContext(CRect rect, CWnd* parent);
	void PrepareScene(int sx, int sy, int cx, int cy);				// Scene preparation
	void DestroyScene();
	int DrawGLScene();				//Here's where we do all the drawing
	bool initAi();
	void CreateCraft(int num); // 우주물체 객체를 생성하는 함수
	
	// 텍스처 관련 함수
	AUX_RGBImageRec* LoadBMPFile(char* filename); // 변수에 이미지를 불러오는 함수
	int LoadGLTextures(); //텍스쳐 작업을 해주는 함수
public:
	// 지구 회전 성분
	GLfloat		xrot = 0;
	GLfloat		yrot = 0;
	GLfloat		zrot = 0;
	GLfloat		GL_PI = (GLfloat)3.141592;

	// 달 회전 성분
	GLfloat		moon_xrot = 0;
	GLfloat		moon_yrot = 0;
	GLfloat		moon_zrot = 0;
	GLfloat		moon_xpos = 7.0f; // 지구와 달거리
	GLfloat		moon_ypos = 0;
	GLfloat		moon_zpos = 0;

	GLUquadricObj* earth = NULL; // 지구 객체
	GLUquadricObj* moon = NULL; // 달 객체

	vecInfo_t spaceCraft[5]; // 우주물체 객체
	int numOfCraft = 0; // 우주물체 갯수

	/// 시간관련 변수
	GLfloat time = 0.000278f; // 단위시간 지정 (초당 속도를 나타내기 위해) -> 0.000278을 기준으로 초당 각속도를 나타낼 수 있음
	int currentTime = 0;
	int oldTime = 0;
	int deltaTime = 0;

	CString test;

	// 텍스처 관련 변수
	AUX_RGBImageRec* pTextureImage[1]; // 이미지 파일에 대한 정보를 가지고 있는 배열
	GLuint textureID[1]; // 텍스처의 핸들을 저장하는 배열

protected:
	bool InitContext(CWnd* parent);					 // Creates OpenGL Rendering Context

	HGLRC m_hrc;                        // OpenGL Rendering Context 
	HDC m_hdc;
	CRect m_rect;

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};

