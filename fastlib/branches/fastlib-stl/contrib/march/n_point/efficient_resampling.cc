/*
 *  efficient_resampling.cc
 *  
 *
 *  Created by William March on 9/8/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

int npt::EfficientResampling::FindRegion_(arma::colvec& col) {
  
  int h_ind = floor(col(0) / height_step_);
  int w_ind = floor(col(1) / width_step_);
  int d_ind = floor(col(2) / depth_step_);
  
  return (h_ind + num_height_partitions_ * w_ind 
          + num_height_partitions_ * num_width_partitions_ * d_ind);
  
} // FindRegion

void npt::EfficientResampling::SplitData_() {
  
  // iterate through the points and add them to the appropriate region
  
  for (int i = 0; i < num_points_; i++) {
    
    arma::colvec& col_i = data_all_mat_.col(i);
    int region_id = FindRegion_(col_i);
    
    // TODO: is this the most efficient way to do this?
    data_mats_[region_id].insert_cols(data_mats_[region_id].n_cols, col_i);
    data_weights_[region_id].insert_rows(data_weights_[region_id].n_rows, 
                                         data_all_weights_(i));
    
  } // for i over points
  
  
} // SplitData


void npt::EfficientResampling::BuildTrees_() {
  
  random_tree_ = new NptNode(random_mat_, leaf_size_);
  
  // TODO: add old_from_new vectors for weight permutations
  for (int i = 0; i < num_resampling_regions_; i++) {
    
    data_trees_[i] = new NptNode(data_mats_[i], leaf_size_);
    
  } // for i
  
} // BuildTrees_
