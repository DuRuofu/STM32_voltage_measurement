
// UpperMachineDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "UpperMachine.h"
#include "UpperMachineDlg.h"
#include "afxdialogex.h"
#include <Dbt.h>
CUIntArray ports, portse, portsu;
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//获取串口配置
CString CUpperMachineDlg::GetUsartAllcation()
{
	//拿到波特率的索引
	int BaudRate = m_BaudRate.GetCurSel();
	CString BaudRate_str;
	m_BaudRate.GetLBText(BaudRate, BaudRate_str);//根据索引获取内容
//	MessageBox(BaudRate_str);

	//拿到停止位的索引
	int StopBit = m_StopBit.GetCurSel();
	CString StopBit_str;
	m_StopBit.GetLBText(StopBit, StopBit_str);//根据索引获取内容
//	MessageBox(StopBit_str);

	//拿到拿到数据位的索引
	int DataBit = m_DataBit.GetCurSel();
	CString DataBit_str;
	m_DataBit.GetLBText(DataBit, DataBit_str);//根据索引获取内容
//	MessageBox(DataBit_str);

	//拿到拿到奇偶校验位的索引
	int Parity = m_Parity.GetCurSel();
	char Parity_data;
	switch (Parity)//根据索引设置校检位
	{
	case 0: Parity_data = 'E';//无
		break;
	case 1: Parity_data = 'O';//奇校检
		break;
	case 2: Parity_data = 'N';//偶校检
		break;
	default:
		break;
	}
//	MessageBox(Parity_str);

	//合并字符串
	CString str;
	str.Format(_T("%s,%c,%s,%s"), BaudRate_str, Parity_data, DataBit_str, StopBit_str);
//	MessageBox(str);

	return str;
}

BOOL CUpperMachineDlg::OnDeviceChange(UINT nEventType, DWORD dwData)
{
	//DEV_BROADCAST_DEVICEINTERFACE* dbd = (DEV_BROADCAST_DEVICEINTERFACE*) dwData;
	switch (nEventType)
	{
	case DBT_DEVICEREMOVECOMPLETE://移除设备
	case DBT_DEVICEARRIVAL://添加设备
		RefreshCom();//刷新组合框的内容
		break;

	default:
		break;
	}

	return TRUE;

}

void CUpperMachineDlg::RefreshCom(void)
{
	int count = m_COM.GetCount();

	for (int i = 0; i < count; i++)
	{
		m_COM.DeleteString(count - 1 - i);
	}
	AddCom();
	m_COM.SetCurSel(0);
}

void CUpperMachineDlg::EnumerateSerialPorts(CUIntArray& ports, CUIntArray& portse, CUIntArray& portsu)
{
	//清除串口数组内容
	ports.RemoveAll();//所有存在串口
	portse.RemoveAll();//可用串口
	portsu.RemoveAll();//已占用串口

	//因为至多有255个串口，所以依次检查各串口是否存在
	//如果能打开某一串口，或打开串口不成功，但返回的是 ERROR_ACCESS_DENIED错误信息，
	//都认为串口存在，只不过后者表明串口已经被占用
	//否则串口不存在
	for (int i = 1; i < 256; i++)
	{
		//Form the Raw device name
		CString sPort;
		sPort.Format(_T("\\\\.\\COM%d"), i);

		//Try to open the port
		BOOL bSuccess = FALSE;
		HANDLE hPort = ::CreateFile(sPort, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
		if (hPort == INVALID_HANDLE_VALUE)
		{
			DWORD dwError = GetLastError();

			if (dwError == ERROR_ACCESS_DENIED)
			{
				bSuccess = TRUE;
				portsu.Add(i);       //已占用的串口
			}
		}
		else
		{
			//The port was opened successfully
			bSuccess = TRUE;
			portse.Add(i);      //可用的串口
			//Don't forget to close the port, since we are going to do nothing with it anyway
			CloseHandle(hPort);
		}

		//Add the port number to the array which will be returned
		if (bSuccess)
			ports.Add(i);   //所有存在的串口
	}
}

void CUpperMachineDlg::AddCom(void)
{

	EnumerateSerialPorts(ports, portse, portsu);
	unsigned short uicounter;
	unsigned short uisetcom;
	CString str;

	//获取可用串口个数
	uicounter = portse.GetSize();
	//如果个数大于0
	if (uicounter > 0)
	{
		//初始化串口列表框
		for (int i = 0; i < uicounter; i++)
		{
			uisetcom = portse.ElementAt(i);
			str.Format(_T("COM%d "), uisetcom);
			m_COM.AddString(str);
		}
	}
}

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CUpperMachineDlg 对话框




CUpperMachineDlg::CUpperMachineDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CUpperMachineDlg::IDD, pParent)
	, m_EditRxData(_T(""))
	, m_EditTxData(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CUpperMachineDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//  DDX_Control(pDX, IDC_BtnOpen, m_BtnOpen);
	DDX_Text(pDX, IDC_EditRxData, m_EditRxData);
	DDX_Control(pDX, IDC_BtnOpen, m_BtnOpen);
	DDX_Control(pDX, IDC_MSCOMM1, m_Comm1);
	DDX_Control(pDX, IDC_BaudRate, m_BaudRate);
	//  DDX_Control(pDX, IDC_COM, m_COM);
	DDX_Control(pDX, IDC_BtnSend, m_BtnSend);
	DDX_Text(pDX, IDC_EditTxData, m_EditTxData);
	DDX_Control(pDX, IDC_Parity, m_Parity);
	DDX_Control(pDX, IDC_StopBit, m_StopBit);
	DDX_Control(pDX, IDC_DataBit, m_DataBit);
	DDX_Control(pDX, IDC_COM, m_COM);
}

