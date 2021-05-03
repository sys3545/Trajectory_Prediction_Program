
#include "pch.h"
#include "OPenGLRenderer.h"

BEGIN_MESSAGE_MAP(OPenGLRenderer, CWnd)
	ON_WM_TIMER()
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()


void OPenGLRenderer::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	switch (nIDEvent)
	{
	case 1:
		wglMakeCurrent(m_hdc, m_hrc);
		DrawGLScene();
		SwapBuffers(m_hdc);
		wglMakeCurrent(m_hdc, NULL);

		break;
	default:
		break;
	}

	CWnd::OnTimer(nIDEvent);
}

OPenGLRenderer::OPenGLRenderer()
{
	m_hdc = NULL;
}

OPenGLRenderer::~OPenGLRenderer()
{

}

bool OPenGLRenderer::CreateGLContext(CRect rect, CWnd* parent)
{
	if (!InitContext(parent))
	{
		MessageBox(_T("ERROR Creating InitContext"));
		return false;
	};
	return true;
}

// 컨텍스트 초기화
bool OPenGLRenderer::InitContext(CWnd* parent)
{
	int bits = 16;
	static	PIXELFORMATDESCRIPTOR pfd =					// pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),					// Size Of This Pixel Format Descriptor
		1,												// Version Number
		PFD_DRAW_TO_WINDOW |							// Format Must Support Window
		PFD_SUPPORT_OPENGL |							// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,								// Must Support Double Buffering
		PFD_TYPE_RGBA,									// Request An RGBA Format
		BYTE(bits),											// Select Our Color Depth
		0, 0, 0, 0, 0, 0,								// Color Bits Ignored
		0,												// No Alpha Buffer
		0,												// Shift Bit Ignored
		0,												// No Accumulation Buffer
		0, 0, 0, 0,										// Accumulation Bits Ignored
		16,												// 16Bit Z-Buffer (Depth Buffer)
		0,												// No Stencil Buffer
		0,												// No Auxiliary Buffer
		PFD_MAIN_PLANE,									// Main Drawing Layer
		0,												// Reserved
		0, 0, 0											// Layer Masks Ignored
	};

	m_hdc = ::GetDC(m_hWnd);
	int nPixelFormat = ChoosePixelFormat(m_hdc, &pfd);
	if (nPixelFormat == 0) return false;
	BOOL bResult = SetPixelFormat(m_hdc, nPixelFormat, &pfd);
	if (!bResult) return false;

	m_hrc = wglCreateContext(m_hdc);
	wglMakeCurrent(m_hdc, m_hrc);

	if (!m_hrc)
		return false;

	CString str;
	str.Format(_T("OpenGL version: %s\n"), (CString)glGetString(GL_VERSION));
	TRACE(str);
	return true;
}

