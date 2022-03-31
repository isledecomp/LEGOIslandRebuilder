// DynDialogItemEx.cpp: implementation of the CDynDialogItemEx class.
//
// Written by Marcel Scherpenisse
//        mailto:Marcel_Scherpenisse@insad.nl
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed unmodified by any means PROVIDING it is
// not sold for profit without the authors written consent, and
// providing that this notice and the authors name and all copyright
// notices remains intact. If the source code in this file is used in
// any commercial application then a statement along the lines of
// "Portions copyright (c) Marcel Scherpenisse, 2002" must be included in
// the startup banner, "About" box or printed documentation. An email
// letting me know that you are using it would be nice as well.
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability for any damage/loss of business that
// this product may cause.
//
// Expect bugs!
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "DynDialogEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDynDialogEx dialog


CDynDialogEx::CDynDialogEx(CWnd* pParent /*=NULL*/)
: CDialog()
{
  //{{AFX_DATA_INIT(CDynDialogEx)
  // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT

  m_DialogTemplate.style				= WS_CAPTION | WS_SYSMENU | WS_VISIBLE | DS_SETFONT;
  m_DialogTemplate.dwExtendedStyle = WS_EX_DLGMODALFRAME;
  m_DialogTemplate.x					= 0;
  m_DialogTemplate.y					= 0;
  m_DialogTemplate.cx					= 0; // 4 horizontal units are the width of one character
  m_DialogTemplate.cy					= 0; // 8 vertical units are the height of one character
  m_DialogTemplate.cdit				= 0;  // nr of dialog items in the dialog

  m_pParentWnd	= pParent;
  m_strCaption	= _T("");
  m_pFont			= NULL;
  m_wFontSize		= 0;
  m_nCurRow		= FIRSTROW1;
  m_bAddSystemButtons = TRUE;

  m_bIsFontCreated = FALSE;
  m_bModelessDlg = FALSE;
}

CDynDialogEx::~CDynDialogEx()
{
  CDynDialogItemEx *pDynDialogItemEx = NULL;
  for (INT_PTR i = m_arrDlgItemPtr.size() - 1; i >= 0; i--) {
    pDynDialogItemEx = m_arrDlgItemPtr[i];
    if (pDynDialogItemEx != NULL) {
      delete pDynDialogItemEx;
      pDynDialogItemEx = NULL;
    }
  }

  if (m_bIsFontCreated) {
    delete m_pFont;
  }
}

CWnd *CDynDialogEx::GetParent()
{
  return m_pParentWnd;
}

void CDynDialogEx::AddStyles(DWORD dwStyles)
{
  m_DialogTemplate.style |= dwStyles;
}

void CDynDialogEx::RemoveStyles(DWORD dwStyles)
{
  m_DialogTemplate.style &= (~dwStyles);
}

void CDynDialogEx::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CDynDialogEx)
  // NOTE: the ClassWizard will add DDX and DDV calls here
  //}}AFX_DATA_MAP

  CDynDialogItemEx *pDynDialogItemEx = NULL;
  for (INT_PTR i = m_arrDlgItemPtr.size() - 1; i >= 0; i--) {
    pDynDialogItemEx = m_arrDlgItemPtr[i];
    if (pDynDialogItemEx != NULL) {
      pDynDialogItemEx->DoDataExchange(pDX);
    }
  }
}

BEGIN_MESSAGE_MAP(CDynDialogEx, CDialog)
  //{{AFX_MSG_MAP(CDynDialogEx)
  ON_WM_CREATE()
  //}}AFX_MSG_MAP
  ON_MESSAGE(WM_HELP, OnHelpMsg)
  ON_WM_MEASUREITEM()		// TMB! 06-12-2001 Adjusted.. was wrongly an ON_MESSAGE()!!
  ON_WM_DRAWITEM()			// TMB! 06-12-2001 Adjusted.. was wrongly an ON_MESSAGE()!!
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDynDialogEx message handlers

