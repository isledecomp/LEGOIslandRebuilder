#include "patchgrid.h"

#include <D3DRM.H>
#include <DDRAW.H>
#include <SHLWAPI.H>
#include <SSTREAM>

#include "../cmn/path.h"

HRESULT CALLBACK ReceiveD3DDevice(GUID *lpGuid, LPSTR szDeviceDescription, LPSTR szDeviceName, LPD3DDEVICEDESC pD3DDD1, LPD3DDEVICEDESC pD3DDD2, LPVOID lpContext)
{
  char buf[256];

  // NOTE: Not really sure where the "0" comes from. I'm guessing it's the index of the ddraw
  //       device, but I don't have any hardware that has a second one (I only have "Primary
  //       Display Driver") so I can't experiment.
  sprintf(buf, "0 0x%lx 0x%x%x 0x%lx 0x%lx", lpGuid->Data1, lpGuid->Data3, lpGuid->Data2,
          *((DWORD*)&lpGuid->Data4), *((DWORD*)&lpGuid->Data4[4]));

  PatchGrid *p = (PatchGrid*)lpContext;
  p->AddD3DDevice(szDeviceName, buf);

  return DDENUMRET_OK;
}

BOOL CALLBACK ReceiveDDrawDevice(GUID *lpGuid, LPSTR szDesc, LPSTR szName, LPVOID lpContext)
{
  LPDIRECTDRAW dd;
  LPDIRECT3D2 d3d2;

  DirectDrawCreate(lpGuid, &dd, NULL);

  dd->QueryInterface(IID_IDirect3D2, (void**)&d3d2);
  d3d2->EnumDevices(ReceiveD3DDevice, lpContext);

  d3d2->Release();
  dd->Release();

  return DDENUMRET_OK;
}