// 초기 씬 준비
void OPenGLRenderer::PrepareScene(int sx, int sy, int cx, int cy)
{
	glClearColor(0.0, 0.0, 1.0, 0.0); //background to clear with.
	wglMakeCurrent(m_hdc, m_hrc);

	GLfloat fAspect;

	if (cy == 0) cy = 1;

	glViewport(sx, sy, cx, cy);

	fAspect = (GLfloat)cx / (GLfloat)cy;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(50.0f, fAspect, 1.0f, 10000.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glEnable(GL_DEPTH_TEST); // 깊이 버퍼를 사용할 것을 알림

	zoom = -30.0f; // 초기 줌 정도 설정
	earth = gluNewQuadric(); // 지구 객체 인스턴스 생성
	moon = gluNewQuadric(); // 달 객체 인스턴스 생성
	LoadGLTextures();

	oldTime = glutGet(GLUT_ELAPSED_TIME); //oldTime 값은 초기화과정 맨 마지막에 적용
	wglMakeCurrent(m_hdc, NULL);
}

// 초기 설정 작업
bool OPenGLRenderer::initAi()
{
	wglMakeCurrent(m_hdc, m_hrc);

	glEnable(GL_LIGHT1);

	wglMakeCurrent(m_hdc, NULL);


	return true;
}

// 그리기 메소드
int OPenGLRenderer::DrawGLScene()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f); //배경 클리어
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 버퍼 비트 클리어 (고정)
	glLoadIdentity(); // (고정)
	glTranslatef(0.0f, 0.0f, zoom); //원점의 이동
	glRotatef(zAngle, 0.0f, 1.0f, 0.0f); // z축을 기준으로 좌표축 회전(화면 회전용)

	////draw Earth
	glPushMatrix(); // 지구중심좌표 추가
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f); // x축을 기준으로 좌표축 -90도 회전( 자전축 맞추기용 )
	
	glRotatef(zrot, 0.0f, 0.0f, 1.0f); //회전 변환 (z축) (지구 자전)
	gluQuadricDrawStyle(earth, GLU_FILL); // 객체를 채우는 형태로 설정
	glColor3f(1.0f, 1.0f, 1.0f); // 색 지정
	gluSphere(earth, radius_Earth, 24, 24); // 구를 그림

	gluQuadricDrawStyle(earth, GLU_LINE); // 선을 긋는 형태로 설정
	glColor3f(0.2f, 0.2f, 1.0f); // 색 지정
	gluSphere(earth, radius_Earth + (GLfloat)0.05f, 24, 24); // 구를 그림

	// Blue coordinate (z축 좌표)
	glColor3f(0, 0, 1);
	glBegin(GL_LINE_LOOP);
	glVertex3f(0.0, 0.0, 40.0);
	glVertex3f(0.0, 0.0, -40.0);
	glEnd();
	glPopMatrix(); // 지구 중심좌표 제거
	/*
	glPushMatrix(); ////// 달 그리기 위한 좌표 추가  /////////
	glRotatef(20.0f, 0.0f, 0.0f, 1.0f); // z축을 기준으로 좌표축 20도 회전( 궤도면 회전 )
	glRotatef(moon_zrot, 0.0f, 1.0f, 0.0f); //회전 변환 (z축) (달 공전) , 그리고 회전을 적용
	glTranslatef(7.0f, 0.0f, 0.0f); // 지구와의 거리만큼 x축에서 이동
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f); // x축을 기준으로 좌표축 -90도 회전( 회전축 맞추기용 )

	gluQuadricDrawStyle(moon, GLU_FILL); // 객체를 채우는 형태로 설정
	glColor3f(0.35f, 0.35f, 0.35f);
	gluSphere(moon, 0.3f, 12, 12);
	gluQuadricDrawStyle(moon, GLU_LINE); // 선을 긋는 형태로 설정
	glColor3f(0.7f, 0.7f, 0.7f);
	gluSphere(moon, 0.32f, 12, 12);
	glPopMatrix(); // 달 좌표 제거
	
	glPushMatrix();// 달 궤도를 그리기 위한 좌표 추가
	glColor3f(1.0f, 1.0f, 1.0f);
	glRotatef(20.0f, 0.0f, 0.0f, 1.0f); // z축을 기준으로 좌표축 20도 회전( 궤도면 회전 )
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f); // x축을 기준으로 좌표축 -90도 회전( 좌표축 맞추기용 )
	glBegin(GL_POINTS);
	for (GLfloat angle = 0; angle < 360; angle += 0.1f)
	{
		moon_xpos = (GLfloat)cos(angle) * 7.0f;
		moon_ypos = (GLfloat)sin(angle) * 7.0f;
		glVertex3f(moon_xpos, moon_ypos, moon_zpos);
	}
	glEnd();
	glPopMatrix(); /////// 달 궤도 좌표 제거 ///////
	*/
	for (int n = 0; n <= numOfCraft; n++) {
		if (spaceCraft[n].craft != NULL) {
			glPushMatrix(); // 추가된 우주물체 궤도를 그리기 위한 좌표 추가
			glColor3f(1.0f, 1.0f, 1.0f);
			glRotatef(-90.0f, 1.0f, 0.0f, 0.0f); // x축을 기준으로 좌표축 -90도 회전( 좌표축 맞추기용 )
			glRotatef(spaceCraft[n].omega, 0.0f, 0.0f, 1.0f); // 이후 z축을 기준으로 omega만큼 회전 (승교점적경 적용)
			glRotatef(spaceCraft[n].i, 1.0f, 0.0f, 0.0f); // 회전된 좌표축에서 x축을 기준으로 i만큼 회전 (궤도경사각 적용)
			DrawTrajectory(n); // n번째 우주물체의 궤적을 그림
			glPopMatrix();// 추가된 우주물체 궤도를 그리기 위한 좌표 삭제
			
			glPushMatrix(); // 우주물체를 그리기위한 좌표 추가
			glRotatef(-90.0f, 1.0f, 0.0f, 0.0f); // x축을 기준으로 좌표축 -90도 회전( 좌표축 맞추기용 )
			glRotatef(spaceCraft[n].omega, 0.0f, 0.0f, 1.0f); // 이후 z축을 기준으로 omega만큼 회전 (승교점적경 적용)
			glRotatef(spaceCraft[n].i, 1.0f, 0.0f, 0.0f); // 회전된 좌표축에서 x축을 기준으로 i만큼 회전 (궤도경사각 적용)
			DrawSphere(n); // n번째 우주물체를 그림
			glPopMatrix();// 추가된 우주물체를 그리기 위한 좌표 삭제

			glPushMatrix(); // 예측 위치의 물체를 그리기 위한 좌표 추가
			glRotatef(-90.0f, 1.0f, 0.0f, 0.0f); // x축을 기준으로 좌표축 -90도 회전( 좌표축 맞추기용 )
			glRotatef(spaceCraft[n].omega, 0.0f, 0.0f, 1.0f); // 이후 z축을 기준으로 omega만큼 회전 (승교점적경 적용)
			glRotatef(spaceCraft[n].i, 1.0f, 0.0f, 0.0f); // 회전된 좌표축에서 x축을 기준으로 i만큼 회전 (궤도경사각 적용)
			glRotatef(spaceCraft[n].w, 0.0f, 0.0f, 1.0f);
			DrawPrediction(n); // 예측 모드 중이면 예측 극좌표에 구를 그려줌
			glPopMatrix(); // 예측 위치의 물체를 그리기 위한 좌표 제거
		}
	}
	glFlush();
	
	// 지구 자전 속도 ( 지구 정지궤도의 반지름 42705.97228 km, 속도 3.05475 km/s )
	zrot += 360.0f / 87840.0f / timeScale;
	if (zrot > 360.0f) {
		zrot -= 360.0f;
		/*currentTime = glutGet(GLUT_ELAPSED_TIME);
		deltaTime = currentTime - oldTime;
		oldTime = currentTime;*/
	}

	moon_zrot += 1.0f;
	if (moon_zrot > 359.0f) {
		moon_zrot = 0.0f;
	}

	return TRUE;
}

