
// MFCApplication2Dlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "MFCApplication2.h"
#include "MFCApplication2Dlg.h"
#include "afxdialogex.h"


#define SERVER_IP L"192.168.1.101"
//192.168.1.103
//10.12.174.61
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();
	
	//Client client;

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
public:
	
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	
END_MESSAGE_MAP()


LPWSTR ConvertCharToLPWSTR(const char* srcString)
{
	int srcLen = strlen(srcString) + 1;
	int nwLen = MultiByteToWideChar(CP_ACP, 0, srcString, srcLen, NULL, 0);//算出合适的长度
	LPWSTR str = new WCHAR[srcLen];
	MultiByteToWideChar(CP_ACP, 0, srcString, srcLen, str, nwLen);
	return str;
}

// CMFCApplication2Dlg 对话框



CMFCApplication2Dlg::CMFCApplication2Dlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFCAPPLICATION2_DIALOG, pParent)
	, m_Ascii(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

}

void CMFCApplication2Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RADIO1, ascii_button);
	DDX_Radio(pDX, IDC_RADIO1, m_Ascii);
	DDX_Control(pDX, IDC_LIST1, ListBox);
	DDX_Control(pDX, IDC_EDIT1, ip_edit);
	DDX_Control(pDX, IDCANCEL, StatusShow);
	DDX_Control(pDX, IDC_EDIT2, status);
}

