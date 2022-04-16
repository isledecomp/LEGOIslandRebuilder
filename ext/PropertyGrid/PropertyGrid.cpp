// PropertyGrid.cpp : implementation file
//

#include "stdafx.h"
#include "CustomItem.h"
#include "PropertyGrid.h"
#include "PropertyGridDirectoryPicker.h"
#include "PropertyGridMonthCalCtrl.h"
#include "DynDialogEx.h"
#include <algorithm>
#include <shlwapi.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define IDC_MONTHCAL 1023

// CPropertyGrid

static const int margin = 2;

IMPLEMENT_DYNAMIC(CPropertyGrid, CWnd)
CPropertyGrid::CPropertyGrid()
{
  m_section_id = 0;
  m_item_id = 0;
  m_resizing_gutter = false;
  m_button_pushed = false;
  m_button_depressed = false;
  m_value_clicked = false;
  m_custom_tracking = false;
  m_scroll_enabled = false;
  m_draw_lines = true;
  m_shade_titles = true;
  m_draw_gutter = true;
  m_focus_disabled = true;
  m_bold_modified = false;
  m_bold_editables = false;
  m_display_mode = DM_CATEGORIZED;
  m_control = NULL;

  m_rect_button = CRect(0,0,0,0);
  m_ptLast = CPoint(0,0);

  m_strTrue = "True";
  m_strFalse = "False";
  m_strDate = "Date";
  m_strTime = "Time";
  m_strUndefined = "";
  m_strEmpty = "";

  m_clrBack = GetSysColor(COLOR_WINDOW);
  m_clrShade = GetSysColor(COLOR_3DFACE);
  m_clrText = GetSysColor(COLOR_WINDOWTEXT);
  m_clrTitle = GetSysColor(COLOR_WINDOWTEXT);
  m_clrFocus = GetSysColor(COLOR_HIGHLIGHT);
  m_clrHilite = GetSysColor(COLOR_HIGHLIGHTTEXT);
  m_clrEditable = GetSysColor(COLOR_WINDOWTEXT);
  m_clrDisabled = GetSysColor(COLOR_GRAYTEXT);

  m_focused_section = -1;
  m_focused_item = -1;
}

CPropertyGrid::~CPropertyGrid()
{
}

//
// customization
//

bool CPropertyGrid::GetShadeTitles()
{
  return m_shade_titles;
}

void CPropertyGrid::SetShadeTitles(bool shade_titles)
{
  m_shade_titles = shade_titles;
  if (GetSafeHwnd())
    Invalidate();
}

bool CPropertyGrid::GetDrawLines()
{
  return m_draw_lines;
}

void CPropertyGrid::SetDrawLines(bool draw_lines)
{
  m_draw_lines = draw_lines;
  if (GetSafeHwnd())
    Invalidate();
}

bool CPropertyGrid::GetDrawGutter()
{
  return m_draw_gutter;
}

void CPropertyGrid::SetDrawGutter(bool draw_gutter)
{
  m_draw_gutter = draw_gutter;
  if (GetSafeHwnd())
    Invalidate();
}

bool CPropertyGrid::GetFocusDisabled()
{
  return m_focus_disabled;
}

void CPropertyGrid::SetFocusDisabled(bool focus_disabled)
{
  m_focus_disabled = focus_disabled;
  if (GetSafeHwnd())
    Invalidate();
}

bool CPropertyGrid::GetBoldModified()
{
  return m_bold_modified;
}

void CPropertyGrid::SetBoldModified(bool bold_modified)
{
  m_bold_modified = bold_modified;
}

bool CPropertyGrid::GetBoldEditables()
{
  return m_bold_editables;
}

void CPropertyGrid::SetBoldEditables(bool bold_editables)
{
  m_bold_editables = bold_editables;
}

//
// gutter width
//

int CPropertyGrid::GetGutterWidth()
{
  return m_gutter_width;
}

void CPropertyGrid::SetGutterWidth(int gutter_width)
{
  m_gutter_width = gutter_width;
  if (GetSafeHwnd())
    Invalidate();
}

//
// custom colors
//

void CPropertyGrid::SetTextColor(COLORREF clrText)
{
  if (m_clrText == m_clrEditable)
    m_clrEditable = clrText;
  m_clrText = clrText;
  if (GetSafeHwnd())
    Invalidate();
}

void CPropertyGrid::SetTitleColor(COLORREF clrTitle)
{
  m_clrTitle = clrTitle;
  if (GetSafeHwnd())
    Invalidate();
}

void CPropertyGrid::SetBackColor(COLORREF clrBack)
{
  m_clrBack = clrBack;
  if (GetSafeHwnd())
    Invalidate();
}

void CPropertyGrid::SetShadeColor(COLORREF clrShade)
{
  m_clrShade = clrShade;
  if (GetSafeHwnd())
    Invalidate();
}

void CPropertyGrid::SetFocusColor(COLORREF clrFocus)
{
  m_clrFocus = clrFocus;
  if (GetSafeHwnd())
    Invalidate();
}

void CPropertyGrid::SetHiliteColor(COLORREF clrHilite)
{
  m_clrHilite = clrHilite;
  if (GetSafeHwnd())
    Invalidate();
}

void CPropertyGrid::SetEditableColor(COLORREF clrEditable)
{
  m_clrEditable = clrEditable;
  if (GetSafeHwnd())
    Invalidate();
}

void CPropertyGrid::SetDisabledColor(COLORREF clrDisabled)
{
  m_clrDisabled = clrDisabled;
  if (GetSafeHwnd())
    Invalidate();
}

//
// localization
//

void CPropertyGrid::SetTrueFalseStrings(string strTrue, string strFalse)
{
  m_strTrue = strTrue;
  m_strFalse = strFalse;
}

void CPropertyGrid::SetOkCancelStrings(string strOk, string strCancel)
{
  m_strOk = strOk;
  m_strCancel = strCancel;
}

void CPropertyGrid::SetDateTimeStrings(string strDate, string strTime)
{
  m_strDate = strDate;
  m_strTime = strTime;
}

void CPropertyGrid::SetUndefinedString(string strUndefined)
{
  m_strUndefined = strUndefined;
}

void CPropertyGrid::SetEmptyString(string strEmpty)
{
  m_strEmpty = strEmpty;
}

//
// appearance
//

void CPropertyGrid::SetDisplayMode(EDisplayMode display_mode)
{
  m_display_mode = display_mode;
  RecalcLayout();
}

void CPropertyGrid::ExpandAll(bool expand)
{
  for (vector<CSection>::iterator it = m_sections.begin(); it != m_sections.end(); ++it)
    it->m_collapsed = !expand;
  RecalcLayout();
}

void CPropertyGrid::ExpandSection(HSECTION hs, bool expand)
{
  CSection* pSection = FindSection(hs);
  if (pSection)
  {
    pSection->m_collapsed = !expand;
    RecalcLayout();
  }
}

bool CPropertyGrid::IsSectionCollapsed(HSECTION hs)
{
  CSection* pSection = FindSection(hs);
  if (pSection)
    return pSection->m_collapsed;
  return false;
}

string CPropertyGrid::GetItemText(HITEM hItem)
{
  CItem *item = FindItem(hItem);
  if (item) {
    return item->m_name;
  }
  return string();
}

//
// item management
//

bool CPropertyGrid::CItem::operator==(const HITEM& item) const
{
  return m_id == item;
}

bool CPropertyGrid::CItem::operator==(const string& name) const
{
  return m_name == name;
}

bool CPropertyGrid::CSection::operator==(const HSECTION& section) const
{
  return m_id == section;
}

void CPropertyGrid::CItem::ValidateChanges()
{
  // save the values
  m_undefined_old = m_undefined;
  m_nValue_old = m_nValue;
  m_dValue_old = m_dValue;
  m_strValue_old = m_strValue;
  m_bValue_old = m_bValue;
  m_dtValue_old = m_dtValue;
  m_clrValue_old = m_clrValue;
  memcpy(&m_lfValue_old, &m_lfValue, sizeof(LOGFONT));

  // callback for custom
  if (m_type == IT_CUSTOM)
    m_pCustom->ValidateChanges();
}

HSECTION CPropertyGrid::AddSection(string title, bool collapsed, HSECTION after)
{
  // build it
  CSection section;
  section.m_id = m_section_id;
  section.m_title = title;
  section.m_collapsed = collapsed;

  // insert it
  // if after does not exist then it is appended
  vector<CSection>::iterator it = find(m_sections.begin(), m_sections.end(), after);
  m_sections.insert(it, section);

  // done
  RecalcLayout();
  return m_section_id++;
}

