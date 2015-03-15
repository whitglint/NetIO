//---------------------------------------------------------------------------
#include <windows.h>
#include <process.h>
#pragma hdrstop
#include "JS_Thread.h"
//---------------------------------------------------------------------------
using namespace JS;
//---------------------------------------------------------------------------
const int CThread::PRIORITY_IDLE          = THREAD_PRIORITY_IDLE;
const int CThread::PRIORITY_LOWEST        = THREAD_PRIORITY_LOWEST;
const int CThread::PRIORITY_BELOW_NORMAL  = THREAD_PRIORITY_BELOW_NORMAL;
const int CThread::PRIORITY_NORMAL        = THREAD_PRIORITY_NORMAL;
const int CThread::PRIORITY_ABOVE_NORMAL  = THREAD_PRIORITY_ABOVE_NORMAL;
const int CThread::PRIORITY_HIGHEST       = THREAD_PRIORITY_HIGHEST;
const int CThread::PRIORITY_TIME_CRITICAL = THREAD_PRIORITY_TIME_CRITICAL;
//---------------------------------------------------------------------------
CThread::CThread()
{
  m_Thread    = NULL;
  m_ThreadID  = 0;
  m_pRunnable = NULL;
}
//---------------------------------------------------------------------------
CThread::CThread(IRunnable *pRunnable)
{
  m_Thread    = NULL;
  m_ThreadID  = 0;
  m_pRunnable = pRunnable;
}
//---------------------------------------------------------------------------
CThread::~CThread()
{
  CloseHandle(m_Thread);
}
//---------------------------------------------------------------------------
unsigned long CThread::GetExitCode()
{
  unsigned long exitCode;

  GetExitCodeThread(m_Thread, &exitCode);

  return exitCode;
}
//---------------------------------------------------------------------------
unsigned int CThread::GetThreadID()
{
  return m_ThreadID;
}
//---------------------------------------------------------------------------
bool CThread::IsAlive()
{
  return (STILL_ACTIVE == GetExitCode());
}
//---------------------------------------------------------------------------
void CThread::Run()
{
  if (m_pRunnable)
    m_pRunnable->Run();
}
//---------------------------------------------------------------------------
bool CThread::SetPriority(int Priority)
{
  return SetThreadPriority(m_Thread, Priority);
}
//---------------------------------------------------------------------------
bool CThread::Start()
{
  if (!m_Thread)
  {
    m_Thread = (HANDLE)_beginthreadex(NULL, 0, StartFunc, this, 0, &m_ThreadID);
    if (!m_Thread)
    {
      //OutputDebugString("CThread::Start(): Creating the thread occurs error!");
      return false;
    }
  }

  return true;
}
//---------------------------------------------------------------------------
unsigned int __stdcall CThread::StartFunc(void *pThread)
{
  ((CThread *)pThread)->Run();

  return 0;
}
//---------------------------------------------------------------------------
void CThread::Terminate()
{
  TerminateThread(m_Thread, 1);
}
//---------------------------------------------------------------------------
unsigned long CThread::Wait()
{
  return Wait(INFINITE);
}
//---------------------------------------------------------------------------
unsigned long CThread::Wait(unsigned long WaitTime)
{
  return WaitForSingleObject(m_Thread, WaitTime);
}
//---------------------------------------------------------------------------
