# Week 10 


## Topics worked on this week


### Used professor's suggestion to patch vitis IDE 


- By my experiment, decreasing the date manually is not required anymore to perform compilation.

- Link: https://support.xilinx.com/s/article/76960?language=en_US

![Patch terminal results](/log-reports/week10_files/patch.png)



### Experimented with the maximum of optimizations gathered up until now 


- As previously stated, performance increased dramatically using the TopSplitBucket class, that does not use the template argument in size_t.
- Slack improved and overall latency and execution time too. 
- p_updateQuantiles improved a bit, however far from expected. Another case of this is p_updateAttributeRange.

![Optimization results](/log-reports/week10_files/BucketAndPragmas/optimizationWithBucketAndPragmas.png)




```
Timing Information (MHz)
Compute Unit  Kernel Name  Module Name                    Target Frequency  Estimated Frequency
------------  -----------  -----------------------------  ----------------  -------------------
krnl_Tree_1   krnl_Tree    p_updateAttributeRange         300.300293        411.015198
krnl_Tree_1   krnl_Tree    p_updateQuantiles              300.300293        388.500397
krnl_Tree_1   krnl_Tree    p_getSampleCountDistribuition  300.300293        411.015198
krnl_Tree_1   krnl_Tree    pow_generic_float_s            300.300293        431.59259
krnl_Tree_1   krnl_Tree    evaluateSplit                  300.300293        366.83786
krnl_Tree_1   krnl_Tree    train                          300.300293        366.83786
krnl_Tree_1   krnl_Tree    krnl_Tree                      300.300293        366.83786

Latency Information
Compute Unit  Kernel Name  Module Name                    Start Interval  Best (cycles)  Avg (cycles)  Worst (cycles)  Best (absolute)  Avg (absolute)  Worst (absolute)
------------  -----------  -----------------------------  --------------  -------------  ------------  --------------  ---------------  --------------  ----------------
krnl_Tree_1   krnl_Tree    p_updateAttributeRange         144 ~ 360       144            242           360             0.480 us         0.807 us        1.200 us
krnl_Tree_1   krnl_Tree    p_updateQuantiles              170             170            170           170             0.567 us         0.567 us        0.567 us
krnl_Tree_1   krnl_Tree    p_getSampleCountDistribuition  9               107            107           107             0.357 us         0.357 us        0.357 us
krnl_Tree_1   krnl_Tree    pow_generic_float_s            1               28             28            28              93.324 ns        93.324 ns       93.324 ns
krnl_Tree_1   krnl_Tree    evaluateSplit                  67059 ~ 102259  67059          82099         102259          0.224 ms         0.274 ms        0.341 ms
krnl_Tree_1   krnl_Tree    train                          5862 ~ 863189   5862           327130        863189          19.538 us        1.090 ms        2.877 ms
krnl_Tree_1   krnl_Tree    krnl_Tree                      undef           undef          undef         undef           undef            undef           undef

Area Information
Compute Unit  Kernel Name  Module Name                    FF     LUT    DSP  BRAM  URAM
------------  -----------  -----------------------------  -----  -----  ---  ----  ----
krnl_Tree_1   krnl_Tree    p_updateAttributeRange         1405   4154   0    0     0
krnl_Tree_1   krnl_Tree    p_updateQuantiles              3826   5954   0    0     0
krnl_Tree_1   krnl_Tree    p_getSampleCountDistribuition  2331   3999   0    0     0
krnl_Tree_1   krnl_Tree    pow_generic_float_s            4030   3161   0    1     0
krnl_Tree_1   krnl_Tree    evaluateSplit                  9761   13046  0    1     0
krnl_Tree_1   krnl_Tree    train                          23360  35723  0    1     0
krnl_Tree_1   krnl_Tree    krnl_Tree                      27054  43436  0    5     0




```







### Experimented with the dataflow pragma 

- Professor's suggestion that could solve the paralelization or pipeline possibilty in the NodeData_update__attributes loop, correcting the failed attempt of performance boost from an unroll pragma: 


```cpp
    NodeData_update__attributes:
        for (attribute_index_t i = 0; i < N_Attributes; i++) {
            // TODO: pipeline of unroll
#pragma HLS dataflow
            _updateAttributeRange(i, sample[i]);
            _updateQuantiles(i, classif, sample[i]);

```

- This pragma is intended to paralelize tasks the compiler finds, minimizing latency and improving concurrency: 


