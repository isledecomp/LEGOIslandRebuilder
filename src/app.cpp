#include "app.h"

#include "launcher.h"
#include "window.h"

CRebuilderApp g_AppInstance;

CRebuilderApp::CRebuilderApp()
{
}

BOOL CRebuilderApp::InitInstance()
{
  bool run_only = false;

  char *tokens = strtok(m_lpCmdLine, " ");
  while (tokens) {
    if (!strcmp(tokens, "--run") || !strcmp(tokens, "-r")) {
      run_only = true;
    } else if (!strcmp(tokens, "--help") || !strcmp(tokens, "-h")) {
      ::MessageBoxA(0, "LEGO Island Rebuilder\n"
                       "\n"
                       "Supported arguments:\n"
                       "\n"
                       "--help, -h\n"
                       "Displays this help page.\n"
                       "\n"
                       "--run, -r\n"
                       "Run LEGO Island immediately with the last used configuration.",
                    "Command Line Argument Help", 0);
      return false;
    }

    tokens = strtok(NULL, " ");
  }

  if (!run_only) {
    m_pMainWnd = new CRebuilderWindow();
    m_pMainWnd->ShowWindow(m_nCmdShow);
    m_pMainWnd->UpdateWindow();

    return TRUE;
  } else {
    Launcher::Launch(NULL);
    return FALSE;
  }
}
