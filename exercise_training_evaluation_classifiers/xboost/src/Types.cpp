#include "Types.h"
#include <algorithm>

bool overlap(const rect &a, const rect & b)
{
  double w_over = std::min(a.x1, b.x1) - std::max(a.x0, b.x0);
  double h_over = std::min(a.y1, b.y1) - std::max(a.y0, b.y0);
  return w_over>0.0 && h_over>0.0; 
}

double boxoverlap(rect a, rect b, OverlapCriterion criterion){

  // get overlapping area
  double x1 = std::max(a.x0, b.x0);
  double y1 = std::max(a.y0, b.y0);
  double x2 = std::min(a.x1, b.x1);
  double y2 = std::min(a.y1, b.y1);

  // compute width and height of overlapping area
  double w = x2-x1;
  double h = y2-y1;

  // set invalid entries to 0 overlap
  if(w<=0 || h<=0)
    return 0;

  // get overlapping areas
  double inter = w*h;
  double a_area = (a.x1-a.x0) * (a.y1-a.y0);
  double b_area = (b.x1-b.x0) * (b.y1-b.y0);

  // intersection over union overlap depending on users choice
  switch(criterion)
  {
    case Overlap_On_Union:
    return inter / (a_area+b_area-inter);
    case Overlap_On_First:
    return inter / a_area;
    case Overlap_On_Second:
    return inter / b_area;
    case Overlap_On_Min:
    return inter / std::min(a_area,b_area);
    case Overlap_On_Max:
    return inter / std::max(a_area,b_area);
}
}
