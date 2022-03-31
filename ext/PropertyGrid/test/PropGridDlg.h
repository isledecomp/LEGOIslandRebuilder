// PropGridDlg.h : header file
//

#pragma once
#include "..\PropertyGrid.h"
#include "GradientItem.h"
#include "SerialItem.h"
#include "RectItem.h"
#include "TreeItem.h"

// CPropGridDlg dialog
class CPropGridDlg : public CDialog
{
  // Construction
public:
  CPropGridDlg(CWnd* pParent = NULL);	// standard constructor

  // Dialog Data
  enum { IDD = IDD_PROPGRID_DIALOG };

protected:
  virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

protected:
  CGradientItem m_item_gradient;
  CSerialItem m_item_serial;
  CRectItem m_item_rect;
  CTreeItem m_item_tree;

  // Implementation
protected:
  HICON m_hIcon;
  HITEM m_hItemLines;
  HITEM m_hItemShade;
  HITEM m_hItemGutter;
  HITEM m_hItemFocusDis;

  // Generated message map functions
  virtual BOOL OnInitDialog();
  afx_msg void OnPaint();
  afx_msg HCURSOR OnQueryDragIcon();
  DECLARE_MESSAGE_MAP()

public:
  CPropertyGrid m_ctrlGrid;
  afx_msg LRESULT OnItemChanged(WPARAM, LPARAM);
  afx_msg void OnBnClickedOk();
  afx_msg void OnBnClickedCollapseAll();
  afx_msg void OnBnClickedExpandAll();
  afx_msg void OnBnClickedCustomColors();
  afx_msg void OnBnClickedCategorized();
  afx_msg void OnBnClickedAlphabetical();
  afx_msg void OnBnClickedNosort();
};
