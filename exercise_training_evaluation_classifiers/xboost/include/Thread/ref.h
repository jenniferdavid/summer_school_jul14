/******************************************************************************
*   sprint::container::ref
*
*   Copyright (C) 2005-2014  Paolo Medici <www.pmx.it>
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

#ifndef _SPRINT_REF_H
#define _SPRINT_REF_H

/** \file
  * @brief reference ref class */

namespace sprint {
          
          namespace detail {

		  /// a reference holder of an object
          template<class T>
          class ref {
                T & m_ptr;
                public:
                ref(T & ptr) : m_ptr(ptr) { }
                
                T * operator ->() { return &m_ptr; }
                const T * operator ->() const { return &m_ptr; }
                T & operator *() { return m_ptr; }
                const T & operator *() const { return m_ptr; }

                operator T& () const  { return m_ptr; }

                };

		  /// a const reference holder of an object
          template<class T>
          class const_ref {
                const T & m_ptr;
                public:
                const_ref(const T & ptr) : m_ptr(ptr) { }
                
                const T * operator ->() const { return &m_ptr; }
                const T & operator *() const { return m_ptr; }
                
                operator T& () const { return m_ptr; }
                };

          } // detail
          
		  /// utility to creare const_ref object
          template<class T>
          inline sprint::detail::const_ref<T> ref(const T & ptr)
			  {
			  return sprint::detail::const_ref<T>(ptr);
			  }
          
                  /// utility to creare const_ref object
          template<class T>
          inline sprint::detail::const_ref<T> c_ref(const T & ptr)
                          {
                          return sprint::detail::const_ref<T>(ptr);
                          }   
                          
		  /// utility to create ref object
          template<class T>
          inline sprint::detail::ref<T> ref(T & ptr)
			  {
			  return sprint::detail::ref<T>(ptr);
			  }
          
          };

#endif
