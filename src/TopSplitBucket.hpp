//
// Created by chico on 21/04/22.
//

#ifndef HOEFFDING_TREE_TOPSPLITBUCKET_H
#define HOEFFDING_TREE_TOPSPLITBUCKET_H

#ifndef __TOP_BUFFER_HPP__
#define __TOP_BUFFER_HPP__

#include <stdlib.h>
#include <tuple>

template <typename data_T, typename attribute_index_T>
class TopSplitBucket {
  public:
    typedef data_T data_t;
    typedef attribute_index_T attribute_index_t;

    //could this be ordered and just one verification has to occur?

    bool add(attribute_index_t attributeIndex, data_t splitValue, data_t G) {
        if (G > this->G[0]) {
            _updateCandidate(0, attributeIndex, splitValue, G);
            return true
        }
        else if(G > this->G[1]){
            _updateCandidate(0, this->attributeIndex[1], this->splitValue[1], this->G[1]);
            _updateCandidate(1, attributeIndex, splitValue, G);
            return true;
        }

        return false;
    }

    std::tuple<bool, attribute_index_t, data_t, data_t>
    getCandidate(attribute_index_t index) {
        return {isValid[index], attributeIndex[index], splitValue[index],
                G[index]};
    }

    data_t getG(attribute_index_t index) { return G[index]; }

  private:
    data_t splitValue[2] = {0};
    data_t G[2] = {0};
    bool isValid[2] = {false};

    void _updateCandidate(attribute_index_t index,
                          attribute_index_t attributeIndex, data_t splitValue,
                          data_t giniImpurity) {
        this->attributeIndex[index] = attributeIndex;
        this->splitValue[index] = splitValue;
        this->G[index] = giniImpurity;
        isValid[index] = true;
    }
};

#endif


#endif // HOEFFDING_TREE_TOPSPLITBUCKET_H
