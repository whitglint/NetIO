//---------------------------------------------------------------------------
#include "JS_CacheHeader.h"
#pragma hdrstop
#include "JS_IPManager.h"
#include "JS_SocketEventHandler.h"
#include "JS_ClientSocket.h"
//---------------------------------------------------------------------------
using namespace JS;
//---------------------------------------------------------------------------
CClientSocket::CClientSocket()
{
  m_State = UNCONNECTED_STATE;
  m_IsSurprisingClosed = false;
  m_IsWritable = false;
  m_ReceivedBytes = 0;
}
//---------------------------------------------------------------------------
CClientSocket::CClientSocket(SOCKET aSocket, sockaddr_in *pClientAddress)
{
  m_Socket = aSocket;
  m_ReceivedBytes = 0;
  if (INVALID_SOCKET != m_Socket)
  {
    m_State = CONNECTED_STATE;
  }
  else
  {
    m_State = UNCONNECTED_STATE;
  }
  m_IsSurprisingClosed = false;

  SetNonblockingMode(true);
}
//---------------------------------------------------------------------------
CClientSocket::~CClientSocket()
{
  Close();
}
//---------------------------------------------------------------------------
//Return Values:
//   1: 連線失敗。
//   0: 狀態不明(可能正在連線中)。
//  -1: 發生錯誤(SOCKET_ERROR)。
int CClientSocket::CheckForError()
{
  int result = 0; //記錄是否連線失敗。
  fd_set fdset;
  timeval waitTime = {0, 0};
  int status;

  if (INVALID_SOCKET == m_Socket)
    return SOCKET_ERROR;

  FD_ZERO(&fdset);
  FD_SET(m_Socket, &fdset);

  //檢查是否連線失敗。
  status = select(0, NULL, NULL, &fdset, &waitTime);
  if (status > 0) //如果連線失敗。
  {
    result = 1;

    if (CONNECTING_STATE == m_State) //如果之前正在連線中。
    {
      m_State = UNCONNECTED_STATE;

      //Trigger socket connect event.
      if (NULL != m_pSocketEventHandler)
      {
        try
        {
          m_pSocketEventHandler->HandleSocketConnectEvent(this, false, m_pContext);
        }
        catch (...)
        {
          //OutputDebugString("The socket connect event handler of the client socket occurs error!");
        }
      }
    }
  }
  else if (SOCKET_ERROR == status)
  {
    result = SOCKET_ERROR;
  }

  return result;
}
//---------------------------------------------------------------------------
int CClientSocket::CheckForEvents()
{
  int result = 0;
  int status;

  switch (m_State)
  {
    case CONNECTING_STATE:
      //檢查這個 client socket 是否有事件(連線成功)。
      status = CheckForWritability();
      if (status > 0)        //如果這個 client socket 連線成功。
      {
        result++;
      }
      else
      {
        //檢查這個 client socket 是否有事件(連線失敗)。
        status = CheckForError();
        if (status > 0)      //如果這個 client socket 連線失敗。
          result++;
      }
      break;

    case CONNECTED_STATE:
      //檢查這個 client socket 是否有事件(有新資料進來或斷線)。
      status = CheckForReadability();
      if (status > 0)        //如果這個 client socket 有新資料進來。
        result++;
      else if (-2 == status) //如果這個 client socket 斷線了。
        result++;
      break;
  }
  return result;
}
//---------------------------------------------------------------------------
//Return Values:
//    1: 有新資料進來。
//    0: 沒有新資料進來。
//   -1: 發生錯誤(SOCKET_ERROR)。
//   -2: 已斷線。
int CClientSocket::CheckForReadability()
{
  int result = 0; //記錄是否有新資料進來或已斷線。
  fd_set fdset;
  timeval waitTime = {0, 0};
  int status;
  unsigned long ioctlsocketArg;

  if (INVALID_SOCKET == m_Socket)
    return SOCKET_ERROR;

  FD_ZERO(&fdset);
  FD_SET(m_Socket, &fdset);

  //檢查有沒有新資料進來或已斷線。
  status = select(0, &fdset, NULL, NULL, &waitTime);
  if (status > 0) //如果有資料可讀或已斷線。
  {
    if (0 == ioctlsocket(m_Socket, FIONREAD, &ioctlsocketArg))
    {
      //ioctlsocketArg 的值為可讀資料的大小。
      if (ioctlsocketArg > 0) //如果有資料可讀。
      {
        if (ioctlsocketArg > m_ReceivedBytes) //如果有新資料進來。
        {
          result = 1;
          m_ReceivedBytes = ioctlsocketArg;

          //Trigger socket data arrival event.
          if (NULL != m_pSocketEventHandler)
          {
            try
            {
              m_pSocketEventHandler->HandleSocketDataArrivalEvent(this, m_ReceivedBytes, m_pContext);
            }
            catch (...)
            {
              //OutputDebugString("The socket data arrival event handler of the client socket occurs error!");
            }
          }
        }
      }
      else                    //如果已斷線。
      {
        result = -2;

        if (UNCONNECTED_STATE != m_State) //如果之前不是斷線狀態。
        {
          m_IsSurprisingClosed = true;
          Close();
        }
      }
    }
    else
    {
      result = SOCKET_ERROR;
    }
  }
  else if (SOCKET_ERROR == status)
  {
    result = SOCKET_ERROR;
  }

  return result;
}
//---------------------------------------------------------------------------
//Return Values:
//   1: 連線成功或可送出資料。
//   0: 正在連線中或不可送出資料。
//  -1: 發生錯誤(SOCKET_ERROR)。
int CClientSocket::CheckForWritability()
{
  int result = 0; //記錄是否連線成功或可送出資料。
  fd_set fdset;
  timeval waitTime = {0, 0};
  int status;

  if (INVALID_SOCKET == m_Socket)
    return SOCKET_ERROR;

  FD_ZERO(&fdset);
  FD_SET(m_Socket, &fdset);

  //檢查是否連線成功或可送出資料。
  m_IsWritable = false;
  status = select(0, NULL, &fdset, NULL, &waitTime);
  if (status > 0) //如果連線成功或可送出資料。
  {
    result = 1;
    m_IsWritable = true;

    if (CONNECTING_STATE == m_State) //如果之前正在連線中。
    {
      m_State = CONNECTED_STATE;

      //Trigger socket connect event.
      if (NULL != m_pSocketEventHandler)
      {
        try
        {
          m_pSocketEventHandler->HandleSocketConnectEvent(this, true, m_pContext);
        }
        catch (...)
        {
          //OutputDebugString("The socket connect event handler of the client socket occurs error!");
        }
      }
    }
  }
  else if (SOCKET_ERROR == status)
  {
    result = SOCKET_ERROR;
  }

  return result;
}
//---------------------------------------------------------------------------
void CClientSocket::Close()
{
  if (INVALID_SOCKET != m_Socket)
  {
    m_State = UNCONNECTED_STATE;

    CSocket::Close();
  }
}
//---------------------------------------------------------------------------
//Return Values:
//   true: The socket is connecting.
//  false: The socket occurs error.
bool CClientSocket::Connect(const char *pAddress, unsigned short port)
{
  char ip[16];
  sockaddr_in remoteAddress;

  m_IsSurprisingClosed = false;
  Close();

  CreateSocket(SOCK_STREAM);
  if (INVALID_SOCKET == m_Socket)
    return false;

  SetNonblockingMode(true);

  //Get remote IP.
  if (false == CIPManager::GetIPByHostName(pAddress, ip))
    return false;

  remoteAddress.sin_family = AF_INET;
  remoteAddress.sin_addr.s_addr = inet_addr(ip);
  remoteAddress.sin_port = htons(port);

  m_ErrorCode = connect(m_Socket, (sockaddr *)&remoteAddress, sizeof(remoteAddress));
  if (WSAEWOULDBLOCK == WSAGetLastError())
  {
    m_State = CONNECTING_STATE;
    return true;
  }

  return false;
}
//---------------------------------------------------------------------------
char *CClientSocket::GetRemoteAddress()
{
  sockaddr_in remoteAddress;
  int remoteAddressLen = sizeof(remoteAddress);

  if (0 == (m_ErrorCode = getpeername(m_Socket, (sockaddr *)&remoteAddress, &remoteAddressLen)))
    return inet_ntoa(remoteAddress.sin_addr);
  else
    return NULL;
}
//---------------------------------------------------------------------------
unsigned short CClientSocket::GetRemotePort()
{
  sockaddr_in remoteAddress;
  int remoteAddressLen = sizeof(remoteAddress);

  if (0 == (m_ErrorCode = getpeername(m_Socket, (sockaddr *)&remoteAddress, &remoteAddressLen)))
    return ntohs(remoteAddress.sin_port);
  else
    return 0;
}
//---------------------------------------------------------------------------
bool CClientSocket::Receive(const char *pRecvBuffer, int bufferLen)
{
  bool result = true;
  int transferedLen;
  char *pBuffer = (char *)pRecvBuffer;

  if (INVALID_SOCKET == m_Socket || NULL == pBuffer || CONNECTED_STATE != m_State)
    return false;

  if (bufferLen > 0)
  {
    while (bufferLen > 0)
    {
      transferedLen = recv(m_Socket, pBuffer, bufferLen, 0);

      if (0 == transferedLen || SOCKET_ERROR == transferedLen)
      {
        if (0 == transferedLen)
          Close();

        m_ErrorCode = SOCKET_ERROR;
        result = false;
        break;
      }

      m_ReceivedBytes -= transferedLen;
      bufferLen -= transferedLen;
      if (bufferLen > 0)
        pBuffer += transferedLen;
    }
  }

  return result;
}
//---------------------------------------------------------------------------
bool CClientSocket::Send(const char *pSendBuffer, int bufferLen)
{
  bool result = true;
  int transferedLen;
  char *pBuffer = (char *)pSendBuffer;

  if (INVALID_SOCKET == m_Socket || NULL == pBuffer || CONNECTED_STATE != m_State)
    return false;

  if (bufferLen > 0)
  {
    while (bufferLen > 0)
    {
      transferedLen = send(m_Socket, pBuffer, bufferLen, 0);

      if (0 == transferedLen || SOCKET_ERROR == transferedLen)
      {
        if (0 == transferedLen)
          Close();

        m_ErrorCode = SOCKET_ERROR;
        result = false;
        break;
      }

      bufferLen -= transferedLen;
      if (bufferLen > 0)
        pBuffer += transferedLen;
    }
  }

  return result;
}
//---------------------------------------------------------------------------
void CClientSocket::SetNoDelay(bool on)
{
  bool optval = on;

  m_ErrorCode = setsockopt(m_Socket, IPPROTO_TCP, TCP_NODELAY, (const char *)&optval, sizeof(bool));
  /*
  if (SOCKET_ERROR == m_ErrorCode)
    OutputDebugString("CClientSocket::SetNoDelay() occurs error!");
  */
}
//---------------------------------------------------------------------------
