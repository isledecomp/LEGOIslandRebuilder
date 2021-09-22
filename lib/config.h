#ifndef CONFIG_H
#define CONFIG_H

#include <STRING>
#include <TCHAR.H>
#include <WINDOWS.H>

class Config
{
public:
  Config();

  BOOL Load();

  UINT GetInt(LPCTSTR name, UINT defaultValue = 0);

private:
  std::basic_string<TCHAR> m_configFile;

};

extern Config config;

#endif // CONFIG_H
