#include "Utility/timer.h"
#include <iostream>

std::ostream & operator << (std::ostream &o, const Statistic &s)
{
  o << "avg:" << s.Avg() << " | min:" << s.Min() << " | max:" << s.Max() << " | var:" << s.Var() << " count:" << s.Count();
  return o;
}