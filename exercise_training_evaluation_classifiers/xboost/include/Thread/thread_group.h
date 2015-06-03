/******************************************************************************
*   SPRINT::thread_group
*
*   Copyright (C) 2011  Paolo Medici <www.pmx.it>
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

/** @file thread_group.h
  * @author Paolo Medici
  * @brief proposal 1 for thread group
  **/

#ifndef _THREAD_POOL_H
#define _THREAD_POOL_H

#include "function.h"
#include <vector>

namespace sprint {

     /** a thread_group is a collection of thread objects. the class is responsible of wait all and destroy all*/          
#ifdef WIN32
	class thread_group {
	  
		std::vector<HANDLE> m_threads;
		
	private:
		/// Uncopiable
		thread_group(const thread_group & src) { }

	public:
		      
	      thread_group() { }

          /// release memory associated to thread
	      ~thread_group()
	      {
	      for(std::vector<HANDLE>::const_iterator i = m_threads.begin(); i!= m_threads.end(); ++i)
		                                          ::CloseHandle(*i);
	      }

	      /// create an additional thread
	      bool create_thread(const sprint::thread_function & p)
	      {
	      HANDLE hThread;
	      DWORD dwThreadId;
	      hThread = CreateThread(NULL, 
			      0,
			      p.proc, 
			      p.param, 
			      0, 
			      &dwThreadId);
		      if(hThread != INVALID_HANDLE_VALUE)
			  m_threads.push_back(hThread);
		      return hThread != INVALID_HANDLE_VALUE;
	      }
		      

        /// wait all threads terminate
	    void join_all()
		{
		::WaitForMultipleObjects(m_threads.size(), &m_threads[0], TRUE, INFINITE);
		}
		
	};
#else

                class thread_group {

                std::vector<pthread_t> m_threads;

        private:
                /// Uncopiable
                thread_group(const thread_group & src) { }

        public:

              thread_group() { }

          /// release memory associated to thread
              ~thread_group()
              {
//               for(std::vector<HANDLE>::const_iterator i = m_threads.begin(); i!= m_threads.end(); ++i)
//                                                           ::CloseHandle(*i);
              }

              /// create an additional thread
              bool create_thread(const sprint::thread_function & p)
              {
              pthread_t thread;
              int ret = pthread_create(&thread, 0,
                                 p.proc,
                                 p.param);
              m_threads.push_back(thread);
                
              }


        /// wait all threads terminate
            void join_all()
                {
                for(std::vector<pthread_t>::iterator i = m_threads.begin(); i!=m_threads.end(); ++i)
                {
                  pthread_join(*i,0);
                }
                }

        };


#endif
          
}

#endif

