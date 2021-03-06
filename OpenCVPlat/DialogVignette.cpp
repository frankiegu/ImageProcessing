// DialogVignette.cpp : 实现文件
//

#include "stdafx.h"
#include "OpenCVPlat.h"
#include "DialogVignette.h"
#include "afxdialogex.h"
#include "MainFrm.h"
#include "OpenCVPlatDoc.h"
#include "OpenCVPlatView.h"
#include "Vignette.h"


// CDialogVignette 对话框

IMPLEMENT_DYNAMIC(CDialogVignette, CDialogEx)

CDialogVignette::CDialogVignette(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDialogVignette::IDD, pParent)
{
	CMainFrame* pMain = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	COpenCVPlatDoc *pDoc = (COpenCVPlatDoc*)pMain->GetActiveDocument();

	maskImg = new double[pDoc->image.rows * pDoc->image.cols];
	generateGradient(maskImg, pDoc->image.cols, pDoc->image.rows);
	if (pDoc->image.channels() == 3)
	{
		labImg = cv::Mat(pDoc->image.size(), CV_8UC3);
		cv::cvtColor(pDoc->image, labImg, CV_BGR2Lab);
	}
	else
	{
		labImg = pDoc->image.clone();
	}
	old_labImg = labImg.clone();
}

CDialogVignette::~CDialogVignette()
{
	delete[] maskImg;
}

void CDialogVignette::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDialogVignette, CDialogEx)
	ON_WM_HSCROLL()
END_MESSAGE_MAP()


// CDialogVignette 消息处理程序


void CDialogVignette::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	CSliderCtrl *pSlidCtrl = (CSliderCtrl*)GetDlgItem(IDC_VIGNETTE_SLIDER);
	double value = 1 - pSlidCtrl->GetPos()/(double)pSlidCtrl->GetRangeMax();
	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
	CMainFrame* pMain = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	COpenCVPlatDoc *pDoc = (COpenCVPlatDoc*)pMain->GetActiveDocument();
	COpenCVPlatView *pView = (COpenCVPlatView*)pMain->GetActiveView();
	int height = labImg.size().height;
	int width = labImg.size().width;
	for (int row = 0; row < height; row++)
	{
		uchar *old_data = old_labImg.ptr<uchar>(row);
		uchar *data = labImg.ptr<uchar>(row);
		for (int col = 0; col < width; col++)
		{
			double m = maskImg[row * width + col];
			if (pDoc->image.channels() == 3)
				data[col * 3] = old_data[col * 3] * (m + value * (1 - m));
			else
				data[col] = old_data[col] * (m + value * (1 - m));
		}
	}
	if (pDoc->image.channels() == 3)
		cv::cvtColor(labImg, pDoc->image, CV_Lab2BGR);
	else
		pDoc->image = labImg;

	pView->Invalidate();
}


BOOL CDialogVignette::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CSliderCtrl *pSlidCtrl = (CSliderCtrl*)GetDlgItem(IDC_VIGNETTE_SLIDER);
	pSlidCtrl->SetRange(0, 1000);
	pSlidCtrl->SetTicFreq(100);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}
