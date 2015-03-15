//---------------------------------------------------------------------------
#ifndef JS_IRunnableH
#define JS_IRunnableH
//---------------------------------------------------------------------------
namespace JS
{
  // The interface of runnable object.
  class IRunnable
  {
    public:
      virtual ~IRunnable() {}

      // Description: The function that the thread will run.
      // Input: None.
      // Output: None.
      // Return: None.
      virtual void Run() = 0;
  };
}
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
