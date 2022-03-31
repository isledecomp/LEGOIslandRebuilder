#pragma once
#include <vector>
#include <string>
using namespace std;

// CPropertyGridCombo frame

#define WM_PG_COMBOSELCHANGED WM_USER+487

class CPropertyGridCombo : public CWnd
{
  DECLARE_DYNCREATE(CPropertyGridCombo)

public:
  CPropertyGridCombo();
  virtual ~CPropertyGridCombo();

  BOOL Create(DWORD dwStyle, CRect& rc, CWnd* pParent, int nId);
  void SetFont(CFont* pFont, BOOL bRedraw = TRUE);
  void SetColors(COLORREF clrBack, COLORREF clrText, COLORREF clrFocus, COLORREF clrHilite);

  void AddString(string strItem);
  void SetCurSel(int nItem);

protected:
  vector<string> m_Items;
  int m_nSelected;

  CFont* m_pFont;
  int m_line_height;

  bool m_bTracking;

  COLORREF m_clrBack;
  COLORREF m_clrText;
  COLORREF m_clrFocus;
  COLORREF m_clrHilite;

protected:
  DECLARE_MESSAGE_MAP()

public:
  afx_msg void OnPaint();
  afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnMouseMove(UINT nFlags, CPoint point);
  afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
  afx_msg void OnDestroy();
  afx_msg void OnKillFocus(CWnd* pNewWnd);
  afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
  afx_msg UINT OnGetDlgCode();
};


