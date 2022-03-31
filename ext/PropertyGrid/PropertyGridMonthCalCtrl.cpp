// PropertyGridMonthCalCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "PropertyGridMonthCalCtrl.h"


// CPropertyGridMonthCalCtrl

// DOWNGRADE: Commented out to prevent link error where CRuntimeStatic for CMonthCalCtrl is unresolved
//IMPLEMENT_DYNAMIC(CPropertyGridMonthCalCtrl, CMonthCalCtrl)
CPropertyGridMonthCalCtrl::CPropertyGridMonthCalCtrl()
{
}

CPropertyGridMonthCalCtrl::~CPropertyGridMonthCalCtrl()
{
}


BEGIN_MESSAGE_MAP(CPropertyGridMonthCalCtrl, CMonthCalCtrl)
  ON_WM_KILLFOCUS()
  ON_NOTIFY_REFLECT(MCN_SELECT, OnMcnSelect)
  ON_WM_GETDLGCODE()
  ON_WM_KEYDOWN()
END_MESSAGE_MAP()



// CPropertyGridMonthCalCtrl message handlers


void CPropertyGridMonthCalCtrl::OnKillFocus(CWnd* pNewWnd)
{
  CMonthCalCtrl::OnKillFocus(pNewWnd);
  CWnd* pParent = pNewWnd ? pNewWnd->GetParent() : NULL;
  if (pParent != this)
    DestroyWindow();
}

void CPropertyGridMonthCalCtrl::OnMcnSelect(NMHDR *pNMHDR, LRESULT *pResult)
{
  LPNMSELCHANGE pSelChange = reinterpret_cast<LPNMSELCHANGE>(pNMHDR);
  GetOwner()->SendMessage(WM_PG_DATESELCHANGED);
  *pResult = 0;
}

UINT CPropertyGridMonthCalCtrl::OnGetDlgCode()
{
  return DLGC_WANTALLKEYS;
}

void CPropertyGridMonthCalCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
  if (nChar == VK_ESCAPE)
  {
    DestroyWindow();
    return;
  }
  else if (nChar == VK_RETURN || nChar == VK_EXECUTE)
  {
    GetOwner()->SendMessage(WM_PG_DATESELCHANGED);
    return;
  }

  CMonthCalCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}