int CDynDialogEx::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  if (CDialog::OnCreate(lpCreateStruct) == -1)
    return -1;

  //if we have no font, create one here
  if (m_pFont == NULL) {
    LOGFONT LogFont;

    // Can do better???
    memset(&LogFont, 0x00, sizeof(LogFont));
    strncpy(LogFont.lfFaceName, _T("MS Sans Serif"), LF_FACESIZE);
    LogFont.lfHeight = 8;

    m_pFont = new CFont;
    m_pFont->CreateFontIndirect(&LogFont);
    SetFont(m_pFont);
    m_bIsFontCreated = TRUE;
  }

  //Create all the controls on the dialog
  CDynDialogItemEx *pDynDialogItemEx = NULL;
  for (int i = 0; i < m_arrDlgItemPtr.size(); i++) {
    pDynDialogItemEx = m_arrDlgItemPtr[i];
    if (pDynDialogItemEx != NULL) {
      if (!pDynDialogItemEx->IsDlgItemSubclassed()) {
        if (!pDynDialogItemEx->CreateEx(this)) {
          AfxMessageBox(_T("Failed to create DlgItem."));
        }
        else if (pDynDialogItemEx->GetSafeHwnd() != NULL) {
          pDynDialogItemEx->SetFont(m_pFont, FALSE);
        }
      }
    }
  }

  return 0;
}

BOOL CDynDialogEx::OnInitDialog()
{
  //Reposition all the controls on the dialog...
  CDynDialogItemEx *pDynDialogItemEx = NULL;
  for (int i = 0; i < m_arrDlgItemPtr.size(); i++) {
    pDynDialogItemEx = m_arrDlgItemPtr[i];
    if (pDynDialogItemEx != NULL) {
      if (!pDynDialogItemEx->IsDlgItemSubclassed() && !pDynDialogItemEx->SetWindowPos(this)) {
        CString strMessage;
        strMessage.Format(_T("Failed SetWindowPos for control %s."), pDynDialogItemEx->GetClassName());
        AfxMessageBox(strMessage);
      }
    }
  }

  CDialog::OnInitDialog();

  CenterWindow();

  return TRUE;  // return TRUE unless you set the focus to a control
  // EXCEPTION: OCX Property Pages should return FALSE
}

