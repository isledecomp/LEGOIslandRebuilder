#include "window.h"

#include "launcher.h"

#define super CWnd

CRebuilderWindow::CRebuilderWindow()
{
  // Declare default width/height
  static const UINT defaultWindowWidth = 420;
  static const UINT defaultWindowHeight = 420;

  // Register custom window class
  LPCTSTR wndclass = AfxRegisterWndClass(CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW,
                                         LoadCursor(NULL, IDC_ARROW),
                                         (HBRUSH) (COLOR_WINDOW),
                                         LoadIcon(AfxGetInstanceHandle(), _T("IDI_ICON1")));

  // Create form
  CreateEx(WS_EX_OVERLAPPEDWINDOW, wndclass, _T("LEGO Island Rebuilder"), WS_OVERLAPPEDWINDOW, 0, 0, 0, 0, NULL, NULL);
  ModifyStyleEx(WS_EX_CLIENTEDGE, 0, 0);

  // Create title
  m_cTopLevelTitle.Create(_T("LEGO Island Rebuilder"), WS_CHILD | WS_VISIBLE | SS_CENTER, CRect(), this);

  // Create subtitle
  m_cTopLevelSubtitle.Create(_T("by MattKC (itsmattkc.com)"), WS_CHILD | WS_VISIBLE | SS_CENTER, CRect(), this);

  // Create tab control
  m_cTabCtrl.Create(WS_CHILD | WS_VISIBLE, CRect(), this, ID_TABCTRL);

  // Add "patches" tab
  TCITEM patchesItem;
  ZeroMemory(&patchesItem, sizeof(patchesItem));
  patchesItem.pszText = _T("Patches");
  patchesItem.mask |= TCIF_TEXT;
  m_cTabCtrl.InsertItem(0, &patchesItem);

  // Add "music" tab
  TCITEM musicItem;
  ZeroMemory(&musicItem, sizeof(musicItem));
  musicItem.pszText = _T("Music");
  musicItem.mask |= TCIF_TEXT;
  m_cTabCtrl.InsertItem(1, &musicItem);

  // Create run button
  m_cRunBtn.Create(_T("Run"), WS_CHILD | WS_VISIBLE, CRect(), this, ID_RUN);

  // Create run button
  m_cKillBtn.Create(_T("Kill"), WS_CHILD, CRect(), this, ID_KILL);

  // Call this after all UI objects are created because this will call LayoutObjects
  SetWindowPos(NULL, 0, 0, defaultWindowWidth, defaultWindowHeight, 0);
  CenterWindow(NULL);

  // Set fonts
  SetGUIFonts();
}

void CRebuilderWindow::OnRunClick()
{
  HANDLE proc = Launcher::Launch(this->GetSafeHwnd());

  if (proc) {
    m_lProcesses.push_back(proc);
    m_cRunBtn.ShowWindow(SW_HIDE);
    m_cKillBtn.ShowWindow(SW_SHOWNORMAL);
  }
}

void CRebuilderWindow::OnKillClick()
{
  for (std::vector<HANDLE>::iterator it=m_lProcesses.begin(); it!=m_lProcesses.end(); it++) {
    TerminateProcess(*it, 0);
  }
  m_lProcesses.clear();

  m_cKillBtn.ShowWindow(SW_HIDE);
  m_cRunBtn.ShowWindow(SW_SHOWNORMAL);
}

void CRebuilderWindow::OnSize(UINT type, int width, int height)
{
  const int padding = m_nFontHeight/2;
  const int dblPadding = padding * 2;

  // Top components
  m_cTopLevelTitle.SetWindowPos(NULL, 0, padding, width, m_nFontHeight*2, 0);

  int topComponentEnd = padding+m_nFontHeight*2;
  m_cTopLevelSubtitle.SetWindowPos(NULL, 0, topComponentEnd, width, m_nFontHeight, 0);
  topComponentEnd += m_nFontHeight;

  // Bottom components
  const int btnHeight = 25;
  int bottomComponentStart = height - btnHeight - padding;
  int bottomComponentWidth = width - dblPadding;
  m_cRunBtn.SetWindowPos(NULL, padding, bottomComponentStart, bottomComponentWidth, btnHeight, 0);
  m_cKillBtn.SetWindowPos(NULL, padding, bottomComponentStart, bottomComponentWidth, btnHeight, 0);

  // Center components
  int centerHeight = bottomComponentStart - topComponentEnd;
  m_cTabCtrl.SetWindowPos(NULL, padding, topComponentEnd + padding, bottomComponentWidth, centerHeight - dblPadding, 0);
}

void CRebuilderWindow::OnGetMinMaxInfo(MINMAXINFO *info)
{
  static const LONG minimumWindowWidth = 160;
  static const LONG minimumWindowHeight = 160;

  info->ptMinTrackSize.x = minimumWindowWidth;
  info->ptMinTrackSize.y = minimumWindowHeight;
}

BOOL CRebuilderWindow::SetFont(HWND child, LPARAM font)
{
  ::SendMessage(child, WM_SETFONT, font, true);
  return true;
}

void CRebuilderWindow::SetGUIFonts()
{
  // Retrieve default GUI font
  HFONT defaultFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);

  // Set on all UI objects
  EnumChildWindows(this->GetSafeHwnd(), (WNDENUMPROC)SetFont, (LPARAM)defaultFont);

  // Get LOGFONT to create bold variant
  LOGFONT lf;
  GetObject(defaultFont, sizeof(lf), &lf);
  lf.lfWeight = FW_BOLD;

  // Create font from LOGFONT
  HFONT bold = CreateFontIndirect(&lf);

  // Set bold variant on relevant objects
  SetFont(m_cTopLevelTitle.GetSafeHwnd(), (LPARAM)bold);
  SetFont(m_cRunBtn.GetSafeHwnd(), (LPARAM)bold);
  SetFont(m_cKillBtn.GetSafeHwnd(), (LPARAM)bold);

  // While here, get height of font for layout purposes
  HDC hDC = ::GetDC(NULL);
  HGDIOBJ hFontOld = SelectObject(hDC, defaultFont);
  TEXTMETRIC tm;
  GetTextMetrics(hDC, &tm);
  m_nFontHeight = tm.tmHeight + tm.tmExternalLeading;
  SelectObject(hDC, hFontOld);
  ::ReleaseDC(NULL, hDC);
}

BEGIN_MESSAGE_MAP(CRebuilderWindow, super)
  ON_WM_SIZE()
  ON_WM_GETMINMAXINFO()
  ON_BN_CLICKED(ID_RUN, OnRunClick)
  ON_BN_CLICKED(ID_KILL, OnKillClick)
END_MESSAGE_MAP()
