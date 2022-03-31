#pragma once
#include "resource.h"

// CRectEditDlg dialog

class CRectEditDlg : public CDialog
{
  DECLARE_DYNAMIC(CRectEditDlg)

public:
  CRectEditDlg(CWnd* pParent = NULL);   // standard constructor
  virtual ~CRectEditDlg();

  // Dialog Data
  enum { IDD = IDD_RECT_EDIT };

protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

  DECLARE_MESSAGE_MAP()
public:
  int m_left;
  int m_top;
  int m_right;
  int m_bottom;
};
