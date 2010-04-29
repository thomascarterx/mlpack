/*
 * =====================================================================================
 *
 *       Filename:  allnn_test.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/11/2008 03:12:57 PM EDT
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Nikolaos Vasiloglou (NV), nvasil@ieee.org
 *        Company:  Georgia Tech Fastlab-ESP Lab
 *
 * =====================================================================================
 */

/**
 * @allnn_test.cc
 * Test file for DiskAllkNN class
 */


#include "allnn.h"
#include "fastlib/base/test.h"

#include <armadillo>
#include <fastlib/base/arma_compat.h>

class TestAllNN {
 public:
  TestAllNN(fx_module *module) {
    module_=module;
  }
  void Init() {
    allnn_ = new AllNN();
    naive_  = new AllNN();
    data_for_tree_ = new Matrix();
    data_for_naive_= new Matrix();
    arma::mat tmp_data;
    arma::mat tmp_data_naive;
    data::Load("test_data_3_1000.csv", tmp_data);
    data::Load("test_data_3_1000.csv", tmp_data_naive);
    arma_compat::armaToMatrix(tmp_data, *data_for_tree_);
    arma_compat::armaToMatrix(tmp_data_naive, *data_for_naive_);
 }

  void Destruct() {
    delete data_for_tree_;
    delete data_for_naive_;
    delete allnn_; 
    delete naive_;
  }

  void TestTreeVsNaive1() {
    Init();
    allnn_->Init(*data_for_tree_, module_);
    naive_->InitNaive(*data_for_naive_, module_);
 
    GenVector<index_t> resulting_neighbors_tree;
    GenVector<double> resulting_distances_tree;
    allnn_->ComputeNeighbors(&resulting_neighbors_tree, &resulting_distances_tree);
    GenVector<index_t> resulting_neighbors_naive;
    GenVector<double> resulting_distances_naive;
    naive_->ComputeNaive(&resulting_neighbors_naive, &resulting_distances_naive);
    for(index_t i=0; i<resulting_neighbors_tree.length(); i++) {
      TEST_ASSERT(resulting_neighbors_tree[i] == resulting_neighbors_naive[i]);
      TEST_DOUBLE_APPROX(resulting_distances_tree[i], resulting_distances_naive[i], 1e-5);
    }
    NOTIFY("Allknn test 1 passed");
    Destruct();
  }
 
  void TestAll() {
    TestTreeVsNaive1();
 }
 
 private:
  AllNN *allnn_;
  AllNN *naive_;
  Matrix *data_for_tree_;
  Matrix *data_for_naive_;
  fx_module *module_;
};

int main(int argc, char *argv[]) {
 fx_module *fx_root=fx_init(argc, argv, NULL); 
 fx_set_param_int(fx_root, "leaf_size", 20);
 TestAllNN test(fx_root);
 test.TestAll();
}
