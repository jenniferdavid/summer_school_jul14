/*
 * P3P_RANSAC.cc
 *
 *  Created on: Jul 7, 2014
 *      Author: sattlert
 *  Copyright 2014 Torsten Sattler
 */

#include <image_based_localization/P3P_RANSAC.h>

#include <iostream>
#include <limits>
#include <random>

#include <image_based_localization/pose_solver.h>

namespace image_based_localization {

P3PRansac::P3PRansac(const double focal_length) : focal_length_(focal_length) {}
P3PRansac::~P3PRansac() {}

int P3PRansac::ComputeCameraPose(const Eigen::MatrixXd& pos2D,
                                 const Eigen::MatrixXd& pos3D,
                                 const double squared_reprojection_error,
                                 const int max_num_iterations,
                                 CameraPose* pose) const {
  if (pos2D.cols() != pos3D.cols()) {
    std::cerr << "ERROR: The number of columns in pos2D and pos3D differs!"
              << std::endl;
    return 0;
  }
  if (pos2D.cols() < 3) {
    std::cerr << "WARNING: Not enough matches!" << std::endl;
    return 0;
  }
  if (max_num_iterations < 0) {
    std::cerr << "ERROR: max_num_iterations must be positive " << std::endl;
    return 0;
  }

  PoseSolver p3p_solver(focal_length_);
  int max_iterations = max_num_iterations;
  int num_matches = pos2D.cols();

  // Initializes the random number generator.
  std::minstd_rand rng_engine;
  std::uniform_int_distribution<int> rng_distribution(0, num_matches - 1);

  Eigen::Matrix<double, 2, 3> sample_pos2D;
  Eigen::Matrix<double, 3, 3> sample_pos3D;
  std::vector<int> sample_indices(3);
  std::vector<CameraPose> computed_poses;

  // The number of inliers observed for the best pose found so far.
  int max_inliers = 0;

  for (int k = 0; k < max_iterations; ++k) {
    // TODO: Randomly sample three matches by calling
    // rng_distribution(rng_engine) repeatedly to obtain three random indices.
    // Ensure that you pick three unique indices and copy the 2D and 3D
    // positions to sample_pos2D and sample_pos3D, respectively.
    // BEGIN(CODE)

    // END(CODE)

    // Computes the camera pose and returns the number of computed poses.
    int num_poses = p3p_solver.ComputePose(sample_pos2D, sample_pos3D,
                                           &computed_poses);

    // TODO: Iteratively evaluate all poses on all matches and update the best
    // pose found so far (stored in the input / output parameter pose) if
    // necessary by keeping track of the maximum number of inliers found so far.
    // BEGIN(CODE)

    // END(CODE)
  }

  return max_inliers;
}

int P3PRansac::ComputeNumRequiredIterations(
    const int num_inliers, const int num_matches,
    const double failure_probability) const {
  if (num_inliers <= 0) return std::numeric_limits<int>::max();
  if (num_inliers == num_matches) return 1;

  // TODO: Compute the inlier ratio and use it to derive the number of required
  // iterations.
  int num_required_iterations = 1;
  // BEGIN(CODE)

  // END(CODE)
  return num_required_iterations;
}

}  // namespace image_based_localization

