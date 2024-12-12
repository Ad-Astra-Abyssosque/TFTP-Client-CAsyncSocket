
// MFCApplication2Dlg.h: 头文件
//

#pragma once
#include "Client.h"

// CMFCApplication2Dlg 对话框
class CMFCApplication2Dlg : public CDialogEx
{
// 构造
public:
	CMFCApplication2Dlg(CWnd* pParent = nullptr);	// 标准构造函数
	Client client;

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCAPPLICATION2_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CButton ascii_button;
	BOOL m_Ascii;
//	afx_msg void OnBnClickedRadio1();
//	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedGetButton();
	afx_msg void OnBnClickedPutButton();
	afx_msg void OnBnClickedChooseFileButton_S();
	afx_msg void OnBnClickedChooseFileButton_C();
	afx_msg LRESULT OnTimeOut(WPARAM wParam, LPARAM lParam);
	CListBox ListBox;
	afx_msg void OnBnClickedTestButton();
	afx_msg void OnBnClickedAsciiRadio();
	afx_msg void OnBnClickedOctRadio();
	CEdit ip_edit;
	afx_msg void OnEnChangeIPEdit();
	CButton StatusShow;
	CEdit status;
};
