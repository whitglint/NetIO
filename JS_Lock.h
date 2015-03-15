//---------------------------------------------------------------------------
#ifndef JS_LockH
#define JS_LockH
//---------------------------------------------------------------------------
#include <windows.h>
//---------------------------------------------------------------------------
namespace JS
{
  template <class CLock>
  class CGuard
  {
    private:
      CLock *m_pLock;

    public:
      inline CGuard(CLock *pLock) : m_pLock(pLock) { m_pLock->Acquire(); }
      inline ~CGuard() { m_pLock->Release(); }
  };

  class CCriticalSection
  {
    private:
      CRITICAL_SECTION m_CS;

    public:
      inline CCriticalSection() { InitializeCriticalSection(&m_CS); }
      inline ~CCriticalSection() { DeleteCriticalSection(&m_CS); }
      inline void Acquire() { EnterCriticalSection(&m_CS); }
      inline void Release() { LeaveCriticalSection(&m_CS); }
  };
}
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
