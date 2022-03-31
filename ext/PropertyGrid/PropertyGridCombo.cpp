// PropertyGridCombo.cpp : implementation file
//

#include "stdafx.h"
#include "PropertyGridCombo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const int margin = 2;

// CPropertyGridCombo

IMPLEMENT_DYNCREATE(CPropertyGridCombo, CWnd)

CPropertyGridCombo::CPropertyGridCombo()
{
  m_pFont = NULL;
  m_nSelected = -1;
  m_bTracking = false;

  m_clrBack = GetSysColor(COLOR_WINDOW);
  m_clrText = GetSysColor(COLOR_WINDOWTEXT);
  m_clrFocus = GetSysColor(COLOR_HIGHLIGHT);
  m_clrHilite = GetSysColor(COLOR_HIGHLIGHTTEXT);
}

CPropertyGridCombo::~CPropertyGridCombo()
{
}

//
// content management
//

void CPropertyGridCombo::AddString(string strItem)
{
  m_Items.push_back(strItem);
}

void CPropertyGridCombo::SetCurSel(int nItem)
{
  m_nSelected = nItem;
}


BEGIN_MESSAGE_MAP(CPropertyGridCombo, CWnd)
  ON_WM_PAINT()
  ON_WM_SHOWWINDOW()
  ON_WM_LBUTTONDOWN()
  ON_WM_MOUSEMOVE()
  ON_WM_LBUTTONUP()
  ON_WM_DESTROY()
  ON_WM_KILLFOCUS()
  ON_WM_KEYDOWN()
  ON_WM_GETDLGCODE()
END_MESSAGE_MAP()

//
// creation
//

BOOL CPropertyGridCombo::Create(DWORD dwStyle, CRect& rc, CWnd* pParent, int nId)
{
  pParent->ClientToScreen(&rc);
  BOOL ret = CWnd::CreateEx(0, AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW), "", dwStyle|WS_POPUP, rc, pParent->GetParent(), nId);
  if (ret) SetOwner(pParent);
  return ret;
}

void CPropertyGridCombo::SetFont(CFont* pFont, BOOL bRedraw)
{
  m_pFont = pFont;
  CWnd::SetFont(pFont, bRedraw);
}

void CPropertyGridCombo::SetColors(COLORREF clrBack, COLORREF clrText, COLORREF clrFocus, COLORREF clrHilite)
{
  m_clrBack = clrBack;
  m_clrText = clrText;
  m_clrFocus = clrFocus;
  m_clrHilite = clrHilite;
}

void CPropertyGridCombo::OnShowWindow(BOOL bShow, UINT nStatus)
{
  if (bShow)
  {
    // get line height
    CDC* pDC = GetDC();
    int save = pDC->SaveDC();
    pDC->SelectObject(m_pFont?m_pFont:GetFont());
    m_line_height = pDC->GetTextExtent("Gg").cy + 2*margin;
    pDC->RestoreDC(save);
    ReleaseDC(pDC);

    // size control
    CRect rc;
    GetWindowRect(&rc);
    SetWindowPos(NULL, 0, 0, rc.Width(), int(m_Items.size())*m_line_height+2, SWP_NOOWNERZORDER|SWP_NOZORDER|SWP_NOMOVE);
    SetFocus();
  }

  CWnd::OnShowWindow(bShow, nStatus);
}

void CPropertyGridCombo::OnDestroy()
{
  CWnd::OnDestroy();
}

void CPropertyGridCombo::OnKillFocus(CWnd* pNewWnd)
{
  CWnd::OnKillFocus(pNewWnd);
  DestroyWindow();
}

//
// painting
//

void CPropertyGridCombo::OnPaint()
{
  // check
  if (m_nSelected<0) m_nSelected = 0;
  if (m_nSelected>int(m_Items.size())-1) m_nSelected = int(m_Items.size())-1;

  // client rect
  CRect rc;
  GetClientRect(&rc);

  // brush
  CBrush brush;
  brush.CreateSolidBrush(m_clrBack);

  // pen
  CPen pen;
  pen.CreatePen(PS_SOLID, 1, m_clrText);

  // the dc
  CPaintDC dc(this);
  CBrush* pOldBrush = dc.SelectObject(&brush);
  CPen* pOldPen = dc.SelectObject(&pen);
  CFont* pOldFont = dc.SelectObject(m_pFont);

  // draw
  dc.SelectObject(&brush);
  dc.SelectObject(&pen);
  dc.Rectangle(rc);

  // put items
  int i = 0;
  int y = 1;
  dc.SelectObject(m_pFont);
  dc.SetBkMode(TRANSPARENT);
  for (vector<string>::iterator it = m_Items.begin(); it != m_Items.end(); ++it)
  {
    CRect rcItem(0, y, rc.Width(), y+m_line_height);
    rcItem.DeflateRect(1,0,1,0);

    if (i == m_nSelected)
    {
      dc.DrawFocusRect(rcItem);
      dc.SetTextColor(m_clrHilite);

      CRect rc = rcItem;
      rc.DeflateRect(1,1);
      dc.FillSolidRect(rc, m_clrFocus);
    }
    else
    {
      dc.SetTextColor(m_clrText);
    }

    // do it
    rcItem.left += 2*margin;
    dc.DrawText(it->c_str(), rcItem, DT_SINGLELINE|DT_VCENTER|DT_LEFT|DT_NOPREFIX);
    y += m_line_height;
    i++;
  }

  // clean up
  dc.SelectObject(pOldFont);
  dc.SelectObject(pOldPen);
  dc.SelectObject(pOldBrush);
}

//
// mouse interaction
//

void CPropertyGridCombo::OnLButtonDown(UINT nFlags, CPoint point)
{
  m_nSelected = point.y/m_line_height;
  m_bTracking = true;
  SetCapture();
  Invalidate();

  CWnd::OnLButtonDown(nFlags, point);
}

void CPropertyGridCombo::OnMouseMove(UINT nFlags, CPoint point)
{
  if (m_bTracking)
  {
    m_nSelected = point.y/m_line_height;
    Invalidate();
  }
  CWnd::OnMouseMove(nFlags, point);
}

void CPropertyGridCombo::OnLButtonUp(UINT nFlags, CPoint point)
{
  if (m_bTracking)
  {
    ReleaseCapture();
    m_bTracking = false;
    Invalidate();
  }

  GetOwner()->SendMessage(WM_PG_COMBOSELCHANGED, m_nSelected, 0);
}

//
// keyboard interaction
//

UINT CPropertyGridCombo::OnGetDlgCode()
{
  return DLGC_WANTALLKEYS;
}

void CPropertyGridCombo::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
  if (nChar == VK_LEFT || nChar == VK_UP)
  {
    m_nSelected = max(0, m_nSelected-1);
    Invalidate();
  }
  else if (nChar == VK_RIGHT || nChar == VK_DOWN)
  {
    m_nSelected = min(int(m_Items.size())-1, m_nSelected+1);
    Invalidate();
  }
  else if (nChar == VK_ESCAPE)
  {
    DestroyWindow();
    return;
  }
  else if (nChar == VK_RETURN || nChar == VK_EXECUTE)
  {
    GetOwner()->SendMessage(WM_PG_COMBOSELCHANGED, m_nSelected, 0);
    return;
  }

  CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}
