//---------------------------------------------------------------------------
#include "JS_CacheHeader.h"
#pragma hdrstop
#include "JS_SocketEventHandler.h"
#include "JS_ClientSocket.h"
#include "JS_ServerSocket.h"
//---------------------------------------------------------------------------
using namespace JS;
//---------------------------------------------------------------------------
CServerSocket::CServerSocket(unsigned short port)
{
  CreateSocket(SOCK_STREAM);
  if (INVALID_SOCKET == m_Socket)
    return;

  Bind(port);
  if (SOCKET_ERROR == m_ErrorCode)
    return;

  SetNonblockingMode(true);

  m_ErrorCode = listen(m_Socket, SOMAXCONN);
  if (SOCKET_ERROR == m_ErrorCode)
  {
    //OutputDebugString("Listen socket occurs error!");
    return;
  }
}
//---------------------------------------------------------------------------
CServerSocket::~CServerSocket()
{
}
//---------------------------------------------------------------------------
CClientSocket *CServerSocket::Accept()
{
  SOCKET aSocket;
  sockaddr_in clientAddress;
  int addrLen = sizeof(sockaddr_in);
  CClientSocket *pClientSocket;

  if (INVALID_SOCKET == m_Socket)
    return NULL;

  aSocket = accept(m_Socket, (sockaddr *)&clientAddress, &addrLen);
  if (INVALID_SOCKET == aSocket)
  {
    //OutputDebugString("Accept socket occurs error!");
    m_ErrorCode = INVALID_SOCKET;
    return NULL;
  }

  pClientSocket = new CClientSocket(aSocket, &clientAddress);

  //Trigger socket accept event.
  if (NULL != m_pSocketEventHandler)
  {
    try
    {
      m_pSocketEventHandler->HandleSocketAcceptEvent(this, pClientSocket, m_pContext);
    }
    catch (...)
    {
      //OutputDebugString("The socket accept event handler of the server socket occurs error!");
    }
  }

  return pClientSocket;
}
//---------------------------------------------------------------------------
int CServerSocket::CheckForEvents()
{
  int result = 0;
  int status;

  status = CheckForReadability();
  if (status > 0) //如果有 client socket 連進來。
  {
    //接受 client socket 連線。
    Accept();
    result++;
  }
  return result;
}
//---------------------------------------------------------------------------
//Return Values:
//   1: 有 client socket 連進來。
//   0: 沒有 client socket 連進來。
//  -1: 發生錯誤(SOCKET_ERROR)。
int CServerSocket::CheckForReadability()
{
  fd_set fdset;
  timeval waitTime = {0, 0};

  if (INVALID_SOCKET == m_Socket)
    return SOCKET_ERROR;

  FD_ZERO(&fdset);
  FD_SET(m_Socket, &fdset);

  //檢查有沒有 client socket 連進來並將結果傳回。
  return select(0, &fdset, NULL, NULL, &waitTime);
}
//---------------------------------------------------------------------------