HITEM CPropertyGrid::AddItem(HSECTION hs, EItemType type, string name, void* pValue, bool editable, bool undefined, HITEM after)
{
  // check section exists
  vector<CSection>::iterator it = find(m_sections.begin(), m_sections.end(), hs);
  if (it == m_sections.end())
    return -1;

  // check item does not already exists
  vector<CItem>::iterator it2 = find(it->m_items.begin(), it->m_items.end(), name);
  if (it2 != it->m_items.end())
    return -1;

  // build the item
  CItem item;
  item.m_id = m_item_id++;
  item.m_type = type;
  item.m_name = name;
  item.m_editable = editable;
  item.m_undefined = undefined;

  // assign the value
  if (type == IT_CUSTOM) item.m_pCustom = (ICustomItem*)pValue;
  else if (type == IT_STRING || type == IT_TEXT || type == IT_FILE || type == IT_FOLDER) item.m_strValue = *(string*)pValue;
  else if (type == IT_COMBO || type == IT_INTEGER) item.m_nValue = *(int*)pValue;
  else if (type == IT_DOUBLE) item.m_dValue = *(double*)pValue;
  else if (type == IT_BOOLEAN) item.m_bValue = *(bool*)pValue;
  else if (type == IT_DATE || type == IT_DATETIME) item.m_dtValue = *(COleDateTime*)pValue;
  else if (type == IT_COLOR) item.m_clrValue = *(COLORREF*)pValue;
  else if (type == IT_FONT) memcpy(&item.m_lfValue, pValue, sizeof(LOGFONT));
  else assert(false);

  // finish and add
  item.ValidateChanges();
  it->m_items.push_back(item);
  RecalcLayout();
  return item.m_id;
}

HITEM CPropertyGrid::AddCustomItem(HSECTION section, string name, ICustomItem* pItem, bool editable, HITEM after)
{
  pItem->m_pGrid = this;
  return AddItem(section, IT_CUSTOM, name, pItem, editable, false, after);
}

HITEM CPropertyGrid::AddStringItem(HSECTION section, string name, string value, bool editable, HITEM after)
{
  return AddItem(section, IT_STRING, name, &value, editable, false, after);
}

HITEM CPropertyGrid::AddTextItem(HSECTION section, string name, string value, bool editable, HITEM after)
{
  return AddItem(section, IT_TEXT, name, &value, editable, false, after);
}

HITEM CPropertyGrid::AddIntegerItem(HSECTION section, string name, int value, string format, bool editable, bool undefined, HITEM after)
{
  HITEM it = AddItem(section, IT_INTEGER, name, &value, editable, undefined, after);
  CItem* pItem = FindItem(it);
  if (pItem) pItem->m_options.push_back(format);
  return it;
}

HITEM CPropertyGrid::AddDoubleItem(HSECTION section, string name, double value, string format, bool editable, bool undefined, HITEM after)
{
  HITEM it = AddItem(section, IT_DOUBLE, name, &value, editable, undefined, after);
  CItem* pItem = FindItem(it);
  if (pItem) pItem->m_options.push_back(format);
  return it;
}

HITEM CPropertyGrid::AddComboItem(HSECTION section, string name, const vector<string>& values, int cur, bool editable, bool undefined, HITEM after)
{
  HITEM it = AddItem(section, IT_COMBO, name, &cur, editable, undefined, after);
  CItem* pItem = FindItem(it);
  if (pItem) pItem->m_options = values;
  return it;
}

HITEM CPropertyGrid::AddBoolItem(HSECTION section, string name, bool value, bool editable, bool undefined, HITEM after)
{
  return AddItem(section, IT_BOOLEAN, name, &value, editable, undefined, after);
}

HITEM CPropertyGrid::AddDateItem(HSECTION section, string name, COleDateTime value, string format, bool editable, bool undefined, HITEM after)
{
  HITEM it = AddItem(section, IT_DATE, name, &value, editable, undefined, after);
  CItem* pItem = FindItem(it);
  if (pItem) pItem->m_options.push_back(format);
  return it;
}

HITEM CPropertyGrid::AddDateTimeItem(HSECTION section, string name, COleDateTime value, string format, bool editable, bool undefined, HITEM after)
{
  HITEM it = AddItem(section, IT_DATETIME, name, &value, editable, undefined, after);
  CItem* pItem = FindItem(it);
  if (pItem) pItem->m_options.push_back(format);
  return it;
}

HITEM CPropertyGrid::AddFileItem(HSECTION section, string name, string value, string filter, bool editable, HITEM after)
{
  HITEM it = AddItem(section, IT_FILE, name, &value, editable, false, after);
  CItem* pItem = FindItem(it);
  if (pItem) pItem->m_options.push_back(filter);
  return it;
}

HITEM CPropertyGrid::AddFolderItem(HSECTION section, string name, string value, string title, bool editable, HITEM after)
{
  HITEM it = AddItem(section, IT_FOLDER, name, &value, editable, false, after);
  CItem* pItem = FindItem(it);
  if (pItem) pItem->m_options.push_back(title);
  return it;
}

HITEM CPropertyGrid::AddColorItem(HSECTION section, string name, COLORREF value, bool editable, bool undefined, HITEM after)
{
  return AddItem(section, IT_COLOR, name, &value, editable, undefined, after);
}

HITEM CPropertyGrid::AddFontItem(HSECTION section, string name, LOGFONT value, bool editable, bool undefined, HITEM after)
{
  return AddItem(section, IT_FONT, name, &value, editable, undefined, after);
}

void CPropertyGrid::ResetContents()
{
  m_sections.clear();
  m_section_id = 0;
  m_item_id = 0;
  RecalcLayout();
}

bool CPropertyGrid::RemoveSection(HSECTION hs)
{
  vector<CSection>::iterator it = find(m_sections.begin(), m_sections.end(), hs);
  if (it == m_sections.end()) return false;
  m_sections.erase(it);
  return true;
}

bool CPropertyGrid::RemoveItem(HITEM item)
{
  for (vector<CSection>::iterator it = m_sections.begin(); it != m_sections.end(); ++it)
  {
    vector<CItem>::iterator it2 = find(it->m_items.begin(), it->m_items.end(), item);
    if (it2 != it->m_items.end())
    {
      it->m_items.erase(it2);
      return true;
    }
  }
  return false;
}

int CPropertyGrid::GetNumSections()
{
  return int(m_sections.size());
}

int CPropertyGrid::GetSectionSize(HSECTION hs)
{
  CSection* pSection = FindSection(hs);
  if (pSection) return int(pSection->m_items.size());
  return 0;
}

void CPropertyGrid::ValidateChanges()
{
  for (vector<CSection>::iterator it = m_sections.begin(); it != m_sections.end(); ++it)
  {
    for (vector<CItem>::iterator it2 = it->m_items.begin(); it2 != it->m_items.end(); ++it2)
      it2->ValidateChanges();
  }
}

CPropertyGrid::CSection* CPropertyGrid::FindSection(HSECTION hs) const
{
  vector<CSection>::const_iterator it = find(m_sections.begin(), m_sections.end(), hs);
  if (it == m_sections.end()) return NULL;
  return const_cast<CSection*>(&(*it));
}

CPropertyGrid::CItem* CPropertyGrid::FindItem(HITEM hi) const
{
  for (vector<CSection>::const_iterator it = m_sections.begin(); it != m_sections.end(); ++it)
  {
    vector<CItem>::const_iterator it2 = find(it->m_items.begin(), it->m_items.end(), hi);
    if (it2 != it->m_items.end())
      return const_cast<CItem*>(&(*it2));
  }
  return NULL;
}

bool CPropertyGrid::GetItemValue(HITEM item, string& strValue) const
{
  // get the item
  CItem* pItem = FindItem(item);
  if (pItem == NULL) return false;
  if (pItem->m_undefined) return false;

  // check
  if (pItem->m_type == IT_STRING || pItem->m_type == IT_TEXT || pItem->m_type == IT_FILE || pItem->m_type == IT_FOLDER)
  {
    strValue = pItem->m_strValue;
    return true;
  }
  else if (pItem->m_type == IT_COMBO)
  {
    if (pItem->m_nValue < 0 || pItem->m_nValue > int(pItem->m_options.size())-1) return false;
    strValue = pItem->m_options[pItem->m_nValue];
    return true;
  }
  return false;
}

bool CPropertyGrid::GetItemValue(HITEM item, int& nValue) const
{
  // get the item
  CItem* pItem = FindItem(item);
  if (pItem == NULL) return false;
  if (pItem->m_undefined) return false;

  // check
  if (pItem->m_type == IT_COMBO || pItem->m_type == IT_INTEGER)
  {
    nValue = pItem->m_nValue;
    return true;
  }
  return false;
}

bool CPropertyGrid::GetItemValue(HITEM item, double& dValue) const
{
  // get the item
  CItem* pItem = FindItem(item);
  if (pItem == NULL) return false;
  if (pItem->m_undefined) return false;

  // check
  if (pItem->m_type == IT_DOUBLE)
  {
    dValue = pItem->m_dValue;
    return true;
  }
  return false;
}

bool CPropertyGrid::GetItemValue(HITEM item, bool& bValue) const
{
  // get the item
  CItem* pItem = FindItem(item);
  if (pItem == NULL) return false;
  if (pItem->m_undefined) return false;

  // check
  if (pItem->m_type == IT_BOOLEAN)
  {
    bValue = pItem->m_bValue;
    return true;
  }
  return false;
}

bool CPropertyGrid::GetItemValue(HITEM item, COleDateTime& dtValue) const
{
  // get the item
  CItem* pItem = FindItem(item);
  if (pItem == NULL) return false;
  if (pItem->m_undefined) return false;

  // check
  if (pItem->m_type == IT_DATE || pItem->m_type == IT_DATETIME)
  {
    dtValue = pItem->m_dtValue;
    return true;
  }
  return false;
}

