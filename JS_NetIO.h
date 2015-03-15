//---------------------------------------------------------------------------
#ifndef JS_NetIOH
#define JS_NetIOH
//---------------------------------------------------------------------------
#include <winsock2.h>
//---------------------------------------------------------------------------
namespace JS
{
  class CIPManager;

  class CNetIO
  {
    public:
      static CNetIO *GetInstance();
      static int ReleaseInstance();

      inline CIPManager *GetIPManager() { return m_pIPManager; }

      int Initialize();

    private:
      static const int INITIALIZED = 1;

      static CNetIO *m_pNetIO;
      static int m_ReferenceCounter;

      int m_State;
      WSADATA m_WSAData;
      CIPManager *m_pIPManager;

      CNetIO();
      ~CNetIO();
  };
}
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