PatchGrid::PatchGrid()
{
  SetBoldModified(true);

  // General section
  HSECTION sectionGeneral = AddSection(_T("General"));

  AddPatch("DebugToggle",
           _T("Enables the in-game debug mode automatically without the need to type OGEL."),
           AddBoolItem(sectionGeneral, _T("Debug Mode"), false));

  AddPatch("MultipleInstances",
           _T("By default, LEGO Island will allow only one instance of itself to run. "
              "This patch allows infinite instances of LEGO Island to run."),
           AddBoolItem(sectionGeneral, _T("Allow Multiple Instances"), false));

  AddPatch("SpeedUpStartUp",
           _T("By default, LEGO Island waits 200 frames to ensure everything is initialized before starting. "
              "That interval can be skipped without consequence in many cases."),
           AddBoolItem(sectionGeneral, _T("Speed Up Startup"), false));

  AddPatch("StayActiveWhenDefocused",
           _T("By default, LEGO Island pauses when it's not the active window. "
              "This patch prevents that behavior."),
           AddBoolItem(sectionGeneral, _T("Stay Active When Defocused"), false));

  AddPatch("RedirectSaveData",
           _T("By default LEGO Island saves its game data in its Program Files folder. In newer versions of "
              "Windows, this folder is considered privileged access, necessitating running LEGO Island as administrator "
              "to save here. This patch sets LEGO Island's save location to %APPDATA% instead, which is an accessible and "
              "standard location that most modern games and apps save to."),
           AddBoolItem(sectionGeneral, _T("Redirect Save Files to %APPDATA%"), false));

  AddPatch("DisableAutoFinishBuilding",
           _T("In LEGO Island v1.1, placing the last block when building will automatically end the building section. While convenient, "
              "this prevents players from making any further changes after placing the last brick. It also notably defies what Bill Ding says - you "
              "don't hit the triangle when you're finished building.\n\nThis patch restores the functionality in v1.0 where placing the last block "
              "will not automatically finish the build section."),
           AddBoolItem(sectionGeneral, _T("Disable Auto-Finish Building Section"), false));

  // Controls section
  HSECTION sectionControls = AddSection(_T("Controls"));

  AddPatch("UseWASD",
           _T("Enables the use of WASD keys for movement rather than the arrow keys. "
              "NOTE: When using Debug Mode, this patch will re-map the conflicting debug keys to the arrow keys."),
           AddBoolItem(sectionControls, _T("Use WASD"), false));
  AddPatch("UseJoystick",
           _T("Enables Joystick functionality."),
           AddBoolItem(sectionControls, _T("Use Joystick"), false));
  AddPatch("MouseDeadzone",
           _T("Sets the radius from the center of the screen where the mouse will do nothing (40 = default)."),
           AddIntegerItem(sectionControls, _T("Mouse Deadzone"), 40));
  AddPatch("UnhookTurnSpeed",
           _T("LEGO Island contains a bug where the turning speed is influenced by the frame rate. Enable this to make the turn speed independent of the frame rate."),
           AddBoolItem(sectionControls, _T("Unhook Turning From Frame Rate"), false));
  AddPatch("TurnUseVelocity",
           _T("By default, LEGO Island ignores the turning acceleration/deceleration values. Set this to TRUE to utilize them (Default = FALSE)"),
           AddBoolItem(sectionControls, _T("Enable Turning Velocity"), false));

  // Navigation section
  HSECTION sectionNavigation = AddSection(_T("Navigation"));

  AddPatch("TurnMaxSpeed",
           _T("Set the maximum turning speed. (Default = 20.0)"),
           AddDoubleItem(sectionNavigation, _T("Turning: Max Speed"), 20.0));
  AddPatch("TurnMaxAcceleration",
           _T("Set the speed at which turning accelerates (requires 'Turning: Enable Velocity') (Default = 30.0)"),
           AddDoubleItem(sectionNavigation, _T("Turning: Max Acceleration"), 30.0));
  AddPatch("TurnMinAcceleration",
           _T("Set the speed at which turning accelerates (requires 'Turning: Enable Velocity') (Default = 15.0)"),
           AddDoubleItem(sectionNavigation, _T("Turning: Min Acceleration"), 15.0));
  AddPatch("TurnDeceleration",
           _T("Set the speed at which turning decelerates (requires 'Turning: Enable Velocity') (Default = 50.0)"),
           AddDoubleItem(sectionNavigation, _T("Turning: Deceleration"), 50.0));
  AddPatch("MovementMaxSpeed",
           _T("Set the movement maximum speed. (Default = 40.0)"),
           AddDoubleItem(sectionNavigation, _T("Movement: Max Speed"), 40.0));
  AddPatch("MovementMaxAcceleration",
           _T("Set the movement acceleration speed (i.e. how long it takes to go from not moving to top speed) (Default = 15.0)"),
           AddDoubleItem(sectionNavigation, _T("Movement: Max Acceleration"), 15.0));
  AddPatch("MovementMinAcceleration",
           _T("Set the movement acceleration speed (i.e. how long it takes to go from not moving to top speed) (Default = 4.0)"),
           AddDoubleItem(sectionNavigation, _T("Movement: Min Acceleration"), 4.0));
  AddPatch("MovementDeceleration",
           _T("Set the movement deceleration speed (i.e. how long it takes to slow to a stop after releasing the controls). "
              "Increase this value to stop faster, decrease it to stop slower. "
              "Usually this is set to a very high value making deceleration almost instant. (Default = 50.0)"),
           AddDoubleItem(sectionNavigation, _T("Movement: Deceleration"), 50.0));

  // Graphics Section
  HSECTION sectionGraphics = AddSection(_T("Graphics"));

  DirectDrawEnumerateA(ReceiveDDrawDevice, this);
  m_d3dDeviceItem = AddComboItem(sectionGraphics, _T("Direct3D Device"), m_d3dDeviceNames, 0);

  AddPatch("D3DDevice",
           _T("Set which Direct3D device to use with LEGO Island."),
           m_d3dDeviceItem);

  AddPatch("FullScreen",
           _T("Allows you to change modes without administrator privileges and registry editing. NOTE: Windowed mode is NOT compatible with \"Flip Video Memory Pages\"."),
           AddBoolItem(sectionGraphics, _T("Run in Full Screen"), true));

  AddPatch("DrawCursor",
           _T("Renders a custom in-game cursor, rather than a standard Windows pointer."),
           AddBoolItem(sectionGraphics, _T("Draw Cursor"), false));

  AddPatch("FlipSurfaces",
           _T(""), // FIXME: Write description for this
           AddBoolItem(sectionGraphics, _T("Flip Video Memory Pages"), false));

  AddPatch("BackBuffersInVRAM",
           _T(""), // FIXME: Write description for this
           AddBoolItem(sectionGraphics, _T("Draw 3D to Video Memory"), false));

  vector<string> fpsList;
  fpsList.push_back(_T("Default"));
  fpsList.push_back(_T("Uncapped"));
  fpsList.push_back(_T("Limited"));
  AddPatch("FPSLimit",
           _T("Modify LEGO Island's frame rate cap"),
           AddComboItem(sectionGraphics, _T("FPS Cap"), fpsList, 0));

  AddPatch("CustomFPS",
           _T("If 'FPS Cap' is set to 'Limited', this will be the frame rate used."),
           AddDoubleItem(sectionGraphics, _T("FPS Cap - Custom Limit"), 24.0));

  vector<string> qualityList;
  qualityList.push_back(_T("Infinite"));
  qualityList.push_back(_T("High"));
  qualityList.push_back(_T("Medium"));
  qualityList.push_back(_T("Low"));
  AddPatch("ModelQuality",
           _T("Change LEGO Island's default model quality"),
           AddComboItem(sectionGraphics, _T("Model Quality"), qualityList, 2));

  AddPatch("FOVMultiplier",
           _T("Globally adjusts the field of view by a multiplier\n\n"
              "1.0 = Default (greater than 1.0 is zoomed in, less than 1.0 is zoomed out)"),
           AddDoubleItem(sectionGraphics, _T("Field of View Adjustment"), 1.0));

  // Audio section
  HSECTION sectionMusic = AddSection(_T("Audio"));

  AddPatch("MusicToggle",
           _T("Turns in-game music on or off."),
           AddBoolItem(sectionMusic, _T("Play Music"), true));
}