int CDynDialogEx::DoModal()
{
  //Do we need OK and Cancel buttons??
  if (m_bAddSystemButtons) {
    AddSystemButtons();
  }

  //
  // Get font info from mainwindow of the application
  //
  CFont* pParentFont = m_pFont;
  if (pParentFont == NULL && m_pParentWnd != NULL) {
    pParentFont = m_pParentWnd->GetFont();
  }
  if (pParentFont == NULL && AfxGetApp()->m_pActiveWnd != NULL) {
    pParentFont = AfxGetApp()->m_pActiveWnd->GetFont();
  }
  LOGFONT LogFont;
  memset(&LogFont, 0x00, sizeof(LogFont));
  if (pParentFont != NULL) {
    pParentFont->GetLogFont(&LogFont);
  }
  else {
    // Can do better???
    strncpy(LogFont.lfFaceName, _T("MS Sans Serif"), LF_FACESIZE);
    LogFont.lfHeight = 8;
  }

  //Prework for setting font in dialog...
  int cWC = MultiByteToWideChar(CP_ACP, 0, LogFont.lfFaceName, -1, NULL, 0);
  int nFontNameLen = cWC + 1;
  WCHAR *szFontName = new WCHAR[nFontNameLen];
  // Copy the string
  MultiByteToWideChar(CP_ACP, 0, LogFont.lfFaceName, -1, (LPWSTR) szFontName, cWC);
  szFontName[cWC] = 0;
  nFontNameLen = (cWC) * sizeof(WCHAR);

  if (m_wFontSize == 0) {
    m_wFontSize = (unsigned short)LogFont.lfHeight;
  }

  //Prework for setting caption in dialog...
  cWC = MultiByteToWideChar(CP_ACP, 0, m_strCaption, -1, NULL, 0);
  int szBoxLen = cWC + 1;
  WCHAR *szBoxCaption = new WCHAR[szBoxLen];
  // Copy the string
  MultiByteToWideChar(CP_ACP, 0, m_strCaption, -1, (LPWSTR) szBoxCaption, cWC);
  szBoxCaption[cWC] = 0;
  szBoxLen = (cWC) * sizeof(WCHAR);

  INT_PTR iRet = -1;
  //Here 's the stuff to build the dialog template in memory
  //without the controls being in the template
  //(Our first try, was this same template with some additional code
  //for each control placed on it, that's why this class is cold Ex :)
  //This gave some problems on WIN9x systems, where EDIT boxes
  //were not shown with 3D-look, but as flat controls)
  int nBufferSize =  sizeof(DLGTEMPLATE) + (2 * sizeof(WORD)) /*menu and class*/ + szBoxLen /*size of caption*/
    + sizeof(WORD) /*fontsize*/ + nFontNameLen /*size of fontname*/;

  //Are there any subclassed controls...
  if (m_DialogTemplate.cdit > 0) {
    nBufferSize = (nBufferSize + 3) & ~3;  // adjust size to make first control DWORD aligned

    CDynDialogItemEx *pDynDialogItemEx = NULL;
    for (int i = 0; i < m_arrDlgItemPtr.size(); i++) {
      pDynDialogItemEx = m_arrDlgItemPtr[i];
      if (pDynDialogItemEx != NULL) {
        if (pDynDialogItemEx->IsDlgItemSubclassed()) {
          int nItemLength = sizeof(DLGITEMTEMPLATE) + 3 * sizeof(WORD);
          nItemLength += (pDynDialogItemEx->GetCaptionLength() + 1) * sizeof(WCHAR);

          if (i != m_DialogTemplate.cdit - 1) {   // the last control does not need extra bytes
            nItemLength = (nItemLength + 3) & ~3;  // take into account gap so next control is DWORD aligned
          }
          nBufferSize += nItemLength;
        }
      }
    }
  }

  HLOCAL hLocal = LocalAlloc(LHND, nBufferSize);
  if (hLocal != NULL) {
    BYTE*	pBuffer = (BYTE*)LocalLock(hLocal);
    if (pBuffer == NULL) {
      LocalFree(hLocal);
      AfxMessageBox(_T("CDynDialogEx::DoModal() : LocalLock Failed"));
    }

    BYTE *pdest = pBuffer;
    // transfer DLGTEMPLATE structure to the buffer
    memcpy(pdest, &m_DialogTemplate, sizeof(DLGTEMPLATE));	// DLGTemplate
    pdest += sizeof(DLGTEMPLATE);
    *(WORD*)pdest = 0;									// no menu						 -- WORD to say it is 0 bytes
    pdest += sizeof(WORD);								// Increment
    *(WORD*)(pdest + 1) = 0;							// use default window class -- WORD to say it is 0 bytes
    pdest += sizeof(WORD);								// Increment
    memcpy(pdest, szBoxCaption, szBoxLen);			// Caption
    pdest += szBoxLen;

    *(WORD*)pdest = m_wFontSize;						// font size
    pdest += sizeof(WORD);
    memcpy(pdest, szFontName, nFontNameLen);		// font name
    pdest += nFontNameLen;

    // will now transfer the information for each one of subclassed controls...
    if (m_DialogTemplate.cdit > 0) {
      CDynDialogItemEx *pDynDialogItemEx = NULL;
      for (int i = 0; i < m_arrDlgItemPtr.size(); i++) {
        pDynDialogItemEx = m_arrDlgItemPtr[i];
        if (pDynDialogItemEx != NULL) {
          if (pDynDialogItemEx->IsDlgItemSubclassed()) {
            pdest = pDynDialogItemEx->FillBufferWithItemTemplate(pdest);
          }
        }
      }
    }
    ASSERT(pdest - pBuffer == nBufferSize); // just make sure we did not overrun the heap

    //Next lines to make sure that MFC makes no ASSERTION when PREVIOUS/NEXT is pressed:)
    if (m_lpDialogTemplate != NULL) {
      m_lpDialogTemplate = NULL;
    }

    //These are the MFC functions, which do the job...
    if (m_bModelessDlg) {
      iRet = CreateIndirect((LPDLGTEMPLATE)pBuffer, m_pParentWnd);
    }
    else {
      InitModalIndirect((LPDLGTEMPLATE)pBuffer, m_pParentWnd);
      iRet = CDialog::DoModal();
    }

    LocalUnlock(hLocal);
    LocalFree(hLocal);

    delete [] szBoxCaption;
    delete [] szFontName;
    return iRet;
  }
  else {
    AfxMessageBox(_T("CDynDialogEx::DoModal() : LocalAllock Failed"));
    return -1;
  }
}

void CDynDialogEx::SetFont(CFont *pFont)
{
  m_pFont = pFont;
}

CFont *CDynDialogEx::GetFont()
{
  return m_pFont;
}

void CDynDialogEx::SetFontSize(WORD wSize)
{
  m_wFontSize = wSize;
}

WORD CDynDialogEx::GetFontSize()
{
  return m_wFontSize;
}

void CDynDialogEx::SetUseSystemButtons(BOOL bUse /*= TRUE*/)
{
  m_bAddSystemButtons = bUse;
}