BEGIN_MESSAGE_MAP(CMFCApplication2Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
//	ON_BN_CLICKED(IDC_RADIO1, &CMFCApplication2Dlg::OnBnClickedRadio1)
//ON_BN_CLICKED(IDC_RADIO1, &CMFCApplication2Dlg::OnBnClickedRadio1)
ON_BN_CLICKED(IDC_BUTTON1, &CMFCApplication2Dlg::OnBnClickedGetButton)
ON_BN_CLICKED(IDC_BUTTON2, &CMFCApplication2Dlg::OnBnClickedPutButton)
ON_BN_CLICKED(IDC_BUTTON3, &CMFCApplication2Dlg::OnBnClickedChooseFileButton_S)
ON_BN_CLICKED(IDC_BUTTON4, &CMFCApplication2Dlg::OnBnClickedChooseFileButton_C)
ON_BN_CLICKED(IDC_BUTTON5, &CMFCApplication2Dlg::OnBnClickedTestButton)
ON_MESSAGE(WM_MY_TIMEOUT, &CMFCApplication2Dlg::OnTimeOut)

ON_BN_CLICKED(IDC_RADIO1, &CMFCApplication2Dlg::OnBnClickedAsciiRadio)
ON_BN_CLICKED(IDC_RADIO2, &CMFCApplication2Dlg::OnBnClickedOctRadio)
ON_EN_CHANGE(IDC_EDIT1, &CMFCApplication2Dlg::OnEnChangeIPEdit)
END_MESSAGE_MAP()


// CMFCApplication2Dlg 消息处理程序

BOOL CMFCApplication2Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	//Client client;
	if (client.initSocket(SERVER_IP) == false)
	{
		MessageBoxA(NULL, "创建套接字失败", "Warning!", MB_OK);
		
	}
	client.list = &ListBox;
	ip_edit.SetWindowTextW(SERVER_IP);
	client.speed_display = &status;
	client.hWnd = m_hWnd;
	//TextOutA()
	
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMFCApplication2Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMFCApplication2Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMFCApplication2Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


/**
* 下载文件
*/
void CMFCApplication2Dlg::OnBnClickedGetButton()
{
	// TODO: 在此添加控件通知处理程序代码
	// 获取文件名
	CString filePath;
	GetDlgItem(IDC_EDIT_DOWNLOAD)->GetWindowTextW(filePath);
	if (filePath.IsEmpty())
	{
		MessageBoxA(NULL, "未选择文件", "提示", MB_OK);
		return;
	}
	// 将文件名转为单字符char类型
	int len = WideCharToMultiByte(CP_ACP, 0, filePath, -1, NULL, 0, NULL, NULL);
	char* ptxtTemp = new char[len + 1];
	WideCharToMultiByte(CP_ACP, 0, filePath, -1, ptxtTemp, len, NULL, NULL);
	ptxtTemp[len] = 0;
	int i;
	for (i = len; i >= 0; i--)
	{
		if (ptxtTemp[i] == '\\') break;
	}
	int fileName_len = len - i;
	char* fileName = new char[fileName_len];
	strcpy_s(fileName, fileName_len, ptxtTemp + i + 1);
	fileName[fileName_len - 1] = 0;
	// 发送Get请求
	client.sendGet(fileName);
	status.SetWindowTextW(L"正在下载");
	delete[] ptxtTemp;
	delete[] fileName;

}



void CMFCApplication2Dlg::OnBnClickedPutButton()
{
	// TODO: 在此添加控件通知处理程序代码
	CString ptxtTemp;
	GetDlgItem(IDC_EDIT_UPLOAD)->GetWindowTextW(ptxtTemp);
	if (ptxtTemp.IsEmpty())
	{
		MessageBoxA(NULL, "未选择文件", "提示", MB_OK);
		return;
	}

	int len = WideCharToMultiByte(CP_ACP, 0, ptxtTemp, -1, NULL, 0, NULL, NULL);
	char* filePath = new char[len + 1];
	WideCharToMultiByte(CP_ACP, 0, ptxtTemp, -1, filePath, len, NULL, NULL);
	filePath[len] = 0;
	int i;
	for (i = len; i >= 0; i--)
	{
		if (filePath[i] == '\\') break;
	}
	int fileName_len = len - i;
	char* fileName = new char[fileName_len];
	strcpy_s(fileName, fileName_len, filePath + i + 1);
	fileName[fileName_len - 1] = 0;

	client.sendPut(filePath, fileName);
	status.SetWindowTextW(L"正在上传");
	delete[] filePath;
	delete[] fileName;
}

// 下载功能的文件选择
void CMFCApplication2Dlg::OnBnClickedChooseFileButton_S()
{
	// TODO: 在此添加控件通知处理程序代码
	CFileDialog fileDialog(TRUE, NULL, L"D:\\Tftp\\Tftpd64\\chk10.txt");
	if (fileDialog.DoModal()== IDOK)
	{
		CString filePath = fileDialog.GetPathName();
		GetDlgItem(IDC_EDIT_DOWNLOAD)->SetWindowTextW(filePath);
	}
}

// 上传功能的文件选择
void CMFCApplication2Dlg::OnBnClickedChooseFileButton_C()
{
	// TODO: 在此添加控件通知处理程序代码
	CFileDialog fileDialog(TRUE, NULL, L"D:\\C++项目\\MFCApplication2\\MFCApplication2\\put.txt");
	if (fileDialog.DoModal() == IDOK)
	{
		CString filePath = fileDialog.GetPathName();
		GetDlgItem(IDC_EDIT_UPLOAD)->SetWindowTextW(filePath);
	}
}

LRESULT CMFCApplication2Dlg::OnTimeOut(WPARAM wParam, LPARAM lParam)
{
	//MessageBoxA(NULL, "hhh", "hhh", MB_OK);
	client.resend();
	return 0;
}

void CMFCApplication2Dlg::OnBnClickedTestButton()
{
	// TODO: 在此添加控件通知处理程序代码
	client.shutdown();
}


void CMFCApplication2Dlg::OnBnClickedAsciiRadio()
{
	// TODO: 在此添加控件通知处理程序代码
	client.which_mode = 1;
	
}


void CMFCApplication2Dlg::OnBnClickedOctRadio()
{
	// TODO: 在此添加控件通知处理程序代码
	client.which_mode = 2;
	
}


void CMFCApplication2Dlg::OnEnChangeIPEdit()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	client.Close();
	wchar_t ip[20];
	ip_edit.GetWindowTextW(ip, 20);
	if (client.initSocket(ip) == false)
	{
		MessageBoxA(NULL, "创建套接字失败", "Warning!", MB_OK);

	}
}
