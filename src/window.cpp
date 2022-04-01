#include "window.h"

#include <WINDOWS.H>

#include "../cmn/path.h"
#include "launcher.h"
#include "../res/resource.h"

#define super CWnd

static const int WM_CHILD_CLOSED = WM_USER + 1;

CString GetResourceString(UINT id)
{
  CString cstr;
  cstr.LoadString(id);
  return cstr;
}

CRebuilderWindow::CRebuilderWindow()
{
  // Register custom window class
  LPCTSTR wndclass = AfxRegisterWndClass(CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW,
                                         LoadCursor(NULL, IDC_ARROW),
                                         (HBRUSH) COLOR_WINDOW,
                                         LoadIcon(AfxGetInstanceHandle(), "IDI_ICON1"));

  const char *title = "LEGO Island Rebuilder";

  // Create form
  CreateEx(WS_EX_OVERLAPPEDWINDOW, wndclass, title, WS_OVERLAPPEDWINDOW, 0, 0, 0, 0, NULL, NULL);
  ModifyStyleEx(WS_EX_CLIENTEDGE, 0, 0);

  // Create title
  m_cTopLevelTitle.Create(title, WS_CHILD | WS_VISIBLE | SS_CENTER, CRect(), this);

  // Create subtitle
  m_cTopLevelSubtitle.Create("by MattKC and Ramen2X", WS_CHILD | WS_VISIBLE | SS_CENTER | SS_NOTIFY, CRect(), this, ID_SUBTITLE);

  // Initialize common TCITEM
  TCITEM tabItem;
  ZeroMemory(&tabItem, sizeof(tabItem));
  tabItem.mask |= TCIF_TEXT;
  tabItem.pszText = new TCHAR[100];

  delete [] tabItem.pszText;

  // Create property grid
  m_cPatchGrid.Create(AfxRegisterWndClass(CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW), "Patch Grid", WS_CHILD | WS_VISIBLE, CRect(), this, ID_PATCHGRID);
  m_cPatchTitle.Create(CString(), WS_CHILD | WS_VISIBLE, CRect(), this, ID_PATCHTITLE);
  m_cPatchDesc.Create(CString(), WS_CHILD | WS_VISIBLE, CRect(), this, ID_PATCHDESC);

  // Create run button
  m_cRunBtn.Create("Run", WS_CHILD | WS_VISIBLE, CRect(), this, ID_RUN);

  // Create run button
  m_cKillBtn.Create("Kill", WS_CHILD, CRect(), this, ID_KILL);

  // Set fonts
  SetGUIFonts();

  // Call this after all UI objects are created because this will call LayoutObjects
  static const UINT defaultWindowWidth = 32;
  static const UINT defaultWindowHeight = 32;
  SetWindowPos(NULL, 0, 0, m_nFontHeight * defaultWindowWidth, m_nFontHeight * defaultWindowHeight, 0);
  CenterWindow(NULL);

  // Set column width
  RECT patchGridClientRect;
  m_cPatchGrid.GetClientRect(&patchGridClientRect);
  m_cPatchGrid.SetGutterWidth((patchGridClientRect.right - patchGridClientRect.left) / 2);

  TCHAR configPath[MAX_PATH];
  if (GetConfigFilename(configPath)) {
    m_cPatchGrid.LoadConfiguration(configPath);
  }
}

CRebuilderWindow::~CRebuilderWindow()
{
  TrySaving();
  OnKillClick();
}

DWORD WINAPI WaitForProcessToClose(HANDLE hProcess)
{
  WaitForSingleObject(hProcess, INFINITE);
  AfxGetMainWnd()->PostMessage(WM_CHILD_CLOSED, (WPARAM)hProcess);

  return 0;
}

BOOL CRebuilderWindow::TrySaving()
{
  TCHAR configPath[MAX_PATH];

  if (GetConfigFilename(configPath)) {
    if (m_cPatchGrid.SaveConfiguration(configPath)) {
      return TRUE;
    } else {
      MessageBox("Failed to save configuration file.");
    }
  } else {
    MessageBox("Failed to determine configuration path.");
  }

  return FALSE;
}

void CRebuilderWindow::OnRunClick()
{
  if (TrySaving()) {
    if (HANDLE proc = Launcher::Launch(this->GetSafeHwnd())) {
      m_lProcesses.push_back(proc);
      SwitchButtonMode(TRUE);

      // Register callback when process exits
      DWORD threadId; // We don't use this, but Windows 95 will fail without it
      CloseHandle(CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)WaitForProcessToClose, proc, 0, &threadId));
    }
  }
}

void CRebuilderWindow::OnKillClick()
{
  for (std::vector<HANDLE>::iterator it=m_lProcesses.begin(); it!=m_lProcesses.end(); it++) {
    TerminateProcess(*it, 0);
  }
  m_lProcesses.clear();

  SwitchButtonMode(FALSE);
}