bool CPropertyGrid::GetItemValue(HITEM item, COLORREF& clrValue) const
{
  // get the item
  CItem* pItem = FindItem(item);
  if (pItem == NULL) return false;
  if (pItem->m_undefined) return false;

  // check
  if (pItem->m_type == IT_COLOR)
  {
    clrValue = pItem->m_clrValue;
    return true;
  }
  return false;
}

bool CPropertyGrid::GetItemValue(HITEM item, LOGFONT& lfValue) const
{
  // get the item
  CItem* pItem = FindItem(item);
  if (pItem == NULL) return false;
  if (pItem->m_undefined) return false;

  // check
  if (pItem->m_type == IT_FONT)
  {
    lfValue = pItem->m_lfValue;
    return true;
  }
  return false;
}

bool CPropertyGrid::SetItemValue(HITEM item, const string strValue)
{
  // get the item
  CItem* pItem = FindItem(item);
  if (pItem == NULL) return false;

  // check
  if (pItem->m_type == IT_STRING || pItem->m_type == IT_TEXT || pItem->m_type == IT_FILE || pItem->m_type == IT_FOLDER)
  {
    pItem->m_strValue = strValue;
    pItem->m_undefined = false;
    Invalidate();
    return true;
  }
  else if (pItem->m_type == IT_COMBO)
  {
    for (size_t i=0; i<pItem->m_options.size(); i++) {
      if (pItem->m_options.at(i) == strValue) {
        return SetItemValue(item, (int) i);
      }
    }
  }
  return false;
}

bool CPropertyGrid::SetItemValue(HITEM item, const int nValue)
{
  // get the item
  CItem* pItem = FindItem(item);
  if (pItem == NULL) return false;

  // check
  if (pItem->m_type == IT_COMBO || pItem->m_type == IT_INTEGER)
  {
    pItem->m_nValue = nValue;
    pItem->m_undefined = false;
    Invalidate();
    return true;
  }
  return false;
}

bool CPropertyGrid::SetItemValue(HITEM item, const double dValue)
{
  // get the item
  CItem* pItem = FindItem(item);
  if (pItem == NULL) return false;

  // check
  if (pItem->m_type == IT_DOUBLE)
  {
    pItem->m_dValue = dValue;
    pItem->m_undefined = false;
    Invalidate();
    return true;
  }
  return false;
}

bool CPropertyGrid::SetItemValue(HITEM item, const bool bValue)
{
  // get the item
  CItem* pItem = FindItem(item);
  if (pItem == NULL) return false;

  // check
  if (pItem->m_type == IT_BOOLEAN)
  {
    pItem->m_bValue = bValue;
    pItem->m_undefined = false;
    Invalidate();
    return true;
  }
  return false;
}

bool CPropertyGrid::SetItemValue(HITEM item, const COleDateTime dtValue)
{
  // get the item
  CItem* pItem = FindItem(item);
  if (pItem == NULL) return false;

  // check
  if (pItem->m_type == IT_DATE || pItem->m_type == IT_DATETIME)
  {
    pItem->m_dtValue = dtValue;
    pItem->m_undefined = false;
    Invalidate();
    return true;
  }
  return false;
}

bool CPropertyGrid::SetItemValue(HITEM item, const COLORREF clrValue)
{
  // get the item
  CItem* pItem = FindItem(item);
  if (pItem == NULL) return false;

  // check
  if (pItem->m_type == IT_COLOR)
  {
    pItem->m_clrValue = clrValue;
    pItem->m_undefined = false;
    Invalidate();
    return true;
  }
  return false;
}

bool CPropertyGrid::SetItemValue(HITEM item, const LOGFONT lfValue)
{
  // get the item
  CItem* pItem = FindItem(item);
  if (pItem == NULL) return false;

  // check
  if (pItem->m_type == IT_FONT)
  {
    memcpy(&pItem->m_lfValue, &lfValue, sizeof(LOGFONT));
    pItem->m_undefined = false;
    Invalidate();
    return true;
  }
  return false;
}

int CPropertyGrid::GetTextMargin()
{
  return 2*margin;
}

CFont* CPropertyGrid::GetFontNormal()
{
  return &m_fntNormal;
}

CFont* CPropertyGrid::GetFontBold()
{
  return &m_fntBold;
}

BEGIN_MESSAGE_MAP(CPropertyGrid, CWnd)
  ON_WM_PAINT()
  ON_WM_LBUTTONDOWN()
  ON_WM_MOUSEMOVE()
  ON_WM_CREATE()
  ON_WM_LBUTTONUP()
  ON_WM_VSCROLL()
  ON_WM_ERASEBKGND()
  ON_MESSAGE(WM_PG_COMBOSELCHANGED, OnComboSelChanged)
  ON_MESSAGE(WM_PG_ENDLABELEDIT, OnEditChanged)
  ON_MESSAGE(WM_PG_DATESELCHANGED, OnDateChanged)
  ON_WM_LBUTTONDBLCLK()
  ON_WM_MOUSEWHEEL()
  ON_WM_DESTROY()
  ON_WM_SIZE()
  ON_WM_GETDLGCODE()
  ON_WM_CHAR()
  ON_WM_KEYDOWN()
END_MESSAGE_MAP()

//
// creation and window stuff
//

void CPropertyGrid::InitControl()
{
  // first gutter
  CRect rc;
  GetClientRect(&rc);
  m_gutter_width = rc.Width()/2;

  // check if already done
  if (m_fntNormal.GetSafeHandle() == NULL)
  {
    // fonts
    LOGFONT lf;
    if (GetParent() && GetParent()->GetFont())
    {
      CFont* pFont = GetParent()->GetFont();
      pFont->GetLogFont(&lf);
      m_fntNormal.CreateFontIndirect(&lf);
      lf.lfWeight = FW_BOLD;
      m_fntBold.CreateFontIndirect(&lf);
    }
    else
    {
      m_fntNormal.CreateStockObject(DEFAULT_GUI_FONT);
      m_fntNormal.GetLogFont(&lf);
      lf.lfWeight = FW_BOLD;
      m_fntBold.CreateFontIndirect(&lf);
    }
  }

  // get line height
  CDC* pDC = GetDC();
  CFont* pOldFont = pDC->SelectObject(&m_fntNormal);
  m_line_height = pDC->GetTextExtent("Gg").cy + 2*margin;
  pDC->SelectObject(pOldFont);
  ReleaseDC(pDC);

  // styles
  ModifyStyle(0, WS_CLIPCHILDREN);

  // try to get some strings
  if (m_strOk.empty())
  {
    m_strOk = "OK";
    if (GetParent() && GetParent()->GetDlgItem(IDOK))
    {
      CString strOk;
      GetParent()->GetDlgItem(IDOK)->GetWindowText(strOk);
      m_strOk = strOk;
    }
  }
  if (m_strCancel.empty())
  {
    m_strCancel = "Cancel";
    if (GetParent() && GetParent()->GetDlgItem(IDCANCEL))
    {
      CString strCancel;
      GetParent()->GetDlgItem(IDCANCEL)->GetWindowText(strCancel);
      m_strCancel = strCancel;
    }
  }
}

int CPropertyGrid::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  if (CWnd::OnCreate(lpCreateStruct) == -1) return -1;
  InitControl();
  return 0;
}

void CPropertyGrid::PreSubclassWindow()
{
  InitControl();
  CWnd::PreSubclassWindow();
}

void CPropertyGrid::OnDestroy()
{
  DeleteEditControl();
  CWnd::OnDestroy();
}

void CPropertyGrid::OnSize(UINT nType, int cx, int cy)
{
  CWnd::OnSize(nType, cx, cy);

  RecalcLayout();

  if (m_scrollbar.GetSafeHwnd())
  {
    CRect rect;
    GetClientRect(&rect);
    m_scrollbar.MoveWindow(rect.right - GetSystemMetrics(SM_CXVSCROLL), rect.top, GetSystemMetrics(SM_CXVSCROLL), rect.Height());
  }
}

//
// painting
//

BOOL CPropertyGrid::OnEraseBkgnd(CDC* pDC)
{
  return TRUE;
}

bool item_alpha_sort(vector<CPropertyGrid::CItem>::iterator it1, vector<CPropertyGrid::CItem>::iterator it2)
{
  return (it1->m_name.compare(it2->m_name) < 0);
}