void CDynDialogEx::GetDlgRect(LPRECT lpRect)
{
  ASSERT(lpRect);
  lpRect->left   = m_DialogTemplate.x;
  lpRect->top    = m_DialogTemplate.y;
  lpRect->right  = lpRect->left + m_DialogTemplate.cx;
  lpRect->bottom = lpRect->top + m_DialogTemplate.cy;
}

void CDynDialogEx::SetDlgRect(LPRECT lpRect)
{
  ASSERT(lpRect);
  //#pragma warning(disable : 4244)
  m_DialogTemplate.cx = (short)(lpRect->right  - lpRect->left);
  m_DialogTemplate.cy = (short)(lpRect->bottom - lpRect->top);
  m_DialogTemplate.x  = (short)(lpRect->left);
  m_DialogTemplate.y  = (short)(lpRect->top);
  //#pragma warning(default : 4244)
}

void CDynDialogEx::SetDlgRectangle(LPRECT pRect)
{
  RECT rect;

  GetDlgRect(&rect);
  if (rect.left > pRect->left) {
    rect.left = pRect->left;
  }
  if (rect.right < pRect->right) {
    rect.right = pRect->right + 5;
  }
  if (rect.top > pRect->top) {
    rect.top = pRect->top;
  }
  if (rect.bottom < pRect->bottom) {
    rect.bottom = pRect->bottom + 5;
  }
  SetDlgRect(&rect);
}

UINT CDynDialogEx::AddDlgControl(DLGITEMTEMPLATECONTROLS TypeControl,
                                 LPCTSTR lpszCaption,
                                 DWORD dwStyle,
                                 DWORD dwExtendedStyle,
                                 LPRECT pRect /*= NULL*/,
                                 void *pData /*= NULL*/,
                                 UINT nID /*= 0*/)
{
  UINT nRet = 0;
  //In case no rectangle given create our own...
  CRect Rect(FIXEDCOL1, m_nCurRow, FIXEDCOL2, m_nCurRow + ROWSTEPSIZE);

  //if no rectangle given use our own...
  if (pRect == NULL) {
    pRect = &Rect;
  }
  //	else {
  //		m_nCurRow = max(m_nCurRow, pRect->bottom) - m_nCurRow;
  //	}

  m_nCurRow += (ROWSTEPSIZE);

  //Update dialogtemplate boundaries
  SetDlgRectangle(pRect);

  //Create control and add to array of controls
  CDynDialogItemEx *pDynDialogItemEx = new CDynDialogItemEx;
  if (pDynDialogItemEx != NULL) {
    nRet = pDynDialogItemEx->InitDialogItem(TypeControl, dwStyle, dwExtendedStyle, pRect, lpszCaption, nID, FALSE, pData);
    m_arrDlgItemPtr.push_back(pDynDialogItemEx);
  }

  //Return ID of Control we created.
  return nRet;
}

UINT CDynDialogEx::AddDlgControl(LPCSTR lpszClassName,
                                 LPCTSTR lpszCaption,
                                 DWORD dwStyle,
                                 DWORD dwExtendedStyle,
                                 LPRECT pRect /*= NULL*/,
                                 void *pData /*= NULL*/,
                                 UINT nID /*= 0*/)
{
  UINT nRet = 0;
  //In case no rectangle given create our own...
  CRect Rect(FIXEDCOL1, m_nCurRow, FIXEDCOL2, m_nCurRow + ROWSTEPSIZE);

  //if no rectangle given use our own...
  if (pRect == NULL) {
    pRect = &Rect;
    Rect.right += INPUTCOL;
  }
  //	else {
  //		m_nCurRow = max(m_nCurRow, pRect->bottom) - m_nCurRow;
  //	}

  m_nCurRow += (ROWSTEPSIZE);

  //Update dialogtemplate boundaries
  SetDlgRectangle(pRect);

  //Create control and add to array of controls
  CDynDialogItemEx *pDynDialogItemEx = new CDynDialogItemEx;
  if (pDynDialogItemEx != NULL) {
    nRet = pDynDialogItemEx->InitDialogItem(lpszClassName, dwStyle, dwExtendedStyle, pRect, lpszCaption, nID, FALSE, pData);
    m_arrDlgItemPtr.push_back(pDynDialogItemEx);
  }

  //Return ID of Control we created.
  return nRet;
}

