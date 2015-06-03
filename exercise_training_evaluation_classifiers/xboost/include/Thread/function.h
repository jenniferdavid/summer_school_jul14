/******************************************************************************
*   sprint::thread (function)
*
*   Copyright (C) 2005-2011  Paolo Medici <www.pmx.it>
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

#ifndef _SPRINT_THREAD_FUNCTION_H
#define _SPRINT_THREAD_FUNCTION_H

/** @file thread/detail/function.h
  * @brief thread_function fornisce i metodi per adattare un Invoker a una funzione per thread
  **/

#ifdef WIN32
# include <windows.h>
#else
# include <pthread.h>
#endif

namespace sprint {
          
#ifdef WIN32

    /// win32 thread creation params
    struct thread_function {
	  // Some Traits
        	
      /// thread retun type
	  typedef DWORD ReturnType;
	  /// thread param type
	  typedef void * ParamType;
	      
	  /// The callback type supporter by local thread architecture
	  typedef LPTHREAD_START_ROUTINE CallBackType;
	  
	  /// the callback
	  CallBackType proc;
	  
	  /// the param passed to callback
	  ParamType param;
	  
	  thread_function(CallBackType _proc, ParamType _param) : proc(_proc), param(_param) { }
	  };

	namespace detail {
	  
    /// 0 parameters static linked ptr_fun
	template<class R, R (*Func)()>
	DWORD __stdcall CALLBACK embedded_ptr_fun(void *__Unused)
	{
	  return Func();
	}

	// static linking for a member function R (T::*F)()
	template<class R, class T, R (T::*F)()>
	DWORD __stdcall CALLBACK embedded_mem_fun(void *p)
	{
	  return ((reinterpret_cast<T*>(p))->*F)();
	}

    /// @brief a jumper function to adapt a _Invoker object to a thread callback function
    /// an invoker is a template class with ParamType type and a static run method
    /// the ParamType will be released after run.
	template<class _Invoker>
	DWORD __stdcall CALLBACK thread_fun(void *_p)
	{
	  typename _Invoker::ParamType *p = reinterpret_cast< typename _Invoker::ParamType *> (_p);

	  _Invoker::run(*p);
	  
	  delete p;
	  return 0;
	}
	
	}
	
#else

/// pthread thread creation params
    struct thread_function {
	  // Some Traits
        	
	  typedef void * ReturnType;
	  typedef void * ParamType;
	      
	  /// The callback type supporter by local thread architecture
	  typedef void *(*CallBackType) (void *);
	  
	  CallBackType proc;
	  ParamType param;
	  
	  thread_function(CallBackType _proc, ParamType _param) : proc(_proc), param(_param) { }
	  };
	
	namespace detail {
	  
    /// 0 parameters ptr_fun
	template<class R, R (*Func)()>
	void * embedded_ptr_fun(void *__Unused)
	{
	  return Func();
	}

	// jumper: funzione trampolino per chiamare una funzione R (T::*F)()
	// NOTA: il valore di R e' importante... come fare per ottenerlo automaticamente?
	template<class R, class T, R (T::*F)()>
	void * embedded_mem_fun(void *p)
	{
	  return ((reinterpret_cast<T*>(p))->*F)();
	}

	template<class _Invoker>
	void * thread_fun(void *_p)
	{
	  typename _Invoker::ParamType *p = reinterpret_cast< typename _Invoker::ParamType *> (_p);

	  _Invoker::run(*p);
	  
	  delete p;
	  return 0;
	}
	
	}
		  
#endif
  
} // naemspace sprint

#endif