void CPropertyGrid::OnPaint()
{
  // stuff needed
  const int sign_size = 8;

  // the scrollbar offset
  int top = GetScrollOffset();

  // the rect
  CRect rc_dummy;
  GetClientRect(&rc_dummy);
  if (m_scroll_enabled)
    rc_dummy.right -= GetSystemMetrics(SM_CXVSCROLL);

  // make sure we do not modify this one
  // because we use it to bitblt
  const CRect rc(rc_dummy);

  // stuff for flicker free drawing
  CDC dcMem;
  CBitmap bmpMem;
  CPaintDC dc(this);

  // create and configure the memdc
  dcMem.CreateCompatibleDC(&dc);
  bmpMem.CreateCompatibleBitmap(&dc, rc.Width(), rc.Height());
  CBitmap* pOldBmp = dcMem.SelectObject(&bmpMem);

  // brush needed
  CBrush brushTitle;
  brushTitle.CreateSolidBrush(m_clrTitle);

  // pen needed
  CPen penShade(PS_SOLID, 1, m_clrShade);
  CPen penTitle(PS_SOLID, 1, m_clrTitle);

  // to make sure we won't leak gdi resources
  CBrush* pOldBrush = dcMem.SelectObject(&brushTitle);
  CPen* pOldPen = dcMem.SelectObject(&penShade);
  CFont* pOldFont = dcMem.SelectObject(&m_fntNormal);

  // needed
  int w = rc.Width();

  // blank
  dcMem.FillSolidRect(rc, m_clrBack);
  dcMem.SetBkMode(TRANSPARENT);

  // empty text
  if (m_sections.empty())
  {
    CRect rect = rc;
    rect.top += 10;
    rect.DeflateRect(rect.Width()/4, 0);
    dcMem.DrawText(m_strEmpty.c_str(), rect, DT_CENTER|DT_WORDBREAK|DT_NOPREFIX);
  }
  else
  {
    // needed
    int sign_left = margin;

    // we start here
    int y = -top;

    // alphabetical needs special
    if (m_display_mode == DM_ALPHABETICAL)
    {
      // put all the items in a vector
      vector<vector<CItem>::iterator> lst;
      for (vector<CSection>::iterator it = m_sections.begin(); it != m_sections.end(); ++it)
      {
        for (vector<CItem>::iterator it2 = it->m_items.begin(); it2 != it->m_items.end(); ++it2)
          lst.push_back(it2);
      }

      // sort the vector
      sort(lst.begin(), lst.end(), item_alpha_sort);

      // display the items
      for (vector<vector<CItem>::iterator>::iterator it2 = lst.begin(); it2 != lst.end(); ++it2)
      {
        // first reset
        (*it2)->m_rcName.SetRectEmpty();
        (*it2)->m_rcValue.SetRectEmpty();

        // draw if visible
        (*it2)->m_rcName = CRect(0, y, w, y+m_line_height);
        CRect rcInter = (*it2)->m_rcName;
        rcInter.IntersectRect(rc, rcInter);
        if (!rcInter.IsRectEmpty())
          DrawItem(dcMem, w, sign_left+sign_size, y, *it2);

        // next line
        y += m_line_height;
      }
    }
    else
    {
      // next iterate on sections
      for (vector<CSection>::iterator it = m_sections.begin(); it != m_sections.end(); ++it)
      {
        // reset
        it->m_rcSign.SetRectEmpty();
        it->m_rcTitle.SetRectEmpty();

        // is visible?
        it->m_rcTitle = CRect(0, y, w, y+m_line_height);
        CRect rcInter = it->m_rcTitle;
        rcInter.IntersectRect(rcInter, rc);
        if (m_display_mode == DM_CATEGORIZED && !rcInter.IsRectEmpty())
        {
          // first shade rect
          if (m_shade_titles)
            dcMem.FillSolidRect(0, y, w, m_line_height, m_clrShade);

          // now draw a separator lines
          if (m_draw_lines)
          {
            dcMem.SelectObject(&penShade);
            dcMem.MoveTo(0, y);
            dcMem.LineTo(w+1, y);
            dcMem.MoveTo(0, y+m_line_height);
            dcMem.LineTo(w+1, y+m_line_height);
          }

          // now draw gutter
          if (m_draw_gutter)
          {
            dcMem.SelectObject(&penShade);
            dcMem.MoveTo(m_gutter_width, y);
            dcMem.LineTo(m_gutter_width, y+m_line_height+1);
          }

          // now draw collapse sign
          int sign_top = y+margin+2;
          dcMem.SelectObject(&penTitle);
          it->m_rcSign = CRect(sign_left, sign_top, sign_left+sign_size+1, sign_top+sign_size+1);
          dcMem.FrameRect(it->m_rcSign, &brushTitle);
          dcMem.MoveTo(sign_left+2, sign_top+sign_size/2);
          dcMem.LineTo(sign_left+2+sign_size/2+1, sign_top+sign_size/2);
          if (it->m_collapsed)
          {
            dcMem.MoveTo(sign_left+sign_size/2, sign_top+2);
            dcMem.LineTo(sign_left+sign_size/2, sign_top+2+sign_size/2+1);
          }

          // prepare draw text
          int title_left = sign_left+sign_size+2*margin;
          int title_top = y;
          dcMem.SelectObject(&m_fntBold);
          it->m_rcTitle = CRect(title_left, title_top, w, title_top+m_line_height);

          // draw focus rect
          if (m_focused_section == it->m_id)
          {
            CSize sz = dcMem.GetTextExtent(it->m_title.c_str());
            int rect_left = title_left;
            int rect_top = title_top+(m_line_height-sz.cy)/2;
            int rect_width = sz.cx+3*margin;
            int rect_height = sz.cy;
            dcMem.DrawFocusRect(CRect(rect_left, rect_top, rect_left+rect_width, rect_top+rect_height));
          }

          // now draw text
          dcMem.SetTextColor(m_clrTitle);
          dcMem.DrawText(it->m_title.c_str(), CRect(title_left+GetTextMargin(), title_top, w, title_top+m_line_height), DT_END_ELLIPSIS|DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_NOPREFIX);
        }

        // next line
        if (m_display_mode == DM_CATEGORIZED)
          y+=m_line_height;

        // iterate on items
        if (!it->m_collapsed || m_display_mode != DM_CATEGORIZED)
        {
          for (vector<CItem>::iterator it2 = it->m_items.begin(); it2 != it->m_items.end(); ++it2)
          {
            // reset
            it2->m_rcName.SetRectEmpty();
            it2->m_rcValue.SetRectEmpty();

            // is visible?
            it2->m_rcName = CRect(0, y, w, y+m_line_height);
            CRect rcInter = it2->m_rcName;
            rcInter.IntersectRect(rc, rcInter);
            if (!rcInter.IsRectEmpty())
              DrawItem(dcMem, w, sign_left+sign_size, y, it2);

            // next line
            y+=m_line_height;
          }
        }
      }
    }
  }

  // Blt the changes to the screen DC.
  dc.BitBlt(rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, &dcMem, 0, 0, SRCCOPY);

  // Done with off-screen bitmap and DC.
  dcMem.SelectObject(pOldBmp);
  dcMem.SelectObject(pOldFont);
  dcMem.SelectObject(pOldPen);
  dcMem.SelectObject(pOldBrush);
  bmpMem.DeleteObject();
  dcMem.DeleteDC();

  // Validate All
  ValidateRgn(NULL);
  ValidateRect(NULL);
}

