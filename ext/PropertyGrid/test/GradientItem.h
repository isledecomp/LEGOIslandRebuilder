#pragma once
#include "..\CustomItem.h"

class CGradientItem :
  public ICustomItem
{
public:
  CGradientItem(void);

public:
  virtual CPropertyGrid::EEditMode GetEditMode();
  virtual void DrawItem(CDC& dc, CRect rc, bool focused);
  virtual bool OnLButtonDown(CRect rc, CPoint pt);
  virtual void OnMouseMove(CRect rc, CPoint pt);
  virtual void OnLButtonUp(CRect rc, CPoint pt);

protected:
  COLORREF m_clrLeft;
  COLORREF m_clrRight;
  int m_nButtonPushed; //0: none, 1: left, 2:right
};
