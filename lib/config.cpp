#include "config.h"

#include <SHLWAPI.H>
#include <SSTREAM>
#include <STRING>

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

float Config::GetFloat(LPCTSTR name, float defaultValue)
{
  // Convert float to string
  std::ostringstream oss;
  oss << defaultValue;
  std::string defaultStr = oss.str();

  std::string currentStr = GetString(name, defaultStr);

  // Convert to float
  return atof(currentStr.c_str());
}

std::string Config::GetString(LPCTSTR name, const std::string &defaultValue)
{
  const int max_string_sz = 100;
  std::string s;
  s.resize(max_string_sz);

  DWORD count = GetPrivateProfileString(appName, name, defaultValue.c_str(), &s[0], max_string_sz, m_configFile.c_str());

  s.resize(count);

  return s;
}