void CPropertyGrid::DrawItem(CDC& dc, int w, int x, int y, vector<CItem>::iterator& it)
{
  // brush needed
  CBrush brushText;
  brushText.CreateSolidBrush(m_clrText);

  // pen needed
  CPen penShade(PS_SOLID, 1, m_clrShade);

  // to make sure we won't leak gdi resources
  CBrush* pOldBrush = dc.SelectObject(&brushText);
  CPen* pOldPen = dc.SelectObject(&penShade);
  CFont* pOldFont = dc.SelectObject(&m_fntNormal);

  // first shade rect
  if (m_shade_titles)
    dc.FillSolidRect(0, y, x+2*margin, m_line_height, m_clrShade);

  // now draw a separator line
  if (m_draw_lines)
  {
    dc.SelectObject(&penShade);
    dc.MoveTo(0, y+m_line_height);
    dc.LineTo(w+1, y+m_line_height);
  }

  // now draw gutter
  if (m_draw_gutter)
  {
    dc.SelectObject(&penShade);
    dc.MoveTo(m_gutter_width, y);
    dc.LineTo(m_gutter_width, y+m_line_height+1);
  }

  // needed
  int name_left = x+2*margin+GetTextMargin();
  int name_right = m_gutter_width-1;
  int value_left = m_gutter_width;
  int value_right = w;

  // is being edited?
  if (m_focused_item == it->m_id && it->m_editable && GetEditMode(*it) != EM_CUSTOM)
  {
    value_right -= m_line_height;

    // the rect of the button
    m_rect_button = CRect(w-m_line_height, y, w, y+m_line_height);

    UINT pushed = m_button_depressed?DFCS_PUSHED:0;

    // now draw the button
    switch (GetEditMode(*it))
    {
    case EM_MODAL:
      // draw a button
      dc.DrawFrameControl(m_rect_button, DFC_BUTTON, DFCS_BUTTONPUSH|pushed);
      dc.SelectObject(&m_fntBold);
      dc.DrawText("...", m_rect_button, DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_NOPREFIX);
      break;

    case EM_DROPDOWN:
      // draw an arrow
      dc.DrawFrameControl(m_rect_button, DFC_SCROLL, DFCS_SCROLLDOWN|pushed);
      break;

    case EM_INPLACE:
      // whole area is edit
      m_rect_button.left = m_gutter_width;
      break;

    default:
      assert(false);
    }
  }

  // update the rects
  it->m_rcName = CRect(0, y, m_gutter_width, y+m_line_height);
  it->m_rcValue = CRect(value_left, y, value_right, y+m_line_height);
  CRect rcValue = it->m_rcValue;
  rcValue.left += GetTextMargin();

  // focused
  if (m_focused_item == it->m_id)
  {
    int rect_left = name_left-2*margin;
    int rect_right = name_right;
    dc.FillSolidRect(rect_left, y, rect_right-rect_left+1, m_line_height, m_clrFocus);
    dc.SetTextColor(m_clrHilite);
  }
  else
  {
    dc.SetTextColor(m_clrText);
  }

  // put name and value
  dc.SelectObject(&m_fntNormal);
  dc.DrawText(it->m_name.c_str(), -1, CRect(name_left, y, name_right, y+m_line_height), DT_END_ELLIPSIS|DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_NOPREFIX);

  // get back to normal text
  if (it->m_editable) dc.SetTextColor(m_clrEditable);
  else dc.SetTextColor(m_clrDisabled);

  // custom item
  if (it->m_type == IT_CUSTOM)
  {
    int save = dc.SaveDC();
    it->m_pCustom->DrawItem(dc, it->m_rcValue, m_focused_item == it->m_id);
    dc.RestoreDC(save);
  }
  else
  {
    // modified flag
    bool modified = (it->m_strValue != it->m_strValue_old);

    // now draw text
    string strValue = it->m_strValue;
    switch (it->m_type)
    {
    case IT_TEXT:
      {
        size_t j;
        for (;( j = strValue.find( "\r\n" )) != string::npos;)
          strValue.replace( j, 2, "�");
        break;
      }

    case IT_INTEGER:
      {
        CString strTemp;
        string strFormat = "%d";
        if (it->m_options.size() && !it->m_options.front().empty()) strFormat = it->m_options.front();
        strTemp.Format(strFormat.c_str(), it->m_nValue);
        strValue = LPCTSTR(strTemp);
        modified = (it->m_nValue != it->m_nValue_old);
        break;
      }

    case IT_DOUBLE:
      {
        CString strTemp;
        string strFormat = "%g";
        if (it->m_options.size() && !it->m_options.front().empty()) strFormat = it->m_options.front();
        strTemp.Format(strFormat.c_str(), it->m_dValue);
        strValue = LPCTSTR(strTemp);
        modified = (it->m_dValue != it->m_dValue_old);
        break;
      }

    case IT_DATE:
      {
        CString strTemp;
        if (it->m_options.size() && !it->m_options.front().empty()) strTemp = it->m_dtValue.Format(it->m_options.front().c_str());
        else strTemp = it->m_dtValue.Format(VAR_DATEVALUEONLY);
        strValue = LPCTSTR(strTemp);
        modified = (it->m_dtValue != it->m_dtValue_old);
        break;
      }

    case IT_DATETIME:
      {
        CString strTemp;
        if (it->m_options.size() && !it->m_options.front().empty()) strTemp = it->m_dtValue.Format(it->m_options.front().c_str());
        else strTemp = it->m_dtValue.Format();
        strValue = LPCTSTR(strTemp);
        modified = (it->m_dtValue != it->m_dtValue_old);
        break;
      }

    case IT_BOOLEAN:
      {
        strValue = it->m_bValue ? m_strTrue : m_strFalse;
        modified = (it->m_bValue != it->m_bValue_old);
        break;
      }

    case IT_COMBO:
      {
        if (it->m_nValue>=0 && it->m_nValue<int(it->m_options.size()))
          strValue = it->m_options[it->m_nValue];
        modified = (it->m_nValue != it->m_nValue_old);
        break;
      }

    case IT_FILE:
    case IT_FOLDER:
      {
        TCHAR szBuffer[1024];
        _tcsncpy(szBuffer, strValue.c_str(), 1024);
        PathCompactPath(dc.GetSafeHdc(), szBuffer, rcValue.Width());
        strValue = szBuffer;
        break;
      }

    case IT_COLOR:
      {
        // draw a sample rectangle
        CRect rc = rcValue;
        rc.DeflateRect(0,2,0,2);
        rc.top++;
        rc.right = rc.left + m_line_height;
        dc.FrameRect(rc, &brushText);
        rc.DeflateRect(1,1);
        dc.FillSolidRect(rc, it->m_clrValue);
        rcValue.left = rc.right + 3*margin;

        // update the text
        CString strTemp;
        strTemp.Format("%d; %d; %d", GetRValue(it->m_clrValue), GetGValue(it->m_clrValue), GetBValue(it->m_clrValue));
        strValue = LPCTSTR(strTemp);
        modified = (it->m_clrValue != it->m_clrValue_old);
        break;
      }

    case IT_FONT:
      {
        CString strTemp;
        strTemp.Format("%s; %dpt", it->m_lfValue.lfFaceName, -MulDiv(it->m_lfValue.lfHeight, 72, dc.GetDeviceCaps(LOGPIXELSY)));
        strValue = LPCTSTR(strTemp);
        modified = (memcmp(&it->m_lfValue, &it->m_lfValue_old, sizeof(LOGFONT))!=0);
        break;
      }
    }

    // we must also take undefined state change into account
    modified |= (it->m_undefined != it->m_undefined_old);

    // set proper font
    if (modified && m_bold_modified) dc.SelectObject(&m_fntBold);
    else if (it->m_editable && m_bold_editables) dc.SelectObject(&m_fntBold);
    else dc.SelectObject(&m_fntNormal);

    // now draw it
    if (it->m_undefined) strValue = m_strUndefined;
    dc.DrawText(strValue.c_str(), -1, rcValue, DT_END_ELLIPSIS|DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_NOPREFIX);
  }

  // clean up
  dc.SelectObject(pOldFont);
  dc.SelectObject(pOldPen);
  dc.SelectObject(pOldBrush);
}

//
// mouse interaction
//

void CPropertyGrid::OnLButtonDown(UINT nFlags, CPoint point)
{
  // destroy edit
  SetFocus();
  DeleteEditControl();

  // click on button?
  if (m_rect_button.PtInRect(point))
  {
    m_button_pushed = true;
    m_button_depressed = true;
    SetCapture();
    Invalidate();
    return;
  }

  // click on button?
  if (m_focused_item != -1)
  {
    CItem* pItem = FindItem(m_focused_item);
    if (   pItem && pItem->m_type == IT_CUSTOM
      && GetEditMode(*pItem) == EM_CUSTOM
      && pItem->m_pCustom->OnLButtonDown(pItem->m_rcValue, point))
    {
      m_custom_tracking = true;
      SetCapture();
      Invalidate();
      return;
    }
  }

  // resizing gutter?
  if (abs(point.x-m_gutter_width)<3)
  {
    ::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEWE));
    m_resizing_gutter = true;
    m_ptLast = point;
    SetCapture();
    Invalidate();
    return;
  }

  // disable focus
  m_focused_item = -1;
  m_focused_section = -1;
  m_rect_button.SetRectEmpty();

  // did we click on a section
  if (m_display_mode == DM_CATEGORIZED)
  {
    for (vector<CSection>::iterator it = m_sections.begin(); it != m_sections.end(); ++it)
    {
      if (it->m_rcSign.PtInRect(point))
      {
        it->m_collapsed = !it->m_collapsed;
        m_focused_section = it->m_id;
        RecalcLayout();
        return;
      }
      else if (it->m_rcTitle.PtInRect(point))
      {
        m_focused_section = it->m_id;
        Invalidate();
        return;
      }
    }
  }

  // focus
  for (vector<CSection>::iterator it = m_sections.begin(); it != m_sections.end(); ++it)
  {
    if (!it->m_collapsed || m_display_mode != DM_CATEGORIZED)
    {
      for (vector<CItem>::iterator it2 = it->m_items.begin(); it2 != it->m_items.end(); ++it2)
      {
        if (it2->m_rcName.PtInRect(point) || it2->m_rcValue.PtInRect(point))
        {
          if (it2->m_editable || m_focus_disabled)
          {
            m_focused_item = it2->m_id;
            GetOwner()->SendMessage(WM_PG_SELECTIONCHANGED, it2->m_id);
            if (it2->m_rcValue.PtInRect(point))
              m_value_clicked = (GetEditMode(*it2) == EM_INPLACE || GetEditMode(*it2) == EM_DROPDOWN);
            Invalidate();
            return;
          }
        }
      }
    }
  }

  CWnd::OnLButtonDown(nFlags, point);
  Invalidate();
}

