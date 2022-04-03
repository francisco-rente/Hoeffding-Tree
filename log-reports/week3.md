# Week 3 

- Due to time constraints little work/progress was made this week.


## Progress made
- Added __faketime__ execution when launching the vitis IDE. 
- Continued study of vitis pragmas and other guides. 
- Extracted the current performance statistics and hardware diagrams.


### New desktop entry 
``` 
[Desktop Entry]
Encoding=UTF-8
Type=Application
Name=Vitis 2020.2
Comment=Vitis 2020.2
Icon=/tools/Xilinx/Vitis/2020.2/doc/images/ide_icon.png
Exec=faketime '2020-12-24 00:00:00' /tools/Xilinx/Vitis/2020.2/bin/vitis 
Name[en_US]=vitis 2020.2
Comment[en_US.UTF-8]=Vitis 2020.2
```

Possible work: 
1. Evaluate_split latency (see how well the for loops behave under paralelism). 
2. Optimize "auxilliar functions": 
    - NodeData_getSampleCountDistribuition__quantiles
    - NodeData_update__attributes
    - BinaryTree_sortSample__nodes
3. Allocation correction. 
4. Software refactoring.
5. Check HLS violations (see guidance in TreeKernels/HW folder. 


Note: kernel compilation offers enough information details. HW_link takes longer than necessary. 


### Evaluate split 

- Can the for loops with fixed ranges be paralelized? 
- See inner calls for latency issues.

```
    NodeData_evaluateSplit__attributes:
        for (attribute_index_t i = 0; i < N_Attributes; i++) {
        NodeData_evaluateSplit__attributes__pt:
            for (point_index_t p = 0; p < N_pt; p++) {
                sample_count_t dist[N_Classes][2], distSum[2] = {0};

                data_t pt = _getSplitPointValue(i, p);
            NodeData_evaluateSplit__attributes__pt__classes:
                for (class_index_t j = 0; j < N_Classes; j++) {
                    #pragma HLS unroll
                    sample_count_t distL, distR;
                    std::tie(distL, distR) =
                        _getSampleCountDistribuition(i, j, pt);

                    dist[j][Left] = distL;
                    dist[j][Right] = distR;

                    distSum[Left] += distL;
                }
                distSum[Right] = getSampleCountTotal() - distSum[Left];

                data_t G_pt = _G(dist, distSum);
                topSplitCandidates.add(i, pt, G_pt);
            }
        }

        std::tuple<bool, attribute_index_t, data_t, data_t> top =
            topSplitCandidates.getCandidate(0);
        std::get<3>(top) -= topSplitCandidates.getG(1);

        return top;
    }
```



## Computer details for SSH access: 

- IP 10.227.118.24 
- user: specs 
- pass: specs25


## New support links 

- https://github.com/Xilinx/Vitis_Accel_Examples/blob/master/cpp_kernels/README.md


### Current performance stats 


```
===============================================================================
Version:    v++ v2020.2 (64-bit)
Build:      SW Build (by xbuild) on 2020-11-18-05:13:29
Copyright:  Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
Created:    Sat Mar 21 16:55:34 2020
===============================================================================

-------------------------------------------------------------------------------
Design Name:             pi_container
Target Device:           xilinx:u250:xdma:201830.2
Target Clock:            300.000000MHz
Total number of kernels: 1
-------------------------------------------------------------------------------

Kernel Summary
Kernel Name  Type  Target              OpenCL Library  Compute Units
-----------  ----  ------------------  --------------  -------------
krnl_Tree    c     fpga0:OCL_REGION_0  pi_container    1


-------------------------------------------------------------------------------
OpenCL Binary:     pi_container
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
krnl_Tree_1   krnl_Tree    p_getSampleCountDistribuition  162 ~ 770       162            429           770             0.540 us         1.430 us        2.566 us
krnl_Tree_1   krnl_Tree    pow_generic_float_s            1               28             28            28              93.324 ns        93.324 ns       93.324 ns
krnl_Tree_1   krnl_Tree    evaluateSplit                  undef           undef          undef         undef           undef            undef           undef
krnl_Tree_1   krnl_Tree    train                          undef           undef          undef         undef           undef            undef           undef
krnl_Tree_1   krnl_Tree    krnl_Tree                      undef           undef          undef         undef           undef            undef           undef

Area Information
Compute Unit  Kernel Name  Module Name                    FF     LUT    DSP  BRAM  URAM
------------  -----------  -----------------------------  -----  -----  ---  ----  ----
krnl_Tree_1   krnl_Tree    p_getSampleCountDistribuition  997    2528   0    0     0
krnl_Tree_1   krnl_Tree    pow_generic_float_s            4030   3155   0    1     0
krnl_Tree_1   krnl_Tree    evaluateSplit                  8068   10619  0    1     0
krnl_Tree_1   krnl_Tree    train                          18345  24049  0    1     0
krnl_Tree_1   krnl_Tree    krnl_Tree                      22036  31772  0    5     0
-------------------------------------------------------------------------------
```



### Quick explanation: unroll and pipeline  

> Pipelining a loop with II="N" means that HLS should try to finish one iteration of the loop every N clock cycles. Normally you'd set II=1, which means that it finishes one loop iteration every clock cycle. Resource count often does not increase by much, and may even decrease - the same hardware is being used but it's being utilized more of the time. Instead of a RAM with the source data being read every fifth cycle (and the other four cycles being used for processing), it's being read every cycle while processing continues for the previous four elements. As a result, for many processing tasks pipelining is a very good approach. However, it has a limitation: you can't pipeline to less than one clock cycle per iteration, so the absolute minimum time taken for a loop with M iterations is M cycles (normally it's a little bit more as starting and stopping the pipeline takes time).

> Unrolling a loop with factor="N" means that HLS should create N copies of the processing hardware and run them in parallel. This has the advantage that it is possible to finish lots of iterations in every single clock cycle - I've had a block that did 54 iterations of a loop in one cycle. The disadvantage is that it takes a lot of hardware. If you're going to process 30 elements from an array at once, you need to be able to read 30 elements at once. Since Xilinx block RAMs have a maximum of two ports, this implies that you're going to need at least 15 block RAMs in parallel. If you don't have that, then HLS will construct a huge state machine so that your processing hardware gets sequential access to the RAM, which essentially drops you back to the un-unrolled performance while using 30 times as much hardware. As a result, unrolling only makes sense when you have (or can make) data structures that work with it. Other key requirements are that the extra hardware it generates (N sets of the processing hardware) is justified by the performance gain.

> An example of where unrolling really helps is if you've got a 72-bit RAM which stores nine 8-bit values per element, and you want to sum those values. Doing this with a normal loop or pipelined loop will result in HLS reading each element nine times, using a mux to select the relevant 8 bits every time, and sequentially feeding them through one adder. On the other hand, unrolling it completely will result in HLS reading each element once and using eight adders to do the additions simultaneously. This eliminates the mux (each adder reads from a constant location in the 72-bit RAM output bus), simplifies the state machine, and gives roughly nine times the performance - with the only tradeoff being that it uses more adders. Since adders are very plentiful on Xilinx FPGAs, this is probably a worthwhile tradeoff.

https://support.xilinx.com/s/question/0D52E00006hpiqhSAA/what-is-the-difference-between-unroll-and-pipeline?language=en_US








