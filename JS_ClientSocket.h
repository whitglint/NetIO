//---------------------------------------------------------------------------
#ifndef JS_ClientSocketH
#define JS_ClientSocketH
//---------------------------------------------------------------------------
#include "JS_Socket.h"
//---------------------------------------------------------------------------
namespace JS
{
  class CClientSocket : public CSocket
  {
    public:
      //Socket state constants:
      static const int UNCONNECTED_STATE = 0; //未連線狀態。
      static const int CONNECTING_STATE  = 1; //連線中狀態。
      static const int CONNECTED_STATE   = 2; //已連線狀態。

      CClientSocket();
      CClientSocket(SOCKET aSocket, sockaddr_in *pClientAddress);
      virtual ~CClientSocket();
      virtual int CheckForEvents();
      virtual void Close();
      bool Connect(const char *pAddress, unsigned short port);
      char *GetRemoteAddress();
      unsigned short GetRemotePort();
      inline int GetState() { return m_State; }
      inline bool IsConnected() { return (CONNECTED_STATE == m_State); }
      inline bool IsSurprisingClosed() { return m_IsSurprisingClosed; }
      inline bool IsWritable() { return m_IsWritable; }
      bool Receive(const char *pRecvBuffer, int bufferLen);
      bool Send(const char *pSendBuffer, int bufferLen);
      void SetNoDelay(bool on);

    protected:
      int m_State;
      bool m_IsSurprisingClosed;
      bool m_IsWritable;
      unsigned long m_ReceivedBytes;

      int CheckForError();
      int CheckForReadability();
      int CheckForWritability();
  };
}
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