void CPropertyGrid::OnLButtonDblClk(UINT nFlags, CPoint point)
{
  if (m_focused_item != -1)
  {
    CItem* pItem = FindItem(m_focused_item);
    if (pItem)
    {
      if (pItem->m_type == IT_BOOLEAN)
      {
        if (!pItem->m_undefined)
        {
          pItem->m_bValue = !pItem->m_bValue;
          GetOwner()->SendMessage(WM_PG_ITEMCHANGED, pItem->m_id);
          Invalidate();
        }
      }
      else if (pItem->m_type == IT_COMBO)
      {
        if (!pItem->m_undefined)
        {
          pItem->m_nValue = (pItem->m_nValue+1)%int(pItem->m_options.size());
          GetOwner()->SendMessage(WM_PG_ITEMCHANGED, pItem->m_id);
          Invalidate();
        }
      }
      else if (GetEditMode(*pItem) == EM_MODAL)
      {
        EditFocusedItem();
      }
    }
  }
  else if (m_focused_section != -1)
  {
    CSection* pSection = FindSection(m_focused_section);
    if (pSection)
    {
      pSection->m_collapsed = !pSection->m_collapsed;
      Invalidate();
    }
  }

  CWnd::OnLButtonDblClk(nFlags, point);
}

void CPropertyGrid::OnMouseMove(UINT nHitTest, CPoint point)
{
  if (m_custom_tracking)
  {
    CItem* pItem = FindItem(m_focused_item);
    if (pItem)
    {
      pItem->m_pCustom->OnMouseMove(pItem->m_rcValue, point);
      Invalidate();
    }
  }
  else if (m_button_pushed)
  {
    m_button_depressed = m_rect_button.PtInRect(point)?true:false;
    Invalidate();
  }
  else if (m_resizing_gutter)
  {
    ::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEWE));
    m_gutter_width += point.x-m_ptLast.x;
    CRect rc;
    GetClientRect(&rc);
    if (m_gutter_width<rc.Width()/5) m_gutter_width = rc.Width()/5;
    if (m_gutter_width>4*rc.Width()/5) m_gutter_width = 4*rc.Width()/5;
    m_ptLast = point;
    Invalidate();
  }
  else if (!m_control)
  {
    if (abs(point.x-m_gutter_width)<3) ::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEWE));
    else ::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
  }

  CWnd::OnMouseMove(nHitTest, point);
}

void CPropertyGrid::OnLButtonUp(UINT nFlags, CPoint point)
{
  if (m_custom_tracking)
  {
    m_custom_tracking = false;
    ReleaseCapture();
    Invalidate();
    CItem* pItem = FindItem(m_focused_item);
    if (pItem)
      pItem->m_pCustom->OnLButtonUp(pItem->m_rcValue, point);
  }
  else if (m_button_pushed || m_value_clicked)
  {
    m_button_pushed = false;
    m_button_depressed = false;
    ReleaseCapture();
    Invalidate();

    if (m_rect_button.PtInRect(point) || (m_value_clicked && m_focused_item != -1 && FindItem(m_focused_item) && FindItem(m_focused_item)->m_rcValue.PtInRect(point)))
    {
      m_value_clicked = false;
      CItem* pItem = FindItem(m_focused_item);
      if (pItem)
      {
        if (GetEditMode(*pItem) == EM_DROPDOWN)
        {
          if (pItem->m_type == IT_CUSTOM)
          {
            CRect rc = m_rect_button;
            rc.left = m_gutter_width;
            pItem->m_pCustom->ShowDropDown(rc);
          }
          else if (pItem->m_type == IT_DATE)
          {
            // the calendar rect
            CRect rc = m_rect_button;
            rc.left = m_gutter_width;
            rc.top += m_line_height;
            rc.bottom = rc.top + 100;
            ClientToScreen(&rc);

            // create it
            m_control = new CPropertyGridMonthCalCtrl;
            CPropertyGridMonthCalCtrl* mc = (CPropertyGridMonthCalCtrl*) m_control;
            mc->CreateEx(0, MONTHCAL_CLASS, NULL, WS_POPUP|WS_BORDER, rc, GetParent(), 0);
            mc->SetCurSel(pItem->m_dtValue);
            mc->SetOwner(this);
            mc->SizeMinReq();

            // now position it
            CRect rc2;
            mc->GetWindowRect(&rc2);
            rc2.OffsetRect(rc.right-rc2.right, 0);
            mc->SetWindowPos(NULL, rc2.left, rc2.top, 0, 0, SWP_NOZORDER|SWP_NOSIZE|SWP_SHOWWINDOW);
          }
          else
          {
            // the combo rect
            CRect rc = m_rect_button;
            rc.left = m_gutter_width;
            rc.top += m_line_height;
            rc.bottom = rc.top + 100;

            // create it
            m_control = new CPropertyGridCombo();
            CPropertyGridCombo* pCombo = (CPropertyGridCombo*)m_control;
            pCombo->Create(WS_CHILD, rc, this, 0);
            pCombo->SetColors(m_clrBack, m_clrText, m_clrFocus, m_clrHilite);
            pCombo->SetFont(&m_fntNormal);

            if (pItem->m_type == IT_BOOLEAN)
            {
              pCombo->AddString(m_strTrue);
              pCombo->AddString(m_strFalse);
              if (!pItem->m_undefined)
                pCombo->SetCurSel(pItem->m_bValue?0:1);
            }
            else
            {
              for (vector<string>::iterator it = pItem->m_options.begin(); it != pItem->m_options.end(); ++it)
                pCombo->AddString(*it);
              if (!pItem->m_undefined)
                pCombo->SetCurSel(pItem->m_nValue);
            }
            pCombo->ShowWindow(SW_SHOW);
          }
        }
        else if (GetEditMode(*pItem) == EM_INPLACE)
        {
          // the in-place edit rect
          CRect rc = m_rect_button;
          rc.left++;
          rc.top += margin;

          // the value
          string strValue;
          if (pItem->m_type == IT_STRING)
          {
            strValue = pItem->m_strValue;
          }
          else if (pItem->m_type == IT_INTEGER)
          {
            if (!pItem->m_undefined)
            {
              CString strTemp;
              strTemp.Format("%d", pItem->m_nValue);
              strValue = LPCTSTR(strTemp);
            }
          }
          else if (pItem->m_type == IT_DOUBLE)
          {
            if (!pItem->m_undefined)
            {
              CString strTemp;
              strTemp.Format("%g", pItem->m_dValue);
              strValue = LPCTSTR(strTemp);
            }
          }
          else if (pItem->m_type == IT_CUSTOM)
          {
            strValue = pItem->m_pCustom->GetStringForInPlaceEdit();
          }
          else
          {
            assert(false);
          }

          // create it
          m_control = new CPropertyGridInPlaceEdit(this, rc, WS_CHILD, 1000, strValue);
          CPropertyGridInPlaceEdit* pEdit = (CPropertyGridInPlaceEdit*)m_control;
          pEdit->SetColors(m_clrBack, m_clrText);
          pEdit->SetFont(&m_fntNormal);
          pEdit->ShowWindow(SW_SHOW);
        }
        else if (GetEditMode(*pItem) == EM_MODAL)
        {
          EditFocusedItem();
        }
        else if (GetEditMode(*pItem) == EM_CUSTOM)
        {
          pItem->m_pCustom->OnLButtonUp(pItem->m_rcValue, point);
        }
        else
        {
          assert(false);
        }
      }
    }
  }
  else if (m_resizing_gutter)
  {
    ReleaseCapture();
    m_resizing_gutter = false;
    ::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
  }

  CWnd::OnLButtonUp(nFlags, point);
}

//
// keyboard interaction
//

UINT CPropertyGrid::OnGetDlgCode()
{
  return CWnd::OnGetDlgCode()|DLGC_WANTCHARS|DLGC_WANTARROWS;
}

void CPropertyGrid::MoveForward(HSECTION& focused_section, HITEM& focused_item)
{
  for (int pass = 0; pass<2; pass++)
  {
    bool found = false;

    bool stop_on_next_valid = false;
    if (focused_section == -1 && focused_item == -1)
      stop_on_next_valid = true;

    for (vector<CSection>::iterator it = m_sections.begin(); it != m_sections.end(); ++it)
    {
      if (m_display_mode == DM_CATEGORIZED)
      {
        if (it->m_id == focused_section)
        {
          stop_on_next_valid = true;
        }
        else if (stop_on_next_valid)
        {
          focused_section = it->m_id;
          focused_item = -1;
          found = true;
          break;
        }
      }

      if (!it->m_collapsed || m_display_mode != DM_CATEGORIZED)
      {
        for (vector<CItem>::iterator it2 = it->m_items.begin(); it2 != it->m_items.end(); ++it2)
        {
          if (it2->m_id == focused_item)
          {
            stop_on_next_valid = true;
          }
          else if (stop_on_next_valid)
          {
            if (it2->m_editable || m_focus_disabled)
            {
              focused_section = -1;
              focused_item = it2->m_id;
              found = true;
              break;
            }
          }
        }

        if (found)
          break;
      }
    }

    if (found)
      break;

    focused_section = -1;
    focused_item = -1;
  }
}

void CPropertyGrid::FocusNextItem()
{
  // simple move forward
  MoveForward(m_focused_section, m_focused_item);
  GetOwner()->SendMessage(WM_PG_SELECTIONCHANGED, m_focused_item);

  // ensure visible
  CRect rc(0,0,0,0);
  if (m_focused_section != -1 && FindSection(m_focused_section)) rc = FindSection(m_focused_section)->m_rcTitle;
  else if (m_focused_item != -1 && FindItem(m_focused_item)) rc = FindItem(m_focused_item)->m_rcName;
  if (!rc.IsRectEmpty())
  {
    CRect rect;
    GetClientRect(&rect);
    rect.IntersectRect(rc, rect);
    if (rect.Height() != m_line_height)
      OnVScroll(SB_THUMBPOSITION, rc.top, &m_scrollbar);
  }

  // done
  Invalidate();
}

