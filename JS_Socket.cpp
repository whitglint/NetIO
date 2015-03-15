//---------------------------------------------------------------------------
#include "JS_CacheHeader.h"
#pragma hdrstop
#include "JS_SocketEventHandler.h"
#include "JS_Socket.h"
//---------------------------------------------------------------------------
using namespace JS;
//---------------------------------------------------------------------------
CSocket::CSocket()
{
  m_Socket = INVALID_SOCKET;
  m_ErrorCode = 0;
  m_pSocketEventHandler = NULL;
}
//---------------------------------------------------------------------------
CSocket::~CSocket()
{
  Close();
}
//---------------------------------------------------------------------------
void CSocket::Bind(unsigned short port)
{
  sockaddr_in localAddress;

  localAddress.sin_family = AF_INET;
  localAddress.sin_addr.s_addr = htonl(INADDR_ANY);
  localAddress.sin_port = htons(port);

  m_ErrorCode = bind(m_Socket, (sockaddr *)&localAddress, sizeof(sockaddr_in));
  /*
  if (SOCKET_ERROR == m_ErrorCode)
    OutputDebugString("Bind socket occurs error!");
  */
}
//---------------------------------------------------------------------------
void CSocket::Close()
{
  if (INVALID_SOCKET != m_Socket)
  {
    shutdown(m_Socket, SD_BOTH);
    m_ErrorCode = closesocket(m_Socket);
    m_Socket = INVALID_SOCKET;

    //Trigger socket close event.
    if (NULL != m_pSocketEventHandler)
    {
      try
      {
        m_pSocketEventHandler->HandleSocketCloseEvent(this, m_pContext);
      }
      catch (...)
      {
        //OutputDebugString("The socket close event handler of the socket occurs error!");
      }
    }
  }
}
//---------------------------------------------------------------------------
void CSocket::CreateSocket(int type)
{
  if (INVALID_SOCKET == m_Socket)
  {
    m_Socket = socket(AF_INET, type, 0);
    if (INVALID_SOCKET == m_Socket)
    {
      //OutputDebugString("Create socket occurs error!");
      m_ErrorCode = INVALID_SOCKET;
    }
  }
}
//---------------------------------------------------------------------------
char *CSocket::GetLocalAddress()
{
  sockaddr_in localAddress;
  int localAddressLen = sizeof(localAddress);

  if (0 == (m_ErrorCode = getsockname(m_Socket, (sockaddr *)&localAddress, &localAddressLen)))
    return inet_ntoa(localAddress.sin_addr);
  else
    return NULL;
}
//---------------------------------------------------------------------------
unsigned short CSocket::GetLocalPort()
{
  sockaddr_in localAddress;
  int localAddressLen = sizeof(localAddress);

  if (0 == (m_ErrorCode = getsockname(m_Socket, (sockaddr *)&localAddress, &localAddressLen)))
    return ntohs(localAddress.sin_port);
  else
    return 0;
}
//---------------------------------------------------------------------------
void CSocket::SetNonblockingMode(bool on)
{
  unsigned long ioctlsocketArg = on;

  m_ErrorCode = ioctlsocket(m_Socket, FIONBIO, &ioctlsocketArg);
  /*
  if (SOCKET_ERROR == m_ErrorCode)
    OutputDebugString("CSocket::SetNonblockingMode() occurs error!");
  */
}
//---------------------------------------------------------------------------
void CSocket::SetReceiveBufferSize(int size)
{
  int optval = size;

  m_ErrorCode = setsockopt(m_Socket, SOL_SOCKET, SO_RCVBUF, (const char *)&optval, sizeof(int));
  /*
  if (SOCKET_ERROR == m_ErrorCode)
    OutputDebugString("CSocket::SetReceiveBufferSize() occurs error!");
  */
}
//---------------------------------------------------------------------------
void CSocket::SetSendBufferSize(int size)
{
  int optval = size;

  m_ErrorCode = setsockopt(m_Socket, SOL_SOCKET, SO_SNDBUF, (const char *)&optval, sizeof(int));
  /*
  if (SOCKET_ERROR == m_ErrorCode)
    OutputDebugString("CSocket::SetSendBufferSize() occurs error!");
  */
}
//---------------------------------------------------------------------------
