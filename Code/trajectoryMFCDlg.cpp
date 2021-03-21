
// trajectoryMFCDlg.h: 헤더 파일
//

#pragma once
#include "OPenGLRenderer.h"

// CtrajectoryMFCDlg 대화 상자
class CtrajectoryMFCDlg : public CDialogEx
{
	OPenGLRenderer* m_test; // OpenGL 객체
// 생성입니다.
public:
	CtrajectoryMFCDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TRAJECTORYMFC_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CStatic m_pLeft; // picture control 객체 변수
	CRect rectLeft; // picture control 사각형 변수
	CString xpos, ypos, zpos; // 입력받을 포지션의 변수
	CString xvel, yvel, zvel; // 입력받을 속도의 변수

	// 궤도요소 라디안 크기의 임시변수
	GLfloat radianI = 0.0f;
	double radianOmega = 0.0;
	double radianU = 0.0;
	double radianF = 0.0;

	double temp = 0.0; // 오메가 구하기 용
	double cosu = 0.0; // u 구하기 용
	double tanf = 0.0f; // f 구하기 용
	// 역수의 임시변수
	double reciprocal_a = 0.0f; // (작은값이라 double 사용)

	// 우주상수 mu = GM
	double GM = 0.0;

	// h^2 , e^2
	double squarh = 0.0;
	double square = 0.0;

	// 위치벡터와 속도벡터 성분의 곱 posXvel
	GLfloat posXvel = 0.0f;
	GLfloat CircleVelocity = 0.0f;


	CListCtrl m_List; // 리스트 컨트롤 객체 변수
	CRect rectList; // 리스트 컨트롤 사각형 변수

	///궤도요소를 구하는 함수
	void CalculateCAndH(int n); // C1, C2, C3, h 구하기
	void CalculateOmegaAndI(int n); // omega 와 i를 구하기
	void CalculateAAndE(int n); // 장반경 a와 이심률 e 구하기
	int CheckTrajShape(int n); // 타원궤도인지 쌍곡선궤도인지 검사
	void CalculatePeriod(int n); // 물체의 궤도를 구함
	void CalculateWAndF(int n); // 근지점인수 w와 진근지점이각 f 구하기
	int CheckSignOfF(int n); // radius 벡터 미분의 부호를 판단하여 f의 사분면을 결정
	void CanMakeCircle(int n); // 원궤도를 만들 수 있는지 판단. 못만드는 궤도에선 w를 뒤집어 줘야함
	void AdjustTrajectory(int n); // 방향에 따른 궤도에서 보정을 해줘야함

	// 리스트 관련 함수
	void AddtoList(int n); // 리스트에 물체를 추가한다

	afx_msg void OnTimer(UINT_PTR nIDEvent); // WM_TIMER로 추가한 함수
	afx_msg void OnBnClickedButtonAdd(); // ADD 버튼이 클릭되면
	afx_msg void OnBnClickedButtonFaster(); // 시간이 빠르게 흐른다.
	afx_msg void OnBnClickedButtonSlower(); // 시간이 느리게 흐른다.
};
