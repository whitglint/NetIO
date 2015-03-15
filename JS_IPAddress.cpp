//---------------------------------------------------------------------------
#include "JS_CacheHeader.h"
#pragma hdrstop
#include "JS_IPAddress.h"
//---------------------------------------------------------------------------
using namespace JS;
//---------------------------------------------------------------------------
CIPAddress::CIPAddress(in_addr ip)
{
  m_IP = ip;
  m_IPString[0] = 0;
  m_BroadcastIPString[0] = 0;
  memcpy(&m_BroadcastIP, &m_IP, 3);

  m_BroadcastIP.S_un.S_un_b.s_b4 = 255;
}
//---------------------------------------------------------------------------
CIPAddress::~CIPAddress()
{
}
//---------------------------------------------------------------------------
char *CIPAddress::GetBroadcastIPString()
{
  if (0 == m_BroadcastIPString[0])
    strcpy(m_BroadcastIPString, inet_ntoa(m_BroadcastIP));

  return m_BroadcastIPString;
}
//---------------------------------------------------------------------------
char *CIPAddress::GetIPString()
{
  if (0 == m_IPString[0])
    strcpy(m_IPString, inet_ntoa(m_IP));

  return m_IPString;
}
//---------------------------------------------------------------------------