// 씬 삭제
void OPenGLRenderer::DestroyScene()
{
	DestroyWindow();

	wglMakeCurrent(m_hdc, NULL);
	if (m_hrc)
	{
		wglDeleteContext(m_hrc);
		m_hrc = NULL;
	}
}

AUX_RGBImageRec* OPenGLRenderer::LoadBMPFile(char* filename)
{
	FILE* hFile = NULL;
	if (!filename) return NULL;

	hFile = fopen(filename, "r");
	wchar_t wtext[20];
	mbstowcs(wtext, filename, strlen(filename) + 1);//Plus null
	LPWSTR ptr = wtext;

	if (hFile) {
		fclose(hFile);
		return auxDIBImageLoad(ptr);
	}

	return NULL;
}

int OPenGLRenderer::LoadGLTextures()
{
	int Status = FALSE;

	gluQuadricTexture(earth, GL_TRUE); // 텍스처 매핑 사용
	memset(pTextureImage, 0, sizeof(void*) * 1); // 포인터 초기화

	if (pTextureImage[0] = LoadBMPFile("earth.bmp")) {
		Status = TRUE;
		glGenTextures(1, &textureID[0]); // 텍스쳐 객체 생성
		glBindTexture(GL_TEXTURE_2D, textureID[0]); // 상태관리자에게 [0]번째 텍스처를 바인딩
		glTexImage2D(GL_TEXTURE_2D, 0, 3, pTextureImage[0]->sizeX, pTextureImage[0]->sizeY, 0,
			GL_RGB, GL_UNSIGNED_BYTE, pTextureImage[0]->data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // 이미지 파일과 물체의 크기를 맞춰준다
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glEnable(GL_TEXTURE_2D);
	}

	//텍스처 공간반납
	if (pTextureImage[0]) {
		if (pTextureImage[0]->data) {
			free(pTextureImage[0]->data);
		}
		free(pTextureImage[0]);
	}
	return Status;
}

// 우주물체를 직접적으로 생성하는 함수
void OPenGLRenderer::CreateCraft(int num) 
{
	spaceCraft[num].craft = gluNewQuadric(); // 물체 객체 인스턴스 생성
}

// 우주물체 궤적을 그려주는 함수
void OPenGLRenderer::DrawTrajectory(int num) 
{
	glRotatef(spaceCraft[num].w, 0.0f, 0.0f, 1.0f);
	
	if (spaceCraft[num].type == 1) {
		glBegin(GL_POINTS);
		for (GLfloat angle_a = 0; angle_a < 360; angle_a += 0.1f)
		{
			spaceCraft[num].traj_range = (GLfloat)(spaceCraft[num].p / (1 + spaceCraft[num].e * cos(angle_a)));
			spaceCraft[num].traj_xpos = (GLfloat)cos(angle_a) * spaceCraft[num].traj_range / 1000.0f;
			spaceCraft[num].traj_ypos = (GLfloat)sin(angle_a) * spaceCraft[num].traj_range / 1000.0f;
			glVertex3f(spaceCraft[num].traj_xpos, spaceCraft[num].traj_ypos, 0.0f);
		}
		glEnd();
	}
	else {
		GLfloat bound = acosf(1 / spaceCraft[num].e);
		glBegin(GL_POINTS);
		for (GLfloat angle_a = 0; angle_a < 360; angle_a += 0.1f)
		{
			spaceCraft[num].traj_range = (GLfloat)(spaceCraft[num].p / (1 + spaceCraft[num].e * cos(angle_a)));
			spaceCraft[num].traj_xpos = (GLfloat)cos(angle_a) * spaceCraft[num].traj_range / 1000.0f;
			spaceCraft[num].traj_ypos = (GLfloat)sin(angle_a) * spaceCraft[num].traj_range / 1000.0f;
			glVertex3f(spaceCraft[num].traj_xpos, spaceCraft[num].traj_ypos, 0.0f); 
		}
		glEnd();
	}
}

// 우주물체 구를 그려주는 함수
void OPenGLRenderer::DrawSphere(int num) 
{
	glRotatef(spaceCraft[num].w, 0.0f, 0.0f, 1.0f); // w 만큼 그리는 기준면 회전
	// 여기서 f만큼 회전한 위치에서 초기 구체 생성
	spaceCraft[num].xpos = (GLfloat)cos(spaceCraft[num].angle * GL_PI / 180.0f) * spaceCraft[num].radius / 1000.0f;
	spaceCraft[num].ypos = (GLfloat)sin(spaceCraft[num].angle * GL_PI / 180.0f) * spaceCraft[num].radius / 1000.0f;

	// drawing
	glTranslatef(spaceCraft[num].xpos, spaceCraft[num].ypos, 0.0f); // 구체 이동 (방정식에 따라 이 포지션이 바뀐다)
	gluQuadricDrawStyle(spaceCraft[num].craft, GLU_FILL); // 객체를 채우는 형태로 설정
	glColor3f(0.35f, 0.35f, 0.35f);
	gluSphere(spaceCraft[num].craft, 0.3f, 12, 12);

	gluQuadricDrawStyle(spaceCraft[num].craft, GLU_LINE); // 선을 긋는 형태로 설정
	if(spaceCraft[num].isSelected == 0)
		glColor3f(0.4f, 0.4f, 0.8f);
	else
		glColor3f(0.9f, 0.0f, 0.0f);
	gluSphere(spaceCraft[num].craft, 0.32f, 12, 12);

	/// Update values ///
	spaceCraft[num].angleSpeed = spaceCraft[num].h / (GLfloat)pow((double)spaceCraft[num].radius, 2); // 각속도 구하기
	spaceCraft[num].angle += (spaceCraft[num].angleSpeed * 180.0f) / GL_PI / timeScale; // 각속도 적용
	
	spaceCraft[num].radius = (GLfloat)(spaceCraft[num].p / (1 + spaceCraft[num].e * cos(spaceCraft[num].angle * GL_PI / 180.0f)));
	if (spaceCraft[num].angle > 360.0f) {
		spaceCraft[num].angle -= 360.0f;
	}

	CalculateT(num); // 근지점 통과시 업데이트
	//// Update End ////

	PredictionPosition(num, spaceCraft[num].preTime); // 시간이 지날때마다 예측결과를 계속 업데이트
}

// 근지점 통과 시각을 구하는 함수
void OPenGLRenderer::CalculateT(int num) 
{ 
	GLfloat cosE;
	GLfloat initialM;
	GLfloat initialE;

	cosE = (1.0f - (spaceCraft[num].radius / spaceCraft[num].a)) / spaceCraft[num].e;
	initialE = acosf(cosE);

	initialM = initialE - (spaceCraft[num].e * sinf(initialE));

	spaceCraft[num].T = -initialM / spaceCraft[num].n;

	if (spaceCraft[num].angle >= 0.0f && spaceCraft[num].angle <= 180.0f)
		spaceCraft[num].T = spaceCraft[num].P + spaceCraft[num].T;
	else
		spaceCraft[num].T = -spaceCraft[num].T;
}

// 예측 극좌표를 구하는 함수
void OPenGLRenderer::PredictionPosition(int num, GLfloat time) 
{
	const int repeat = 5;
	GLfloat M[repeat]; // M0, M1, M2, M3 
	GLfloat E[repeat]; // E0, E1, E2, E3
	GLfloat F[repeat]; // F0, F1, F2, F3
	GLfloat deltaE;
	GLfloat deltaF;
	
	GLfloat tanHalf_F;
	GLfloat GM = (float)G * mass_Earth;

	if (time >= spaceCraft[num].T)
		deltaTime = time - spaceCraft[num].T;
	else
		deltaTime = time + spaceCraft[num].P - spaceCraft[num].T;
	
	if (spaceCraft[num].type == 1) { // 타원궤도인 경우의 위치 예측 

		if (spaceCraft[num].e <= 0.7f) { // 이심률이 높지 않은 타원궤도
			spaceCraft[num].M = spaceCraft[num].n * deltaTime;
			E[0] = spaceCraft[num].M; // E0 = M0 근사
			for (int i = 0; i < repeat; i++) {
				M[i] = E[i] - (spaceCraft[num].e * sinf(E[i]));
				deltaE = (spaceCraft[num].M - M[i]) / (1 - spaceCraft[num].e * cosf(E[0]));

				if (i < repeat - 1)
					E[i + 1] = E[i] + deltaE;
			}
			spaceCraft[num].M = M[repeat - 1];
			spaceCraft[num].E = E[repeat - 1];

			spaceCraft[num].preRadius = spaceCraft[num].a * (1 - (spaceCraft[num].e * cosf(spaceCraft[num].E))); // r
			tanHalf_F = sqrtf((1 + spaceCraft[num].e) / (1 - spaceCraft[num].e)) * tanf(spaceCraft[num].E / 2.0f); // tan(f/2)
			spaceCraft[num].preF = 2.0f * atanf(tanHalf_F) * 180.0f / GL_PI; // f
		}
		else {  // 이심률이 높은 타원궤도 (Barker's equation 계산할 것)
			q = (spaceCraft[num].a * (1 - spaceCraft[num].e)) * 1000.0f; // q = a(1-e)
			B = 1.0f;

			for (int i = 0; i < repeat; i++) {
				///// Barker's equation /////
				cots = (3.0f / B) * sqrtf((float)(G * mass_Earth) / (2 * powf(q, 3))) * sqrtf((1 + 9 * spaceCraft[num].e) / 10.0f) * deltaTime;
				s = atanf(1.0f / cots); // s는 라디안
				cotw = powf(1.0f / tanf(s / 2.0f), 1.0f / 3.0f);
				cot2w = (powf(cotw, 2) - 1) / (2 * cotw);
				tanHalf_W = 2 * cot2w;
				A = 5.0f * (1 - spaceCraft[num].e) / (1 + 9 * spaceCraft[num].e) * powf(tanHalf_W, 2); // B와 barker 방정식으로 A를 구함
				equationA = sqrtf(A) + ((1 + 9 * spaceCraft[num].e) / (1 - spaceCraft[num].e)) * (powf(A, 1.5f) / 15.0f);
				B = (sqrtf(GM * (1 - spaceCraft[num].e)) * deltaTime) / (2 * powf(q, 1.5f) * equationA); // 구한 A로 새로운 B를 구함
			}
			
			spaceCraft[num].y = A / (1 - 0.8f * A + (8.0f / 175.0f) * powf(A, 2.0f));
			tanHalf_F = sqrtf(((1+ spaceCraft[num].e)/(1- spaceCraft[num].e))* spaceCraft[num].y); // tan(f/2)
			//tanHalf_F = (5.0f * (1 - spaceCraft[num].e) / (1 + 9 * spaceCraft[num].e))/ (1 - 0.8f * A + (8.0f / 175.0f)) * powf(tanHalf_W, 2);
			spaceCraft[num].preRadius = (q * (1 + powf(tanHalf_F, 2))) / (1 + spaceCraft[num].y);
			spaceCraft[num].preRadius /= 1000.0f;
			spaceCraft[num].preF = 2.0f * atanf(tanHalf_F) * 180.0f / GL_PI; // f
			
		}
	}

	else { /////////////////////////////////////////// 쌍곡선 궤도인 경우의 위치 예측 ( 시간관련 수정필요 - 나머지 완벽) /////////////////////////////////////////////
		// t - T를 받은 값 그대로 이용하여 근지점 통과 후 그 시간의 위치를 바로 예측해보자.
		spaceCraft[num].n = sqrtf((float)(G * mass_Earth) / powf(spaceCraft[num].a * 1000.0f, 3.0f)); // 쌍곡선에서의 n 구하기
		spaceCraft[num].M = spaceCraft[num].n * deltaTime;

		F[0] = spaceCraft[num].M;
		for (int i = 0; i < 20; i++) { // 반복 처리 횟수 20번 - F0 을 구한다.
			F[0] = asinhf((float)(F[0] + spaceCraft[num].M) / spaceCraft[num].e);
		}
		for (int i = 0; i < repeat; i++) { // 최종 F와 M을 구하는 반복 절차
			M[i] = spaceCraft[num].e * sinhf(F[i]) - F[i];
			deltaF = (spaceCraft[num].M - M[i]) / (spaceCraft[num].e * coshf(F[0]) - 1);

			if (i < repeat - 1)
				F[i + 1] = F[i] + deltaF;
		}
		spaceCraft[num].M = M[repeat - 1];
		spaceCraft[num].F = F[repeat - 1];

		tanHalf_F = sqrtf((spaceCraft[num].e + 1) / (spaceCraft[num].e - 1)) * tanf(spaceCraft[num].F / 2.0f); // tan(f/2)
		spaceCraft[num].preF = 2.0f * atanf(tanHalf_F) * 180.0f / GL_PI; // f
		spaceCraft[num].preRadius = (spaceCraft[num].a * (powf(spaceCraft[num].e, 2) - 1)) / (1 + spaceCraft[num].e * cosf(spaceCraft[num].preF * GL_PI / 180.0f));
	}

	if (spaceCraft[num].preF < 0) {
		spaceCraft[num].preF = 360.0f + spaceCraft[num].preF;
	}
}

void OPenGLRenderer::CreatPreCraft(int num)
{
	preCraft[num] = gluNewQuadric(); // 물체 객체 인스턴스 생성
}

// 예측 극좌표에 그려주는 함수
void OPenGLRenderer::DrawPrediction(int num)
{
	GLfloat preX;
	GLfloat preY;
	
	if (spaceCraft[num].isSelected == 1 && spaceCraft[num].isStarted == 1) {
		if (preCraft[num] != NULL) {
			preX = (GLfloat)cos(spaceCraft[num].preF * GL_PI / 180.0f) * spaceCraft[num].preRadius / 1000.0f;
			preY = (GLfloat)sin(spaceCraft[num].preF * GL_PI / 180.0f) * spaceCraft[num].preRadius / 1000.0f;

			// drawing
			glTranslatef(preX, preY, 0.0f); // 구체 이동 (방정식에 따라 이 포지션이 바뀐다)
			gluQuadricDrawStyle(preCraft[num], GLU_FILL); // 객체를 채우는 형태로 설정
			glColor3f(0.0f, 0.0f, 0.0f);
			gluSphere(preCraft[num], 0.3f, 12, 12);

			gluQuadricDrawStyle(preCraft[num], GLU_LINE); // 선을 긋는 형태로 설정
			glColor3f(0.7f, 0.7f, 0.7f);
			gluSphere(preCraft[num], 0.32f, 12, 12);
		}
	}
}


///// user input functions /////
void OPenGLRenderer::OnLButtonDown(UINT nFlags, CPoint point) // 클릭하면 클릭 시의 마우스 위치가 저장된다.
{
	GLfloat x;

	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	b_Rotate = TRUE; // 회전모드 시작
	x = (GLfloat)point.x;
	mousePoint = x;

	CWnd::OnLButtonDown(nFlags, point);
}


void OPenGLRenderer::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	GLfloat x;
	
	x = (GLfloat)point.x;
	if (b_Rotate) {  
		differ = x - mousePoint;
	}
	mousePoint = x;
	differ *= 40.0f; // 회전 민감도
	zAngle += differ/100.0f;
 
	if (zAngle > 359.0f || zAngle < -359.0f) zAngle = 0.0f;

	CWnd::OnMouseMove(nFlags, point);
}


void OPenGLRenderer::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	b_Rotate = FALSE;
	differ = 0.0f;
	CWnd::OnLButtonUp(nFlags, point);
}


BOOL OPenGLRenderer::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) // 화면 줌아웃
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	GLfloat temp_zoom;
	temp_zoom = ((GLfloat)zDelta) / 100.0f;

	zoom += temp_zoom;

	if (zoom < -80.0f) zoom = -80.0f;
	if (zoom > -20.0f) zoom = -20.0f;

	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}
