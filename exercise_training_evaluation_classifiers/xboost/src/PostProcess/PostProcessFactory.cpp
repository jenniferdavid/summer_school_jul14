#include "PostProcessFactory.h"
#include <iostream>

bool RegisterPostProcess(PostProcessMap &m, const std::string &s, PostProcessCallback p)
{
  m[s] = p;
  return true;
}

bool CallPostProcess(const PostProcessMap &m, const std::string &k, std::istream & param)
{
PostProcessMap::const_iterator i = m.find(k);
if(i==m.end())
{
  std::cerr << k << " not found. Available classifiers are:\n";
  for(PostProcessMap::const_iterator k = m.begin(); k!=m.end(); ++k)
    std::cerr << k->first << ' ';
  std::cerr << std::endl;
  return false;
}
else
{
  return (i->second)(param);
}
}