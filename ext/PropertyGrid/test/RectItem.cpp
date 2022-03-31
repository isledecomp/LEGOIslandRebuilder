#include "StdAfx.h"
#include "RectItem.h"
#include "RectEditDlg.h"

CRectItem::CRectItem(void)
{
  m_left = 20;
  m_top = 20;
  m_right = 100;
  m_bottom = 100;
}

CRectItem::~CRectItem(void)
{
}

CPropertyGrid::EEditMode CRectItem::GetEditMode()
{
  return CPropertyGrid::EM_MODAL;
}

void CRectItem::DrawItem(CDC& dc, CRect rc, bool focused)
{
  CString str;
  str.Format("%d; %d; %d; %d", m_left, m_top, m_right, m_bottom);
  rc.left += m_pGrid->GetTextMargin();
  dc.DrawText(str, rc, DT_SINGLELINE|DT_LEFT|DT_VCENTER|DT_END_ELLIPSIS|DT_NOPREFIX);
}

bool CRectItem::OnEditItem()
{
  CRectEditDlg dlg(m_pGrid);
  dlg.m_left = m_left;
  dlg.m_top = m_top;
  dlg.m_right = m_right;
  dlg.m_bottom = m_bottom;
  if (dlg.DoModal() == IDOK)
  {
    m_left = dlg.m_left;
    m_top = dlg.m_top;
    m_right = dlg.m_right;
    m_bottom = dlg.m_bottom;
    return true;
  }
  return false;
}
