// PropGridDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PropGrid.h"
#include "PropGridDlg.h"
#include ".\propgriddlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPropGridDlg dialog



CPropGridDlg::CPropGridDlg(CWnd* pParent /*=NULL*/)
: CDialog(CPropGridDlg::IDD, pParent)
{
  m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPropGridDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_GRID, m_ctrlGrid);
}

BEGIN_MESSAGE_MAP(CPropGridDlg, CDialog)
  ON_WM_PAINT()
  ON_WM_QUERYDRAGICON()
  ON_BN_CLICKED(IDOK, OnBnClickedOk)
  ON_MESSAGE(WM_PG_ITEMCHANGED, OnItemChanged)
  ON_BN_CLICKED(IDC_COLLAPSE_ALL, OnBnClickedCollapseAll)
  ON_BN_CLICKED(IDC_EXPAND_ALL, OnBnClickedExpandAll)
  ON_BN_CLICKED(IDC_CUSTOM_COLORS, OnBnClickedCustomColors)
  ON_BN_CLICKED(IDC_CATEGORIZED, OnBnClickedCategorized)
  ON_BN_CLICKED(IDC_ALPHABETICAL, OnBnClickedAlphabetical)
  ON_BN_CLICKED(IDC_NOSORT, OnBnClickedNosort)
END_MESSAGE_MAP()


// CPropGridDlg message handlers

