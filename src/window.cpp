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
  const int btnHeight = 25;

  int padding = m_nFontHeight/2;

  m_cTopLevelTitle.SetWindowPos(NULL, 0, padding, width, m_nFontHeight*2, 0);
  m_cTopLevelSubtitle.SetWindowPos(NULL, 0, padding+m_nFontHeight*2, width, m_nFontHeight, 0);

  m_cRunBtn.SetWindowPos(NULL, padding, height - btnHeight - padding, width - padding * 2, btnHeight, 0);
}
