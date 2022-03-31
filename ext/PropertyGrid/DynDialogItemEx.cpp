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
#include "DynDialogItemEx.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

struct _RuntimeLicense {
  LPCTSTR lpszRegisteredControlName;
  WCHAR *wchLicenseKey;
  long lLicenseLength;
}_TAGRUNTIMELICENSE;

/*mgkgtgnnmnmninigthkgogggvmkhinjggnvm*/						//(MS Multimedia MCI Control - mci32.ocx)
WCHAR pwchMCIMMControl1LicenseKey[] =
{
  0x006D,	0x0067,	0x006B,	0x0067,	0x0074,	0x0067,	
  0x006E,	0x006E,	0x006D,	0x006E,	0x006D,	0x006E,	
  0x0069,	0x006E,	0x0069,	0x0067,	0x0074,	0x0068,	
  0x006B,	0x0067,	0x006F,	0x0067,	0x0067,	0x0067,	
  0x0076,	0x006D,	0x006B,	0x0068,	0x0069,	0x006E,	
  0x006A,	0x0067,	0x0067,	0x006E,	0x0076,	0x006D
};

/*Copyright (c) 1994 */												//(MS Communications Control - mscomm32.ocx)
WCHAR pwchMSCOMMLibMSComm1LicenseKey[] =
{
  0x0043,	0x006F,	0x0070,	0x0079,	0x0072,	0x0069,	
  0x0067,	0x0068,	0x0074,	0x0020,	0x0028,	0x0063,	
  0x0029,	0x0020,	0x0031,	0x0039,	0x0039,	0x0034,	
  0x0020
};

/*72E67120-5959-11cf-91F6-C2863C385E30*/						//(MS Flex Grid Control - msflxgrd.ocx)
WCHAR pwchMSFlexGridLibMSFlexGrid1LicenseKey[] =
{
  0x0037,	0x0032,	0x0045,	0x0036,	0x0037,	0x0031,	
  0x0032,	0x0030,	0x002D,	0x0035,	0x0039,	0x0035,	
  0x0039,	0x002D,	0x0031,	0x0031,	0x0063,	0x0066,	
  0x002D,	0x0039,	0x0031,	0x0046,	0x0036,	0x002D,	
  0x0043,	0x0032,	0x0038,	0x0036,	0x0033,	0x0043,	
  0x0033,	0x0038,	0x0035,	0x0045,	0x0033,	0x0030
};

/*mgkgtgnnmnmninigthkgogggvmkhinjggnvm*/						//(MS Masked Edit - msmask32.ocx)
WCHAR pwchMSMaskMaskEdBox1LicenseKey[] =
{
  0x006D,	0x0067,	0x006B,	0x0067,	0x0074,	0x0067,	
  0x006E,	0x006E,	0x006D,	0x006E,	0x006D,	0x006E,	
  0x0069,	0x006E,	0x0069,	0x0067,	0x0074,	0x0068,	
  0x006B,	0x0067,	0x006F,	0x0067,	0x0067,	0x0067,	
  0x0076,	0x006D,	0x006B,	0x0068,	0x0069,	0x006E,	
  0x006A,	0x0067,	0x0067,	0x006E,	0x0076,	0x006D
};

/*GL........*/															//(MS Grid Control - grid32.ocx)
WCHAR pwchMSDBGridDBGridLicenseKey[] =
{
  0x0047,	0x004C,	0x0005,	0x0008,	0x0001,	0x0005,	
  0x0002,	0x0008,	0x0001,	0x0004
};

/*DB4C0D09-400B-101B-A3C9-08002B2F49FB*/						//(MS Picture Clip Control - picclp32.ocx)
WCHAR pwchPicClipPictureClip1LicenseKey[] =
{
  0x0044,	0x0042,	0x0034,	0x0043,	0x0030,	0x0044,	
  0x0030,	0x0039,	0x002D,	0x0034,	0x0030,	0x0030,	
  0x0042,	0x002D,	0x0031,	0x0030,	0x0031,	0x0042,	
  0x002D,	0x0041,	0x0033,	0x0043,	0x0039,	0x002D,	
  0x0030,	0x0038,	0x0030,	0x0030,	0x0032,	0x0042,	
  0x0032,	0x0046,	0x0034,	0x0039,	0x0046,	0x0042
};