void CPropertyGrid::FocusPrevItem()
{
  for (vector<CSection>::iterator it = m_sections.begin(); it != m_sections.end(); ++it)
  {
    if (m_display_mode == DM_CATEGORIZED)
    {
      HSECTION focused_section = it->m_id;
      HITEM focused_item = -1;
      MoveForward(focused_section, focused_item);
      if (focused_section == m_focused_section && focused_item == m_focused_item)
      {
        m_focused_section = it->m_id;
        m_focused_item = -1;
        break;
      }
    }

    if (!it->m_collapsed || m_display_mode != DM_CATEGORIZED)
    {
      bool found = false;
      for (vector<CItem>::iterator it2 = it->m_items.begin(); it2 != it->m_items.end(); ++it2)
      {
        if (!it2->m_editable && !m_focus_disabled)
          continue;

        HSECTION focused_section = -1;
        HITEM focused_item = it2->m_id;
        MoveForward(focused_section, focused_item);
        if (focused_section == m_focused_section && focused_item == m_focused_item)
        {
          m_focused_section = -1;
          m_focused_item = it2->m_id;
          GetOwner()->SendMessage(WM_PG_SELECTIONCHANGED, it2->m_id);
          found = true;
          break;
        }
      }

      if (found)
        break;
    }
  }

  // ensure visible
  CRect rc(0,0,0,0);
  if (m_focused_section != -1 && FindSection(m_focused_section)) rc = FindSection(m_focused_section)->m_rcTitle;
  else if (m_focused_item != -1 && FindItem(m_focused_item)) rc = FindItem(m_focused_item)->m_rcName;
  if (!rc.IsRectEmpty())
  {
    CRect rect;
    GetClientRect(&rect);
    rect.IntersectRect(rc, rect);
    if (rect.Height() != m_line_height)
      OnVScroll(SB_THUMBPOSITION, rc.top, &m_scrollbar);
  }

  // done
  Invalidate();
}

void CPropertyGrid::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
  if (nChar == '*')
  {
    ExpandAll(true);
  }
  else if (nChar == '/')
  {
    ExpandAll(false);
  }
  else if (nChar == '+' || nChar == '-')
  {
    if (m_focused_section != -1)
    {
      CSection* pSection = FindSection(m_focused_section);
      if (pSection) pSection->m_collapsed = (nChar=='-');
      RecalcLayout();
    }
  }

  CWnd::OnChar(nChar, nRepCnt, nFlags);
}

void CPropertyGrid::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
  if (nChar == VK_DOWN)
  {
    FocusNextItem();
  }
  else if (nChar == VK_UP)
  {
    FocusPrevItem();
  }
  else if (nChar == VK_LEFT)
  {
    if (m_focused_section != -1 && FindSection(m_focused_section) && FindSection(m_focused_section)->m_collapsed == false)
    {
      ExpandSection(m_focused_section, false);
    }
    else
    {
      FocusPrevItem();
    }
  }
  else if (nChar == VK_RIGHT)
  {
    if (m_focused_section != -1 && FindSection(m_focused_section) && FindSection(m_focused_section)->m_collapsed == true)
    {
      ExpandSection(m_focused_section, true);
    }
    else
    {
      FocusNextItem();
    }
  }

  CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

//
// scrolling
//

void CPropertyGrid::RecalcLayout()
{
  // save current scroll offset
  int offset = GetScrollOffset();

  // total height
  int height = 0;
  for (vector<CSection>::iterator it = m_sections.begin(); it != m_sections.end(); ++it)
  {
    if (m_display_mode == DM_CATEGORIZED)
      height += m_line_height;
    if (!it->m_collapsed || m_display_mode != DM_CATEGORIZED)
      height += int(it->m_items.size())*m_line_height;
  }

  // client rect
  CRect rc;
  GetClientRect(&rc);
  if (height < rc.Height())
  {
    if (m_scrollbar.GetSafeHwnd() != NULL)
    {
      m_scrollbar.EnableScrollBar(ESB_DISABLE_BOTH);
      m_scrollbar.ShowScrollBar(FALSE);
    }
    m_scroll_enabled = false;
  }
  else
  {
    if (m_scrollbar.GetSafeHwnd() == NULL)
    {
      CRect rect = rc;
      rect.left = rect.right - GetSystemMetrics(SM_CXVSCROLL);
      m_scrollbar.Create(WS_CHILD|SBS_VERT, rect, this, 1000);
    }

    m_scrollbar.EnableScrollBar(ESB_ENABLE_BOTH);

    SCROLLINFO info;
    info.cbSize = sizeof(SCROLLINFO);
    info.fMask = SIF_ALL;
    info.nMin = 0;
    info.nMax = height;
    info.nPage = rc.Height();
    info.nPos = min(offset, height);
    info.nTrackPos = 2;
    m_scrollbar.SetScrollInfo(&info);

    m_scrollbar.ShowScrollBar();
    m_scroll_enabled = true;
  }

  if (GetSafeHwnd())
    Invalidate();
}

int CPropertyGrid::GetScrollOffset()
{
  if (m_scrollbar && m_scrollbar.IsWindowEnabled() == TRUE)
    return m_scrollbar.GetScrollPos();
  return 0;
}

void CPropertyGrid::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
  // check
  if (!m_scroll_enabled) return;
  if (pScrollBar != &m_scrollbar) return;
  if (nSBCode == SB_ENDSCROLL) return;

  // set focus to us
  SetFocus();

  // get the scroll info
  SCROLLINFO info;
  info.cbSize = sizeof(SCROLLINFO);
  info.fMask = SIF_ALL;
  m_scrollbar.GetScrollInfo(&info);
  int min = info.nMin;
  int pos = info.nPos;
  int max = info.nMax-info.nPage;

  // the entire rect
  CRect rect;
  GetClientRect(&rect);
  int h = rect.Height();

  // the rect without the scrollbar
  CRect rc(0,0,rect.right-GetSystemMetrics(SM_CXVSCROLL),rect.bottom);

  switch(nSBCode)
  {
  case SB_TOP:
    pScrollBar->SetScrollPos(min);
    break;

  case SB_BOTTOM:
    pScrollBar->SetScrollPos(max);
    break;

  case SB_LINEDOWN:
    if (pos+m_line_height>=max) pScrollBar->SetScrollPos(max);
    else pScrollBar->SetScrollPos(pos+m_line_height);
    break;

  case SB_LINEUP:
    if (pos-m_line_height<=min) pScrollBar->SetScrollPos(min);
    else pScrollBar->SetScrollPos(pos-m_line_height);
    break;

  case SB_PAGEDOWN:
    if (pos+h>=max) pScrollBar->SetScrollPos(max);
    else pScrollBar->SetScrollPos(pos+h);
    break;

  case SB_PAGEUP:
    if (pos-h<=min) pScrollBar->SetScrollPos(min);
    else pScrollBar->SetScrollPos(pos-h);
    break;

  case SB_THUMBPOSITION:
  case SB_THUMBTRACK:
    int diff = nPos - pos;
    if (diff == 0) return;
    if (pos <= min && diff<0) return;
    if (pos >= max && diff>0) return;
    pScrollBar->SetScrollPos(nPos);
  }

  Invalidate();

  CWnd::OnVScroll(nSBCode, nPos, pScrollBar);

}

BOOL CPropertyGrid::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
  /*int steps = abs(zDelta)/WHEEL_DELTA;
  for (int i=0; i<3*steps; i++)
  {
    if (zDelta>0) OnVScroll(SB_LINEUP, 0, &m_scrollbar);
    if (zDelta<0) OnVScroll(SB_LINEDOWN, 0, &m_scrollbar);
  }*/
  if (zDelta>0) OnVScroll(SB_LINEUP, 0, &m_scrollbar);
  if (zDelta<0) OnVScroll(SB_LINEDOWN, 0, &m_scrollbar);

  return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}

//
// editing
//

CPropertyGrid::EEditMode CPropertyGrid::GetEditMode(CItem& item)
{
  switch (item.m_type)
  {
  case IT_CUSTOM:
    return item.m_pCustom->GetEditMode();

  case IT_STRING:
  case IT_INTEGER:
  case IT_DOUBLE:
    return EM_INPLACE;

  case IT_COMBO:
  case IT_BOOLEAN:
  case IT_DATE:
    return EM_DROPDOWN;

  case IT_TEXT:
  case IT_DATETIME:
  case IT_FILE:
  case IT_FOLDER:
  case IT_COLOR:
  case IT_FONT:
    return EM_MODAL;

  default:
    assert(false);
    return EM_CUSTOM;
  }
}

void CPropertyGrid::DeleteEditControl()
{
  // destroy edit
  if (m_control)
  {
    if (m_control->GetSafeHwnd())
      m_control->DestroyWindow();
    delete m_control;
    m_control = NULL;
  }
}

