//---------------------------------------------------------------------------
#ifndef JS_ServerSocketH
#define JS_ServerSocketH
//---------------------------------------------------------------------------
#include "JS_Socket.h"
//---------------------------------------------------------------------------
namespace JS
{
  class CServerSocket : public CSocket
  {
    public:
      CServerSocket(unsigned short port);
      virtual ~CServerSocket();
      CClientSocket *Accept();
      virtual int CheckForEvents();

    protected:
      int CheckForReadability();
  };
}
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
