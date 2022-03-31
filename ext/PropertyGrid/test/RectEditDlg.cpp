// RectEditDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PropGrid.h"
#include "RectEditDlg.h"


// CRectEditDlg dialog

IMPLEMENT_DYNAMIC(CRectEditDlg, CDialog)
CRectEditDlg::CRectEditDlg(CWnd* pParent /*=NULL*/)
: CDialog(CRectEditDlg::IDD, pParent)
, m_left(10)
, m_top(10)
, m_right(10)
, m_bottom(10)
{
}

CRectEditDlg::~CRectEditDlg()
{
}

void CRectEditDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_LEFT, m_left);
  DDV_MinMaxInt(pDX, m_left, 1, 1000);
  DDX_Text(pDX, IDC_TOP, m_top);
  DDV_MinMaxInt(pDX, m_top, 1, 1000);
  DDX_Text(pDX, IDC_RIGHT, m_right);
  DDV_MinMaxInt(pDX, m_right, 1, 1000);
  DDX_Text(pDX, IDC_BOTTOM, m_bottom);
  DDV_MinMaxInt(pDX, m_bottom, 1, 1000);
}


BEGIN_MESSAGE_MAP(CRectEditDlg, CDialog)
END_MESSAGE_MAP()


// CRectEditDlg message handlers
