#include "StdAfx.h"
#include ".\serialitem.h"

CSerialItem::CSerialItem(void)
{
  m_serial = "12348765";
}

CSerialItem::~CSerialItem(void)
{
}

CPropertyGrid::EEditMode CSerialItem::GetEditMode()
{
  return CPropertyGrid::EM_INPLACE;
}

void CSerialItem::DrawItem(CDC& dc, CRect rc, bool focused)
{
  string serial = m_serial;
  while (serial.length()<8) serial += " ";
  serial = serial.substr(0,4) + "-" + serial.substr(4,4);
  rc.left += m_pGrid->GetTextMargin();
  dc.DrawText(serial.c_str(), rc, DT_SINGLELINE|DT_LEFT|DT_VCENTER|DT_END_ELLIPSIS|DT_NOPREFIX);
}

string CSerialItem::GetStringForInPlaceEdit()
{
  return m_serial;
}

bool CSerialItem::OnItemEdited(string strNewValue)
{
  if (strNewValue.length()!=8)
  {
    AfxMessageBox("Invalid serial number");
    return false;
  }
  else
  {
    m_serial = strNewValue;
    return true;
  }
}
