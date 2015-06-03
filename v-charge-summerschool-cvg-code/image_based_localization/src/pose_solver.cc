/*
 * PoseSolver.cc
 *
 *  Created on: Jul 7, 2014
 *      Author: sattlert
 *  Copyright 2014 Torsten Sattler
 */

#include <image_based_localization/pose_solver.h>
#include <P3P/P3p.h>

namespace image_based_localization {

PoseSolver::PoseSolver(const double focal_length)
    : focal_length_(focal_length) {}

PoseSolver::~PoseSolver() {}

int PoseSolver::ComputePose(const Eigen::Matrix<double, 2, 3>& pos2D,
                            const Eigen::Matrix<double, 3, 3>& pos3D,
                            std::vector<CameraPose>* poses) const {
  // Computes the viewing rays corresponding to the 2D image positions.
  // Notice that we use the camera model from Bundler, where the camera is
  // looking down the -z axis.
  Eigen::Matrix<double, 3, 3> viewing_rays;
  for (int i = 0; i < 3; ++i) {
    Eigen::Vector3d r(pos2D(0, i), pos2D(1, i), -focal_length_);
    viewing_rays.col(i) = r.normalized();
  }

  poses->clear();

  std::vector<Eigen::Matrix3d> rotations;
  std::vector<Eigen::Vector3d> camera_centers;
  if (ext_p3p::P3PComputePoses(viewing_rays, pos3D, &rotations, &camera_centers)
      != 0) {
    return 0;
  }

  int num_poses = static_cast<int>(rotations.size());

  poses->resize(num_poses);

  for (int i = 0; i < num_poses; ++i) {
    (*poses)[i] = CameraPose(focal_length_, rotations[i], camera_centers[i]);
  }

  return num_poses;
}

}  // namespace image_based_localization
