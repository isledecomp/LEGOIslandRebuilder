//////////////////////////////////////////////////////////////////////
// InPlaceEdit.h : header file
//
// MFC Grid Control - inplace editing class
//
// Written by Chris Maunder <cmaunder@mail.com>
// Copyright (c) 1998-2002. All Rights Reserved.
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed unmodified by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name and all copyright 
// notices remains intact. 
//
// An email letting me know how you are using it would be nice as well. 
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability for any damage/loss of business that
// this product may cause.
//
// For use with CGridCtrl v2.10+
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GRIDINPLACEEDIT_H__ECD42821_16DF_11D1_992F_895E185F9C72__INCLUDED_)
#define AFX_GRIDINPLACEEDIT_H__ECD42821_16DF_11D1_992F_895E185F9C72__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <string>
using namespace std;

#define WM_PG_ENDLABELEDIT WM_USER+488

class CPropertyGridInPlaceEdit : public CEdit
{
  // Construction
public:
  CPropertyGridInPlaceEdit(CWnd* pParent, CRect& rect, DWORD dwStyle, UINT nID, string sInitText);
  void SetColors(COLORREF clrBack, COLORREF clrText);

  // Attributes
public:

  // Operations
public:
  void CancelEdit();
  void EndEdit();

  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CPropertyGridInPlaceEdit)
public:
  virtual BOOL PreTranslateMessage(MSG* pMsg);
  //}}AFX_VIRTUAL

  // Implementation
public:
  virtual ~CPropertyGridInPlaceEdit();

  // Generated message map functions
protected:
  //{{AFX_MSG(CPropertyGridInPlaceEdit)
  afx_msg void OnKillFocus(CWnd* pNewWnd);
  afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
  afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
  afx_msg UINT OnGetDlgCode();
  afx_msg HBRUSH CtlColor ( CDC* pDC, UINT nCtlColor );
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()

private:
  CString m_sInitText;
  BOOL    m_bExitOnArrows;
  CRect   m_Rect;

  COLORREF m_clrBack;
  COLORREF m_clrText;
  CBrush  m_Brush;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GRIDINPLACEEDIT_H__ECD42821_16DF_11D1_992F_895E185F9C72__INCLUDED_)
