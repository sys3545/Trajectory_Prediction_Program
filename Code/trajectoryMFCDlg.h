
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
	CStatic m_pLeft; // picture 객체 변수
	CRect rectLeft; // picture 사각형 변수
	CString xpos, ypos, zpos; // 입력받을 포지션의 변수
	CString xvel, yvel, zvel; // 입력받을 속도의 변수

	// 궤도요소 라디안 크기의 임시변수
	GLfloat radianI = 0.0f;
	GLfloat radianOmega = 0.0f;
	
	afx_msg void OnTimer(UINT_PTR nIDEvent); // WM_TIMER로 추가한 함수
	afx_msg void OnBnClickedButtonAdd(); // ADD 버튼이 클릭되면
};
