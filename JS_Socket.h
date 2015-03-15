//---------------------------------------------------------------------------
#ifndef JS_SocketH
#define JS_SocketH
//---------------------------------------------------------------------------
#include <winsock2.h>
//---------------------------------------------------------------------------
namespace JS
{
  class CSocketEventHandler;

  class CSocket
  {
    public:
      virtual ~CSocket();
      void Bind(unsigned short port);
      virtual int CheckForEvents() = 0;
      virtual void Close();
      char *GetLocalAddress();
      unsigned short GetLocalPort();
      inline SOCKET GetSocket() { return m_Socket; }
      inline int GetErrorCode() { return m_ErrorCode; }
      void SetNonblockingMode(bool on);
      void SetReceiveBufferSize(int size);
      void SetSendBufferSize(int size);
      inline void SetSocketEventHandler(CSocketEventHandler *pSocketEventHandler, void *pContext)
      { m_pSocketEventHandler = pSocketEventHandler; m_pContext = pContext;}

    protected:
      SOCKET m_Socket;
      int m_ErrorCode;
      CSocketEventHandler *m_pSocketEventHandler;
      void *m_pContext;

      CSocket();
      void CreateSocket(int type);
  };
}
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