/*04746E60CE4F11CDB23C0000C076FE*/								//(MS Tab Control - tabctl32.ocx) 
static WCHAR pwchTabDlgSSTab1LicenseKey[] =
{
  0x0030,	0x0034,	0x0037,	0x0034,	0x0036,	0x0045,	
  0x0036,	0x0030,	0x0043,	0x0045,	0x0034,	0x0046,	
  0x0031,	0x0031,	0x0043,	0x0044,	0x0042,	0x0032,	
  0x0033,	0x0043,	0x0030,	0x0030,	0x0030,	0x0030,	
  0x0043,	0x0030,	0x0037,	0x0036,	0x0046,	0x0045
};

static _RuntimeLicense RuntimeLicenses[] = 
{
  {_T("MCI.MMControl.1"), pwchMCIMMControl1LicenseKey, sizeof(pwchMCIMMControl1LicenseKey)},
  {_T("MSCOMMLib.MSComm.1"), pwchMSCOMMLibMSComm1LicenseKey, sizeof(pwchMSCOMMLibMSComm1LicenseKey)},
  {_T("MSFlexGridLib.MSFlexGrid.1"), pwchMSFlexGridLibMSFlexGrid1LicenseKey, sizeof(pwchMSFlexGridLibMSFlexGrid1LicenseKey)},
  {_T("MSMask.MaskEdBox.1"), pwchMSMaskMaskEdBox1LicenseKey, sizeof(pwchMSMaskMaskEdBox1LicenseKey)},
  {_T("MSDBGrid.DBGrid"), pwchMSDBGridDBGridLicenseKey, sizeof(pwchMSDBGridDBGridLicenseKey)},
  {_T("PicClip.PictureClip.1"), pwchPicClipPictureClip1LicenseKey, sizeof(pwchPicClipPictureClip1LicenseKey)},
  {_T("TabDlg.SSTab.1"), pwchTabDlgSSTab1LicenseKey, sizeof(pwchTabDlgSSTab1LicenseKey)},
  {NULL, NULL, 0}
};

static UINT glb_nNextID = WM_USER;  // We have to start somewhere...

