#pragma once
#include <afxwin.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/GLAUX.H>
#include <math.h>

#pragma comment(lib,"GLAUX.LIB")
#pragma comment(lib,"legacy_stdio_definitions.lib")

typedef struct vecInfo_t {
	GLfloat xpos = 0.0f, ypos = 0.0f, zpos = 0.0f;
	GLfloat xvel = 0.0f, yvel = 0.0f, zvel = 0.0f;
	GLfloat C1 = 0.0f, C2 = 0.0f, C3 = 0.0f, h = 0.0f;
	GLfloat i = 0.0f, omega = 0.0f, a = 0.0f, e = 0.0f, w = 0.0f, T = 0.0f; // 궤도의 6요소
	GLfloat n = 0.0f, M = 0.0f, E = 0.0f, preRadius = 0.0f, preF = 0.0f; // n : mean motion, M : mean anomaly, E : eccentric anomaly
	GLfloat u = 0.0f, f = 0.0f, p = 0.0f; // u = w + f , f는 진근지점이각 True anomaly , p는 반직현
	GLfloat range = 0.0f, velocity = 0.0f; // 초기 거리, 초기 속도
	GLfloat traj_range = 0.0f, traj_xpos = 0.0f, traj_ypos = 0.0f;  // 궤도를 그리기 위한 변하는 변수들
	GLfloat angleSpeed = 0.0f, angle = 0.0f, P = 0.0f; // 각속도, 변하는 각  (초기각은 f) , 주기
	GLfloat radius = 0.0f; // 변하는 거리 (초기 거리에서 시작)
	GLint isSelected = 0; // 선택이 되었는가?
	GLint isStarted = 0; // 예측 모드가 시작 되었는가?
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

	// 그리기용 사용자 설정 함수
	void CreateCraft(int num); // 우주물체 객체를 생성하는 함수
	void DrawTrajectory(int num); // 우주물체 궤적을 그려주는 함수
	void DrawSphere(int num); // 우주물체 구를 그려주는 함수

	// 텍스처 관련 함수
	AUX_RGBImageRec* LoadBMPFile(char* filename); // 변수에 이미지를 불러오는 함수
	int LoadGLTextures(); //텍스쳐 작업을 해주는 함수

public:
	// 지구의 성분
	GLfloat		zrot = 0; // 지구 자전 속도
	GLfloat     radius_Earth = 6.378f; ////////// 1.0f = 1000km //////////
	GLfloat     mass_Earth = (GLfloat)5.972 * (GLfloat)pow(10, 24); // kg
	double      G = 6.673 * pow(10, -11); // m^3/kg/s^2 (중력상수)

	// 달 회전 성분
	GLfloat		moon_zrot = 0; // 달 공전 속도
	GLfloat		moon_xpos = 7.0f; // 지구와 달거리
	GLfloat		moon_ypos = 0;
	GLfloat		moon_zpos = 0;

	GLUquadricObj* earth = NULL; // 지구 객체
	GLUquadricObj* moon = NULL; // 달 객체
	vecInfo_t spaceCraft[6]; // 우주물체 객체
	int numOfCraft = 0; // 우주물체 갯수
	GLUquadricObj* preCraft[6]; // 예측궤도 그릴시에 사용할 객체

	/// 수식에 필요한 변수선언
	GLfloat		GL_PI = (GLfloat)3.141592;
	GLfloat     mass_Sun = 2.0f * (GLfloat)pow(10, 30); // kg

	/// 시간관련 변수
	GLfloat timeScale = 63.38f; // 1초는 63.38 tick 이다. 원래 각속도에 이 값을 나누면 된다. 
	int currentTime = 0;
	int oldTime = 0;
	int deltaTime = 0;
	int realTime = 1; // 실제시간과의 배속이 어느정도 인지 보여주는 변수

	CString test;

	// 텍스처 관련 변수
	AUX_RGBImageRec* pTextureImage[1]; // 이미지 파일에 대한 정보를 가지고 있는 배열
	GLuint textureID[1]; // 텍스처의 핸들을 저장하는 배열

	// 마우스 입력시 회전 변수
	BOOL b_Rotate; // 마우스 상태에 따른 회전을 감지
	GLfloat mousePoint; // 마우스 위치를 잡는 변수
	GLfloat differ; // 마우스가 움직인 정도
	GLfloat zAngle = 0.0f; // 화면 회전 각도
	GLfloat zoom = 0.0f; // 줌 정도

	// 궤도 예측 관련 함수
	void PredictionPosition(int num, GLfloat time); // mean motion, E 등을 이용하여 일정시간 뒤의 선택된 물체의 위치를 구하는 함수
	void DrawPrediction(int num); // 예측 극좌표 결과를 그려주는 함수
	void CalculateT(int num); // 근지점 통과시를 구하는 함수 (초 단위)
	void CreatPreCraft(int num); // 예측 물체의 객체를 생성하는 함수

protected:
	bool InitContext(CWnd* parent);					 // Creates OpenGL Rendering Context

	HGLRC m_hrc;                        // OpenGL Rendering Context 
	HDC m_hdc;
	CRect m_rect;

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
};
