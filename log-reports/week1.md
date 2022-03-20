# Week 1 

## Introduction to the project

### References 

- Manual sobre pragmas HLS: https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/HLS-Pragmas

- Exemplos Vitis: https://github.com/Xilinx/Vitis-HLS-Introductory-Examples

- Algoritmo da árvore a optimizar: https://github.com/lm-sousa/Hoeffding-Tree

- Info sobre o Vitis Analyser: https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Vitis-Analyzer-GUI-and-Window-Manager


## Articles Analysis 

#### Mining High-Speed Data Streams 

- This article explains the Hoeffding Tree Algorithm and its advantages over current high performing algorithms. 
- This type of tree is perfect for a continuous data stream, since they can be learned in constant time per example (seeing them only once) and mining them in less time than it takes to input the data. 
- The foundation behaviour of this algorithm is similiar to other trees, given a set of training examples, it needs to predict future scenarios. The model creates a decision tree, testing an attribute in a node and constantly replacing leaves by predicting/test nodes. 
- Hoeffding Trees relly on the Chernoff bound in order to discover the necessary examples are necessary at each node. 
- The value is chosen, so that with high probability, the attribute used with n examples (small as possible), is the same chosen with infinite examples (complete accuracy). 
- The complexity required is O(dvc), d - no of attributes, v - max number of values per node, c - no of classes. 
- The incremental natury of the algorithm does not affect the quality of the trees. 
- The maximum disagreement of the tree, when compared to a batch tree, could be described as delta/ro, so instead of delta, the maximum expected disagreement, that the user is wiling to accept, even obtained even with a small number of example, can be modified.
- Some inherent advantages of VFDT: 
    - Ties: the user can decide parameters that dictate wether a tie or a split is made on the current best attribute
    - G computation: a minimum number of new examples can be given, that must be accumulated at a leaf before G is recomputed. 
    - Memory: memory required is a dynamic quantity, least promising leaves can be removed for newer ones. 
    - Poor attributes: unpromising attributes can also be dropped. 
    - Initialization: the tree can be inicialized by a conventional one, that studied a smaller subset of data

- In empyrical studies: Hoeffding tree's results show advantages over large datasets, mainly in dealing with noise, achieving accuracy similiar to the C4.5 model, and memory requirements that can incorporate datasets captured over weeks. 
> VFDT combines the best of both worlds, accessing data sequentially and using subsampling to potentially require much less than one scan, as opposed to many.



#### Towards Efficient and Scalable Acceleration of Online Decision Tree Learning on FPGA

#### Using dynamic partial reconfiguration of FPGAs in Real-Time Systems 
