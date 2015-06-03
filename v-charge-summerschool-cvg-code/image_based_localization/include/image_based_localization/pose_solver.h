/*
 * pose_solver.h
 *
 *  Created on: Jul 7, 2014
 *      Author: sattlert
 *  Copyright 2014 Torsten Sattler
 */

// Simple pose solver that computes the absolute pose of a calibrated camera
// from three 2D-3D correspondences.

#ifndef IMAGE_BASED_LOCALIZATION_POSE_SOLVER_H_
#define IMAGE_BASED_LOCALIZATION_POSE_SOLVER_H_

#include <vector>
#include <Eigen/Core>

#include <image_based_localization/camera_pose.h>

namespace image_based_localization {

class PoseSolver {
 public:
  PoseSolver(const double focal_length);
  ~PoseSolver();

  // Computes the camera pose from three 2D-3D correspondences and returns the
  // number of computed poses.
  // The 2D and 3D points are given by the columns of pos2D and pos3D. The
  // computed poses are returned in the last parameter.
  int ComputePose(const Eigen::Matrix<double, 2, 3>& pos2D,
                  const Eigen::Matrix<double, 3, 3>& pos3D,
                  std::vector<CameraPose>* poses) const;
 private:
  double focal_length_;
};

}  // namespace image_based_localization

#endif  // IMAGE_BASED_LOCALIZATION_POSE_SOLVER_H_
