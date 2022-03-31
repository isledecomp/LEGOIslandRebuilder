// ListDynDialogEx.h: interface for the CListDynDialogEx class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LISTDYNDIALOGEX_H__57BA7AFA_B679_4FAC_B6BE_1AD925E45194__INCLUDED_)
#define AFX_LISTDYNDIALOGEX_H__57BA7AFA_B679_4FAC_B6BE_1AD925E45194__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "dyndialogex.h"

class CListDynDialogEx : public CDynDialogEx
{
public:
  CListDynDialogEx(CWnd* pParent = NULL);
  virtual ~CListDynDialogEx();

  CListBox m_lstBox;

  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CListDynDialogEx)
public:
  virtual int DoModal();
  //}}AFX_VIRTUAL

  // Implementation
protected:

  // Generated message map functions
  //{{AFX_MSG(CListDynDialogEx)
  virtual BOOL OnInitDialog();
  afx_msg void OnDblclkList();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_LISTDYNDIALOGEX_H__57BA7AFA_B679_4FAC_B6BE_1AD925E45194__INCLUDED_)
