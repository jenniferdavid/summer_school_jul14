/*
 * kd_tree_localization.cc
 *
 *  Created on: Jul 7, 2014
 *      Author: sattlert
 *  Copyright 2014 Torsten Sattler
 */

#include <image_based_localization/kd_tree_localization.h>

#include <chrono>
#include <fstream>
#include <iostream>
#include <map>
#include <stdint.h>

#include <image_based_localization/P3P_RANSAC.h>

namespace image_based_localization {

float SIFTDescDist(const std::vector<unsigned char>& a,
                   const std::vector<unsigned char>& b) {
  float dist = 0.0f;
  for (int i = 0; i < 128; ++i) {
    float x = static_cast<float>(a[i]) - static_cast<float>(b[i]);
    dist += x * x;
  }
  return dist;
}

KdTreeLocalization::KdTreeLocalization(
    const float squared_ratio_test_threshold)
    : kd_tree_(0),
      squared_ratio_test_threshold_(squared_ratio_test_threshold) {}

KdTreeLocalization::~KdTreeLocalization() {
  if (kd_tree_ != 0) {
    delete kd_tree_;
    kd_tree_ = 0;
  }
}

bool KdTreeLocalization::Initialize(const std::string& filename) {
  point_information_.clear();

  // open file for reading
  std::ifstream ifs(filename, std::ios::in | std::ios::binary);
  if (!ifs) {
    std::cerr << "Cannot read file " << filename << std::endl;
    return false;
  }

  uint32_t num_cams = 0;
  uint32_t num_points = 0;

  // Ignores the database camera information present in the binary file.
  ifs.read((char*) &num_cams, sizeof(uint32_t));

  for (uint32_t i = 0; i < num_cams; ++i) {
    double focal_length, kappa_1, kappa_2;
    int32_t width, height;
    double *rotation = new double[9];
    double *translation = new double[3];
    ifs.read((char*) &focal_length, sizeof(double));
    ifs.read((char*) &kappa_1, sizeof(double));
    ifs.read((char*) &kappa_2, sizeof(double));
    ifs.read((char*) &width, sizeof(int32_t));
    ifs.read((char*) &height, sizeof(int32_t));
    ifs.read((char*) rotation, 9 * sizeof(double));
    ifs.read((char*) translation, 3 * sizeof(double));

    delete[] rotation;
    delete[] translation;
  }

  // Loads the 3D points and their associated information.
  ifs.read((char*) &num_points, sizeof(uint32_t));

  if (num_points == 0)
    return false;

  point_information_.resize(num_points);

  for (uint32_t i = 0; i < num_points; ++i) {
    float *pos = new float[3];
    ifs.read((char*) pos, 3 * sizeof(float));
    for (int j = 0; j < 3; ++j) {
      point_information_[i].point_(j) = pos[j];
    }

    uint32_t size_view_list = 0;
    ifs.read((char*) &size_view_list, sizeof(uint32_t));

    point_information_[i].keypoints_.resize(size_view_list);

    unsigned char *desc = new unsigned char[128];
    for (uint32_t j = 0; j < size_view_list; ++j) {
      float x, y, scale, orientation;
      uint32_t cam_id;
      ifs.read((char*) &cam_id, sizeof(uint32_t));
      ifs.read((char*) &x, sizeof(float));
      ifs.read((char*) &y, sizeof(float));
      ifs.read((char*) &scale, sizeof(float));
      ifs.read((char*) &orientation, sizeof(float));
      point_information_[i].keypoints_[j].x_ = x;
      point_information_[i].keypoints_[j].y_ = y;
      point_information_[i].keypoints_[j].scale_ = scale;
      point_information_[i].keypoints_[j].orientation_ = orientation;
      point_information_[i].keypoints_[j].descriptor_.resize(128);
      point_information_[i].keypoints_[j].camera_id_ = static_cast<int>(cam_id);
      ifs.read((char*) desc, 128 * sizeof(unsigned char));

      // save the descriptor
      for (uint32_t k = 0; k < 128; ++k) {
        point_information_[i].keypoints_[j].descriptor_[k] = desc[k];
      }
    }
    delete[] desc;
  }
  ifs.close();

  return true;
}

bool KdTreeLocalization::BuildTree() {
  if (point_information_.empty()) return false;

  // TODO: For each 3D point, compute the mean descriptor and store all
  // descriptors in a FLANN matrix (such that each descriptor is stored in a
  // single row).
  // BEGIN(CODE)

  // END(CODE)

  // TODO: Build a KDTreeIndex with one tree (see FLANN documentation in
  // ext/flann-1.8.4-src/doc) using the L2 distance functor.
  // BEGIN(CODE)

  // END(CODE)

  return true;
}

void KdTreeLocalization::LocalizeImage(const QueryImage& query_image,
                                       LocalizationResult* result) {
  using namespace std::chrono;

  result->num_inliers_ = 0;
  result->ransac_time_ = 0.0;
  result->search_time_ = 0.0;

  // Loads the keypoints.
  std::vector<SIFTKeypoint> query_keypoints;
  if (!LoadSIFTKeypoints(query_image.keyfile_name_, &query_keypoints)) {
    return;
  }

  // Centers all keypoints such that the point (0, 0) is in the center of the
  // query image.
  int num_query_keypoints = static_cast<int>(query_keypoints.size());
  for (int i = 0; i < num_query_keypoints; ++i) {
    query_keypoints[i].x_ -= static_cast<double>(query_image.width_ - 1) / 2.0;
    // Lowe's image coordinate system has the y-axis pointing downwards while
    // our system has the y-axis pointing upwards.
    query_keypoints[i].y_ = static_cast<double>(query_image.height_ - 1) / 2.0
        - query_keypoints[i].y_;
  }

  // Performs tree-based matching and returns (keypoint, 3D point) pairs that
  // form 2D-3D matches.
  high_resolution_clock::time_point t1 = high_resolution_clock::now();
  std::vector<std::pair<int, int>> matches;
  TreeBasedMatching(query_keypoints, &matches);
  result->num_unique_matches_ = static_cast<int>(matches.size());
  high_resolution_clock::time_point t2 = high_resolution_clock::now();
  result->search_time_ = duration_cast<duration<double>>(t2 - t1).count();

  // TODO: Copy the 2D and 3D point positions into two Eigen Matrices using the
  // indices stored in matches. Each column stores a 2D position or 3D point.
  // BEGIN(CODE)

  // END(CODE)

  // Performs RANSAC-based camera pose estimation. We allow a reprojection error
  // of sqrt(3) pixels.
  // TODO: Set the number of RANSAC iteration such that RANSAC is able to handle
  // inlier ratios of >= 10 with a probability of 1% to miss the correct pose.
  high_resolution_clock::time_point t3 = high_resolution_clock::now();
  // BEGIN(CODE)

  // END(CODE)
  high_resolution_clock::time_point t4 = high_resolution_clock::now();
  result->ransac_time_ = duration_cast<duration<double>>(t4 - t3).count();
}

void KdTreeLocalization::TreeBasedMatching(
    const std::vector<SIFTKeypoint>& keypoints,
    std::vector<std::pair<int, int> >* matches) const {
  int num_query_keypoints = static_cast<int>(keypoints.size());
  matches->clear();

  // TODO: Copy the descriptors of the query features into a single FLANN
  // matrix such that each descriptor fills a single row.
  // BEGIN(CODE)

  // END(CODE)


  // TODO: Query the kd-tree. For each query descriptor, obtain the indices
  // of its two nearest neighboring points and the squared distances to
  // the corresponding descriptors. Visiting 300 leaf nodes is a good parameter
  // for flann::SearchParams.
  // BEGIN(CODE)

  // END(CODE)

  // TODO: Apply Lowe's ratio test using squared_ratio_test_threshold_ as the
  // ratio test threshold. Store the id's to all matches passing the test.
  // BEGIN(CODE)

  // END(CODE)

  // TODO: Ensure that we have 1-to-1 matches, i.e., that each feature in the
  // query image has at most one matching 3D point and each 3D point is matched
  // against at most one image feature. If there are multiple possible matches,
  // keep only the one with the smallest descriptor distance.
  // Use only the matches passing the ratio test for this filtering step and
  // store the remaining matches in the output parameter matches (as (feature
  // id, point id) pairs).
  // BEGIN(CODE)

  // END(CODE)
}

bool KdTreeLocalization::LoadSIFTKeypoints(
    const std::string filename, std::vector<SIFTKeypoint>* keypoints) const {
  keypoints->clear();

  std::ifstream instream(filename, std::ios::in);

  if (!instream.is_open()) {
    std::cerr << "ERROR: Could not load the SIFT features from " << filename
              << std::endl;
    return false;
  }

  int size_descriptor = 0;
  int num_keypoints = 0;
  instream >> num_keypoints >> size_descriptor;

  if (size_descriptor != 128) {
    return false;
  }

  // load the keypoints and their descriptors
  keypoints->resize(num_keypoints);

  for (uint32_t i = 0; i < num_keypoints; ++i) {
    (*keypoints)[i].descriptor_.resize(128);
    instream >> (*keypoints)[i].y_ >> (*keypoints)[i].x_
        >> (*keypoints)[i].scale_ >> (*keypoints)[i].orientation_;

    // read the descriptor
    for (int j = 0; j < 128; ++j) {
      int desc_element;
      instream >> desc_element;
      (*keypoints)[i].descriptor_[j] = desc_element;
    }
  }
  instream.close();

  return true;
}

}  // namespace image_based_localization

