/** 
 *  @file cosine_tree.h
 *
 *  This file implement the CosineNode class which holds a list of
 *  columns of an original matrix. The node is splitted by sorting
 *  the cosine values of all columns with respect to a 'center' vector.
 *  In this way, the node is splitted in to 2 child nodes, each of
 *  which holds a subset of columns that are more correlated than
 *  in the parent node.
 *
 *  @see cosine_tree.cc
 */

#ifndef QUICSVD_COSINE_TREE_H
#define QUICSVD_COSINE_TREE_H

#include <armadillo>
#include <fastlib/fastlib.h>
#include <fastlib/base/arma_compat.h>

class CosineNode {
  /** The alias of the matrix to be approximated A ~ A' = U S VT*/
  Matrix A_;

  /** Indices of columns of matrix A in this node */
  ArrayList<int> origIndices_;

  /** L2 norms of the columns in this node */
  ArrayList<double> norms_;

  /** Cummulative sum of L2 norm squares to use in column sampling */
  ArrayList<double> cum_norms_;

  /** Mean vector to be added to the basis when this node is chosen */
  Vector mean_;

  /** Is this node the left child of its parent? */
  bool isLeft_;

  /** Error in Frobenius norm of this node when projected onto
   *  the subspace of the basis. This is use to indicate the priority
   *  of this node in the priority queue
   */
  double L2Err_;

  /** pointers to this node's parent and children */
  CosineNode *parent_, *left_, *right_;

 public:
  /** Constructor of the root cosine node */
  CosineNode(const Matrix& A);

  /** Constructor of the childen */
  CosineNode(CosineNode& parent, const ArrayList<int>& indices,
	     bool isLeft);

  /** Get a column in this node, we have to use origIndices to
   *  find the real index of column in matrix A 
   */
  void GetColumn(int i_col, Vector* col) {
    A_.MakeColumnVector(origIndices_[i_col], col);
  }

  /** Return number of columns in this node */
  index_t n_cols() const {
    return origIndices_.size();
  }

  /** Split this node into 2 nodes: left child and right child
   *  by sorting cosine value of the columns with respect to a center
   *  which is chosen randomly following the probability distribution
   *  generated by L2 norms of the columns
   */
  void Split();
  
  /** Return sum of L2 norm square of the columns */
  double getSumL2() const {
    return cum_norms_[n_cols()-1];
  }

  /** Get the mean vector */
  const Vector& getMean() const {
    return mean_;
  }

  /** Get the original index of certain column */
  index_t getOrigIndex(index_t i_col) const {
    return origIndices_[i_col];
  }

  /** Set L2 error to be used in priority queue */
  void setL2Err(double L2Err) {
    L2Err_ = L2Err;
  }

  /** Check if having left child */
  bool hasLeft() const {
    return left_ != NULL;
  }

  /** Check if having right child */
  bool hasRight() const {
    return right_ != NULL;
  }

  /** Get pointer to the left child */
  CosineNode* getLeft() {
    return left_;
  }

  /** Get pointer to the right child */
  CosineNode* getRight() {
    return right_;
  }

 private:
  // Helper functions

  /** Calculate cumulative L2 norm squares and the mean vector */
  void CalStats();

  /** Choose a center randomly following probability distribution
   *  generated by the L2 norm squares of the column 
   */
  void ChooseCenter(Vector* center);

  /** Calculate cosine values of all column with respect to the center */
  void CalCosines(const Vector& center, ArrayList<double>* cosines);

  /** Create an array list of indices 0..n_cols()-1 */
  void CreateIndices(ArrayList<int>* indices);

  /** Friend unit test class */
  friend class CosineNodeTest;

  /** Compare class to be used in priority queue */
  friend class CompareCosineNode;
};

class CompareCosineNode {
 public:
  bool operator ()(CosineNode* a, CosineNode* b) {
    return a->L2Err_ < b->L2Err_;
  }
};

class CosineNodeTest {
  FILE * logfile;

  void test_CosineTreeNode() {
    Matrix A;
    arma::mat tmpA;
    data::Load("input.txt", tmpA);
    arma_compat::armaToMatrix(tmpA, A);
    CosineNode root(A);
    //ot::Print(root, "cosine root", logfile);

    root.Split();

    //ot::Print(*root.left_, "left node", logfile);
    //ot::Print(*root.right_, "right node", logfile);
  }

public:
  CosineNodeTest() {
    logfile = fopen("LOG", "w");
  }
  ~CosineNodeTest() {
    fclose(logfile);
  }

  void run_tests() {
    test_CosineTreeNode();
  }
};


#endif
