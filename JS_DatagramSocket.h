//---------------------------------------------------------------------------
#ifndef JS_DatagramSocketH
#define JS_DatagramSocketH
//---------------------------------------------------------------------------
#include "JS_Socket.h"
//---------------------------------------------------------------------------
namespace JS
{
  class CDatagramSocket : public CSocket
  {
    public:
      CDatagramSocket();
      CDatagramSocket(unsigned short port);
      virtual ~CDatagramSocket();
      virtual int CheckForEvents();
      bool ReceiveFrom(char *pRecvBuffer, int *pBufferLen, char *pIP, unsigned short *pPort);
      bool SendBroadcast(const char *pSendBuffer, int bufferLen, unsigned short port);
      bool SendTo(const char *pSendBuffer, int bufferLen, const char *pIP, unsigned short port);
      void SetBroadcast(bool on);

    protected:
      unsigned long m_ReceivedBytes;

      int CheckForReadability();
      int CheckForWritability();
      bool GetBroadcastAddress(sockaddr_in *pNetAddress);
  };
}
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
