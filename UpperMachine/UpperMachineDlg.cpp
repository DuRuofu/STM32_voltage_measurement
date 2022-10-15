
// UpperMachineDlg.cpp : ʵ���ļ�
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

//��ȡ��������
CString CUpperMachineDlg::GetUsartAllcation()
{
	//�õ������ʵ�����
	int BaudRate = m_BaudRate.GetCurSel();
	CString BaudRate_str;
	m_BaudRate.GetLBText(BaudRate, BaudRate_str);//����������ȡ����
//	MessageBox(BaudRate_str);

	//�õ�ֹͣλ������
	int StopBit = m_StopBit.GetCurSel();
	CString StopBit_str;
	m_StopBit.GetLBText(StopBit, StopBit_str);//����������ȡ����
//	MessageBox(StopBit_str);

	//�õ��õ�����λ������
	int DataBit = m_DataBit.GetCurSel();
	CString DataBit_str;
	m_DataBit.GetLBText(DataBit, DataBit_str);//����������ȡ����
//	MessageBox(DataBit_str);

	//�õ��õ���żУ��λ������
	int Parity = m_Parity.GetCurSel();
	char Parity_data;
	switch (Parity)//������������У��λ
	{
	case 0: Parity_data = 'E';//��
		break;
	case 1: Parity_data = 'O';//��У��
		break;
	case 2: Parity_data = 'N';//żУ��
		break;
	default:
		break;
	}
//	MessageBox(Parity_str);

	//�ϲ��ַ���
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
	case DBT_DEVICEREMOVECOMPLETE://�Ƴ��豸
	case DBT_DEVICEARRIVAL://����豸
		RefreshCom();//ˢ����Ͽ������
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
	//���������������
	ports.RemoveAll();//���д��ڴ���
	portse.RemoveAll();//���ô���
	portsu.RemoveAll();//��ռ�ô���

	//��Ϊ������255�����ڣ��������μ��������Ƿ����
	//����ܴ�ĳһ���ڣ���򿪴��ڲ��ɹ��������ص��� ERROR_ACCESS_DENIED������Ϣ��
	//����Ϊ���ڴ��ڣ�ֻ�������߱��������Ѿ���ռ��
	//���򴮿ڲ�����
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
				portsu.Add(i);       //��ռ�õĴ���
			}
		}
		else
		{
			//The port was opened successfully
			bSuccess = TRUE;
			portse.Add(i);      //���õĴ���
			//Don't forget to close the port, since we are going to do nothing with it anyway
			CloseHandle(hPort);
		}

		//Add the port number to the array which will be returned
		if (bSuccess)
			ports.Add(i);   //���д��ڵĴ���
	}
}

void CUpperMachineDlg::AddCom(void)
{

	EnumerateSerialPorts(ports, portse, portsu);
	unsigned short uicounter;
	unsigned short uisetcom;
	CString str;

	//��ȡ���ô��ڸ���
	uicounter = portse.GetSize();
	//�����������0
	if (uicounter > 0)
	{
		//��ʼ�������б��
		for (int i = 0; i < uicounter; i++)
		{
			uisetcom = portse.ElementAt(i);
			str.Format(_T("COM%d "), uisetcom);
			m_COM.AddString(str);
		}
	}
}

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CUpperMachineDlg �Ի���




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


// CUpperMachineDlg ��Ϣ�������

BOOL CUpperMachineDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	m_EditTxData="�˹��ܲ����ã�";
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

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��


	// TODO: Add extra initialization here
	AddCom();//����Ͽ�����Ӵ����豸	
	m_COM.SetCurSel(1);	//����Ĭ�ϵ�ѡ��
	//...


	/*//COM��
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
	//����Ĭ�ϵ�ѡ��
	m_COM.SetCurSel(5);*/


	//������
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

	//����Ĭ�ϵ�ѡ��
	m_BaudRate.SetCurSel(0);


	//ֹͣλ
	m_StopBit.AddString(TEXT("1"));
	m_StopBit.AddString(TEXT("1.5"));
	m_StopBit.AddString(TEXT("2"));

	//����Ĭ�ϵ�ѡ��
	m_StopBit.SetCurSel(0);


	//����λ
	m_DataBit.AddString(TEXT("8"));
	m_DataBit.AddString(TEXT("7"));
	m_DataBit.AddString(TEXT("6"));
	m_DataBit.AddString(TEXT("5"));

	//����Ĭ�ϵ�ѡ��
	m_DataBit.SetCurSel(3);



	m_Parity.AddString(TEXT("��"));
	m_Parity.AddString(TEXT("��У��"));
	m_Parity.AddString(TEXT("żУ��"));

	//����Ĭ�ϵ�ѡ��
	m_Parity.SetCurSel(2);



	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CUpperMachineDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CUpperMachineDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CUpperMachineDlg::OnBnClickedBtnopen()//�򿪴���
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	if (m_Comm1.get_PortOpen())//��ȡ���ڹ���״̬��Ϊ0�������
	{
		m_Comm1.put_PortOpen(FALSE);//����Ѿ��򿪣��͹رմ���
		m_BtnOpen.SetWindowText(TEXT("�򿪴���"));//���򿪴��ڰ�ť���ɹرմ���
	}
	else
	{
/*		//�õ�COM������
		int Com = m_COM.GetCurSel()+1;
		m_Comm1.put_CommPort(Com); //ѡ��com���ɸ��ݾ����������*/


		//��ǰ�˿ں�
		int curPort = portse.ElementAt(m_COM.GetCurSel());//���������ҵ���Ӧ��com�ڱ��
		m_Comm1.put_CommPort(curPort);//�˿ں�

		m_Comm1.put_InBufferSize(1024); //�������뻺�����Ĵ�С��Bytes
		m_Comm1.put_OutBufferSize(1024); //�������뻺�����Ĵ�С��Bytes//

		CString Usart_Allcation = GetUsartAllcation();//��ȡ���ڲ�������
		m_Comm1.put_Settings(Usart_Allcation); //������9600����У�飬8������λ��1��ֹͣλ
		m_Comm1.put_InputMode(1); //1����ʾ�Զ����Ʒ�ʽ��ȡ����
		m_Comm1.put_RThreshold(1);
		//����1��ʾÿ�����ڽ��ջ��������ж��ڻ����1���ַ�ʱ������һ���������ݵ�OnComm�¼�
		m_Comm1.put_InputLen(0); //���õ�ǰ���������ݳ���Ϊ0

		m_Comm1.put_PortOpen(TRUE);//�򿪴���
		m_BtnOpen.SetWindowText(TEXT("�رմ���"));//���򿪴��ڰ�ť���ɹرմ���

		m_Comm1.get_Input();//��Ԥ���������������������
	}

	UpdateData(false);//������������ͬ�����ؼ���
}
BEGIN_EVENTSINK_MAP(CUpperMachineDlg, CDialogEx)
	ON_EVENT(CUpperMachineDlg, IDC_MSCOMM1, 1, CUpperMachineDlg::OnCommMscomm1, VTS_NONE)
