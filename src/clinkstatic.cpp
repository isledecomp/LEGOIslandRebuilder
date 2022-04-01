#include "clinkstatic.h"

#define super CStatic

#ifndef IDC_HAND
#define IDC_HAND MAKEINTRESOURCE(32649)
#endif

CLinkStatic::CLinkStatic()
{
  // We want a pointing hand cursor and different Windows versions provide this differently
  OSVERSIONINFO info;
  ZeroMemory(&info, sizeof(info));
  info.dwOSVersionInfoSize = sizeof(info);
  GetVersionEx(&info);

  if (info.dwMajorVersion <= 4) {
    // Windows 9x didn't have a pointing hand cursor so we'll pull it from winhlp32
    CString strWndDir;
    GetWindowsDirectory(strWndDir.GetBuffer(MAX_PATH), MAX_PATH);
    strWndDir.ReleaseBuffer();

    strWndDir += _T("\\winhlp32.exe");
    HMODULE hModule = LoadLibrary(strWndDir);
    if (hModule) {
      HCURSOR hHandCursor = ::LoadCursor(hModule, MAKEINTRESOURCE(106));
      if (hHandCursor)
        m_hPointHand = CopyCursor(hHandCursor);
    }
    FreeLibrary(hModule);
  } else {
    // With NT5+, we can use IDC_HAND
    m_hPointHand = LoadCursor(NULL, IDC_HAND);
  }
}

BOOL CLinkStatic::OnSetCursor(CWnd *pWnd, UINT nHitTest, UINT message)
{
  ::SetCursor(m_hPointHand);
  return TRUE;
}

HBRUSH CLinkStatic::CtlColor(CDC *pDC, UINT nCtlColor)
{
  pDC->SetTextColor(RGB(0, 0, 240));
  pDC->SetBkMode(TRANSPARENT);

  return (HBRUSH) GetStockObject(NULL_BRUSH);
}

BOOL CLinkStatic::OnEraseBkgnd(CDC*)
{
  return TRUE;
}

BEGIN_MESSAGE_MAP(CLinkStatic, super)
  ON_WM_SETCURSOR()
  ON_WM_CTLCOLOR_REFLECT()
  ON_WM_ERASEBKGND()
END_MESSAGE_MAP()
