#ifndef PATCHGRID_H
#define PATCHGRID_H

#include "../ext/PropertyGrid/stdafx.h"
#include "../ext/PropertyGrid/PropertyGrid.h"

class PatchGrid : public CPropertyGrid
{
public:
  PatchGrid();

  BOOL SaveConfiguration(LPCTSTR filename);

private:
  void AddPatch(const std::string &id, const CString &description, HITEM item);

  std::map<std::string, HITEM> m_mPatchItems;
  std::map<std::string, CString> m_mPatchDescriptions;

};

#endif // PATCHGRID_H
