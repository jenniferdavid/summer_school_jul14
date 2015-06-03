/*
 * kd_tree_localization.h
 *
 *  Created on: Jul 7, 2014
 *      Author: sattlert
 *  Copyright 2014 Torsten Sattler
 */

// Class that performs image-based localization by computing 2D-3D matches via
// kd-tree search, followed by RANSAC-based pose estimation.

#ifndef IMAGE_BASED_LOCALIZATION_KD_TREE_LOCALIZATION_H_
#define IMAGE_BASED_LOCALIZATION_KD_TREE_LOCALIZATION_H_

#include <string>
#include <vector>

#include <Eigen/Core>
#include <flann/flann.hpp>

#include <image_based_localization/camera_pose.h>

namespace image_based_localization {

// A query image, represented by the filename of the text file containing its
// SIFT descriptors, the width and height of the image, and the focal length of
// the camera.
struct QueryImage {
  std::string keyfile_name_;
  int width_;
  int height_;
  double focal_length_;
};

struct LocalizationResult {
  CameraPose pose_;
  int num_inliers_;
  int num_unique_matches_;
  double ransac_time_;  // The time required to run RANSAC, in seconds.
  double search_time_;  // The time required for kd-tree search.
};

struct SIFTKeypoint {
  double x_;
  double y_;
  double scale_;
  double orientation_;
  std::vector<unsigned char> descriptor_;
  // The id of the database image in which the feature was seen or -1 if the
  // feature was extracted from a database image.
  int camera_id_;
};

// Information about a single 3D point.
struct PointInformation {
  Eigen::Vector3d point_;
  // The image keypoints associated with the 3D point.
  std::vector<SIFTKeypoint> keypoints_;
};

class KdTreeLocalization {
 public:
  // The ratio test threshold to be used for Lowe's ratio test. Notice that you
  // need to pass the squared ratio test threshold!
  KdTreeLocalization(const float squared_ratio_test_threshold);
  ~KdTreeLocalization();

  // Loads 3D point, co-visibility, and appearance information from a binary
  // file. Returns false if initialization failed.
  bool Initialize(const std::string& filename);

  // Builds the kd-tree using the appearance information obtained by calling
  // Initialize. Returns false if building the tree failed.
  bool BuildTree();

  // Localizes an query image. Returns the computed pose, the number of
  // inliers, and other usefull information.
  void LocalizeImage(const QueryImage& query_image, LocalizationResult* result);

 protected:
  // Given a set of SIFT keypoints computes the corresponding 3D points by
  // nearest neighbor search through the kd-tree. Returns a set of (feature id,
  // point id) pairs for those features whose two nearest neighbors pass Lowe's
  // ratio test.
  void TreeBasedMatching(
      const std::vector<SIFTKeypoint>& keypoints,
      std::vector<std::pair<int, int> >* matches) const;

  // Loads SIFT keypoints from a text file in Lowe's file format. Returns false
  // if the keypoints could not be loaded.
  bool LoadSIFTKeypoints(const std::string filename,
                         std::vector<SIFTKeypoint>* keypoints) const;


 private:
  std::vector<PointInformation> point_information_;
  flann::Index<flann::L2<unsigned char> >* kd_tree_;
  float squared_ratio_test_threshold_;
};

}  // namespace image_based_localization


#endif  // IMAGE_BASED_LOCALIZATION_KD_TREE_LOCALIZATION_H_
