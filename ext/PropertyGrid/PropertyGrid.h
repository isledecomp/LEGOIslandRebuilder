#pragma once
#include "PropertyGridCombo.h"
#include "PropertyGridInPlaceEdit.h"

#include <map>
#include <vector>
#include <string>
using namespace std;

// CPropertyGrid

#define WM_PG_ITEMCHANGED WM_USER+486
#define WM_PG_SELECTIONCHANGED WM_USER+487

typedef UINT HSECTION;
typedef UINT HITEM;

class ICustomItem;

class CPropertyGrid : public CWnd
{
  DECLARE_DYNAMIC(CPropertyGrid)

public:
  // display mode
  enum EDisplayMode
  {
    DM_CATEGORIZED = 0,
    DM_ALPHABETICAL,
    DM_NOSORT
  };

  // editing
  enum EEditMode
  {
    EM_CUSTOM = 0,
    EM_INPLACE,
    EM_DROPDOWN,
    EM_MODAL
  };

  enum EItemType
  {
    IT_CUSTOM = 0,
    IT_STRING,
    IT_TEXT,
    IT_INTEGER,
    IT_DOUBLE,
    IT_COMBO,
    IT_BOOLEAN,
    IT_DATE,
    IT_DATETIME,
    IT_FILE,
    IT_FOLDER,
    IT_COLOR,
    IT_FONT
  };

public:
  CPropertyGrid();
  virtual ~CPropertyGrid();

  // customization
  bool GetShadeTitles();
  void SetShadeTitles(bool shade_titles);
  bool GetDrawLines();
  void SetDrawLines(bool draw_lines);
  bool GetDrawGutter();
  void SetDrawGutter(bool draw_gutter);
  bool GetFocusDisabled();
  void SetFocusDisabled(bool focus_disabled);
  bool GetBoldModified();
  void SetBoldModified(bool bold_modified);
  bool GetBoldEditables();
  void SetBoldEditables(bool bold_editables);

  // gutter width
  int GetGutterWidth();
  void SetGutterWidth(int gutter_width);

  // custom colors
  void SetTextColor(COLORREF clrText);
  void SetTitleColor(COLORREF clrText);
  void SetBackColor(COLORREF clrBack);
  void SetShadeColor(COLORREF clrShade);
  void SetFocusColor(COLORREF clrFocus);
  void SetHiliteColor(COLORREF clrHilite);
  void SetEditableColor(COLORREF clrEditable);
  void SetDisabledColor(COLORREF clrDisabled);

  // localization
  void SetTrueFalseStrings(string strTrue, string strFalse);
  void SetOkCancelStrings(string strOk, string strCancel);
  void SetDateTimeStrings(string strDate, string strTime);
  void SetUndefinedString(string strUndefined);
  void SetEmptyString(string strEmpty);

  // add a section
  HSECTION AddSection(string title, bool collapsed = false, HSECTION after = -1);

  // add items
  HITEM AddCustomItem(HSECTION, string name, ICustomItem* pItem, bool editable = true, HITEM after = -1);
  HITEM AddStringItem(HSECTION section, string name, string value, bool editable = true, HITEM after = -1);
  HITEM AddTextItem(HSECTION section, string name, string value, bool editable = true, HITEM after = -1);
  HITEM AddIntegerItem(HSECTION section, string name, int value, string format = "", bool editable = true, bool undefined = false, HITEM after = -1);
  HITEM AddDoubleItem(HSECTION section, string name, double value, string format = "", bool editable = true, bool undefined = false, HITEM after = -1);
  HITEM AddComboItem(HSECTION section, string name, const vector<string>& values, int cur, bool editable = true, bool undefined = false, HITEM after = -1);
  HITEM AddBoolItem(HSECTION section, string name, bool value, bool editable = true, bool undefined = false, HITEM after = -1);
  HITEM AddDateItem(HSECTION section, string name, COleDateTime value, string format = "", bool editable = true, bool undefined = false, HITEM after = -1);
  HITEM AddDateTimeItem(HSECTION section, string name, COleDateTime value, string format = "", bool editable = true, bool undefined = false, HITEM after = -1);
  HITEM AddFileItem(HSECTION section, string name, string value, string filter = "", bool editable = true, HITEM after = -1);
  HITEM AddFolderItem(HSECTION section, string name, string value, string title = "", bool editable = true, HITEM after = -1);
  HITEM AddColorItem(HSECTION section, string name, COLORREF value, bool editable = true, bool undefined = false, HITEM after = -1);
  HITEM AddFontItem(HSECTION section, string name, LOGFONT value, bool editable = true, bool undefined = false, HITEM after = -1);

  // contents
  void ResetContents();
  bool RemoveSection(HSECTION hs);
  bool RemoveItem(HITEM item);
  void ValidateChanges();

  // status
  int GetNumSections();
  int GetSectionSize(HSECTION hs);

  // get item value
  bool GetItemValue(HITEM item, string& strValue) const;
  bool GetItemValue(HITEM item, int& nValue) const;
  bool GetItemValue(HITEM item, double& dValue) const;
  bool GetItemValue(HITEM item, bool& bValue) const;
  bool GetItemValue(HITEM item, COleDateTime& dtValue) const;
  bool GetItemValue(HITEM item, COLORREF& clrValue) const;
  bool GetItemValue(HITEM item, LOGFONT& lfValue) const;

