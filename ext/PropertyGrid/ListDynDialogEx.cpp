// ListDynDialogEx.cpp: implementation of the CListDynDialogEx class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ListDynDialogEx.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define IDC_LIST1 1600
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CListDynDialogEx::CListDynDialogEx(CWnd* pParent /*= NULL*/)
: CDynDialogEx(pParent)
{
  m_bAddSystemButtons = FALSE;
}

CListDynDialogEx::~CListDynDialogEx()
{
}

BEGIN_MESSAGE_MAP(CListDynDialogEx, CDynDialogEx)
  //{{AFX_MSG_MAP(CListDynDialogEx)
  ON_LBN_DBLCLK(IDC_LIST1, OnDblclkList)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CListDynDialogEx::OnInitDialog()
{
  BOOL bRet = CDynDialogEx::OnInitDialog();

  m_lstBox.AddString(_T("First String"));
  m_lstBox.AddString(_T("Second String"));

  return bRet;
}

int CListDynDialogEx::DoModal()
{
  CRect rect(7,7,150,150);
  AddDlgControl(_T("LISTBOX"), _T("ListboxText"), STYLE_LISTBOX, EXSTYLE_LISTBOX, &rect, &m_lstBox, IDC_LIST1);
  return CDynDialogEx::DoModal();
}

void CListDynDialogEx::OnDblclkList()
{
  CString strBuf;
  int nIndex = m_lstBox.GetCurSel();
  m_lstBox.GetText(nIndex, strBuf);
  AfxMessageBox(strBuf);
  OnOK();
}
