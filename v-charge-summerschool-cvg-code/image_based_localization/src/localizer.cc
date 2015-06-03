/*
 * localizer.cc
 *
 *  Created on: Jul 7, 2014
 *      Author: sattlert
 *  Copyright 2014 Torsten Sattler
 */

// Main program to perform image-based localization.

#include <fstream>
#include <iostream>
#include <string>

#include <image_based_localization/kd_tree_localization.h>

int main (int argc, char **argv) {
  using namespace image_based_localization;

  if (argc < 4) {
    std::cout << "Usage: " << argv[0]
              << " binary_data query_directory/ ratio_test_threshold"
              << std::endl;
    std::cout << "binary_data: The .info file you downloaded,"
              << " including the path to the file." << std::endl;
    std::cout << "query_directory/: The path to the directory containing the"
              << " query images (don't forget the trailing /)." << std::endl;
    std::cout << "ratio_test_threshold: The threshold to be used for Lowe's"
                  << " ratio test." << std::endl;
    return 0;
  }

  std::string binary_data_file(argv[1]);
  std::string query_image_directory(argv[2]);
  double ratio_threshold = atof(argv[3]);

  if (query_image_directory.empty()) return 1;
  if (binary_data_file.empty()) return 1;

  // Loads the list of query images.
  std::vector<QueryImage> query_images;
  std::string query_image_list(query_image_directory);
  query_image_list.append("list.txt");

  std::cout << " Loading the query image information from " << query_image_list
            << std::endl;
  {
    std::ifstream ifs(query_image_list.c_str(), std::ios::in);
    if (!ifs.is_open()) {
      std::cerr << "ERROR: Could not read the list of query images from "
                << query_image_list << std::endl;
      return -1;
    }

    while (!ifs.eof()) {
      QueryImage q_img;
      ifs >> q_img.keyfile_name_ >> q_img.width_ >> q_img.height_
          >> q_img.focal_length_;
      std::string query_key_file(query_image_directory);
      query_key_file.append(q_img.keyfile_name_);
      q_img.keyfile_name_ = query_key_file;
      query_images.push_back(q_img);
    }

    ifs.close();
  }

  // Builds the kd-tree.
  KdTreeLocalization tree_localizer(ratio_threshold * ratio_threshold);

  std::cout << " Initializing the localizer" << std::flush;
  if (!tree_localizer.Initialize(binary_data_file)) {
    std::cerr << "ERROR: Could not initialize the localizer" << std::endl;
    return -1;
  }
  std::cout << " ... done" << std::endl;

  std::cout << " Constructing the kd-tree" << std::flush;
  if (!tree_localizer.BuildTree()) {
    std::cerr << "ERROR: Could not construct the kd-tree" << std::endl;
    return -1;
  }
  std::cout << " ... done" << std::endl;

  // Performs image-based localization for all query images.
  int num_localized_images = 0;
  int num_query_images = static_cast<int>(query_images.size());

  for (int i = 0; i < num_query_images; ++i) {
    std::cout << std::endl << "--------------- Image " << i + 1
              << " ---------------" << std::endl;
    LocalizationResult result;
    tree_localizer.LocalizeImage(query_images[i], &result);
    std::cout << "  Matching took " << result.search_time_ << " s" << std::endl;
    std::cout << "  Found " << result.num_unique_matches_ << " matches"
              << std::endl;
    std::cout << "  RANSAC took " << result.ransac_time_ << " s" << std::endl;
    std::cout << "  Found " << result.num_inliers_ << " inliers" << std::endl;
    if (result.num_inliers_ >= 12) {
      ++num_localized_images;
    }

    std::cout << std::endl << " Num. successfully localized images "
              << num_localized_images << " / " << i + 1 << std::endl;
  }

  return 0;
}

