//---------------------------------------------------------------------------
#include "JS_CacheHeader.h"
#pragma hdrstop
#include "JS_IPManager.h"
#include "JS_SocketEventHandler.h"
#include "JS_DatagramSocket.h"
//---------------------------------------------------------------------------
using namespace JS;
//---------------------------------------------------------------------------
CDatagramSocket::CDatagramSocket()
{
  m_ReceivedBytes = 0;

  CreateSocket(SOCK_DGRAM);
  if (INVALID_SOCKET == m_Socket)
    return;

  SetNonblockingMode(true);
  SetBroadcast(false);
}
//---------------------------------------------------------------------------
CDatagramSocket::CDatagramSocket(unsigned short port)
{
  m_ReceivedBytes = 0;

  CreateSocket(SOCK_DGRAM);
  if (INVALID_SOCKET == m_Socket)
    return;

  Bind(port);
  if (SOCKET_ERROR == m_ErrorCode)
    return;

  SetNonblockingMode(true);
  SetBroadcast(false);
}
//---------------------------------------------------------------------------
CDatagramSocket::~CDatagramSocket()
{
}
//---------------------------------------------------------------------------
int CDatagramSocket::CheckForEvents()
{
  int result = 0;
  int status;

  //檢查這個 datagram socket 是否有事件(有新資料進來)。
  status = CheckForReadability();
  if (status > 0) //如果這個 datagram socket 有新資料進來。
    result++;
  return result;
}
//---------------------------------------------------------------------------
//Return Values:
//    1: 有新資料進來。
//    0: 沒有新資料進來。
//   -1: 發生錯誤(SOCKET_ERROR)。
int CDatagramSocket::CheckForReadability()
{
  int result = 0; //記錄是否有新資料進來。
  fd_set fdset;
  timeval waitTime = {0, 0};
  int status;
  unsigned long ioctlsocketArg;

  if (INVALID_SOCKET == m_Socket)
    return SOCKET_ERROR;

  FD_ZERO(&fdset);
  FD_SET(m_Socket, &fdset);

  //檢查有沒有新資料進來。
  status = select(0, &fdset, NULL, NULL, &waitTime);
  if (status > 0) //如果有資料可讀。
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
              //OutputDebugString("The socket data arrival event handler of the datagram socket occurs error!");
            }
          }
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
//   1: 可送出資料。
//   0: 不可送出資料。
//  -1: 發生錯誤(SOCKET_ERROR)。
int CDatagramSocket::CheckForWritability()
{
  fd_set fdset;
  timeval waitTime = {0, 0};

  if (INVALID_SOCKET == m_Socket)
    return SOCKET_ERROR;

  FD_ZERO(&fdset);
  FD_SET(m_Socket, &fdset);

  //檢查是否可送出資料並將結果傳回。
  return select(0, NULL, &fdset, NULL, &waitTime);
}
//---------------------------------------------------------------------------
bool CDatagramSocket::GetBroadcastAddress(sockaddr_in *pNetAddress)
{
  if (NULL == pNetAddress)
  {
    //OutputDebugString("CDatagramSocket::GetBroadcastAddress(): Occurs error! NULL == pAddress.");
    return false;
  }

  if (INVALID_SOCKET != m_Socket)
  {
    INTERFACE_INFO InterfaceList[20];
    unsigned long nBytesReturned;
    if (SOCKET_ERROR == WSAIoctl(m_Socket, SIO_GET_INTERFACE_LIST, 0, 0,
        &InterfaceList, sizeof(InterfaceList), &nBytesReturned, 0, 0))
    {
      //OutputDebugString("CDatagramSocket::GetBroadcastAddress(): WSAIoctl() occurs error!");
  		return false;
    }

    int nNumInterfaces = nBytesReturned / sizeof(INTERFACE_INFO);
    if (nNumInterfaces <= 1)
    {
      //OutputDebugString("CDatagramSocket::GetBroadcastAddress(): Occurs error! nNumInterfaces <= 1.");
      return false;
    }

    sockaddr_in hostAddress;
    sockaddr_in maskAddress;
    sockaddr_in *pAddress;
    u_long host_addr;
    u_long net_mask;
    u_long net_addr;
    u_long dir_bcast_addr;
    bool bFound = false;
    for (int i = 0; i < nNumInterfaces; ++i)
    {
      /*
      cout << endl;

      sockaddr_in *pAddress;
      pAddress = (sockaddr_in *) & (InterfaceList[i].iiAddress);
      cout << " " << inet_ntoa(pAddress->sin_addr);

      pAddress = (sockaddr_in *) & (InterfaceList[i].iiBroadcastAddress);
      cout << " has bcast " << inet_ntoa(pAddress->sin_addr);

      pAddress = (sockaddr_in *) & (InterfaceList[i].iiNetmask);
      cout << " and netmask " << inet_ntoa(pAddress->sin_addr) << endl;

      cout << " Iface is ";
      u_long nFlags = InterfaceList[i].iiFlags;
      if (nFlags & IFF_UP) cout << "up";
      else                 cout << "down";
      if (nFlags & IFF_POINTTOPOINT) cout << ", is point-to-point";
      if (nFlags & IFF_LOOPBACK)     cout << ", is a loopback iface";
      cout << ", and can do: ";
      if (nFlags & IFF_BROADCAST) cout << "bcast ";
      if (nFlags & IFF_MULTICAST) cout << "multicast ";
      cout << endl;
      */
      pAddress = (sockaddr_in *) & (InterfaceList[i].iiAddress);
      hostAddress = *pAddress;
      pAddress = (sockaddr_in *) & (InterfaceList[i].iiNetmask);
      maskAddress = *pAddress;

      /*
      u_long host_addr = inet_addr("172.16.77.88");   // local IP addr
      u_long net_mask = inet_addr("255.255.224.0");   // LAN netmask
      u_long net_addr = host_addr & net_mask;         // 172.16.64.0
      u_long dir_bcast_addr = net_addr | (~net_mask); // 172.16.95.255
      */
      host_addr = hostAddress.sin_addr.S_un.S_addr;  // local IP addr
      net_mask = maskAddress.sin_addr.S_un.S_addr;   // LAN netmask
      net_addr = host_addr & net_mask;
      dir_bcast_addr = net_addr | (~net_mask);

      /*
      char str[100];
      sprintf(str, "CDatagramSocket::GetBroadcastAddress(): Found a IP[%s].", inet_ntoa(hostAddress.sin_addr));
      OutputDebugString(str);
      */

      if (0x0100007F != host_addr && 0 != host_addr)
      {
        //pNetAddress->sin_addr.s_addr = htonl(dir_bcast_addr);
        pNetAddress->sin_addr.s_addr = dir_bcast_addr;

        /*
        sprintf(str, "CDatagramSocket::GetBroadcastAddress(): Select the broadcast IP[%s].", inet_ntoa(pNetAddress->sin_addr));
        OutputDebugString(str);
        */

        bFound = true;
        break;
      }
    }

    /*
    if (false == bFound)
      OutputDebugString("CDatagramSocket::GetBroadcastAddress(): Can't find the host address! false == bFound.");
    */

    return bFound;
  }
  else
  {
    //OutputDebugString("CDatagramSocket::GetBroadcastAddress(): Occurs error! INVALID_SOCKET == m_Socket.");
    return false;
  }
}
//---------------------------------------------------------------------------
bool CDatagramSocket::ReceiveFrom(char *pRecvBuffer, int *pBufferLen, char *pIP, unsigned short *pPort)
{
  bool result = true;
  int transferedLen;
  char *pBuffer = pRecvBuffer;
  sockaddr_in remoteAddress;
  int remoteAddressLen = sizeof(remoteAddress);

  if (INVALID_SOCKET == m_Socket || NULL == pBuffer || NULL == pBufferLen)
  {
    if (NULL != pBufferLen)
      *pBufferLen = 0;
    return false;
  }

  if (*pBufferLen > 0)
  {
    transferedLen = recvfrom(m_Socket, pBuffer, *pBufferLen, 0, (sockaddr *)&remoteAddress, &remoteAddressLen);
    *pBufferLen = transferedLen;

    if (0 == transferedLen || SOCKET_ERROR == transferedLen)
    {
      if (0 == transferedLen)
        Close();

      m_ErrorCode = SOCKET_ERROR;
      result = false;
    }

    m_ReceivedBytes = 0;
  }

  if (NULL != pIP)
    strcpy(pIP, inet_ntoa(remoteAddress.sin_addr));

  if (NULL != pPort)
    *pPort = ntohs(remoteAddress.sin_port);

  return result;
}
//---------------------------------------------------------------------------
bool CDatagramSocket::SendBroadcast(const char *pSendBuffer, int bufferLen, unsigned short port)
{
  /*
  CIPAddress *pIPAddress;

  //sa.sin_addr.s_addr = htonl(INADDR_BROADCAST); //用 INADDR_BROADCAST 廣播自己收不到封包。
  if (0 == g_pNetIO->GetIPManager()->GetIPAddressCount())
  {
    OutputDebugString("Can't get the IP of the host!");
    return false;
  }
  pIPAddress = g_pNetIO->GetIPManager()->GetIPAddress(0);
  if (NULL == pIPAddress)
  {
    OutputDebugString("Can't get the IP of the host!");
    return false;
  }

  return SendTo(pSendBuffer, bufferLen, "255.255.255.255", port);
  */
  bool result = true;
  sockaddr_in remoteAddress;
  int transferedLen;
  char *pBuffer = (char *)pSendBuffer;

  if (INVALID_SOCKET == m_Socket || NULL == pBuffer)
    return false;

  remoteAddress.sin_family = AF_INET;
  //remoteAddress.sin_addr.s_addr = htonl(INADDR_BROADCAST);
  remoteAddress.sin_port = htons(port);
  if (false == GetBroadcastAddress(&remoteAddress))
  {
    //OutputDebugString("CDatagramSocket::SendBroadcast(): Calling GetBroadcastAddress() returns false!");
    return false;
  }

  if (bufferLen > 0)
  {
    while (bufferLen > 0)
    {
      transferedLen = sendto(m_Socket, pBuffer, bufferLen, 0, (sockaddr *)&remoteAddress, sizeof(remoteAddress));

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
bool CDatagramSocket::SendTo(const char *pSendBuffer, int bufferLen, const char *pIP, unsigned short port)
{
  bool result = true;
  char ip[16];
  sockaddr_in remoteAddress;
  int transferedLen;
  char *pBuffer = (char *)pSendBuffer;

  if (INVALID_SOCKET == m_Socket || NULL == pBuffer || NULL == pIP || 0 == pIP[0])
    return false;

  //Get remote IP.
  if (false == CIPManager::GetIPByHostName(pIP, ip))
    return false;

  remoteAddress.sin_family = AF_INET;
  remoteAddress.sin_addr.s_addr = inet_addr(ip);
  remoteAddress.sin_port = htons(port);

  if (bufferLen > 0)
  {
    while (bufferLen > 0)
    {
      transferedLen = sendto(m_Socket, pBuffer, bufferLen, 0, (sockaddr *)&remoteAddress, sizeof(remoteAddress));

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
void CDatagramSocket::SetBroadcast(bool on)
{
  BOOL optval = on;

  m_ErrorCode = setsockopt(m_Socket, SOL_SOCKET, SO_BROADCAST, (const char *)&optval, sizeof(BOOL));
  /*
  if (SOCKET_ERROR == m_ErrorCode)
    OutputDebugString("CDatagramSocket::SetBroadcast() occurs error!");
  */
}
//---------------------------------------------------------------------------