UINT CDynDialogEx::AddSubclassedDlgControl(LPCSTR lpszClassName,
                                           LPCTSTR lpszCaption,
                                           DWORD dwStyle,
                                           DWORD dwExtendedStyle,
                                           LPRECT pRect /*= NULL*/,
                                           UINT nID /*= 0*/)
{
  UINT nRet = 0;
  //In case no rectangle given create our own...
  CRect Rect(FIXEDCOL1, m_nCurRow, FIXEDCOL2, m_nCurRow + ROWSTEPSIZE);

  //if no rectangle given use our own...
  if (pRect == NULL) {
    pRect = &Rect;
    Rect.right += INPUTCOL;
  }
  //	else {
  //		m_nCurRow = max(m_nCurRow, pRect->bottom) - m_nCurRow;
  //	}

  m_nCurRow += (ROWSTEPSIZE);

  //Update dialogtemplate boundaries
  SetDlgRectangle(pRect);

  //Create control and add to array of controls
  CDynDialogItemEx *pDynDialogItemEx = new CDynDialogItemEx;
  if (pDynDialogItemEx != NULL) {
    nRet = pDynDialogItemEx->InitDialogItem(lpszClassName, dwStyle, dwExtendedStyle, pRect, lpszCaption, nID, TRUE);
    m_arrDlgItemPtr.push_back(pDynDialogItemEx);
    m_DialogTemplate.cdit++;
  }

  //Return ID of Control we created.
  return nRet;
}

void CDynDialogEx::AddSystemButtons()
{
  m_nCurRow += 6;			// Leave some room!
  CRect rect(FIXEDCOL1, m_nCurRow, FIXEDCOL2 - 60, m_nCurRow + (long)(ROWSTEPSIZE * 1.2));

  AddDlgControl(BUTTON, _T("OK"), STYLE_BUTTON, EXSTYLE_BUTTON, &rect, NULL, IDOK);

  // This has to be revised later.
  rect.left  += 55;
  rect.right += 55;
  AddDlgControl(BUTTON, _T("Annuleren"), STYLE_BUTTON, EXSTYLE_BUTTON, &rect, NULL, IDCANCEL);

}

void CDynDialogEx::SetWindowTitle(LPCSTR lpszCaption)
{
  m_strCaption = lpszCaption;
}

void CDynDialogEx::SetUseModeless(BOOL bModelessDlg /*= TRUE*/)
{
  m_bModelessDlg = bModelessDlg;
}

void CDynDialogEx::OnCancel()
{
  if (m_bModelessDlg) {
    DestroyWindow();
  }
  else {
    CDialog::OnCancel();
  }
}

void CDynDialogEx::OnOK()
{
  if (m_bModelessDlg) {
    DestroyWindow();
  }
  else {
    CDialog::OnOK();
  }
}

BOOL CDynDialogEx::OnCommand(WPARAM wParam, LPARAM lParam)
{
  //wParam
  //The low-order word of wParam identifies the command ID of the menu item, control, or accelerator.
  //The high-order word of wParam specifies the notification message if the message is from a control.
  //If the message is from an accelerator, the high-order word is 1.
  //If the message is from a menu, the high-order word is 0.

  //lParam
  //Identifies the control that sends the message if the message is from a control. Otherwise, lParam is 0.

  WORD wControlID = LOWORD(wParam);
  WORD wMessageID = HIWORD(wParam);

  if (wControlID != 0) {
    switch (wControlID) {
    case IDOK:
      OnOK();
      return TRUE;
      break;
    case IDCANCEL:
      OnCancel();
      return TRUE;
      break;
    default:
      //if we have a parent send the message to the parent, so we can handle the message over there.
      if (m_pParentWnd != NULL) {
        ::SendMessage(m_pParentWnd->GetSafeHwnd(), WM_COMMAND, wParam, lParam);
      }
      break;
    }
  }

  return CDialog::OnCommand(wParam, lParam);
}

// Hellup!
afx_msg LRESULT CDynDialogEx::OnHelpMsg(WPARAM wParam, LPARAM lParam)
{
  //lParam		<<-- Contains: (LPHELPINFO)lParam
  // >> typedef  struct  tagHELPINFO {
  //     UINT     cbSize;
  //     int      iContextType
  //     int      iCtrlId;
  //     HANDLE   hItemHandle;
  //     DWORD    dwContextId;
  //     POINT    MousePos;
  // } HELPINFO, FAR *LPHELPINFO;

  //
  // User pressed F1 in dialog, call the OnHelp() function, this can be overridden...
  //
  OnHelp();
  return TRUE;
}

// Default implementation of the OnHelp()
// This one is to be overridden by a caller ;)
void CDynDialogEx::OnHelp()
{
  CDialog::OnHelp();
}
