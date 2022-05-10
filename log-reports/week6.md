# Week 6 

- Week 5: Easter vacations


## Work developed 

### TopSplitBuffer Refactor 

- As mentioned in the previous week's report, this function had the pipelined attribute set to false and was flagged as a possible performance bottleneck (not significantely).
- This function describes a loop that goes through values that are smaller than 
- The professor suggested using the insertion sort implementation here, that could possibly improve de performance. 

> https://xilinx.github.io/Vitis_Libraries/database/2020.1/guide/hw_api.html#insertsort

- He noted that this class is only instantiated with size_t = 2, being a simple bucket with two positions: if the new data as a gini value bigger than the first position (one ), it goes in that position (the loop matches the first element). Else, if it's bigger than the second element we shift the current values to the left and place the new element in the second position. The behaviour is the same as before, however for loops are avoided and simple conditions are placed in the method. 

- Question, is it sorted by default, could it be smaller than the first value if it's bigger than the second? Is the shifting done right? 

#### TopSplitBuffer 
```cpp
template <uint size_T, typename data_T, typename attribute_index_T>
class TopSplitBuffer {
  public:
    typedef data_T data_t;
    typedef attribute_index_T attribute_index_t;

    // TODO: fine a better method (possible xillinx api for this insertion sort)
    bool add(attribute_index_t attributeIndex, data_t splitValue, data_t G) {
    TopSplitBuffer_add__size:
        for (attribute_index_t i = 0; i < size_T; i++) {
            if (G > this->G[i]) {
            TopSplitBuffer_add__size__size:
                for (attribute_index_t j = size_T - 1; j > i; j--) {
                    _updateCandidate(j, this->attributeIndex[j - 1],
                                     this->splitValue[j - 1], this->G[j - 1]);
                }
                _updateCandidate(i, attributeIndex, splitValue, G);
                return true;
            }
        }
        return false; // value not inserted G < G[i]
    }
```


#### TopSplitBucket

```cpp
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
```

- We're planning to answer the following question: Is a if else statement more friendly or less friendly than a for loop with no pipeline possibilty to the fpga performance. 


## Data Results 

- Specs lab pc was offline for remote access and other students were using it in the lab, so I was unable to run tests. I plan to run them the next week.


### SortSample nodes 

- This function is past analyser runs was declared as a major 



#### Sort sample 

```cpp 
node_index_t sortSample(data_t x[]) {
    node_index_t newNode = getRootNodeIndex(), node;

BinaryTree_sortSample__nodes:
    do {
        #pragma HLS LOOP_TRIPCOUNT min = 1 max = capacity
        node = newNode;
        newNode = getNode(node).sortSample(x);
    } while (newNode != 0);

    return node;
}

```

#### Node Sort Sample

```cpp
 node_index_t sortSample(data_t x[]) {

        if (!isSplit()) {
            return 0;
        }

        if (_checkSplit(x)) {
            return getLeftChild();
        } else {
            return getRightChild();
        }
    }
```

- The tree itself contains useful information for each attribute (in each node), used to calculate the respective split value of new generated data, and is the backbone of the algorithm. 
- Since a binary tree needs to be sorted in each add or remove operation and recursion is out of option for an fpga board, a do while "until no more refinements are possibl" approach is taken. The current structure of the tree is the following: 

![Current binary tree organization](/log-reports/week6_files/current_tree_organization.png)

- This uneven organization is helpful, since we don't need blank spaces to "pad" blank child nodes in every parent, ordering them in a continuous array (|N1|N2|N3|N6|N7..., instead of |N1|N2|N3|X|X|N6|N7 ) and saving a lot of unused space. Doing so, however, takes away the common 2p + 1 and 2p deterministic access of the array, following the tree structure, therefore creating a lot of slow access steps and a huge bottleneck, for operations that are performed several times for a supposely big tree (although the max number of nodes is defined). 

- A possible alteration was mentioned by the teacher: 
- Having a structure that encompasses the four numeric arrays (K the attribute in which the split is performed, v_k the value of that attribute, the left and right childs), would take advantage of the large array handling (storing and access). For a given attribute in the top array, we get the respective split value from the second array and decide which node id we select adding it as a next array(s) entry. The access is done in a more linear fashion, we can allocate the array max size because we know the number of max nodes.

#### Example for the previous tree 
![Possible refactor](/log-reports/week6_files/possible_tree_refactor.png)


- As mentioned by the professor, large arrays are things the fpga board handles well, in the enqueue/migration fase we can calculate and pre fabricate this structure and pass it on into the memory blocks of the board, very close to the logic fabric, and efficient in access.
- However, since the memory blocks are independent, a data coherence/consistency issue (simmilar to cache vs RAM in a regular computer would arise). 

![Dev scheme](/log-reports/week6_files/fpgascheme.png)

- A sparse array matrix was also discussed (storing only the essencial child nodes and no unused child slots), but in terms of access performance it would be almost the same as the solution already in use. 

- It was decided that, for now, this method should be left as it is. 


## Key takeaways 

- Sometimes the best solution either takes to long to implement or brings another issue to the table. 


## Future work 

- Take data for the first topic. 
- See possible improvements to the math of _void updateQuantiles._