template<typename T>
std::string toString(const T &value)
{
  std::ostringstream oss;
  oss << value;
  return oss.str();
}

void PatchGrid::LoadConfiguration(LPCTSTR filename)
{
  for (std::map<std::string, HITEM>::const_iterator it=m_mPatchItems.begin(); it!=m_mPatchItems.end(); it++) {
    CItem *item = FindItem(it->second);

    std::string value;
    value.resize(1024);

    char buf[1024];

    DWORD sz = GetPrivateProfileString(appName, it->first.c_str(), NULL, &value[0], value.size(), filename);

    // If this entry wasn't in the profile, skip it
    if (!sz) {
      continue;
    }

    value.resize(sz);

    // Convert value to string
    switch (item->m_type) {
    case IT_STRING:
    case IT_TEXT:
    case IT_FILE:
    case IT_FOLDER:
    case IT_COMBO:
      SetItemValue(it->second, value);
      break;
    case IT_BOOLEAN:
      SetItemValue(it->second, (bool)StrToIntA(value.c_str()));
      break;
    case IT_INTEGER:
      SetItemValue(it->second, StrToIntA(value.c_str()));
      break;
    case IT_DOUBLE:
      SetItemValue(it->second, atof(value.c_str()));
      break;
    case IT_COLOR:
      SetItemValue(it->second, (COLORREF) StrToIntA(value.c_str()));
      break;
    case IT_CUSTOM:
    case IT_DATE:
    case IT_DATETIME:
    case IT_FONT:
    {
      // Report inability to serialize
      TCHAR buf[200];
      sprintf(buf, "Failed to serialize %s from string.", it->first.c_str());
      MessageBox(buf);
      break;
    }
    }
  }
}

BOOL PatchGrid::SaveConfiguration(LPCTSTR filename)
{
  for (std::map<std::string, HITEM>::const_iterator it=m_mPatchItems.begin(); it!=m_mPatchItems.end(); it++) {
    CItem *item = FindItem(it->second);

    std::string value;

    // Convert value to string
    switch (item->m_type) {
    case IT_STRING:
    case IT_TEXT:
    case IT_FILE:
    case IT_FOLDER:
    case IT_COMBO:
      GetItemValue(it->second, value);
      break;
    case IT_BOOLEAN:
    {
      bool b;
      GetItemValue(it->second, b);
      value = toString(b);
      break;
    }
    case IT_INTEGER:
      int i;
      GetItemValue(it->second, i);
      value = toString(i);
      break;
    case IT_DOUBLE:
      double d;
      GetItemValue(it->second, d);
      value = toString(d);
      break;
    case IT_COLOR:
      COLORREF c;
      GetItemValue(it->second, c);
      value = toString(c);
      break;
    case IT_CUSTOM:
    case IT_DATE:
    case IT_DATETIME:
    case IT_FONT:
    {
      // Report inability to serialize
      TCHAR buf[200];
      sprintf(buf, "Failed to serialize %s to string.", it->first.c_str());
      MessageBox(buf);
      break;
    }
    }

    if (!WritePrivateProfileString(appName, it->first.c_str(), value.c_str(), filename)) {
      return FALSE;
    }

    if (it->second == m_d3dDeviceItem) {
      int device_index;
      GetItemValue(it->second, device_index);
      WritePrivateProfileString(appName, "D3DDeviceID", m_d3dDeviceIDs.at(device_index).c_str(), filename);
    }
  }

  return TRUE;
}

void PatchGrid::AddD3DDevice(const string &name, const string &id)
{
  m_d3dDeviceNames.push_back(name);
  m_d3dDeviceIDs.push_back(id);
}

CString PatchGrid::GetItemDescription(HITEM item)
{
  for (std::map<std::string, HITEM>::const_iterator it=m_mPatchItems.begin(); it!=m_mPatchItems.end(); it++) {
    if (it->second == item) {
      return GetItemDescription(it->first);
    }
  }
  return CString();
}

void PatchGrid::AddPatch(const string &id, const CString &description, HITEM item)
{
  m_mPatchItems[id] = item;
  m_mPatchDescriptions[id] = description;
}
