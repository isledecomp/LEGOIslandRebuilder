#include "StdAfx.h"
#include ".\treeitem.h"

CTreeItem::CTreeItem(void)
{
}

CTreeItem::~CTreeItem(void)
{
}

void CTreeItem::SetValue(string value)
{
  m_value =  value;
  m_pGrid->Invalidate();
}

CPropertyGrid::EEditMode CTreeItem::GetEditMode()
{
  return CPropertyGrid::EM_DROPDOWN;
}

void CTreeItem::DrawItem(CDC& dc, CRect rc, bool focused)
{
  rc.left += m_pGrid->GetTextMargin();
  dc.DrawText(m_value.c_str(), rc, DT_SINGLELINE|DT_LEFT|DT_VCENTER|DT_END_ELLIPSIS|DT_NOPREFIX);
}

void CTreeItem::ShowDropDown(CRect rc)
{
  rc.top += rc.Height();
  rc.bottom = rc.top + 100;
  m_pGrid->ClientToScreen(&rc);
  m_tree = new CCustomTreeCtrl;
  m_tree->m_item = this;
  m_tree->CWnd::CreateEx(0, WC_TREEVIEW, NULL, WS_POPUP|WS_BORDER|TVS_HASBUTTONS|TVS_HASLINES|TVS_LINESATROOT, rc, m_pGrid->GetParent(), 0);
  m_tree->SetOwner(m_pGrid->GetParent());
  HTREEITEM hf = m_tree->InsertItem("Folder 1");
  m_tree->InsertItem("Item 1", hf);
  m_tree->InsertItem("Item 2", hf);
  m_tree->Expand(hf, TVE_EXPAND);
  hf = m_tree->InsertItem("Folder 2");
  m_tree->InsertItem("Item 3", hf);
  m_tree->Expand(hf, TVE_EXPAND);
  m_tree->ShowWindow(SW_SHOW);
  m_tree->RedrawWindow();
}
