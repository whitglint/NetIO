//---------------------------------------------------------------------------
#ifndef JS_SocketEventHandlerH
#define JS_SocketEventHandlerH
//---------------------------------------------------------------------------
namespace JS
{
  class CSocket;
  class CClientSocket;
  class CServerSocket;

  class CSocketEventHandler
  {
    public:
      virtual void HandleSocketAcceptEvent(CServerSocket *pServerSocket, CClientSocket *pClientSocket, void *pContext) = 0;
      virtual void HandleSocketCloseEvent(CSocket *pSocket, void *pContext) = 0;
      virtual void HandleSocketConnectEvent(CClientSocket *pClientSocket, bool isSuccessful, void *pContext) = 0;
      virtual void HandleSocketDataArrivalEvent(CSocket *pSocket, unsigned long receivedBytes, void *pContext) = 0;
  };
}
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