BEGIN_MESSAGE_MAP(CUpperMachineDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BtnOpen, &CUpperMachineDlg::OnBnClickedBtnopen)
	ON_BN_CLICKED(IDC_BtnSend, &CUpperMachineDlg::OnBnClickedBtnsend)
	ON_BN_CLICKED(IDC_BUTTON3, &CUpperMachineDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON2, &CUpperMachineDlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// CUpperMachineDlg 消息处理程序

BOOL CUpperMachineDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	m_EditTxData="此功能不可用！";
	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
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


	// TODO: Add extra initialization here
	AddCom();//向组合框中添加串口设备	
	m_COM.SetCurSel(1);	//设置默认的选项
	//...


	/*//COM口
	m_COM.AddString(TEXT("COM1"));
	m_COM.AddString(TEXT("COM2"));
	m_COM.AddString(TEXT("COM3"));
	m_COM.AddString(TEXT("COM4"));
	m_COM.AddString(TEXT("COM5"));
	m_COM.AddString(TEXT("COM6"));
	m_COM.AddString(TEXT("COM7"));
	m_COM.AddString(TEXT("COM8"));
	m_COM.AddString(TEXT("COM9"));
	m_COM.AddString(TEXT("COM10"));
	m_COM.AddString(TEXT("COM11"));
	m_COM.AddString(TEXT("COM12"));
	m_COM.AddString(TEXT("COM13"));
	//设置默认的选项
	m_COM.SetCurSel(5);*/


	//波特率
	m_BaudRate.AddString(TEXT("1382400"));
	m_BaudRate.AddString(TEXT("921600"));
	m_BaudRate.AddString(TEXT("460800"));
	m_BaudRate.AddString(TEXT("256000"));
	m_BaudRate.AddString(TEXT("230400"));
	m_BaudRate.AddString(TEXT("128000"));
	m_BaudRate.AddString(TEXT("115200"));
	m_BaudRate.AddString(TEXT("76800"));
	m_BaudRate.AddString(TEXT("57600"));
	m_BaudRate.AddString(TEXT("43000"));
	m_BaudRate.AddString(TEXT("38400"));
	m_BaudRate.AddString(TEXT("19200"));
	m_BaudRate.AddString(TEXT("14400"));
	m_BaudRate.AddString(TEXT("9600"));
	m_BaudRate.AddString(TEXT("4800"));
	m_BaudRate.AddString(TEXT("2400"));
	m_BaudRate.AddString(TEXT("1200"));

	//设置默认的选项
	m_BaudRate.SetCurSel(0);


	//停止位
	m_StopBit.AddString(TEXT("1"));
	m_StopBit.AddString(TEXT("1.5"));
	m_StopBit.AddString(TEXT("2"));

	//设置默认的选项
	m_StopBit.SetCurSel(0);


	//数据位
	m_DataBit.AddString(TEXT("8"));
	m_DataBit.AddString(TEXT("7"));
	m_DataBit.AddString(TEXT("6"));
	m_DataBit.AddString(TEXT("5"));

	//设置默认的选项
	m_DataBit.SetCurSel(3);



	m_Parity.AddString(TEXT("无"));
	m_Parity.AddString(TEXT("奇校检"));
	m_Parity.AddString(TEXT("偶校验"));

	//设置默认的选项
	m_Parity.SetCurSel(2);



	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CUpperMachineDlg::OnSysCommand(UINT nID, LPARAM lParam)
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
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CUpperMachineDlg::OnPaint()
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
HCURSOR CUpperMachineDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CUpperMachineDlg::OnBnClickedBtnopen()//打开串口
{
	// TODO:  在此添加控件通知处理程序代码
	if (m_Comm1.get_PortOpen())//获取串口工作状态，为0代表代开
	{
		m_Comm1.put_PortOpen(FALSE);//如果已经打开，就关闭串口
		m_BtnOpen.SetWindowText(TEXT("打开串口"));//将打开串口按钮换成关闭串口
	}
	else
	{
/*		//拿到COM口索引
		int Com = m_COM.GetCurSel()+1;
		m_Comm1.put_CommPort(Com); //选择com，可根据具体情况更改*/


		//当前端口号
		int curPort = portse.ElementAt(m_COM.GetCurSel());//根据索引找到对应的com口编号
		m_Comm1.put_CommPort(curPort);//端口号

		m_Comm1.put_InBufferSize(1024); //设置输入缓冲区的大小，Bytes
		m_Comm1.put_OutBufferSize(1024); //设置输入缓冲区的大小，Bytes//

		CString Usart_Allcation = GetUsartAllcation();//获取串口参数配置
		m_Comm1.put_Settings(Usart_Allcation); //波特率9600，无校验，8个数据位，1个停止位
		m_Comm1.put_InputMode(1); //1：表示以二进制方式检取数据
		m_Comm1.put_RThreshold(1);
		//参数1表示每当串口接收缓冲区中有多于或等于1个字符时将引发一个接收数据的OnComm事件
		m_Comm1.put_InputLen(0); //设置当前接收区数据长度为0

		m_Comm1.put_PortOpen(TRUE);//打开串口
		m_BtnOpen.SetWindowText(TEXT("关闭串口"));//将打开串口按钮换成关闭串口

		m_Comm1.get_Input();//先预读缓冲区以清除残留数据
	}

	UpdateData(false);//将变量的内容同步到控件中
}
BEGIN_EVENTSINK_MAP(CUpperMachineDlg, CDialogEx)
	ON_EVENT(CUpperMachineDlg, IDC_MSCOMM1, 1, CUpperMachineDlg::OnCommMscomm1, VTS_NONE)
END_EVENTSINK_MAP()


void CUpperMachineDlg::OnCommMscomm1()//串口处理程序
{
	// TODO:  在此处添加消息处理程序代码
	VARIANT variant_inp;
	COleSafeArray safearray_inp;
	LONG len, k;
	BYTE rxdata[2048]; //设置BYTE数组 An 8-bit integerthat is not signed.
	CString strtemp;
	if (m_Comm1.get_CommEvent() == 2) //事件值为2表示接收缓冲区内有字符
	{             
		variant_inp = m_Comm1.get_Input(); //读缓冲区
		safearray_inp = variant_inp; //VARIANT型变量转换为ColeSafeArray型变量
		len = safearray_inp.GetOneDimSize(); //得到有效数据长度
		for (k = 0; k < len; k++)
			safearray_inp.GetElement(&k, rxdata + k);//转换为BYTE型数组
		for (k = 0; k < len; k++) //将数组转换为Cstring型变量
		{
			BYTE bt = *(char*)(rxdata + k); //字符型
			strtemp.Format(_T("%c"), bt); //将字符送入临时变量strtemp存放
			m_EditRxData += strtemp; //加入接收编辑框对应字符串 
		}
	}
	//UpdateData(FALSE); //更新编辑框内容
	SetDlgItemText(IDC_EditRxData, m_EditRxData);//不使用UpdateData(FALSE);的原因是该函数会
	//刷新整个对话框的数据，而SetDlgItemText()
	//只更新接收编辑框的数据。
}

void CUpperMachineDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类

	CDialogEx::OnOK();
}