BOOL CPropGridDlg::OnInitDialog()
{
  CDialog::OnInitDialog();

  // Set the icon for this dialog.  The framework does this automatically
  //  when the application's main window is not a dialog
  SetIcon(m_hIcon, TRUE);			// Set big icon
  SetIcon(m_hIcon, FALSE);		// Set small icon

  // basic items
  HSECTION hs = m_ctrlGrid.AddSection("Basic Items");
  m_ctrlGrid.AddStringItem(hs, "String", "A single line string item");
  m_ctrlGrid.AddTextItem(hs, "Text", "A multi line text item.\r\nSecond line...");
  m_ctrlGrid.AddStringItem(hs, "Disabled item", "A disabled item", false);
  m_ctrlGrid.AddIntegerItem(hs, "Integer", 10);
  m_ctrlGrid.AddIntegerItem(hs, "Formatted Integer", 8, "%d inches");
  m_ctrlGrid.AddDoubleItem(hs, "Double", 7.33);
  m_ctrlGrid.AddDoubleItem(hs, "Formatted Double", 10.25, "%gmm");
  m_ctrlGrid.AddBoolItem(hs, "Boolean", true);

  // custom items
  hs = m_ctrlGrid.AddSection("Custom Items");
  m_ctrlGrid.AddCustomItem(hs, "Custom In-place Edit", &m_item_serial);
  m_ctrlGrid.AddCustomItem(hs, "Custom Modal Edit", &m_item_rect);
  m_ctrlGrid.AddCustomItem(hs, "Ccustom Dropdown Edit", &m_item_tree);
  m_ctrlGrid.AddCustomItem(hs, "Full Custom Edit", &m_item_gradient);

  // more items inserted before the custom ones
  hs = m_ctrlGrid.AddSection("More Items", true, hs);

  // a combo
  vector<string> v;
  v.push_back("English");
  v.push_back("French");
  v.push_back("German");
  v.push_back("Spanish");
  m_ctrlGrid.AddComboItem(hs, "Combo", v, 2, true);

  // some "simple"    
  m_ctrlGrid.AddDateItem(hs, "Date", COleDateTime::GetCurrentTime());
  m_ctrlGrid.AddDateItem(hs, "Formatted Date", COleDateTime::GetCurrentTime(), "%A, %d %B %Y");
  m_ctrlGrid.AddDateTimeItem(hs, "Date Time", COleDateTime::GetCurrentTime());
  m_ctrlGrid.AddColorItem(hs, "Color", RGB(255,156,12));
  m_ctrlGrid.AddFileItem(hs, "File", "C:\\AUTOEXEC.BAT", "All Files (*.*)|*.*||");
  m_ctrlGrid.AddFolderItem(hs, "Folder", "C:\\", "Select a folder");

  // a font
  LOGFONT lf;
  CFont font;
  font.CreatePointFont(80, "Tahoma");
  font.GetLogFont(&lf);
  m_ctrlGrid.AddFontItem(hs, "Font", lf, true);

  // customization
  hs = m_ctrlGrid.AddSection("Grid customization");
  m_hItemShade = m_ctrlGrid.AddBoolItem(hs, "Shade titles", m_ctrlGrid.GetShadeTitles());
  m_hItemLines = m_ctrlGrid.AddBoolItem(hs, "Draw lines", m_ctrlGrid.GetDrawLines());
  m_hItemGutter = m_ctrlGrid.AddBoolItem(hs, "Draw gutter", m_ctrlGrid.GetDrawGutter());
  m_hItemFocusDis = m_ctrlGrid.AddBoolItem(hs, "Focus disabled items", m_ctrlGrid.GetFocusDisabled());

  // stuff
  m_ctrlGrid.SetOkCancelStrings("OK", "Annuler");
  m_ctrlGrid.SetDateTimeStrings("Date", "Heure");

  // 
  ((CButton*)GetDlgItem(IDC_CATEGORIZED))->SetCheck(1);

  return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CPropGridDlg::OnPaint() 
{
  if (IsIconic())
  {
    CPaintDC dc(this); // device context for painting

    SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

    // Center icon in client rectangle
    int cxIcon = GetSystemMetrics(SM_CXICON);
    int cyIcon = GetSystemMetrics(SM_CYICON);
    CRect rect;
    GetClientRect(&rect);
    int x = (rect.Width() - cxIcon + 1) / 2;
    int y = (rect.Height() - cyIcon + 1) / 2;

    // Draw the icon
    dc.DrawIcon(x, y, m_hIcon);
  }
  else
  {
    CDialog::OnPaint();
  }
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CPropGridDlg::OnQueryDragIcon()
{
  return static_cast<HCURSOR>(m_hIcon);
}

void CPropGridDlg::OnBnClickedOk()
{
  //string strValue;
  //if (m_ctrlGrid.GetItemValue(m_hItem, strValue))
  //    AfxMessageBox(strValue.c_str());
  OnOK();
}

LRESULT CPropGridDlg::OnItemChanged(WPARAM wParam, LPARAM lParam)
{
  if (wParam == m_hItemLines)
  {
    bool draw_lines;
    m_ctrlGrid.GetItemValue(m_hItemLines, draw_lines);
    m_ctrlGrid.SetDrawLines(draw_lines);
  }
  else if (wParam == m_hItemShade)
  {
    bool shade_titles;
    m_ctrlGrid.GetItemValue(m_hItemShade, shade_titles);
    m_ctrlGrid.SetShadeTitles(shade_titles);
  }
  else if (wParam == m_hItemGutter)
  {
    bool draw_gutter;
    m_ctrlGrid.GetItemValue(m_hItemGutter, draw_gutter);
    m_ctrlGrid.SetDrawGutter(draw_gutter);
  }
  else if (wParam == m_hItemFocusDis)
  {
    bool focus_disabled;
    m_ctrlGrid.GetItemValue(m_hItemFocusDis, focus_disabled);
    m_ctrlGrid.SetFocusDisabled(focus_disabled);
  }

  return 0;
}

void CPropGridDlg::OnBnClickedCollapseAll()
{
  m_ctrlGrid.ExpandAll(false);
}

void CPropGridDlg::OnBnClickedExpandAll()
{
  m_ctrlGrid.ExpandAll(true);
}

void CPropGridDlg::OnBnClickedCustomColors()
{
  CButton* pBtn = (CButton*) GetDlgItem(IDC_CUSTOM_COLORS);
  if (pBtn->GetCheck() == 0)
  {
    m_ctrlGrid.SetBackColor(GetSysColor(COLOR_WINDOW));
    m_ctrlGrid.SetShadeColor(GetSysColor(COLOR_3DFACE));
    m_ctrlGrid.SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
    m_ctrlGrid.SetTitleColor(GetSysColor(COLOR_WINDOWTEXT));
  }
  else
  {
    m_ctrlGrid.SetBackColor(RGB(0xFF, 0xFF, 0xE0));
    m_ctrlGrid.SetShadeColor(RGB(0,187,94));
    m_ctrlGrid.SetTextColor(RGB(0,0,192));
    m_ctrlGrid.SetTitleColor(RGB(255,255,255));
  }
}

void CPropGridDlg::OnBnClickedCategorized()
{
  m_ctrlGrid.SetDisplayMode(CPropertyGrid::DM_CATEGORIZED);
  GetDlgItem(IDC_EXPAND_ALL)->EnableWindow(TRUE);
  GetDlgItem(IDC_COLLAPSE_ALL)->EnableWindow(TRUE);
}

void CPropGridDlg::OnBnClickedAlphabetical()
{
  m_ctrlGrid.SetDisplayMode(CPropertyGrid::DM_ALPHABETICAL);
  GetDlgItem(IDC_EXPAND_ALL)->EnableWindow(FALSE);
  GetDlgItem(IDC_COLLAPSE_ALL)->EnableWindow(FALSE);
}

void CPropGridDlg::OnBnClickedNosort()
{
  m_ctrlGrid.SetDisplayMode(CPropertyGrid::DM_NOSORT);
  GetDlgItem(IDC_EXPAND_ALL)->EnableWindow(FALSE);
  GetDlgItem(IDC_COLLAPSE_ALL)->EnableWindow(FALSE);
}
