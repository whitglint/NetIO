//---------------------------------------------------------------------------
#ifndef JS_IPManagerH
#define JS_IPManagerH
//---------------------------------------------------------------------------
#include <vector>
//---------------------------------------------------------------------------
namespace JS
{
  class CIPAddress;

  class CIPManager
  {
    public:
      CIPManager();
      virtual ~CIPManager();
      char *GetHostName();
      CIPAddress *GetIPAddress(unsigned int index);
      inline unsigned int GetIPAddressCount() { return m_pIPAddresses->size(); }
      static bool GetIPByHostName(const char *pHostName, char *pIP);
      void Refresh();

    protected:
      typedef std::vector<CIPAddress *> ipaddresses_type;

      char m_HostName[256];
      ipaddresses_type *m_pIPAddresses;

      void ClearIPAddresses();
  };
}
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
