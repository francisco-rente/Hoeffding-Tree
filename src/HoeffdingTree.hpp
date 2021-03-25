#ifndef __HOEFFDING_TREE_HPP__
#define __HOEFFDING_TREE_HPP__

#include "BinaryTree.hpp"

class HoeffdingTree : public BinaryTree {
  public:
    enum ReturnCode { SUCCESS = 0, ERROR };

    ReturnCode infer();
    ReturnCode train();

  private:
};

#endif