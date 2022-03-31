#pragma once
#include "..\PropertyGrid.h"

class CTreeItem;

// CCustomTreeCtrl

class CCustomTreeCtrl : public CTreeCtrl
{
  DECLARE_DYNAMIC(CCustomTreeCtrl)

public:
  CCustomTreeCtrl();
  virtual ~CCustomTreeCtrl();

  CTreeItem* m_item;

protected:
  DECLARE_MESSAGE_MAP()

  void Validate();

public:
  afx_msg UINT OnGetDlgCode();
  afx_msg void OnKillFocus(CWnd* pNewWnd);
  afx_msg void OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
protected:
  virtual void PostNcDestroy();
};


