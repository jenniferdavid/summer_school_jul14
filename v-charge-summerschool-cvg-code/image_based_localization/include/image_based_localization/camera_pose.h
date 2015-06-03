/*
 * CameraPose.h
 *
 *  Created on: Jul 7, 2014
 *      Author: sattlert
 *  Copyright 2014 Torsten Sattler
 */

// Definition of a camera pose. Provides functionality to check whether a 2D-3D
// match is an inlier to the pose.

#ifndef IMAGE_BASED_LOCALIZATION_CAMERA_POSE_H_
#define IMAGE_BASED_LOCALIZATION_CAMERA_POSE_H_

#include <Eigen/Core>

namespace image_based_localization {

class CameraPose {
 public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

  CameraPose();
  CameraPose(const double focal_length,
             const Eigen::Matrix<double, 3, 3>& R,
             const Eigen::Vector3d& center);
  ~CameraPose();

  // Returns the position of the camera in 3D.
  Eigen::Vector3d GetCameraPosition() const;

  // Checks whether a 2D-3D match is an inlier to the pose given a threshold
  // on the squared reprojection error. Returns true if the 3D points pos3D
  // projects within the circle around the 2D position pos2D with radius
  // sqrt(squared_reprojection_error_threshold).
  bool IsInlier(const Eigen::Vector2d& pos2d, const Eigen::Vector3d& pos3d,
                const double squared_reprojection_error_threshold) const;

  // The projection matrix of the camera.
  Eigen::Matrix<double, 3, 4> projection_matrix_;
  // The 3D position of the camera.
  Eigen::Vector3d camera_position_;
};

}  // namespace image_based_localization


#endif  // IMAGE_BASED_LOCALIZATION_CAMERA_POSE_H_