> The DATAFLOW optimization enables the operations in a function or loop to start operation before the previous function or loop completes all its operations.

![Data flow explanation](/log-reports/week10_files/dataflow.png)


#### Results 

- However, the results we 

- getSampleCountDistribution interval increased a lot and there was a set back to the improvements we made so far regarding the slack metric (-0.30 in the worst case to -2.25). However, the kernel tree total LUTs 

- Estimated frequency in train and kernel tree decreased (366.83786 to 213.4924) and for some reason we lost some important metrics and loop statistics in the HLS synthesis report. 

- Possible explanations for this limitation: https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/Dataflow-Optimization-Limitations

    - Invalid reading and writing operations (both invocations read and write to arrays, they do not follow the regular pipeline structure (R-E-W)) 
    > Reading of inputs of the function should be done at the start of the dataflow region, and writing to outputs should be done at the end of the dataflow regio
    - There are consuming type violations (if contrary to my belief the arrays are linked and dependent on each other, they are utilized, both read and write simultaneously): 
    > All elements passed between tasks must follow a single-producer-consumer model. Each variable must be driven from a single task and only be consumed by a single task 
    - Not sure if the conditional execution applies not only to the loop structure or the behaviour of each function: 
    > The DATAFLOW optimization does not optimize tasks that are conditionally executed

![HLS synthesis](/log-reports/week10_files/DataFlowPragmaTest/correctedDataFlow.png)


```

Kernel Summary
Kernel Name  Type  Target              OpenCL Library  Compute Units
-----------  ----  ------------------  --------------  -------------
krnl_Tree    c     fpga0:OCL_REGION_0  krnl_Tree       1


-------------------------------------------------------------------------------
OpenCL Binary:     krnl_Tree
Kernels mapped to: clc_region

Timing Information (MHz)
Compute Unit  Kernel Name  Module Name                    Target Frequency  Estimated Frequency
------------  -----------  -----------------------------  ----------------  -------------------
krnl_Tree_1   krnl_Tree    p_getSampleCountDistribuition  300.300293        411.015198
krnl_Tree_1   krnl_Tree    pow_generic_float_s            300.300293        431.59259
krnl_Tree_1   krnl_Tree    evaluateSplit                  300.300293        213.492737
krnl_Tree_1   krnl_Tree    train                          300.300293        213.492737
krnl_Tree_1   krnl_Tree    krnl_Tree                      300.300293        213.492737

Latency Information
Compute Unit  Kernel Name  Module Name                    Start Interval  Best (cycles)  Avg (cycles)  Worst (cycles)  Best (absolute)  Avg (absolute)  Worst (absolute)
------------  -----------  -----------------------------  --------------  -------------  ------------  --------------  ---------------  --------------  ----------------
krnl_Tree_1   krnl_Tree    p_getSampleCountDistribuition  107             107            107           107             0.357 us         0.357 us        0.357 us
krnl_Tree_1   krnl_Tree    pow_generic_float_s            1               28             28            28              93.324 ns        93.324 ns       93.324 ns
krnl_Tree_1   krnl_Tree    evaluateSplit                  undef           undef          undef         undef           undef            undef           undef
krnl_Tree_1   krnl_Tree    train                          undef           undef          undef         undef           undef            undef           undef
krnl_Tree_1   krnl_Tree    krnl_Tree                      undef           undef          undef         undef           undef            undef           undef

Area Information
Compute Unit  Kernel Name  Module Name                    FF     LUT    DSP  BRAM  URAM
------------  -----------  -----------------------------  -----  -----  ---  ----  ----
krnl_Tree_1   krnl_Tree    p_getSampleCountDistribuition  2033   4306   0    0     0
krnl_Tree_1   krnl_Tree    pow_generic_float_s            4030   3155   0    1     0
krnl_Tree_1   krnl_Tree    evaluateSplit                  9104   12397  0    1     0
krnl_Tree_1   krnl_Tree    train                          19381  25827  0    1     0
krnl_Tree_1   krnl_Tree    krnl_Tree                      23072  33550  0    5     0
-------------------------------------------------------------------------------




```


## Future improvements

- Possible optimizations/refactors that would improve the dataflow pragma results: https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/Exploiting-Task-Level-Parallelism-Dataflow-Optimization and https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/Configuring-Dataflow-Memory-Channels. 

- Take a look at the Violations and warnings, analyse their gravity and possible corrections.









