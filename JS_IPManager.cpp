//---------------------------------------------------------------------------
#include "JS_CacheHeader.h"
#pragma hdrstop
#include "JS_IPAddress.h"
#include "JS_IPManager.h"
//---------------------------------------------------------------------------
using namespace JS;
//---------------------------------------------------------------------------
CIPManager::CIPManager()
{
  m_HostName[0] = 0;
  m_pIPAddresses = new ipaddresses_type();
  GetHostName();
  Refresh();
}
//---------------------------------------------------------------------------
CIPManager::~CIPManager()
{
  ClearIPAddresses();
  if (NULL != m_pIPAddresses)
    delete m_pIPAddresses;
}
//---------------------------------------------------------------------------
void CIPManager::ClearIPAddresses()
{
  ipaddresses_type::iterator pointer;

  for (pointer = m_pIPAddresses->begin(); m_pIPAddresses->end() != pointer;
       pointer++)
    delete *pointer;

  m_pIPAddresses->clear();
}
//---------------------------------------------------------------------------
char *CIPManager::GetHostName()
{
  if (0 == m_HostName[0])
  {
    if (SOCKET_ERROR == gethostname(m_HostName, 256))
    {
      //OutputDebugString("CIPManager::GetHostName() occurs error!");
      return NULL;
    }
  }

  return m_HostName;
}
//---------------------------------------------------------------------------
CIPAddress *CIPManager::GetIPAddress(unsigned int index)
{
  if (index >= m_pIPAddresses->size())
    return NULL;

  return m_pIPAddresses->at(index);
}
//---------------------------------------------------------------------------
bool CIPManager::GetIPByHostName(const char *pHostName, char *pIP)
{
  HOSTENT *pHostent;
  in_addr ip;

  if (NULL == pHostName || NULL == pIP)
    return false;

  //取得 IP。
  pHostent = gethostbyname(pHostName);
  if (NULL != pHostent)
  {
    memcpy(&ip, pHostent->h_addr_list[0], 4);
    strcpy(pIP, inet_ntoa(ip));

    return true;
  }
  else
  {
    //OutputDebugString("CIPManager::GetIPByHostName() occurs error!");
    return false;
  }
}
//---------------------------------------------------------------------------
void CIPManager::Refresh()
{
  HOSTENT *pHostent;
  in_addr ip;
  CIPAddress *pIPAddress;

  ClearIPAddresses();

  //取得第一個 IP。
  pHostent = gethostbyname(m_HostName);
  if (NULL != pHostent)
  {
    memcpy(&ip, pHostent->h_addr_list[0], 4);

    pIPAddress = new CIPAddress(ip);
    m_pIPAddresses->push_back(pIPAddress);
  }
}
//---------------------------------------------------------------------------