void CUpperMachineDlg::OnBnClickedBtnsend()//发送按钮
{
	// TODO: 在此添加控件通知处理程序代码
	//UpdateData(TRUE); //读取编辑框内容

	//m_Comm1.put_Output(COleVariant(m_EditTxData));//发送数据
	AfxMessageBox(_T("此功能不可用！"));
}


void CUpperMachineDlg::OnBnClickedButton3()//清除数据
{
	// TODO: 在此添加控件通知处理程序代码
	
	m_EditRxData.Empty();
	UpdateData(FALSE);//从控件同步到控制台显示
}


void CUpperMachineDlg::OnBnClickedButton2()//保存数据
{
	// TODO: 在此添加控件通知处理程序代码
	CString FileName;
	//CFileDialog dlgFile(FALSE, _T("*.txt"), NULL, OFN_HIDEREADONLY, _T("Describe File(*.txt)|*.txt|All Files(*.*)|*.*||"), NULL);
	//if (IDOK == dlgFile.DoModal())
	//{
	//	FileName = dlgFile.GetPathName();
	//}
	//else
	//{
	//	return;
	//}
	FileName = "D:\\data.txt";
	CStdioFile FileWrite;
	//CFile::modeNoTruncate表示给文件追加 ，不覆盖原有内容
	if (!FileWrite.Open(FileName, CFile::modeWrite | CFile::modeCreate | CFile::typeText| CFile::modeNoTruncate, NULL))
	{
		AfxMessageBox(_T("打开文件失败!"));
		return;
	}
	SYSTEMTIME t;
	GetLocalTime(&t);
	CString temp ,time;
	temp =m_EditRxData;
	time.Format(_T("[%d-%d-%d %d:%d:%d]\n"),t.wYear,t.wMonth,t.wDay, t.wHour, t.wMinute, t.wSecond);//需要保存的数据，注意格式
	//去除多余换行符
 
	FileWrite.SeekToEnd();    //表示在文件最后添加。
	FileWrite.WriteString(time);
	FileWrite.WriteString(temp);
	FileWrite.WriteString(_T("\n"));
	FileWrite.Close();
	AfxMessageBox(_T("保存成功！"));
}