void CRebuilderWindow::OnSubtitleClick()
{
  ShellExecute(NULL, "open", "https://legoisland.org/", NULL, NULL, SW_SHOWNORMAL);
}

void CRebuilderWindow::OnSize(UINT type, int width, int height)
{
  const int padding = m_nFontHeight/2;
  const int dblPadding = padding * 2;

  int y, h;

  // Top labels
  y = padding;
  h = m_nFontHeight*2;

  m_cTopLevelTitle.SetWindowPos(NULL, 0, y, width, h, 0);

  y += h;
  h = m_nFontHeight*1;

  m_cTopLevelSubtitle.SetWindowPos(NULL, 0, y, width, h, 0);

  y += h;
  y += padding;

  // Bottom buttons
  const int btnHeight = m_nFontHeight*18/10;
  int btnY = height - btnHeight - padding;
  int btnWidth = width - dblPadding;
  m_cRunBtn.SetWindowPos(NULL, padding, btnY, btnWidth, btnHeight, 0);
  m_cKillBtn.SetWindowPos(NULL, padding, btnY, btnWidth, btnHeight, 0);

  h = m_nFontHeight*4;
  int patchDescY = btnY - h;
  m_cPatchDesc.SetWindowPos(NULL, padding, patchDescY, btnWidth, h, 0);

  h = m_nFontHeight;
  patchDescY -= h;
  m_cPatchTitle.SetWindowPos(NULL, padding, patchDescY, btnWidth, h, 0);

  // Consume remaining space with patch grid
  m_cPatchGrid.SetWindowPos(NULL, padding, y, btnWidth, patchDescY - y - padding, 0);
}

void CRebuilderWindow::OnGetMinMaxInfo(MINMAXINFO *info)
{
  static const LONG minimumWindowWidth = 12;
  static const LONG minimumWindowHeight = 12;

  info->ptMinTrackSize.x = m_nFontHeight * minimumWindowWidth;
  info->ptMinTrackSize.y = m_nFontHeight * minimumWindowHeight;
}

LRESULT CRebuilderWindow::OnGridSelChange(WPARAM wParam, LPARAM lParam)
{
  HITEM hItem = (HITEM) wParam;

  m_cPatchTitle.SetWindowText(m_cPatchGrid.GetItemText(hItem).c_str());
  m_cPatchDesc.SetWindowText(m_cPatchGrid.GetItemDescription(hItem));
  return 0;
}

BOOL CRebuilderWindow::SetFont(HWND child, LPARAM font)
{
  ::SendMessage(child, WM_SETFONT, font, true);
  return true;
}

LRESULT CRebuilderWindow::WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  if (uMsg == WM_CHILD_CLOSED) {
    HANDLE hProcess = (HANDLE)wParam;

    for (std::vector<HANDLE>::iterator it=m_lProcesses.begin(); it!=m_lProcesses.end(); it++) {
      if (*it == hProcess) {
        m_lProcesses.erase(it);
        break;
      }
    }

    if (m_lProcesses.empty()) {
      SwitchButtonMode(FALSE);
    }

    return 0;
  } else {
    return super::WindowProc(uMsg, wParam, lParam);
  }
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
  SetFont(m_cPatchTitle.GetSafeHwnd(), (LPARAM)bold);

  // Create link variant for subtitle
  lf.lfWeight = FW_NORMAL;
  lf.lfUnderline = TRUE;

  HFONT link = CreateFontIndirect(&lf);
  SetFont(m_cTopLevelSubtitle.GetSafeHwnd(), (LPARAM)link);

  // While here, get height of font for layout purposes
  HDC hDC = ::GetDC(NULL);
  HGDIOBJ hFontOld = SelectObject(hDC, defaultFont);
  TEXTMETRIC tm;
  GetTextMetrics(hDC, &tm);
  m_nFontHeight = tm.tmHeight + tm.tmExternalLeading;
  SelectObject(hDC, hFontOld);
  ::ReleaseDC(NULL, hDC);
}

void CRebuilderWindow::SwitchButtonMode(BOOL running)
{
  if (running) {
    m_cRunBtn.ShowWindow(SW_HIDE);
    m_cKillBtn.ShowWindow(SW_SHOWNORMAL);
  } else {
    m_cKillBtn.ShowWindow(SW_HIDE);
    m_cRunBtn.ShowWindow(SW_SHOWNORMAL);
  }
}

BEGIN_MESSAGE_MAP(CRebuilderWindow, super)
  ON_WM_SIZE()
  ON_WM_GETMINMAXINFO()
  ON_BN_CLICKED(ID_RUN, OnRunClick)
  ON_BN_CLICKED(ID_KILL, OnKillClick)
  ON_BN_CLICKED(ID_SUBTITLE, OnSubtitleClick)
  ON_MESSAGE(WM_PG_SELECTIONCHANGED, OnGridSelChange)
END_MESSAGE_MAP()
