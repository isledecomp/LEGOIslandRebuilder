#pragma once
#include "..\customitem.h"
#include "CustomTreeCtrl.h"

class CTreeItem :
  public ICustomItem
{
public:
  CTreeItem(void);
  ~CTreeItem(void);

  virtual CPropertyGrid::EEditMode GetEditMode();
  virtual void DrawItem(CDC& dc, CRect rc, bool focused);
  virtual void ShowDropDown(CRect rc);

  void SetValue(string value);

protected:
  string m_value;
  CCustomTreeCtrl* m_tree;
};
