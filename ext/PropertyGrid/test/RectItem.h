#pragma once
#include "..\CustomItem.h"

class CRectItem : public ICustomItem
{
public:
  CRectItem(void);
  ~CRectItem(void);

  virtual CPropertyGrid::EEditMode GetEditMode();
  virtual void DrawItem(CDC& dc, CRect rc, bool focused);
  virtual bool OnEditItem();

protected:
  int m_left;
  int m_top;
  int m_right;
  int m_bottom;
};
