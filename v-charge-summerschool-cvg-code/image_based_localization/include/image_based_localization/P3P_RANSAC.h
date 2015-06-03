/*
 * RANSAC.h
 *
 *  Created on: Jul 7, 2014
 *      Author: sattlert
 *  Copyright 2014 Torsten Sattler
 */

// Implemetation of RANSAC-based camera pose estimation.

#ifndef IMAGE_BASED_LOCALIZATION_P3P_RANSAC_H_
#define IMAGE_BASED_LOCALIZATION_P3P_RANSAC_H_

#include <Eigen/Core>

#include <image_based_localization/camera_pose.h>

namespace image_based_localization {

class P3PRansac {
 public:
  P3PRansac(const double focal_length);
  ~P3PRansac();

  // Computes the camera pose by applying a P3P solver inside a standard RANSAC
  // loop using the 2D-3D matches defined by pos2D and pos3D. The i-th match is
  // given by the i-th columns of pos2D and pos3D. Returns the number of inliers
  // to the computed pose, where squared_reprojection_error is used to
  // distinguish between inliers and outliers, or -1 if there was any error
  // during the computation.
  // max_num_iterations is used to limit the number of RANSAC iterations.
  int ComputeCameraPose(const Eigen::MatrixXd& pos2D,
                        const Eigen::MatrixXd& pos3D,
                        const double squared_reprojection_error,
                        const int max_num_iterations,
                        CameraPose* pose) const;

  // Returns the number of required iterations such that the probability of
  // having missed the correct pose falls below the given threshold.
  int ComputeNumRequiredIterations(const int num_inliers, const int num_matches,
                                   const double failure_probability) const;

 private:
  double focal_length_;
};

}  // namespace image_based_localization

#endif  // IMAGE_BASED_LOCALIZATION_P3P_RANSAC_H_
