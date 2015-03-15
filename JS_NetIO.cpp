//---------------------------------------------------------------------------
#include "JS_CacheHeader.h"
#pragma hdrstop
#include "JS_IPManager.h"
#include "JS_NetIO.h"
//---------------------------------------------------------------------------
using namespace JS;
CNetIO *CNetIO::m_pNetIO = NULL;
int CNetIO::m_ReferenceCounter = 0;
//---------------------------------------------------------------------------
CNetIO::CNetIO()
{
  m_State = 0;
  m_pIPManager = NULL;
}
//---------------------------------------------------------------------------
CNetIO::~CNetIO()
{
  m_State = 0;

  delete m_pIPManager;
  m_pIPManager = NULL;

  WSACleanup();
}
//---------------------------------------------------------------------------
CNetIO *CNetIO::GetInstance()
{
  if (NULL == m_pNetIO)
    m_pNetIO = new CNetIO();

  m_ReferenceCounter++;

  return m_pNetIO;
}
//---------------------------------------------------------------------------
int CNetIO::Initialize()
{
  int state;

  if (INITIALIZED == m_State)
    return 0;

  state = WSAStartup(MAKEWORD(2, 0), &m_WSAData);
  if (0 != state)
  {
    //OutputDebugString("CNetIO::Initialize(): Calling WSAStartup() occurs error!");
    return state;
  }

  m_pIPManager = new CIPManager();
  if (NULL == m_pIPManager)
  {
    //OutputDebugString("CNetIO::Initialize(): Creating IP Manager occurs error!");
    state = -1;
  }

  m_State = INITIALIZED;

  return state;
}
//---------------------------------------------------------------------------
int CNetIO::ReleaseInstance()
{
  if (m_ReferenceCounter > 0)
  {
    m_ReferenceCounter--;

    if (0 == m_ReferenceCounter)
    {
      delete m_pNetIO;
      m_pNetIO = NULL;
    }
  }

  return m_ReferenceCounter;
}
//---------------------------------------------------------------------------
