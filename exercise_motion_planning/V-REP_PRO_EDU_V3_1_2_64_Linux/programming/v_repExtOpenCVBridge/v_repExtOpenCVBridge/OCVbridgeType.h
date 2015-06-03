/*
 * v-rep OpenCVBridge v1.1
 * by Marco "marcomurk" Bellaccini - marco.bellaccini[at!]gmail.com
 *
 * This software is provided "as is", without any warranty,
 * under the terms of the GNU LESSER GENERAL PUBLIC LICENSE, Version 3
 *
 */

#ifndef OCV_BRIDGE_TYPE
#define OCV_BRIDGE_TYPE

#include <opencv2/core/core.hpp>

typedef struct OCVBridgeS
{

  cv::Mat img;
  std::vector<float> returnValues;

} OCVBridge;

#endif
