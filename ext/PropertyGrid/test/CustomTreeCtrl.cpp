// CustomTreeCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "PropGrid.h"
#include "CustomTreeCtrl.h"
#include "TreeItem.h"
#include ".\customtreectrl.h"


// CCustomTreeCtrl

IMPLEMENT_DYNAMIC(CCustomTreeCtrl, CTreeCtrl)
CCustomTreeCtrl::CCustomTreeCtrl()
{
}

CCustomTreeCtrl::~CCustomTreeCtrl()
{
}


BEGIN_MESSAGE_MAP(CCustomTreeCtrl, CTreeCtrl)
  ON_WM_GETDLGCODE()
  ON_WM_KILLFOCUS()
  ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnTvnSelchanged)
  ON_WM_KEYDOWN()
END_MESSAGE_MAP()



// CCustomTreeCtrl message handlers


UINT CCustomTreeCtrl::OnGetDlgCode()
{
  return DLGC_WANTALLKEYS;
}

void CCustomTreeCtrl::OnKillFocus(CWnd* pNewWnd)
{
  CTreeCtrl::OnKillFocus(pNewWnd);
  DestroyWindow();
}

void CCustomTreeCtrl::OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
  LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
  if (pNMTreeView->action == TVC_BYMOUSE)
    Validate();
  *pResult = 0;
}

void CCustomTreeCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
  if (nChar == VK_ESCAPE)
  {
    DestroyWindow();
    return;
  }
  else if (nChar == VK_RETURN || nChar == VK_EXECUTE)
  {
    Validate();
    return;
  }

  CTreeCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CCustomTreeCtrl::Validate()
{
  if (GetSafeHwnd())
  {
    HTREEITEM hItem = GetSelectedItem();
    if (hItem && !ItemHasChildren(hItem))
    {
      CString strText = GetItemText(hItem);
      m_item->SetValue(LPCTSTR(strText));
      DestroyWindow();
    }
  }
}

void CCustomTreeCtrl::PostNcDestroy()
{
  CTreeCtrl::PostNcDestroy();
  delete this;
}