UINT GetNewUniqueID(void)
{
  glb_nNextID++;
  return glb_nNextID - 1;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDynDialogItemEx::CDynDialogItemEx()
: CWnd()
{
  m_eTypeControl = NOCONTROL;
  m_strClassName = _T("");
  m_dwStyle = 0;
  m_dwStyleEx = 0;
  m_strCaption = _T("");
  m_ControlID = 0;
  m_pData = NULL;
  m_bSubclassed = FALSE;
}

void CDynDialogItemEx::DoDataExchange(CDataExchange *pDX)
{
  if (m_pData != NULL) {
    switch(m_eTypeControl) {
case BUTTON:
  if ((m_dwStyle & BS_AUTORADIOBUTTON) == BS_AUTORADIOBUTTON) {
    DDX_Radio(pDX, m_ControlID, *(int*)m_pData);
  }
  else if ((m_dwStyle & BS_AUTOCHECKBOX) == BS_AUTOCHECKBOX) {
    DDX_Check(pDX, m_ControlID, *(int*)m_pData);
  }
  else {
    DDX_Control(pDX, m_ControlID, *(CWnd*)m_pData);
  }
  break;
case EDITCONTROL:
  DDX_Text(pDX, m_ControlID, *(CString*)m_pData);
  break;
case STATICTEXT:
  DDX_Text(pDX, m_ControlID, *(CString*)m_pData);
  break;
case LISTBOX:
  DDX_Control(pDX, m_ControlID, *(CWnd*)m_pData);
  break;
case HSCROLL:
  DDX_Scroll(pDX, m_ControlID, *(int*)m_pData);
  break;
case COMBOBOX:
  DDX_Control(pDX, m_ControlID, *(CWnd*)m_pData);
  break;
case SPIN:
  DDX_Control(pDX, m_ControlID, *(CWnd*)m_pData);
  break;
case PROGRES:
  DDX_Control(pDX, m_ControlID, *(CWnd*)m_pData);
  break;
case SLIDER:
  DDX_Slider(pDX, m_ControlID, *(int*)m_pData);
  break;
case HOTKEY:
  DDX_Control(pDX, m_ControlID, *(CWnd*)m_pData);
  break;
case LISTCTRL:
  DDX_Control(pDX, m_ControlID, *(CWnd*)m_pData);
  break;
case TREECTRL:
  DDX_Control(pDX, m_ControlID, *(CWnd*)m_pData);
  break;
case TABCTRL:
  DDX_Control(pDX, m_ControlID, *(CWnd*)m_pData);
  break;
case ANIMATE:
  DDX_Control(pDX, m_ControlID, *(CWnd*)m_pData);
  break;
case RICHEDIT:
  DDX_Control(pDX, m_ControlID, *(CWnd*)m_pData);
  break;
case DATETIMEPICKER:
  if ((m_dwStyle & DTS_TIMEFORMAT) == DTS_TIMEFORMAT) {
    DDX_DateTimeCtrl(pDX, m_ControlID, *(CTime*)m_pData);
  }
  else {
    DDX_DateTimeCtrl(pDX, m_ControlID, *(COleDateTime*)m_pData);
  }
  break;
case MONTHCALENDER:
  DDX_MonthCalCtrl(pDX, m_ControlID, *(COleDateTime*)m_pData);
  break;
case IPADRESS:
  DDX_Control(pDX, m_ControlID, *(CWnd*)m_pData);
  break;
case COMBOBOXEX:
  DDX_Control(pDX, m_ControlID, *(CWnd*)m_pData);
  break;
default:
  break;
    }
  }
  CWnd::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDynDialogItemEx, CWnd)
  //{{AFX_MSG_MAP(CDynDialogItemEx)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

char* CDynDialogItemEx::GetClassNameByType(DLGITEMTEMPLATECONTROLS TypeControl)
{
  switch(TypeControl) {
case BUTTON:
  return _T("BUTTON");
case EDITCONTROL:
  return _T("EDIT");
case STATICTEXT:
  return _T("STATIC");
case LISTBOX:
  return _T("LISTBOX");
case HSCROLL:
  return _T("SCROLLBAR");
case COMBOBOX:
  return _T("COMBOBOX");
case SPIN:
  return _T("msctls_updown32");
case PROGRES:
  return _T("msctls_progress32");
case SLIDER:
  return _T("msctls_trackbar32");
case HOTKEY:
  return _T("msctls_hotkey32");
case LISTCTRL:
  return _T("SysListView32");
case TREECTRL:
  return _T("SysTreeView32");
case TABCTRL:
  return _T("SysTabControl32");
case ANIMATE:
  return _T("SysAnimate32");
case RICHEDIT:
  return _T("RICHEDIT");
case DATETIMEPICKER:
  return _T("SysDateTimePick32");
case MONTHCALENDER:
  return _T("SysMonthCal32");
case IPADRESS:
  return _T("SysIPAddress32");
case COMBOBOXEX:
  return _T("ComboBoxEx32");
  }
  return _T("");
}

DLGITEMTEMPLATECONTROLS CDynDialogItemEx::GetClassTypeByName(LPCSTR lpszClassName)
{
  if (memcmp(lpszClassName, _T("BUTTON"), 6) == 0) {
    return BUTTON;
  }
  else if (memcmp(lpszClassName, _T("EDIT"), 4) == 0) {
    return EDITCONTROL;
  }
  else if (memcmp(lpszClassName, _T("STATIC"), 6) == 0) {
    return STATICTEXT;
  }
  else if (memcmp(lpszClassName, _T("LISTBOX"), 7) == 0) {
    return LISTBOX;
  }
  else if (memcmp(lpszClassName, _T("SCROLLBAR"), 9) == 0) {
    return HSCROLL;
  }
  else if (memcmp(lpszClassName, _T("COMBOBOX"), 8) == 0) {
    return COMBOBOX;
  }
  else if (memcmp(lpszClassName, _T("msctls_updown32"), 15) == 0) {
    return SPIN;
  }
  else if (memcmp(lpszClassName, _T("msctls_progress32"), 17) == 0) {
    return PROGRES;
  }
  else if (memcmp(lpszClassName, _T("msctls_trackbar32"), 17) == 0) {
    return SLIDER;
  }
  else if (memcmp(lpszClassName, _T("msctls_hotkey32"), 15) == 0) {
    return HOTKEY;
  }
  else if (memcmp(lpszClassName, _T("SysListView32"), 13) == 0) {
    return LISTCTRL;
  }
  else if (memcmp(lpszClassName, _T("SysTreeView32"), 13) == 0) {
    return TREECTRL;
  }
  else if (memcmp(lpszClassName, _T("SysTabControl32"), 15) == 0) {
    return TABCTRL;
  }
  else if (memcmp(lpszClassName, _T("SysAnimate32"), 12) == 0) {
    return ANIMATE;
  }
  else if (memcmp(lpszClassName, _T("RICHEDIT"), 8) == 0) {
    return RICHEDIT;
  }
  else if (memcmp(lpszClassName, _T("SysDateTimePick32"), 17) == 0) {
    return DATETIMEPICKER;
  }
  else if (memcmp(lpszClassName, _T("SysMonthCal32"), 13) == 0) {
    return MONTHCALENDER;
  }
  else if (memcmp(lpszClassName, _T("SysIPAddress32"), 14) == 0) {
    return IPADRESS;
  }
  else if (memcmp(lpszClassName, _T("ComboBoxEx32"), 12) == 0) {
    return COMBOBOXEX;
  }

  return NOCONTROL;
}

UINT CDynDialogItemEx::InitDialogItem(DLGITEMTEMPLATECONTROLS TypeControl,
                                      DWORD dwStyle,
                                      DWORD dwExtendedStyle,
                                      LPRECT pRect,
                                      LPCTSTR lpszCaption,
                                      UINT nID /*= 0*/,
                                      BOOL bSubclassed /*= FALSE*/,
                                      void	*pData /*= NULL*/)
{
  m_eTypeControl = TypeControl;
  m_strClassName = GetClassNameByType(m_eTypeControl);
  m_dwStyle = dwStyle;
  m_dwStyleEx = dwExtendedStyle;
  m_Rect = pRect;
  m_strCaption = lpszCaption;
  m_bSubclassed = bSubclassed;
  m_pData = pData;
  if (nID == 0) {
    m_ControlID = ::GetNewUniqueID();
  }
  else {
    m_ControlID = nID;
  }
  return m_ControlID;
}

UINT CDynDialogItemEx::InitDialogItem(LPCSTR lpszClassName,
                                      DWORD dwStyle,
                                      DWORD dwExtendedStyle,
                                      LPRECT pRect,
                                      LPCTSTR lpszCaption,
                                      UINT nID /*= 0*/,
                                      BOOL bSubclassed /*= FALSE*/,
                                      void	*pData /*= NULL*/)
{
  m_strClassName = lpszClassName;
  m_eTypeControl = GetClassTypeByName(lpszClassName);
  m_dwStyle = dwStyle;
  m_dwStyleEx = dwExtendedStyle;
  m_Rect = pRect;
  m_strCaption = lpszCaption;
  m_bSubclassed = bSubclassed;
  m_pData = pData;
  if (nID == 0) {
    m_ControlID = ::GetNewUniqueID();
  }
  else {
    m_ControlID = nID;
  }
  return m_ControlID;
}

BOOL CDynDialogItemEx::CreateEx(CWnd *pParent)
{
  BOOL bRet = FALSE;
  if (m_eTypeControl == NOCONTROL) { //It will probably be an OCX...
    //
    // Create the control later....
    // if it's created here then the rectangle is not OK and SetWindowPos doesn't work on OCX's????
    //
    bRet = TRUE;
  }
  else if (m_pData != NULL && IsDataMemberPointerToWnd()) {
    bRet = ((CWnd*)m_pData)->CreateEx(m_dwStyleEx, m_strClassName, m_strCaption, m_dwStyle, m_Rect, pParent, m_ControlID);
  }
  else  {
    bRet = CWnd::CreateEx(m_dwStyleEx, m_strClassName, m_strCaption, m_dwStyle, m_Rect, pParent, m_ControlID);
  }

  return bRet;
}

BOOL CDynDialogItemEx::SetWindowPos(CWnd *pParent)
{
  BOOL bRet = FALSE;
  //Conversion of Dialog units to screenunits
  CRect rect(m_Rect);
  ((CDialog *)pParent)->MapDialogRect(&rect);
  ASSERT(rect.IsRectEmpty() == FALSE);

  if (m_eTypeControl == NOCONTROL) {
    BSTR bstrLicKey = GetRuntimeLicense(m_strClassName);
    bRet = CreateControl(m_strClassName, m_strCaption, m_dwStyle, rect, pParent, m_ControlID, NULL, FALSE, bstrLicKey);
    if (bstrLicKey != NULL) {
      ::SysFreeString(bstrLicKey);
    }
  }
  else if (m_pData != NULL && IsDataMemberPointerToWnd()) {
    bRet = ((CWnd*)m_pData)->SetWindowPos(NULL, rect.left, rect.top, rect.Width(), rect.Height(), SWP_NOZORDER);
  }
  else {
    bRet = CWnd::SetWindowPos(NULL, rect.left, rect.top, rect.Width(), rect.Height(), SWP_NOZORDER);
  }

  return bRet;
}

void CDynDialogItemEx::SetFont(CFont* pFont, BOOL bRedraw /*= TRUE*/)
{
  if (m_pData != NULL && IsDataMemberPointerToWnd()) {
    ((CWnd*)m_pData)->SetFont(pFont, bRedraw);
  }
  else {
    CWnd::SetFont(pFont, bRedraw);
  }
}

PBYTE CDynDialogItemEx::FillBufferWithItemTemplate(BYTE *pdest)
{
  pdest = (BYTE*)(((DWORD)pdest + 3) & ~3);  // make the pointer DWORD aligned

  DLGITEMTEMPLATE dlgItemTemplate;
  dlgItemTemplate.x = (short)m_Rect.left;
  dlgItemTemplate.y = (short)m_Rect.top;
  dlgItemTemplate.cx = (short)m_Rect.Width();
  dlgItemTemplate.cy = (short)m_Rect.Height();
  dlgItemTemplate.style = m_dwStyle;
  dlgItemTemplate.dwExtendedStyle = m_dwStyleEx;
  dlgItemTemplate.id = (USHORT)m_ControlID;

  memcpy(pdest, (void *)&dlgItemTemplate, sizeof(DLGITEMTEMPLATE));
  pdest += sizeof(DLGITEMTEMPLATE);
  *(WORD*)pdest = 0xFFFF;  // indicating atom value
  pdest += sizeof(WORD);
  *(WORD*)pdest = (USHORT)m_eTypeControl;    // atom value for the control
  pdest += sizeof(WORD);

  // transfer the caption even when it is an empty string
  WCHAR*  pchCaption;
  int     nChars, nActualChars;

  nChars = m_strCaption.GetLength() + 1;
  pchCaption = new WCHAR[nChars];
  nActualChars = MultiByteToWideChar(CP_ACP, 0, m_strCaption, -1, pchCaption, nChars);
  ASSERT(nActualChars > 0);
  memcpy(pdest, pchCaption, nActualChars * sizeof(WCHAR));
  pdest += nActualChars * sizeof(WCHAR);
  delete pchCaption;

  *(WORD*)pdest = 0;  // How many bytes in data for control
  pdest += sizeof(WORD);

  return pdest;
}

BSTR CDynDialogItemEx::GetRuntimeLicense(CString &strControlName)
{
  BSTR bstrLicKey = NULL;
  int i = 0;
  while (RuntimeLicenses[i].lpszRegisteredControlName != NULL) {
    if (strControlName.Compare(RuntimeLicenses[i].lpszRegisteredControlName) == 0) {
      bstrLicKey = ::SysAllocStringLen(RuntimeLicenses[i].wchLicenseKey, RuntimeLicenses[i].lLicenseLength/sizeof(WCHAR));
      break;
    }
    i++;
  }
  return bstrLicKey;
}

BOOL CDynDialogItemEx::IsDataMemberPointerToWnd()
{
  BOOL bRet = TRUE;
  switch(m_eTypeControl)
  {
  case BUTTON:
    if ((m_dwStyle & BS_AUTORADIOBUTTON) == BS_AUTORADIOBUTTON) {
      bRet = FALSE;
    }
    else if ((m_dwStyle & BS_AUTOCHECKBOX) == BS_AUTOCHECKBOX) {
      bRet = FALSE;
    }
    break;
  case EDITCONTROL:
    bRet = FALSE;
    break;
  case STATICTEXT:
    bRet = FALSE;
    break;
  case HSCROLL:
    bRet = FALSE;
    break;
  case SLIDER:
    bRet = FALSE;
    break;
  case DATETIMEPICKER:
    bRet = FALSE;
    break;
  case MONTHCALENDER:
    bRet = FALSE;
    break;
  default:
    break;
  }
  return bRet;
}