  // set item value
  bool SetItemValue(HITEM item, const string strValue);
  bool SetItemValue(HITEM item, const int nValue);
  bool SetItemValue(HITEM item, const double nValue);
  bool SetItemValue(HITEM item, const bool bValue);
  bool SetItemValue(HITEM item, const COleDateTime dtValue);
  bool SetItemValue(HITEM item, const COLORREF clrValue);
  bool SetItemValue(HITEM item, const LOGFONT lfValue);

  // for custom items
  int GetTextMargin();
  CFont* GetFontNormal();
  CFont* GetFontBold();

  // appearance stuff
  void SetDisplayMode(EDisplayMode display_mode);
  void ExpandAll(bool expand);
  void ExpandSection(HSECTION hs, bool expand);
  bool IsSectionCollapsed(HSECTION hs);

  std::string GetItemText(HITEM hItem);

protected:
  class CItem
  {
  public:
    HITEM m_id;
    bool m_editable;
    bool m_undefined;
    EItemType m_type;
    string m_name;

    vector<string> m_options;

    int m_nValue;
    double m_dValue;
    string m_strValue;
    bool m_bValue;
    COleDateTime m_dtValue;
    COLORREF m_clrValue;
    LOGFONT m_lfValue;
    ICustomItem* m_pCustom;

    bool m_undefined_old;
    int m_nValue_old;
    double m_dValue_old;
    string m_strValue_old;
    bool m_bValue_old;
    COleDateTime m_dtValue_old;
    COLORREF m_clrValue_old;
    LOGFONT m_lfValue_old;

    CRect m_rcName;
    CRect m_rcValue;

    bool operator==(const HITEM& item) const;
    bool operator==(const string& name) const;

    void ValidateChanges();
  };

  // DOWNGRADE: Commented out to prevent "undefined identifier CPropertyGrid" error
  //friend bool item_alpha_sort(vector<CPropertyGrid::CItem>::iterator it1, vector<CPropertyGrid::CItem>::iterator it2);

  class CSection
  {
  public:
    HSECTION m_id;
    string m_title;
    bool m_collapsed;
    vector<CItem> m_items;

    CRect m_rcSign;
    CRect m_rcTitle;

    bool operator==(const HSECTION& section) const;
  };

  vector<CSection> m_sections;

  HSECTION m_focused_section;
  HITEM m_focused_item;

  EDisplayMode m_display_mode;

  bool m_shade_titles;
  bool m_draw_lines;
  bool m_draw_gutter;
  bool m_focus_disabled;
  bool m_bold_modified;
  bool m_bold_editables;

  int m_gutter_width;
  bool m_resizing_gutter;
  CPoint m_ptLast;

  CFont m_fntNormal;
  CFont m_fntBold;

  int m_line_height;

  CRect m_rect_button;
  CWnd* m_control;
  bool m_button_pushed;
  bool m_button_depressed;
  bool m_value_clicked;
  bool m_custom_tracking;

  HSECTION m_section_id;
  HITEM m_item_id;

  string m_strTrue;
  string m_strFalse;
  string m_strOk;
  string m_strCancel;
  string m_strDate;
  string m_strTime;
  string m_strUndefined;
  string m_strEmpty;

  COLORREF m_clrText;
  COLORREF m_clrTitle;
  COLORREF m_clrBack;
  COLORREF m_clrShade;
  COLORREF m_clrFocus;
  COLORREF m_clrHilite;
  COLORREF m_clrEditable;
  COLORREF m_clrDisabled;

protected:
  DECLARE_MESSAGE_MAP()

  // init control
  void InitControl();

  // drawing
  void DrawItem(CDC& dc, int w, int x, int y, vector<CItem>::iterator& it);

  // item management
  CSection* FindSection(HSECTION hs) const;
  CItem* FindItem(HITEM hi) const;
  HITEM AddItem(HSECTION hs, EItemType type, string name, void* pValue, bool editable, bool undefined, HITEM after);

  // scrolling stuff
  CScrollBar m_scrollbar;
  bool m_scroll_enabled;
  int GetScrollOffset();
  void RecalcLayout();

  // editing
  EEditMode GetEditMode(CItem& item);
  void DeleteEditControl();
  void EditFocusedItem();

  // movement in list
  void MoveForward(HSECTION& focused_section, HITEM& focused_item);

  // keyboard
  void FocusNextItem();
  void FocusPrevItem();

protected:
  virtual void PreSubclassWindow();
public:
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnDestroy();
  afx_msg BOOL OnEraseBkgnd(CDC* pDC);
  afx_msg void OnPaint();
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnMouseMove(UINT nHitTest, CPoint point);
  afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
  afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
  afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
  afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
  afx_msg LRESULT OnComboSelChanged(WPARAM wParam, LPARAM lParam);
  afx_msg LRESULT OnEditChanged(WPARAM wParam, LPARAM lParam);
  afx_msg LRESULT OnDateChanged(WPARAM wParam, LPARAM lParam);
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg UINT OnGetDlgCode();
  afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
  afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};
