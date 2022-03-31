#if !defined(AFX_DYNDIALOGEX_H__CF5AF5E8_BD00_11D3_AA7C_0008C7083CA9__INCLUDED_)
#define AFX_DYNDIALOGEX_H__CF5AF5E8_BD00_11D3_AA7C_0008C7083CA9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DynDialogEx.h : header file
//

#include <VECTOR>
#include "DynDialogItemEx.h"

// Control styles
#define	STYLE_EDIT							(WS_VISIBLE | WS_CHILD | WS_TABSTOP | SS_LEFT)
#define	STYLE_MULTIEDIT					(WS_VISIBLE | WS_CHILD | WS_TABSTOP | SS_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | WS_VSCROLL | ES_WANTRETURN)
#define	STYLE_STATIC						(WS_VISIBLE | WS_CHILD | SS_LEFT)
#define	STYLE_STATIC_CENTER				(WS_VISIBLE | WS_CHILD | SS_CENTER)
#define	STYLE_STATIC_RIGHT				(WS_VISIBLE | WS_CHILD | SS_RIGHT)
#define	STYLE_RADIO							(WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON)
#define	STYLE_RADIO_GROUP					(STYLE_RADIO| WS_GROUP | WS_TABSTOP)
#define	STYLE_BUTTON						(WS_VISIBLE | WS_CHILD | WS_TABSTOP | BS_PUSHBUTTON)
#define	STYLE_GROUPBOX						(WS_VISIBLE | WS_CHILD | BS_GROUPBOX)
#define	STYLE_CHECKBOX						(WS_VISIBLE | WS_CHILD | WS_TABSTOP | WS_GROUP | BS_AUTOCHECKBOX)
#define	STYLE_COMBOBOX_DROPDOWN			(WS_VISIBLE | WS_CHILD | WS_TABSTOP | WS_GROUP | WS_VSCROLL | CBS_DROPDOWN | CBS_SORT | CBS_AUTOHSCROLL) // | CBS_DISABLENOSCROLL)
#define	STYLE_COMBOBOX_DROPDOWN_NOSORT (WS_VISIBLE | WS_CHILD | WS_TABSTOP | WS_GROUP | WS_VSCROLL | CBS_DROPDOWN | CBS_AUTOHSCROLL) // | CBS_DISABLENOSCROLL)
#define	STYLE_COMBOBOX_SIMPLE			(WS_VISIBLE | WS_CHILD | WS_TABSTOP | WS_GROUP | WS_VSCROLL | CBS_SIMPLE | CBS_SORT | CBS_AUTOHSCROLL) // | CBS_DISABLENOSCROLL)
#define	STYLE_COMBOBOX_SIMPLE_NOSORT	(WS_VISIBLE | WS_CHILD | WS_TABSTOP | WS_GROUP | WS_VSCROLL | CBS_SIMPLE | CBS_AUTOHSCROLL) // | CBS_DISABLENOSCROLL)
#define	STYLE_COMBOBOX_DROPDOWNLIST	(WS_VISIBLE | WS_CHILD | WS_TABSTOP | WS_GROUP | WS_VSCROLL | CBS_DROPDOWNLIST | CBS_SORT | CBS_AUTOHSCROLL) // | CBS_DISABLENOSCROLL)
#define	STYLE_COMBOBOX_DROPDOWNLIST_NOSORT	(WS_VISIBLE | WS_CHILD | WS_TABSTOP | WS_GROUP | WS_VSCROLL | CBS_DROPDOWNLIST | CBS_AUTOHSCROLL) // | CBS_DISABLENOSCROLL)
#define  STYLE_LISTBOX						(WS_VISIBLE | WS_CHILD | WS_TABSTOP | LBS_STANDARD)

#define STYLE_DATETIMEPICKER				(WS_VISIBLE | WS_CHILD | WS_TABSTOP | DTS_RIGHTALIGN)
#define STYLE_TIMEPICKER					(WS_VISIBLE | WS_CHILD | WS_TABSTOP | DTS_RIGHTALIGN | DTS_TIMEFORMAT)
// Default combo-style
#define	STYLE_COMBOBOX						(STYLE_COMBOBOX_DROPDOWNLIST)
#define	STYLE_COMBOBOX_NOSORT			(STYLE_COMBOBOX_DROPDOWNLIST_NOSORT)

