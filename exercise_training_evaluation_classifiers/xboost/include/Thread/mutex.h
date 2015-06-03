/******************************************************************************
*   sprint::mutex
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

/** @file mutex.h
  * @author Paolo Medici
  * @brief mutex and threadmutex classes
  **/

#ifndef _SPRINT_MUTEX_H
#define _SPRINT_MUTEX_H

// #include <sprint/sysdef.h>
// #include <sprint/cpptraits.h>

#ifndef WIN32
# include <pthread.h>
#endif


namespace sprint {

/** scope lock design pattern
 * \code
 * sprint::scoped_lock<sprint::mutex> l(mutex);
 *
 * sprint::mutex::scoped_lock l(mutex);
 * \endcode
 **/
template<class T>
class scoped_lock {
	T & m_mutex;
	public:
        /// ctor: lock the resource
		scoped_lock(T & m) : m_mutex(m) { m_mutex.lock(); }
		/// dtor: unlock the resource
		~scoped_lock() { m_mutex.unlock(); }
};

#ifdef WIN32

/// HANDLE vs CRITICAL_SECTION
///  A Win32 CriticalSection is for intra-process synchronization 
///  (and is much faster as far as lock times), however it cannot be 
///  shared across processes.
#define USE_CRITICAL_SECTION

/* ************************************* WIN32 ******************************* */

#ifdef USE_CRITICAL_SECTION

/// mutex class (implemented using Critical Section)
class mutex: public sprint::uncopiable {
private:
	CRITICAL_SECTION m_cs;
public:
    typedef sprint::scoped_lock<sprint::mutex> scoped_lock;	
public:
	inline mutex()
	{
		 ::InitializeCriticalSection(&m_cs);
	}
	inline ~mutex()
	{
		 ::DeleteCriticalSection(&m_cs);
	}
  /// lock resource
	inline void lock()
	{
		::EnterCriticalSection (&m_cs);
	}
  /// unlock resource
	inline void unlock()
	{
	    ::LeaveCriticalSection( &m_cs );
	}

};

#else

/// mutex class (implemented using Mutex object)
class mutex: public sprint::uncopiable {
private:
	HANDLE m_mutex;
	
	/// AVOID MUTEX COPY
	mutex(const mutex & src) { }
public:
    typedef sprint::scoped_lock<sprint::mutex> scoped_lock;	
public:
	inline mutex()
	{
		m_mutex = ::CreateMutex(NULL, FALSE, NULL);
	}
	inline ~mutex()
	{
 	::CloseHandle(m_mutex);
	}

	inline bool lock(unsigned int timeout = INFINITE)
	{
		return ::WaitForSingleObject(m_mutex, timeout) == WAIT_OBJECT_0;
	}

	inline bool unlock()
	{
	return ::ReleaseMutex(m_mutex);
	}

	inline HANDLE operator HANDLE()
	{
		return m_mutex;
	}

};

#endif

/// Under Win32 Mutex are also thread dependent mutex
typedef mutex thread_mutex;

#else	// LINUX

/// Mutex (implemented using pthread)
class mutex {
private:
	pthread_mutex_t m_mutex;
public:
    typedef sprint::scoped_lock<sprint::mutex> scoped_lock;	
public:
	mutex()
	{
	pthread_mutexattr_t attr;

	pthread_mutexattr_init(&attr);

	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE_NP);

	pthread_mutex_init(&m_mutex, &attr);

	pthread_mutexattr_destroy(&attr);
	}

        ~mutex()
	{
	pthread_mutex_destroy(&m_mutex);
	}

	inline bool lock()
	{
		return pthread_mutex_lock(&m_mutex);
	}

	inline void unlock()
	{
	pthread_mutex_unlock(&m_mutex);
	}

	inline pthread_mutex_t gethandle()
	{
		return m_mutex;
	}

};

#endif	// WIN32


} // namespace sprint


#endif	// _PM_MUTEX_
