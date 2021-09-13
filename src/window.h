#ifndef CREBUILDERWINDOW_H
#define CREBUILDERWINDOW_H

#include <AFXCMN.H>
#include <AFXWIN.H>
#include <VECTOR>

class CRebuilderWindow : public CFrameWnd
{
public:
  CRebuilderWindow();

  afx_msg void OnRunClick();

  afx_msg void OnKillClick();

  afx_msg void OnSize(UINT type, int width, int height);

  afx_msg void OnGetMinMaxInfo(MINMAXINFO *info);

  static BOOL CALLBACK SetFont(HWND child, LPARAM font);

private:
  void SetGUIFonts();

  enum {
    ID_RUN = 1000,
    ID_KILL,
    ID_TABCTRL
  };

  UINT m_nFontHeight;

  CStatic m_cTopLevelTitle;
  CStatic m_cTopLevelSubtitle;

  CTabCtrl m_cTabCtrl;

  CButton m_cRunBtn;
  CButton m_cKillBtn;

  std::vector<HANDLE> m_lProcesses;

  DECLARE_MESSAGE_MAP()

};

#endif // CREBUILDERWINDOW_H
