#ifndef CREBUILDERWINDOW_H
#define CREBUILDERWINDOW_H

#include <AFXCMN.H>
#include <AFXWIN.H>
#include <VECTOR>

#include "clinkstatic.h"
#include "patchgrid.h"
#include "tabs.h"

class CRebuilderWindow : public CFrameWnd
{
public:
  CRebuilderWindow();

  ~CRebuilderWindow();

  afx_msg void OnRunClick();

  afx_msg void OnKillClick();

  afx_msg void OnSubtitleClick();

  afx_msg void OnSize(UINT type, int width, int height);

  afx_msg void OnGetMinMaxInfo(MINMAXINFO *info);

  afx_msg void OnTabSelChange(NMHDR* pNMHDR, LRESULT* pResult);

  static BOOL CALLBACK SetFont(HWND child, LPARAM font);

  virtual LRESULT WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
  void SetGUIFonts();

  void SwitchButtonMode(BOOL running);

  BOOL TrySaving();

  enum {
    ID_RUN = 1000,
    ID_KILL,
    ID_TABCTRL,
    ID_SUBTITLE,
    ID_COUNT
  };

  enum Tab {
    TAB_PATCHES,
    TAB_MUSIC
  };

  UINT m_nFontHeight;

  CStatic m_cTopLevelTitle;
  CLinkStatic m_cTopLevelSubtitle;

  TabCtrl m_cTabCtrl;

  CButton m_cRunBtn;
  CButton m_cKillBtn;

  std::vector<HANDLE> m_lProcesses;

  DECLARE_MESSAGE_MAP()

};

#endif // CREBUILDERWINDOW_H
