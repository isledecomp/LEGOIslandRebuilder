#include "window.h"

#define super CFrameWnd

CRebuilderWindow::CRebuilderWindow()
{
  // Declare default width/height
  static const UINT defaultWindowWidth = 420;
  static const UINT defaultWindowHeight = 420;

  // Create form
  Create(NULL, _T("LEGO Island Rebuilder"));

  // Set window icon to application icon
  TCHAR filename[MAX_PATH];
  GetModuleFileName(NULL, filename, MAX_PATH);
  WORD index;
  HICON icon = ExtractAssociatedIcon(AfxGetInstanceHandle(), filename, &index);
  SetIcon(icon, TRUE);

  // Get default Win32 dialog font
  m_fDialogFont.CreateStockObject(DEFAULT_GUI_FONT);

  // Create bolded variant of the above
  LOGFONT lf;
  m_fDialogFont.GetLogFont(&lf);
  lf.lfWeight = FW_BOLD;
  m_fBoldDialogFont.CreateFontIndirect(&lf);

  // Get information about font height for layout
  HDC hDC = ::GetDC(NULL);
  HGDIOBJ hFontOld = SelectObject(hDC, m_fDialogFont.GetSafeHandle());
  TEXTMETRIC tm;
  GetTextMetrics(hDC, &tm);
  m_nFontHeight = tm.tmHeight + tm.tmExternalLeading;
  SelectObject(hDC, hFontOld);
  ::ReleaseDC(NULL, hDC);

  // Create title
  m_cTopLevelTitle.Create(_T("LEGO Island Rebuilder"), WS_CHILD | WS_VISIBLE | SS_CENTER, CRect(), this);
  m_cTopLevelTitle.SetFont(&m_fBoldDialogFont);

  // Create subtitle
  m_cTopLevelSubtitle.Create(_T("by MattKC (itsmattkc.com)"), WS_CHILD | WS_VISIBLE | SS_CENTER, CRect(), this);
  m_cTopLevelSubtitle.SetFont(&m_fDialogFont);

  // Create tab control
  m_cTabCtrl.Create(WS_CHILD | WS_VISIBLE, CRect(), this, IDI_TABCTRL);
  m_cTabCtrl.SetFont(&m_fDialogFont);

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
  m_cRunBtn.Create(_T("Run"), WS_CHILD | WS_VISIBLE, CRect(), this, IDI_RUN);
  m_cRunBtn.SetFont(&m_fBoldDialogFont);

  // Call this after all UI objects are created because this will call LayoutObjects
  SetWindowPos(NULL, 0, 0, defaultWindowWidth, defaultWindowHeight, 0);
  CenterWindow(NULL);

  // Set background color to Win32 window color
  ::SetClassLong(GetSafeHwnd(), GCL_HBRBACKGROUND, COLOR_WINDOW);
}

LRESULT CRebuilderWindow::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
  switch (message) {
  case WM_SIZE:
  {
    UINT width = LOWORD(lParam);
    UINT height = HIWORD(lParam);

    LayoutObjects(width, height);
    break;
  }
  case WM_GETMINMAXINFO:
  {
    static const LONG minimumWindowWidth = 160;
    static const LONG minimumWindowHeight = 160;

    MINMAXINFO *minmaxInfo = (MINMAXINFO*)lParam;

    minmaxInfo->ptMinTrackSize.x = minimumWindowWidth;
    minmaxInfo->ptMinTrackSize.y = minimumWindowHeight;

    return 0;
  }
  }

  return super::WindowProc(message, wParam, lParam);
}

void CRebuilderWindow::LayoutObjects(UINT width, UINT height)
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

  // Center components
  int centerHeight = bottomComponentStart - topComponentEnd;
  m_cTabCtrl.SetWindowPos(NULL, padding, topComponentEnd + padding, bottomComponentWidth, centerHeight - dblPadding, 0);
}
