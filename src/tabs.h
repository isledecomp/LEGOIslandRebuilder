#ifndef CREBUILDERTABS_H
#define CREBUILDERTABS_H

#include <AFXCMN.H>
#include <AFXWIN.H>
#include <VECTOR>

#include "clinkstatic.h"
#include "patchgrid.h"

class TabCtrl : public CTabCtrl
{
public:
  enum {
    ID_PATCHGRID = 1000,
    ID_PATCHTITLE,
    ID_PATCHDESC,
    ID_MUSICLINK,
    ID_COUNT
  };

  void CreateChildren();

  afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

  afx_msg LRESULT OnGridSelChange(WPARAM, LPARAM);

  afx_msg void OnSize(UINT type, int width, int height);

  afx_msg void OnMusicLinkClick();

  PatchGrid &GetPatchGrid() { return m_cPatchGrid; }
  CStatic &GetPatchTitle() { return m_cPatchTitle; }
  CStatic &GetPatchDesc() { return m_cPatchDesc; }
  CStatic &GetMusicLink() { return m_cMusicLink; }
  
private:
  PatchGrid m_cPatchGrid;
  CStatic m_cPatchTitle;
  CStatic m_cPatchDesc;

  CLinkStatic m_cMusicLink;

  DECLARE_MESSAGE_MAP()

};

#endif // CREBUILDERWINDOW_H
