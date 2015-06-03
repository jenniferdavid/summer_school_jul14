/*
 * CameraPose.cc
 *
 *  Created on: Jul 7, 2014
 *      Author: sattlert
 *  Copyright 2014 Torsten Sattler
 */

#include <Eigen/Core>
#include <image_based_localization/camera_pose.h>

namespace image_based_localization {

CameraPose::CameraPose()
    : projection_matrix_(Eigen::Matrix<double, 3, 4>::Zero()),
      camera_position_(Eigen::Matrix<double, 3, 1>::Zero()) {}

CameraPose::CameraPose(const double focal_length,
                       const Eigen::Matrix<double, 3, 3>& R,
                       const Eigen::Vector3d& center)
    : camera_position_(center) {
  Eigen::Matrix3d K(Eigen::Matrix3d::Identity());
  K(0, 0) = -focal_length;
  K(1, 1) = -focal_length;
  projection_matrix_.topLeftCorner<3, 3>() = K * R;
  projection_matrix_.col(3) = -projection_matrix_.topLeftCorner<3, 3>()
      * center;
}

CameraPose::~CameraPose() {}

Eigen::Vector3d CameraPose::GetCameraPosition() const {
  return camera_position_;
}

bool CameraPose::IsInlier(
    const Eigen::Vector2d& pos2d, const Eigen::Vector3d& pos3d,
    const double squared_reprojection_error_threshold) const {
  Eigen::Vector4d point;
  point.head<3>() = pos3d;
  point(3) = 1.0;
  Eigen::Vector3d proj_point = projection_matrix_ * point;
  proj_point /= proj_point(2);
  return (proj_point.head<2>() - pos2d).squaredNorm()
      <= squared_reprojection_error_threshold;
}

}  // namespace image_based_localization

