# Week 4 

<hr>

# __Important Warning:__

- Unfortunately, due to a lack of attention on my part, some commits were made with the incorrect date (Vitis license problem) and a different user (sfsi).
- Rebase was not possible since the date of the commits was very far behind. 
- The only option was reverting and repeating them, in the correct order. 
- The user information has been updated in the Specs Lab PC local repository, and the script command implemented on the previous week appears to be working (so date changes won't be an issue anymore). 

## The following commits have been reverted and reinserted in the repository

> 74a3ee0ee688a0f6d4546d113bd6e0cc0f7d87c1: changed getSampleCountDistribuition loop structure and added unroll

> a4158d7989a1f6bddbcb68a6908c500d98037442: added pipeline pragma to evaluate split outer loop

> 93cda3477f768f2f328832853ee420f5b38f1efb: added comments to top split buffer algorithm for a clearer understanding

> f2ff3e26ce8ce24401accdcc09d5f6b2dfd05e06: added report files for the loop pragmas and bool sum loop experiment

It's also worth noting some commits made in week 3, do not match name wise. However, since they were pushed with the correct timestamp, I opted to let them stay as they are. Only the name (pfsi), has to be taken in consideration. These commits are: 

> ebda9cf0a6e727af6f3c5c4ff53a1013d970f190: updated zip 3 and added summary archive of current build

> 5152201ab29cef1603b2692baba26e7d9b28bfdf: added first SW emulation log and system diagram

> 69a4ce46ad0c897c2107eda5264844a3622637dd: added platform diagram

Once again, I apologize for the mistake.

<hr>


## PC addresses/relevant info 

- Interface: enp68s0
- IP: 10.227.118.24/24 
- MAC address: a8:a1:59:68:07:a4


## Work developed during the week 

- Throught this week, in conjunction with the teacher, we analysed some segments of code/functions, that were pointed by Vitis, considering their current performance stats.  

### Restructuring the _getSampleCountDistribuition loop 

#### What it does 

- The loop in question goes through all the quantile zones (probability areas) for a given attribute, and increments a given left length until the tree split point is found on the quantile distribuition. The loop breaks, and in now, saved in distL is the number of quantiles that have a smaller probability than the current split point (which is also used to calculate distR).

#### How it was improved 

- The break conditional branch in the middle of the loop prevented any unroll/pipeline possibilities, since it was not known at the time which if branch was going to be choosen at compile time, and therefore no optimizations were performed. 
- The loop was restructed and the conditional logic segment was removed. Since we only needed to count how many quantiles are before the split point probability, we could simply add the boolean condition that is currently in the if clause. 
- There is of course a set back (the loop continues until the end of the quantile list), however it can now be unrolled, compensating in that regard. 
- One could search for low level implementations of this search algorithm (maybe from xillinx), that would improve the current implementation.
- Question: Can a binary search be an alternative? (or do the necessary search condition stop it from being used).

#### Results 

- Target and estimated frequencey remained the same. 
- However two lines clearly differ from the week3 summary. There were obvious improvements in the latency aspect of performance. The number of cycles needed to perform such operation decreased and the function doesn't appear to be flagged in vitis analyser. 

```
Latency Information
Compute Unit  Kernel Name  Module Name                    Start Interval  Best (cycles)  Avg (cycles)  Worst (cycles)  Best (absolute)  Avg (absolute)  Worst (absolute)
------------  -----------  -----------------------------  --------------  -------------  ------------  --------------  ---------------  --------------  ----------------


Week 4 
krnl_Tree_1   krnl_Tree    p_getSampleCountDistribuition  107             107            107           107             0.357 us         0.357 us        0.357 us

vs 

Week 3
krnl_Tree_1   krnl_Tree    p_getSampleCountDistribuition  162 ~ 770       162            429           770             0.540 us         1.430 us        2.566 us

```


![Bool sum loop](/log-reports/week4_files/boolsumloop/boolsumloopwithpragma.png "HLS syntesis")


#### Code 

Previous version: 

```cpp
_getSampleCountDistribuition(attribute_index_t attributeIndex,
                                 class_index_t classIndex, data_t splitPoint) {
        sample_count_t distL = 0;
    NodeData_getSampleCountDistribuition__quantiles:

        for (quantile_index_t k = 0; k < N_Quantiles; k++) {
            if (splitPoint > _Attributes[attributeIndex][classIndex][k]) {
                distL++;
            } else {
                break;
            }
        distL = tcm::round(((data_t)distL / N_pt) *
                           _sampleCountPerClass[classIndex]);
        sample_count_t distR = _sampleCountPerClass[classIndex] -
                               distL;

        return {distL, distR};
    }
```

Current version: 

```cpp
_getSampleCountDistribuition(attribute_index_t attributeIndex,
                                 class_index_t classIndex, data_t splitPoint) {
        sample_count_t distL = 0;
    NodeData_getSampleCountDistribuition__quantiles:

        for (quantile_index_t k = 0; k < N_Quantiles; k++) {
            #pragma HLS unroll
            distL += (splitPoint > _Attributes[attributeIndex][classIndex][k]);
        }
        distL = tcm::round(((data_t)distL / N_pt) *
                           _sampleCountPerClass[classIndex]);
        sample_count_t distR = _sampleCountPerClass[classIndex] -
                               distL;

        return {distL, distR};
    }

```




### Restructuring the _getSampleCountDistribuition loop 

#### What it does 


#### How it was improved 

- The loop follows the task list: get values, operate on those values and write/save the results. It does this every iteration, so we can place a pipeline pragma to explore this flow.
- The _getSplitPointValue call could be made in parallel with the  _NodeData_evaluateSplit__attributes__pt__classes loop_, which is also unrolled (being very efficient), and updating _distSum_ and the candidates could also be a seperate parallelized step. 


![](/log-reports/week4_files/pipelineexample.png)

#### Results 

- The main improvement we can notice from this experiment is the reduction in the slack quantifier (from -2.25 to -1.06). Although it is still not positive, this means that the board could possibly use its hardware in a more convenient way, maximizing resources usage and output performance. 

![Pipelined pragma results](/log-reports/week4_files/pragmasandbool/finalstagewithforloopragmasandboolsum.png)


- Other improvements include the raise in the estimated frequency of the total operations in the _evaluateSplit_ sequence (286 vs 213), the total LUT operations also decreased. 

```
Timing Information (MHz)
Compute Unit  Kernel Name  Module Name                    Target Frequency  Estimated Frequency
------------  -----------  -----------------------------  ----------------  -------------------
krnl_Tree_1   krnl_Tree    evaluateSplit                  300.300293        286.615082
krnl_Tree_1   krnl_Tree    train                          300.300293        286.615082
krnl_Tree_1   krnl_Tree    krnl_Tree                      300.300293        286.615082

```



```cpp
std::tuple<bool, attribute_index_t, data_t, data_t> evaluateSplit() {
        TopSplitBuffer<2, data_t, attribute_index_t> topSplitCandidates;

    NodeData_evaluateSplit__attributes:
        for (attribute_index_t i = 0; i < N_Attributes; i++) {

        NodeData_evaluateSplit__attributes__pt:
            for (point_index_t p = 0; p < N_pt; p++) {

                #pragma HLS pipeline // added this pragma 

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


## Main conclusions 

- The main takeway of this week is that, the work involved in improving the code, is not as simple as adding pragmas to the code files. It includes a large amount of work, not only in understanding the fpga hardware, the necessary pragmas (and their functionality), and above all, how we can restructure the code so that the pragmas we place take effect and take the most advantage of the available resources.



## Future work for next week 

- The TopSplitBuffer function is flagged in the vitis analyser has in need of pipelining. This should be the next week task. 
- Since it is an insertion sort, one could search for a proprietary implementation that optimizes the algorithm to hardware specifications.
- This will be developed in the branch week5-testing, for work related purposes.


- Ask the teacher for an upstream update (tests correction)