END_EVENTSINK_MAP()


void CUpperMachineDlg::OnCommMscomm1()//���ڴ������
{
	// TODO:  �ڴ˴������Ϣ����������
	VARIANT variant_inp;
	COleSafeArray safearray_inp;
	LONG len, k;
	BYTE rxdata[2048]; //����BYTE���� An 8-bit integerthat is not signed.
	CString strtemp;
	if (m_Comm1.get_CommEvent() == 2) //�¼�ֵΪ2��ʾ���ջ����������ַ�
	{             
		variant_inp = m_Comm1.get_Input(); //��������
		safearray_inp = variant_inp; //VARIANT�ͱ���ת��ΪColeSafeArray�ͱ���
		len = safearray_inp.GetOneDimSize(); //�õ���Ч���ݳ���
		for (k = 0; k < len; k++)
			safearray_inp.GetElement(&k, rxdata + k);//ת��ΪBYTE������
		for (k = 0; k < len; k++) //������ת��ΪCstring�ͱ���
		{
			BYTE bt = *(char*)(rxdata + k); //�ַ���
			strtemp.Format(_T("%c"), bt); //���ַ�������ʱ����strtemp���
			m_EditRxData += strtemp; //������ձ༭���Ӧ�ַ��� 
		}
	}
	//UpdateData(FALSE); //���±༭������
	SetDlgItemText(IDC_EditRxData, m_EditRxData);//��ʹ��UpdateData(FALSE);��ԭ���Ǹú�����
	//ˢ�������Ի�������ݣ���SetDlgItemText()
	//ֻ���½��ձ༭������ݡ�
}

void CUpperMachineDlg::OnOK()
{
	// TODO: �ڴ����ר�ô����/����û���

	CDialogEx::OnOK();
}


void CUpperMachineDlg::OnBnClickedBtnsend()//���Ͱ�ť
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//UpdateData(TRUE); //��ȡ�༭������

	//m_Comm1.put_Output(COleVariant(m_EditTxData));//��������
	AfxMessageBox(_T("�˹��ܲ����ã�"));
}


void CUpperMachineDlg::OnBnClickedButton3()//�������
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	
	m_EditRxData.Empty();
	UpdateData(FALSE);//�ӿؼ�ͬ��������̨��ʾ
}


void CUpperMachineDlg::OnBnClickedButton2()//��������
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
	//CFile::modeNoTruncate��ʾ���ļ�׷�� ��������ԭ������
	if (!FileWrite.Open(FileName, CFile::modeWrite | CFile::modeCreate | CFile::typeText| CFile::modeNoTruncate, NULL))
	{
		AfxMessageBox(_T("���ļ�ʧ��!"));
		return;
	}
	SYSTEMTIME t;
	GetLocalTime(&t);
	CString temp ,time;
	temp =m_EditRxData;
	time.Format(_T("[%d-%d-%d %d:%d:%d]\n"),t.wYear,t.wMonth,t.wDay, t.wHour, t.wMinute, t.wSecond);//��Ҫ��������ݣ�ע���ʽ
	//ȥ�����໻�з�
 
	FileWrite.SeekToEnd();    //��ʾ���ļ������ӡ�
	FileWrite.WriteString(time);
	FileWrite.WriteString(temp);
	FileWrite.WriteString(_T("\n"));
	FileWrite.Close();
	AfxMessageBox(_T("����ɹ���"));
}
