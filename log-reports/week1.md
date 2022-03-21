# Week 1 

## Introduction to the project

### References 

- Manual sobre pragmas HLS: https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/HLS-Pragmas

- Exemplos Vitis: https://github.com/Xilinx/Vitis-HLS-Introductory-Examples

- Algoritmo da árvore a optimizar: https://github.com/lm-sousa/Hoeffding-Tree

- Info sobre o Vitis Analyser: https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Vitis-Analyzer-GUI-and-Window-Manager

- HLS tutorials https://github.com/Xilinx/Vitis-Tutorials


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

- Doubts: quantiles, figure 8 data fowarding, Dynamic and Memory-based Resource Sharing.


- Standard Decision Trees have memory consumption proportional to the size of the datasets and don't adapt once the training process is terminated. 
- Hoeffding trees are capable of performing training and inference concurrently. 
- Main disadvantages of the hardware implementation: 
    - High cost of memory to store the required subset of samples and characteristics of leafs. 
    - Latency between iterations of the learing proccess. 
    - Tradeoff between increasing computational intensity in order to reduce memory usage. 
- High cost of memory: fixed number of bin for attributes, that are discarded if not necessary. This method requires a bin of a large size in order to fit different type of attributes. 
- High Computational Intensity with Dependency and Long Latency: Gaussian-based methods tradeoff computitational intensity and memory efficiency. The incremental update proccess leads to high computer demand, proportional to the number of attributes and classes. The cumulative density functions at each split and the update process increase latency.

> Quantiles are cutting points dividing the range of a probability distribution into a certain number of intervals with equal probabilities.
- Calculation uses asymmetric signum functions that calibrate the approximations in sequencial manner, in every incoming sample. 
- The quantile estimation algorithm was developped with some features in mind:
    - A separate set of quantiles is maintained per attribute per class
    - The strategy to get left/right partitions based on the attribute distributions has been customized to support the quantile method
> We generate a set of split points evenly distributed in the full range of attribute values. These split points are compared to the quantiles individually to find out the interval of two quantiles [Q(αk), Q(αk+1 )] containing the split point. 

- Advantages of this method: 
    - Not storing any examples, they have their characteristics saved in quantiles
    - Reduced computational demand
    - Data dependency resolved with parallelism and pipelines

##### System overview Section 

- Sample buffer => Reading and decoding data => fetch relevant tree nodes and filters the samples to leaf nodes (pipelines) => Processes the samples
- Learning process: samples are decomposed into attributes (with their characteristics learned and stored) => split trial, for each attribute, uses quantiles and split points to deduce left and right partitions=> partition information is evaluated, by computing the split gain of the split point => Split decision by the bound judge => store the information. 

##### Tree management units 

- Filters samples to leaf nodes and splitts leaf nodes, overwriting the tree memory 
- Three stages: Node reading, attribute selection and branch decision

##### Numeric attributes Optimized quantile learning

- Attribute level: attributes are independent and the quantiles per attribute are also independent, ergo parallelism. 
- Dynamic and Memory Resource sharing: Each leaf node mantains a number of quantiles per attribute per class. The quantile update routine also needs a dynamic leaf node allocation. 
- A node-elment table is implemented to keep track of the leaf node element pairs
- A memory based resource sharing scheme: each sample is only sorted to one leaf node and for each attribute only the set of quantiles corresponding to the sample label will be activated per sample. 

##### Pipelining with Data Fowarding

- Data dependencies:  two successive samples sorted to the same leaf node should update the same element in a sequential way.
- A 5 step pipeline architecture for the quantile update routine is implemented. Data fowarding helps memory operations with dependencies. 

##### Categorical attributes 

- Similiar to numeric attributes. 
- Dynamic leaf has to be adapted, histograms for attribute class combinations have to be simultaneosly initialized.
- A status table for histograms is created to help. 

#####  Simplification of Split Measurement with Hoeffding Bound

> Gini impurity is a measure of the chance for an example to be incorrectly classified if it is randomly labeled according to the distribution of the labels.

##### Results

- The inference accuracy may be degraded significantly as the number of  the number of quantiles becomes either too small or too large
- If the quantile number is small, the learning ability of the model may be constrained
- If the quantile number becomes too large, the generalization ability may be impaired as well, since tree. The parameter settings related to the Hoeffding bound the design is more prone to noise in the datasets.

> By contrast, our proposed online training architecture only requires reading each sample once in the entire learning process, thus reducing a large amount of high-cost inter-chip communication

- A SoC solution with FPGA implementation and data compression is inefficient for large datasets because it requires transmission between the FPGA and the off-chip memory repeatedly. 

> By contrast, our work introduces a light-weight algorithm along with a hardware-friendly architecture for online decision tree learning, placing no restrictions on the size of datasets and only requiring one-time inter-chip transmission of the datasets.


#### Using dynamic partial reconfiguration of FPGAs in Real-Time Systems 
