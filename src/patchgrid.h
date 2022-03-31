#ifndef PATCHGRID_H
#define PATCHGRID_H

#include "../ext/PropertyGrid/stdafx.h"
#include "../ext/PropertyGrid/PropertyGrid.h"

class PatchGrid : public CPropertyGrid
{
public:
  PatchGrid();

  void LoadConfiguration(LPCTSTR filename);
  BOOL SaveConfiguration(LPCTSTR filename);

  void AddD3DDevice(const std::string &name, const std::string &id);

  CString GetItemDescription(HITEM item);
  CString GetItemDescription(const std::string &key)
  {
    return m_mPatchDescriptions[key];
  }

private:
  void AddPatch(const std::string &id, const CString &description, HITEM item);

  std::map<std::string, HITEM> m_mPatchItems;
  std::map<std::string, CString> m_mPatchDescriptions;

  HITEM m_d3dDeviceItem;

  std::vector<std::string> m_d3dDeviceNames;
  std::vector<std::string> m_d3dDeviceIDs;

};

#endif // PATCHGRID_H
