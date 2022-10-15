
// UpperMachineDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "mscomm1.h"

// CUpperMachineDlg 对话框
class CUpperMachineDlg : public CDialogEx
{
// 构造
public:
	CUpperMachineDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_UPPERMACHINE_DIALOG };

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
//	CButton m_BtnOpen;
private:
	CString m_EditRxData;
	CButton m_BtnOpen;
	CMscomm1 m_Comm1;
public:
	afx_msg void OnBnClickedBtnopen();
	DECLARE_EVENTSINK_MAP()
	void OnCommMscomm1();
	virtual void OnOK();
	void AddCom(void);
	void EnumerateSerialPorts(CUIntArray& ports, CUIntArray& portse, CUIntArray& portsu);//获取可用串口函数
	afx_msg BOOL OnDeviceChange(UINT nEventType, DWORD dwData);
	void RefreshCom(void);
	CString GetUsartAllcation();
    CString deleteCharString0(CString sourceString, char chElemData);
private:
	CComboBox m_BaudRate;
//	CComboBox m_COM;
	CButton m_BtnSend;
	CString m_EditTxData;
	CComboBox m_Parity;
	CComboBox m_StopBit;
	CComboBox m_DataBit;
public:
	afx_msg void OnBnClickedBtnsend();
private:
	CComboBox m_COM;
public:
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton2();
};
