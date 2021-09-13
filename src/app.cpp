#include "app.h"

#include "window.h"

CRebuilderApp g_AppInstance;

CRebuilderApp::CRebuilderApp()
{

}

BOOL CRebuilderApp::InitInstance()
{
  m_pMainWnd = new CRebuilderWindow();
  m_pMainWnd->ShowWindow(m_nCmdShow);
  m_pMainWnd->UpdateWindow();

  return TRUE;
}
