/* XBoost: Ada-Boost and Friends on Haar/ICF/HOG Features, Library and ToolBox
 *
 * Copyright (c) 2008-2014 Paolo Medici <medici@ce.unipr.it>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef _H_TIMER_
#define _H_TIMER_

/** @file timer.h
 * @brief Cross Platform High Performance timer
 * */

#include <iosfwd>

#ifdef WIN32

class Counter {
	double time_factor;    // Time Scaling Factor
public:
	Counter() {
	LONGLONG perf_cnt;
	
	if (QueryPerformanceFrequency((LARGE_INTEGER *) &perf_cnt)) 
		{
	     time_factor=1.0/perf_cnt; 
		} 
	}

	inline double GetTime()
	{
	LONGLONG time;

	QueryPerformanceCounter((LARGE_INTEGER *) &time);
	return (time * time_factor);
	}
};

#else

#include <sys/time.h>
#include <time.h>

class Counter {
	//double time_factor;    // Time Scaling Factor
	/*inline unsigned long long rdtsc()	
	{
	unsigned long long v; 

	 __asm__ ("rdtsc"  : "=A" (v)); 

	return v; 
	}*/
public:
	Counter() 
	{ 
	}

	inline double GetTime() const
	{
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	return (ts.tv_sec*1.0 + ts.tv_nsec * 0.000000001);
	}
};


#endif

class Timer: public Counter {
	double user_time;
public:
	Timer() : Counter()
	{
	user_time = Counter::GetTime();
	}

	inline void Start()
	{
	user_time = Counter::GetTime();
	}

	inline double GetTime() const
	{
	return Counter::GetTime() - user_time;
	}

};

/// Compute timing statistics
class Statistic: public Counter {
    int count;
    double min_time, max_time, sum_time, sum2_time;
    double start_time;
    double last;
    double tmp;
  public:
    void Reset() { count = 0; min_time = max_time = sum2_time = sum_time = 0.0; }
    Statistic() { Reset(); }
    
    void Start()
    {
      start_time = Counter::GetTime();
    }
    
    void Pause()
    {
      tmp = Counter::GetTime() - start_time;
    }
    
    void Resume()
    {
      start_time = Counter::GetTime() - tmp;
    }
    
    void Stop()
    {
      last = Counter::GetTime() - start_time; 
      sum_time += last;
      sum2_time += last*last;
      if(count == 0)
      {
	min_time = max_time = last;
      }
      else
      {
	if(last<min_time)
	  min_time = last;
	if(last>max_time)
	  max_time = last;
      }
      count ++;
    }
    
    double Last() const { return last; }
    double Avg() const { return sum_time / count; }
    double Var() const { return sum2_time / count - (sum_time / count) * (sum_time / count); }
    double Min() const { return min_time; }
    double Max() const { return max_time; }
    int Count() const { return count; }
    
    operator bool() const { return count>0; }
    
};

std::ostream & operator << (std::ostream &, const Statistic &);

#endif