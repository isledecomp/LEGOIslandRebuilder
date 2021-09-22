#include "config.h"

#include <SHLWAPI.H>

#include "../cmn/path.h"

Config config;

Config::Config()
{

}

BOOL Config::Load()
{
  // Get config file
  m_configFile.resize(MAX_PATH);
  return GetConfigFilename(&m_configFile[0]) && PathFileExists(m_configFile.c_str());
}

UINT Config::GetInt(LPCTSTR name, UINT defaultValue)
{
  return GetPrivateProfileInt(appName, name, defaultValue, m_configFile.c_str());
}
