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

- Specs lab pc was offline for remote access and other students were using it in the lab, so I was unable to run tests. I plan to run them the next week


### SortSample nodes 


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

- Matriz alocada, sparse 

Alteração antes de training inference no HW link do kernel



falar 

PROBLEMA: consistência memória RAM CPU e RAM FPGA 


## Key takeaways 

- Understand things that cannot be changed 


## Future work 

- Take data for the first topic 
- See possible improvements to the math of  void _updateQuantiles. 