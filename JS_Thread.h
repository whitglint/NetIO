//---------------------------------------------------------------------------
#ifndef JS_ThreadH
#define JS_ThreadH
//---------------------------------------------------------------------------
#include "JS_IRunnable.h"
//---------------------------------------------------------------------------
namespace JS
{
  class CThread : public IRunnable
  {
    private:
      void         *m_Thread;
      unsigned int  m_ThreadID;
      IRunnable    *m_pRunnable;

      static unsigned int __stdcall StartFunc(void *pThread);

    public:
      //====== Begin: Priority constants. ======//
      static const int PRIORITY_IDLE;
      static const int PRIORITY_LOWEST;
      static const int PRIORITY_BELOW_NORMAL;
      static const int PRIORITY_NORMAL;
      static const int PRIORITY_ABOVE_NORMAL;
      static const int PRIORITY_HIGHEST;
      static const int PRIORITY_TIME_CRITICAL;
      //====== End: Priority constants. ======//

      CThread();
      CThread(IRunnable *pRunnable);
      virtual ~CThread();
      unsigned long GetExitCode();
      unsigned int GetThreadID();
      bool IsAlive();
      bool SetPriority(int Priority);
      bool Start();
      void Terminate();
      unsigned long Wait();
      unsigned long Wait(unsigned long WaitTime);

      //====== Begin: Implements IRunnable. ======//
      void Run();
      //====== End: Implements IRunnable. ======//
  };
}
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
