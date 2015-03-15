//---------------------------------------------------------------------------
#ifndef JS_IPAddressH
#define JS_IPAddressH
//---------------------------------------------------------------------------
#include <winsock2.h>
//---------------------------------------------------------------------------
namespace JS
{
  class CIPAddress
  {
    public:
      CIPAddress(in_addr ip);
      virtual ~CIPAddress();
      inline in_addr GetBroadcastIP() { return m_BroadcastIP; }
      char *GetBroadcastIPString();
      inline in_addr GetIP() { return m_IP; }
      char *GetIPString();

    protected:
      in_addr m_IP;
      in_addr m_BroadcastIP;
      char m_IPString[16];
      char m_BroadcastIPString[16];
  };
}
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