LRESULT CPropertyGrid::OnComboSelChanged(WPARAM wParam, LPARAM lParam)
{
  CItem* pItem = FindItem(m_focused_item);
  if (pItem)
  {
    if (pItem->m_type == IT_BOOLEAN)
    {
      pItem->m_bValue = (wParam == 0);
      pItem->m_undefined = false;
      GetOwner()->SendMessage(WM_PG_ITEMCHANGED, pItem->m_id);
      DeleteEditControl();
      Invalidate();
    }
    else if (pItem->m_type == IT_COMBO)
    {
      pItem->m_nValue = int(wParam);
      pItem->m_undefined = false;
      GetOwner()->SendMessage(WM_PG_ITEMCHANGED, pItem->m_id);
      DeleteEditControl();
      Invalidate();
    }
    else
    {
      assert(false);
    }
  }
  return 0;
}

LRESULT CPropertyGrid::OnEditChanged(WPARAM wParam, LPARAM lParam)
{
  CItem* pItem = FindItem(m_focused_item);
  if (pItem)
  {
    if (pItem->m_type == IT_STRING)
    {
      pItem->m_strValue = string((char*)wParam);
      pItem->m_undefined = false;
      GetOwner()->SendMessage(WM_PG_ITEMCHANGED, pItem->m_id);
      DeleteEditControl();
      Invalidate();
    }
    else if (pItem->m_type == IT_INTEGER)
    {
      if (strlen((char*)wParam))
      {
        pItem->m_nValue = atoi((char*)wParam);
        pItem->m_undefined = false;
        GetOwner()->SendMessage(WM_PG_ITEMCHANGED, pItem->m_id);
      }
      DeleteEditControl();
      Invalidate();
    }
    else if (pItem->m_type == IT_DOUBLE)
    {
      if (strlen((char*)wParam))
      {
        pItem->m_dValue = atof((char*)wParam);
        pItem->m_undefined = false;
        GetOwner()->SendMessage(WM_PG_ITEMCHANGED, pItem->m_id);
      }
      DeleteEditControl();
      Invalidate();
    }
    else if (pItem->m_type == IT_CUSTOM)
    {
      if (pItem->m_pCustom->OnItemEdited(string((char*)wParam)))
        GetOwner()->SendMessage(WM_PG_ITEMCHANGED, pItem->m_id);
      DeleteEditControl();
      Invalidate();
    }
    else
    {
      assert(false);
    }
  }
  return 0;
}

LRESULT CPropertyGrid::OnDateChanged(WPARAM wParam, LPARAM lParam)
{
  CItem* pItem = FindItem(m_focused_item);
  if (pItem)
  {
    if (pItem->m_type == IT_DATE)
    {
      CPropertyGridMonthCalCtrl* mc = (CPropertyGridMonthCalCtrl*) m_control;
      mc->GetCurSel(pItem->m_dtValue);
      pItem->m_undefined = false;
      GetOwner()->SendMessage(WM_PG_ITEMCHANGED, pItem->m_id);
      DeleteEditControl();
      Invalidate();

    }
    else
    {
      assert(false);
    }
  }
  return 0;
}

void CPropertyGrid::EditFocusedItem()
{
  CItem* pItem = FindItem(m_focused_item);
  if (pItem)
  {
    if (!pItem->m_editable)
      return;

    if (pItem->m_type == IT_TEXT)
    {
      CDynDialogEx dlg(GetParent());
      dlg.SetUseSystemButtons(FALSE);
      dlg.SetWindowTitle(pItem->m_name.c_str());
      dlg.SetFont(&m_fntNormal);
      CString strValue = pItem->m_strValue.c_str();
      dlg.AddDlgControl("EDIT", pItem->m_strValue.c_str(), STYLE_EDIT|WS_VSCROLL|WS_HSCROLL|ES_AUTOHSCROLL|ES_AUTOVSCROLL|ES_LEFT|ES_MULTILINE|ES_WANTRETURN, EXSTYLE_EDIT, CRect(7, 7, 200, 100), (void*) &strValue);
      dlg.AddDlgControl("BUTTON", m_strOk.c_str(), STYLE_BUTTON, EXSTYLE_BUTTON, CRect(56, 106, 106, 120), NULL, IDOK);
      dlg.AddDlgControl("BUTTON", m_strCancel.c_str(), STYLE_BUTTON, EXSTYLE_BUTTON, CRect(110, 106, 160, 120), NULL, IDCANCEL);
      if (dlg.DoModal() == IDOK)
      {
        pItem->m_strValue = LPCTSTR(strValue);
        pItem->m_undefined = false;
        GetOwner()->SendMessage(WM_PG_ITEMCHANGED, pItem->m_id);
        Invalidate();
      }
    }
    else if (pItem->m_type == IT_DATETIME)
    {
      CDynDialogEx dlg(GetParent());
      dlg.SetUseSystemButtons(FALSE);
      dlg.SetWindowTitle(pItem->m_name.c_str());
      dlg.SetFont(&m_fntNormal);
      COleDateTime dtValueDate = pItem->m_dtValue;
      CTime dtValueTime(pItem->m_dtValue.GetYear(), pItem->m_dtValue.GetMonth(), pItem->m_dtValue.GetDay(), pItem->m_dtValue.GetHour(), pItem->m_dtValue.GetMinute(), pItem->m_dtValue.GetSecond());
      dlg.AddDlgControl("STATIC", m_strDate.c_str(), STYLE_STATIC, EXSTYLE_STATIC, CRect(7, 3, 60, 12));
      dlg.AddDlgControl("STATIC", m_strTime.c_str(), STYLE_STATIC, EXSTYLE_STATIC, CRect(67, 3, 120, 12));
      dlg.AddDlgControl("SysDateTimePick32", "", STYLE_DATETIMEPICKER|DTS_SHORTDATEFORMAT, EXSTYLE_DATETIMEPICKER, CRect(7, 13, 60, 26), (void*) &dtValueDate);
      dlg.AddDlgControl("SysDateTimePick32", "", STYLE_DATETIMEPICKER|DTS_TIMEFORMAT , EXSTYLE_DATETIMEPICKER, CRect(67, 13, 120, 26), (void*) &dtValueTime);
      dlg.AddDlgControl("BUTTON", m_strOk.c_str(), STYLE_BUTTON, EXSTYLE_BUTTON, CRect(7, 37, 60, 51), NULL, IDOK);
      dlg.AddDlgControl("BUTTON", m_strCancel.c_str(), STYLE_BUTTON, EXSTYLE_BUTTON, CRect(67, 37, 120, 51), NULL, IDCANCEL);
      if (dlg.DoModal() == IDOK)
      {
        pItem->m_dtValue.SetDateTime(dtValueDate.GetYear(), dtValueDate.GetMonth(), dtValueDate.GetDay(),
          dtValueTime.GetHour(), dtValueTime.GetMinute(), dtValueTime.GetSecond());
        pItem->m_undefined = false;
        GetOwner()->SendMessage(WM_PG_ITEMCHANGED, pItem->m_id);
        Invalidate();
      }
    }
    else if (pItem->m_type == IT_COLOR)
    {
      CColorDialog dlg(pItem->m_clrValue, 0, GetParent());
      if (dlg.DoModal() == IDOK)
      {
        pItem->m_clrValue = dlg.GetColor();
        pItem->m_undefined = false;
        GetOwner()->SendMessage(WM_PG_ITEMCHANGED, pItem->m_id);
        Invalidate();
      }
    }
    else if (pItem->m_type == IT_FILE)
    {
      CFileDialog dlg(TRUE, NULL, pItem->m_strValue.c_str(), OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, pItem->m_options.front().c_str(), GetParent());
      if (dlg.DoModal() == IDOK)
      {
        pItem->m_strValue = dlg.GetPathName();
        pItem->m_undefined = false;
        GetOwner()->SendMessage(WM_PG_ITEMCHANGED, pItem->m_id);
        Invalidate();
      }
    }
    else if (pItem->m_type == IT_FOLDER)
    {
      CPropertyGridDirectoryPicker::m_strTitle = pItem->m_options.front();
      if (CPropertyGridDirectoryPicker::PickDirectory(pItem->m_strValue, GetParent()->GetSafeHwnd()))
      {
        pItem->m_undefined = false;
        GetOwner()->SendMessage(WM_PG_ITEMCHANGED, pItem->m_id);
        Invalidate();
      }
    }
    else if (pItem->m_type == IT_FONT)
    {
      CFontDialog dlg(&pItem->m_lfValue, CF_EFFECTS|CF_SCREENFONTS, NULL, GetParent());
      if (dlg.DoModal() == IDOK)
      {
        memcpy(&pItem->m_lfValue, dlg.m_cf.lpLogFont, sizeof(LOGFONT));
        pItem->m_undefined = false;
        GetOwner()->SendMessage(WM_PG_ITEMCHANGED, pItem->m_id);
        Invalidate();
      }
    }
    else if (pItem->m_type == IT_CUSTOM)
    {
      if (pItem->m_pCustom->OnEditItem())
      {
        GetOwner()->SendMessage(WM_PG_ITEMCHANGED, pItem->m_id);
        Invalidate();
      }
    }
    else
    {
      assert(false);
    }
  }
}
