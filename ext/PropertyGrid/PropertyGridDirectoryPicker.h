////////////////////////////////////////////////////////////////////////////////
//
//               Copyright (C) 2001, nabocorp
//                     All Rights Reserved
//
////////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIRECTORYPICKER_H__CBBD1C74_A552_11D2_8ECA_00104BDC35E6__INCLUDED_)
#define AFX_DIRECTORYPICKER_H__CBBD1C74_A552_11D2_8ECA_00104BDC35E6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CPropertyGridDirectoryPicker
{
public:
  CPropertyGridDirectoryPicker();
  virtual ~CPropertyGridDirectoryPicker();

  static bool PickDirectory(std::string&, HWND hwnd);
  static std::string m_strTitle;

protected:
  static int CALLBACK BrowseCallbackProc(HWND hwnd,UINT uMsg,LPARAM lp, LPARAM pData);
};

#endif // !defined(AFX_DIRECTORYPICKER_H__CBBD1C74_A552_11D2_8ECA_00104BDC35E6__INCLUDED_)
