/******************************************************************************
*   SPRINT::thread
*
*   Copyright (C) 2003-2011  Paolo Medici <www.pmx.it>
*
*  This library is free software; you can redistribute it and/or
*  modify it under the terms of the GNU Lesser General Public
*  License as published by the Free Software Foundation; either
*  version 2.1 of the License, or (at your option) any later version.
*
*  This library is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*  Lesser General Public License for more details.
*
*  You should have received a copy of the GNU Lesser General Public
*  License along with this library; if not, write to the Free Software
*  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*
*******************************************************************************/

/** @file thread.h
  * @author Paolo Medici
  * @brief abstracting thread
  **/
  
#ifndef _THREAD_H
#define _THREAD_H

#include <unistd.h>
#include "function.h"

#ifdef WIN32

namespace sprint {

/** a Wrapper per C/C++ Thread
  * a thread object.
  * \code
  * th.run<ServerCom,DWORD,&ServerCom::Thread>(this);
  * \endcode
  **/
class thread {
  HANDLE hThread;
  DWORD dwThreadId;
  
  private:
  /// Uncopiable
  thread(const thread & src) { }

  public:
		
  /// Ctor
  thread() : hThread(INVALID_HANDLE_VALUE), dwThreadId(0) {} 

  ~thread()
  {
  if(hThread != INVALID_HANDLE_VALUE)
   ::CloseHandle(hThread);
  } 


  /** run per C thread using a sprint::thread::function
  * \code
  * m_thread.create_thread(sprint::thread_function( ... ) );
  * \endcode
  */
  inline bool create_thread(const sprint::thread_function & p)
  {
  	hThread = ::CreateThread(NULL, 
		0,
		p.proc, 
		p.param, 
		0, 
		&dwThreadId);
		return hThread != INVALID_HANDLE_VALUE;
  }
  

  /// return the status of thread and eventually the exitcode
  /// (WIN32 specific function)
  inline bool get_exit_code(LPDWORD lpdwExitCode)
    {
      return ::GetExitCodeThread(hThread, lpdwExitCode)!=0;
    }
  
  /// Test if thread is terminated
  /// (WIN32 specific function)
  inline bool is_terminated() const
    {
      if(hThread==INVALID_HANDLE_VALUE) return true;
	  return ::WaitForSingleObject(hThread, 0) == WAIT_OBJECT_0	;
    }
    
  /// test if thread is running
  /// (WIN32 specific function)
  inline bool is_running() const
  	{
      if(hThread==INVALID_HANDLE_VALUE) return false;
      return ::WaitForSingleObject(hThread, 0) != WAIT_OBJECT_0	;
    }

  /// force termination of thread
  /// @note this is a dangerous function and should not be used. Heap and mutex are not released.
  inline bool kill()
  	{
		return ::TerminateThread(hThread, 0)!=0;
	}

  /// wait for thread terminataion
  bool join(unsigned int timeout = INFINITE)
    {
      return ::WaitForSingleObject(hThread, timeout) == WAIT_OBJECT_0	;
    }

  static int hardware_concurrency()
  {
    SYSTEM_INFO info={{0}};
    ::GetSystemInfo(&info);
    return info.dwNumberOfProcessors;
  }
    
};

/** @brief a java-like runnable task using virtual function.
  *  class must implement a void *the_thread() method.
  *
  * \code
  *  class MyClass: public sprint::runnable {
  * void *the_thread() { .... }
  * };
  * ....
  * MyClass class;
  * class.run();
  * class.join();
  * \endcode
  **/
class runnable: public thread {
   
   // jump method
   static DWORD __stdcall CALLBACK _thread_proc(void *p)
    {
    return (DWORD) (reinterpret_cast<runnable*>(p))->the_thread();
    }
    
    public:

    /// class must implement the thread method
    virtual void * the_thread() = 0;
    
    virtual ~runnable() { }

    /// run the thread method
    void run()
    {
        thread::create_thread(sprint::thread_function(_thread_proc, reinterpret_cast<void*>(this)) );
    }
    
};


} // sprint

#else

#include <pthread.h>

namespace sprint {

class thread {
  pthread_t m_thread;
  private:
  /// Uncopiable
  thread(const thread & src) { }
  public:
		
  public:
		
  /// Ctor
  thread()  {} 

  ~thread()
  {
  } 

  /// run per C thread using a sprint::thread::function
  inline bool create_thread(const sprint::thread_function & p)
  {
  	int ret = pthread_create(&m_thread, 0, 
				 p.proc, 
				 p.param);
  }
  
  /// force termination of thread
  inline bool kill() const
  	{
	// TODO
	return false;
	}

  /// wait for thread terminataion
  bool join() const
    {
      return pthread_join(m_thread, 0);
    }

  static int hardware_concurrency()
  {
         int const count=sysconf(_SC_NPROCESSORS_ONLN);
        return (count>0)?count:0;
  }
};

class runnable: public thread {
   
   static void * _thread_proc(void *p)
    {
    return (void *) (reinterpret_cast<runnable*>(p))->the_thread();
    }
    
    public:
    virtual void * the_thread() = 0;
    
    virtual ~runnable() { }

    /// run the thread method
    void run()
    {
        thread::create_thread(sprint::thread_function(_thread_proc, reinterpret_cast<void*>(this)) );
    }
};

}

#endif

#endif
