#include "StdAfx.h"
#include "GradientItem.h"

CGradientItem::CGradientItem(void)
{
  m_clrLeft = RGB(0,0,0);
  m_clrRight = RGB(0,255,0);
  m_nButtonPushed = 0;
}

CPropertyGrid::EEditMode CGradientItem::GetEditMode()
{
  return CPropertyGrid::EM_CUSTOM;
}

void CGradientItem::DrawItem(CDC& dc, CRect rc, bool focused)
{
  CRect rect = rc;
  rect.DeflateRect(focused?rc.Height()+2:2, 2);

  CBrush brush;
  brush.CreateSolidBrush(GetSysColor(COLOR_WINDOWTEXT));
  dc.FrameRect(rect, &brush);

  rect.DeflateRect(1,1);

  TRIVERTEX        vert[2] ;
  GRADIENT_RECT    gRect;
  vert [0] .x      = rect.left;
  vert [0] .y      = rect.top;
  vert [0] .Red    = GetRValue(m_clrLeft)*256;
  vert [0] .Green  = GetGValue(m_clrLeft)*256;
  vert [0] .Blue   = GetBValue(m_clrLeft)*256;
  vert [0] .Alpha  = 0x0000;

  vert [1] .x      = rect.right;
  vert [1] .y      = rect.bottom;
  vert [1] .Red    = GetRValue(m_clrRight)*256;
  vert [1] .Green  = GetGValue(m_clrRight)*256;
  vert [1] .Blue   = GetBValue(m_clrRight)*256;
  vert [1] .Alpha  = 0x0000;

  gRect.UpperLeft  = 0;
  gRect.LowerRight = 1;

  // DOWNGRADE: Commented out because MFC 4.2 doesn't have GradientFill
  //dc.GradientFill(vert,2,&gRect,1,GRADIENT_FILL_RECT_H);

  if (focused)
  {
    // for propert clean up
    CFont* pOldFont = dc.SelectObject(m_pGrid->GetFontBold());

    // the left button
    CRect rc1 = rc;
    rc1.right = rc1.left + rc1.Height();
    dc.DrawFrameControl(rc1, DFC_BUTTON, DFCS_BUTTONPUSH|(m_nButtonPushed==1?DFCS_PUSHED:0));
    dc.SelectObject(m_pGrid->GetFontBold());
    dc.DrawText("...", rc1, DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_NOPREFIX);

    // the right button
    CRect rc2 = rc;
    rc2.left = rc2.right - rc2.Height();
    dc.DrawFrameControl(rc2, DFC_BUTTON, DFCS_BUTTONPUSH|(m_nButtonPushed==2?DFCS_PUSHED:0));
    dc.SelectObject(m_pGrid->GetFontBold());
    dc.DrawText("...", rc2, DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_NOPREFIX);

    // clean up
    dc.SelectObject(pOldFont);
  }
}

bool CGradientItem::OnLButtonDown(CRect rc, CPoint pt)
{
  m_nButtonPushed = 0;
  if (rc.PtInRect(pt))
  {
    if (pt.x<=rc.left+rc.Height())
    {
      m_nButtonPushed = 1;
      return true;
    }
    if (pt.x>=rc.right-rc.Height())
    {
      m_nButtonPushed = 2;
      return true;
    }
  }
  return false;
}

void CGradientItem::OnMouseMove(CRect rc, CPoint pt)
{
  OnLButtonDown(rc, pt);
}

void CGradientItem::OnLButtonUp(CRect rc, CPoint pt)
{
  COLORREF& clr = (m_nButtonPushed==1) ? m_clrLeft : m_clrRight;
  m_nButtonPushed = 0;
  CColorDialog dlg(clr, 0, m_pGrid);
  if (dlg.DoModal() == IDOK)
  {
    clr = dlg.GetColor();
    m_pGrid->Invalidate();
  }
}

