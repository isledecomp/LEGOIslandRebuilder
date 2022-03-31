#pragma once
#include "..\CustomItem.h"

class CSerialItem : public ICustomItem
{
public:
  CSerialItem(void);
  ~CSerialItem(void);
  virtual CPropertyGrid::EEditMode GetEditMode();
  virtual void DrawItem(CDC& dc, CRect rc, bool focused);
  virtual string GetStringForInPlaceEdit();
  virtual bool OnItemEdited(string strNewValue);

protected:
  string m_serial;
};