// Control Extended styles
#define	EXSTYLE_EDIT						(WS_EX_CLIENTEDGE)
#define	EXSTYLE_MULTIEDIT					(WS_EX_CLIENTEDGE)
#define	EXSTYLE_LISTBOX					(WS_EX_CLIENTEDGE)
#define	EXSTYLE_STATIC						(0)
#define	EXSTYLE_RADIO						(0)
#define	EXSTYLE_BUTTON						(0)
#define	EXSTYLE_GROUPBOX					(0)
#define	EXSTYLE_CHECKBOX					(0)
#define	EXSTYLE_COMBOBOX					(0)
#define  EXSTYLE_DATETIMEPICKER			(0)
#define  EXSTYLE_TIMEPICKER				(0)

#define ROWSTEPSIZE	 12
#define FIRSTROW1		 10
#define FIRSTROW2		 37
#define FIXEDCOL1		 10
#define FIXEDCOL2		120
#define INPUTCOL		150
#define GROWLIMIT		  6

#define MAX_COLS_PER_DESCR		25						// Just a number..

/////////////////////////////////////////////////////////////////////////////
// CDynDialogEx dialog

class CDynDialogEx : public CDialog
{
  // Construction
public:
  CDynDialogEx(CWnd* pParent = NULL);   // standard constructor
  ~CDynDialogEx();

  virtual CWnd *GetParent();

  // Dialog Data
  //{{AFX_DATA(CDynDialogEx)
  // NOTE: the ClassWizard will add data members here
  //}}AFX_DATA

  virtual UINT AddDlgControl(DLGITEMTEMPLATECONTROLS TypeControl,
    LPCTSTR lpszCaption,
    DWORD dwStyle,
    DWORD dwExtendedStyle,
    LPRECT pRect = NULL,
    void *pData = NULL,
    UINT nID = 0);
  virtual UINT AddDlgControl(LPCSTR lpszClassName,
    LPCTSTR lpszCaption,
    DWORD dwStyle,
    DWORD dwExtendedStyle,
    LPRECT pRect = NULL,
    void *pData = NULL,
    UINT nID = 0);
  virtual UINT AddSubclassedDlgControl(LPCSTR lpszClassName,
    LPCTSTR lpszCaption,
    DWORD dwStyle,
    DWORD dwExtendedStyle,
    LPRECT pRect = NULL,
    UINT nID = 0);
  void SetWindowTitle(LPCSTR lpszCaption);
  void SetFont(CFont *pFont);
  CFont *GetFont();
  void SetFontSize(WORD wSize);
  WORD GetFontSize();
  void SetUseSystemButtons(BOOL bUse = TRUE);
  void SetUseModeless(BOOL bModelessDlg = TRUE);
  long GetNumberOfConrols() {return long(m_arrDlgItemPtr.size());}

  //Additional functions by Tom Daffin
  void AddStyles(DWORD dwStyles);
  void RemoveStyles(DWORD dwStyles);

  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CDynDialogEx)
public:
  virtual int DoModal();
protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual void OnCancel();
  virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
  virtual void OnOK();
  //}}AFX_VIRTUAL
  virtual void OnHelp();										// To be overridden..

protected:
  CWnd *m_pParentWnd;
  CString m_strCaption;
  CFont *m_pFont;
  WORD m_wFontSize;
  long m_nCurRow;
  BOOL m_bAddSystemButtons;
  BOOL m_bIsFontCreated;
  BOOL m_bModelessDlg;

  DLGTEMPLATE m_DialogTemplate;

  // DOWNGRADE: Switch from CArray to std::vector because MFC 4.2 doesn't have CArray
  std::vector<CDynDialogItemEx*>	m_arrDlgItemPtr;

  // Implementation
protected:
  void AddSystemButtons();
  void GetDlgRect(LPRECT lpRect);
  void SetDlgRect(LPRECT lpRect);
  void SetDlgRectangle(LPRECT pRect);


  // Generated message map functions
  //{{AFX_MSG(CDynDialogEx)
  virtual BOOL OnInitDialog();
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  //}}AFX_MSG
  afx_msg LRESULT OnHelpMsg(WPARAM wParam, LPARAM lParam);

  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DYNDIALOGEX_H__CF5AF5E8_BD00_11D3_AA7C_0008C7083CA9__INCLUDED_)
