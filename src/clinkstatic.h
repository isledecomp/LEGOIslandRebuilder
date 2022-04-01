#ifndef CLINKSTATIC_H
#define CLINKSTATIC_H

#include <AFXWIN.H>

class CLinkStatic : public CStatic
{
public:
  CLinkStatic();

  afx_msg BOOL OnSetCursor(CWnd *pWnd, UINT nHitTest, UINT message);

  afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);

  afx_msg BOOL OnEraseBkgnd(CDC*);

private:
  HCURSOR m_hPointHand;

  DECLARE_MESSAGE_MAP()

};

#endif // CLINKSTATIC_H
