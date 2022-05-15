# Week 9 


## Previous weeks 

- Week 7: technical issues regarding vitis IDE execution 
- Week 8: Queima das Fitas 


## Topics worked on this week

### Changes to TopSplitBucket 

#### Corrected the initial implementation for TopSplitBucket (conditions were inverted)

- Corrections 

```cpp
 bool add(attribute_index_t attributeIndex, data_t splitValue, data_t G) {
        if (G > this->G[0]) {
            _updateCandidate(1, this->attributeIndex[0], this->splitValue[0],
                             this->G[0]);
            _updateCandidate(0, attributeIndex, splitValue, G);
            return true;
        } else if (G > this->G[1]) {
            _updateCandidate(1, attributeIndex, splitValue, G);
            return true;
        }

        return false;
    }
```


- Values
    - TopSplitBuffer add size dissapeared, a loop does not exist.
    - The slack of the kernel tree and evaluation features increased (-1.06 to -0.30). 
    - LUTs increased from 12927 to 12993. 
    - Estimated Frequency increased. 

![Possible refactor with ifs](/log-reports/week9_files/TopSplitBucketWithIFs/TopSplitWithIfs.png)


```
Timing Information (MHz)
Compute Unit  Kernel Name  Module Name                    Target Frequency  Estimated Frequency
------------  -----------  -----------------------------  ----------------  -------------------
krnl_Tree_1   krnl_Tree    p_updateAttributeRange         300.300293        411.015198
krnl_Tree_1   krnl_Tree    p_updateQuantiles              300.300293        411.015198
krnl_Tree_1   krnl_Tree    p_getSampleCountDistribuition  300.300293        411.015198
krnl_Tree_1   krnl_Tree    pow_generic_float_s            300.300293        431.59259
krnl_Tree_1   krnl_Tree    evaluateSplit                  300.300293        366.83786
krnl_Tree_1   krnl_Tree    train                          300.300293        366.83786
krnl_Tree_1   krnl_Tree    krnl_Tree                      300.300293        366.83786

Latency Information
Compute Unit  Kernel Name  Module Name                    Start Interval  Best (cycles)  Avg (cycles)  Worst (cycles)  Best (absolute)  Avg (absolute)  Worst (absolute)
------------  -----------  -----------------------------  --------------  -------------  ------------  --------------  ---------------  --------------  ----------------
krnl_Tree_1   krnl_Tree    p_updateAttributeRange         144 ~ 360       144            242           360             0.480 us         0.807 us        1.200 us
krnl_Tree_1   krnl_Tree    p_updateQuantiles              1441            1441           1441          1441            4.803 us         4.803 us        4.803 us
krnl_Tree_1   krnl_Tree    p_getSampleCountDistribuition  9               107            107           107             0.357 us         0.357 us        0.357 us
krnl_Tree_1   krnl_Tree    pow_generic_float_s            1               28             28            28              93.324 ns        93.324 ns       93.324 ns
krnl_Tree_1   krnl_Tree    evaluateSplit                  67059 ~ 102259  67059          82099         102259          0.224 ms         0.274 ms        0.341 ms
krnl_Tree_1   krnl_Tree    train                          26198 ~ 883525  26198          347466        883525          87.318 us        1.158 ms        2.945 ms
krnl_Tree_1   krnl_Tree    krnl_Tree                      undef           undef          undef         undef           undef            undef           undef


Area Information
Compute Unit  Kernel Name  Module Name                    FF     LUT    DSP  BRAM  URAM
------------  -----------  -----------------------------  -----  -----  ---  ----  ----
krnl_Tree_1   krnl_Tree    p_updateAttributeRange         1405   4154   0    0     0
krnl_Tree_1   krnl_Tree    p_updateQuantiles              1433   2610   0    0     0
krnl_Tree_1   krnl_Tree    p_getSampleCountDistribuition  2331   3946   0    0     0
krnl_Tree_1   krnl_Tree    pow_generic_float_s            4030   3161   0    1     0
krnl_Tree_1   krnl_Tree    evaluateSplit                  9761   12993  0    1     0
krnl_Tree_1   krnl_Tree    train                          20967  32344  0    1     0
krnl_Tree_1   krnl_Tree    krnl_Tree                      24661  40057  0    5     0
-------------------------------------------------------------------------------


```




#### Added possible HLS pragma to original loop implementation (Approach taken into the next experiment)


- A pragma unroll, pipeline and loop flatten were considered, but since the loop only takes 2 iterations and one iteration in the nested one, a unroll pragma is possibly the best bet for optimizing this code. 

- Other possibilities could be: 
    - #pragma HLS pipeline II = 1 
    - #pragma HLS loop_merge force

```cpp
    bool add(attribute_index_t attributeIndex, data_t splitValue, data_t G) {
    TopSplitBuffer_add__size:
        for (attribute_index_t i = 0; i < size_T; i++) {
#pragma HLS unroll
            if (G > this->G[i]) {
            TopSplitBuffer_add__size__size:
                for (attribute_index_t j = size_T - 1; j > i; j--) {
#pragma HLS unroll
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


- Values
    - Results match other iterations results.
    - add_size_size remains with the same iter latency 
    - Slack is the same. 
    - LUTs increased from 12927 to 12949. 
    - Estimated frequencies remain the same as week4. 

![Possible refactor with pragmas](/log-reports/week9_files/TopSplitBufferUnrolls/TopSplitWithLoops.png)


```
Timing Information (MHz)
Compute Unit  Kernel Name  Module Name                    Target Frequency  Estimated Frequency
------------  -----------  -----------------------------  ----------------  -------------------
krnl_Tree_1   krnl_Tree    p_getSampleCountDistribuition  300.300293        411.015198
krnl_Tree_1   krnl_Tree    pow_generic_float_s            300.300293        431.59259
krnl_Tree_1   krnl_Tree    evaluateSplit                  300.300293        286.615082
krnl_Tree_1   krnl_Tree    train                          300.300293        286.615082
krnl_Tree_1   krnl_Tree    krnl_Tree                      300.300293        286.615082

Latency Information
Compute Unit  Kernel Name  Module Name                    Start Interval  Best (cycles)  Avg (cycles)  Worst (cycles)  Best (absolute)  Avg (absolute)  Worst (absolute)
------------  -----------  -----------------------------  --------------  -------------  ------------  --------------  ---------------  --------------  ----------------
krnl_Tree_1   krnl_Tree    p_getSampleCountDistribuition  9               107            107           107             0.357 us         0.357 us        0.357 us
krnl_Tree_1   krnl_Tree    pow_generic_float_s            1               28             28            28              93.324 ns        93.324 ns       93.324 ns
krnl_Tree_1   krnl_Tree    evaluateSplit                  undef           undef          undef         undef           undef            undef           undef
krnl_Tree_1   krnl_Tree    train                          undef           undef          undef         undef           undef            undef           undef
krnl_Tree_1   krnl_Tree    krnl_Tree                      undef           undef          undef         undef           undef            undef           undef

Area Information
Compute Unit  Kernel Name  Module Name                    FF     LUT    DSP  BRAM  URAM
------------  -----------  -----------------------------  -----  -----  ---  ----  ----
krnl_Tree_1   krnl_Tree    p_getSampleCountDistribuition  2331   3999   0    0     0
krnl_Tree_1   krnl_Tree    pow_generic_float_s            4030   3161   0    1     0
krnl_Tree_1   krnl_Tree    evaluateSplit                  9698   12949  0    1     0
krnl_Tree_1   krnl_Tree    train                          19975  26405  0    1     0
krnl_Tree_1   krnl_Tree    krnl_Tree                      23669  34118  0    5     0
```




#### Changes to the update Quantiles set of functions 


- *update(data_t sample[N_Attributes], class_index_t classif)*: since the two operations are related (TODO confirm that), a pipeline pragma is out of the question since the two calls can conflict (attribute range operates on the max and min range in sample, possibly changing the outcome of updateQuantiles) 

```cpp
void update(data_t sample[N_Attributes], class_index_t classif) {

    NodeData_update__attributes:
        for (attribute_index_t i = 0; i < N_Attributes; i++) {
            // TODO: pipeline of unroll
#pragma HLS unroll
            _updateAttributeRange(i, sample[i]);
            _updateQuantiles(i, classif, sample[i]);
        }

        _sampleCountTotal++;
        _sampleCountPerClass[classif]++;

        if (_sampleCountPerClass[classif] >
            _sampleCountPerClass[_mostCommonClass]) {
            _mostCommonClass = classif;
        }
    }

```

- * _updateQuantiles(attribute_index_t attributeIndex, class_index_t classif, data_t value)*: the N_Quantiles is known at compile time so we can use the unroll pragma to apply all the operations at once. 


```cpp
void _updateQuantiles(attribute_index_t attributeIndex,
                          class_index_t classif, data_t value) {
    NodeData_updateQuantiles__quantiles:
        for (quantile_index_t k = 0; k < N_Quantiles; k++) {
            // TODO: can a pragma be used here? Maybe unroll
            // is there any other pragma?
#pragma HLS unroll
            _Attributes[attributeIndex][classif][k] -=
                _lambda *
                _sgnAlpha(_Attributes[attributeIndex][classif][k] - value,
                          _getAlphaFromQuantileIndex(k));
        }
    }

```

- TODO: *_updateAttributeRange(attribute_index_t attributeIndex, data_t value)*: at first glance there is nothing that could be changed in this if else chains. If conditional branches should be avoided but in this instance it's the only option we got. 
- A possible alteration would be to find if there is a min and max for data_t types (that are conditional nontheless but would improve code elegance): 

```cpp
    if (value < _attributeRanges[attributeIndex][AttributeRange::Min]) {
    _attributeRanges[attributeIndex][AttributeRange::Min] = value;
    }

    if (value > _attributeRanges[attributeIndex][AttributeRange::Max]) {
    _attributeRanges[attributeIndex][AttributeRange::Max] = value;
    }

    // to this 

    _attributeRanges[attributeIndex][AttributeRange::Min] = min_function(_attributeRanges[attributeIndex][AttributeRange::Min], value); 
    _attributeRanges[attributeIndex][AttributeRange::Max] = max_function(_attributeRanges[attributeIndex][AttributeRange::Max], value); 

```

- Even better if this implementation could get a performance improvement. 


```cpp

void _updateAttributeRange(attribute_index_t attributeIndex, data_t value) {
        // TODO: see what option is available here
        // Change to assign type min max calls

        if (_sampleCountTotal) {
            if (value < _attributeRanges[attributeIndex][AttributeRange::Min]) {
                _attributeRanges[attributeIndex][AttributeRange::Min] = value;
            }

            if (value > _attributeRanges[attributeIndex][AttributeRange::Max]) {
                _attributeRanges[attributeIndex][AttributeRange::Max] = value;
            }
        } else {
            _attributeRanges[attributeIndex][AttributeRange::Min] = value;
            _attributeRanges[attributeIndex][AttributeRange::Max] = value;
        }
    }

```


- Values
    - We now get information about p_updateAttributeRange, and appears to have a lot of cycles operations. 
    - Evaluate split remains somehow with the same estimated frequency (We should switch into a if based approach from the first experiment and see if it changes). 
    - UpdateQuantiles decreased significantly latency wise (start inteval and best time performance) and increased the frequency amount. 



![Possible updateQuantiles refactor with pragmas](/log-reports/week9_files/UpdateQuantilesPragmas/updateQuantilesWithPragmas.png)


```
First experiment table 

krnl_Tree_1   krnl_Tree    p_updateAttributeRange         144 ~ 360       144            242           360             0.480 us         0.807 us        1.200 us
krnl_Tree_1   krnl_Tree    p_updateQuantiles              1441            1441           1441          1441            4.803 us         4.803 us        4.803 us

krnl_Tree_1   krnl_Tree    p_updateAttributeRange         144 ~ 360       144            242           360             0.480 us         0.807 us        1.200 us
krnl_Tree_1   krnl_Tree    p_updateQuantiles              170             170            170           170             0.567 us         0.567 us        0.567 us

```

Timing Information (MHz)
Compute Unit  Kernel Name  Module Name                    Target Frequency  Estimated Frequency
------------  -----------  -----------------------------  ----------------  -------------------
krnl_Tree_1   krnl_Tree    p_updateAttributeRange         300.300293        411.015198
krnl_Tree_1   krnl_Tree    p_updateQuantiles              300.300293        388.500397
krnl_Tree_1   krnl_Tree    p_getSampleCountDistribuition  300.300293        411.015198
krnl_Tree_1   krnl_Tree    pow_generic_float_s            300.300293        431.59259
krnl_Tree_1   krnl_Tree    evaluateSplit                  300.300293        286.615082
krnl_Tree_1   krnl_Tree    train                          300.300293        286.615082
krnl_Tree_1   krnl_Tree    krnl_Tree                      300.300293        286.615082

Latency Information
Compute Unit  Kernel Name  Module Name                    Start Interval  Best (cycles)  Avg (cycles)  Worst (cycles)  Best (absolute)  Avg (absolute)  Worst (absolute)
------------  -----------  -----------------------------  --------------  -------------  ------------  --------------  ---------------  --------------  ----------------
krnl_Tree_1   krnl_Tree    p_updateAttributeRange         144 ~ 360       144            242           360             0.480 us         0.807 us        1.200 us
krnl_Tree_1   krnl_Tree    p_updateQuantiles              170             170            170           170             0.567 us         0.567 us        0.567 us
krnl_Tree_1   krnl_Tree    p_getSampleCountDistribuition  9               107            107           107             0.357 us         0.357 us        0.357 us
krnl_Tree_1   krnl_Tree    pow_generic_float_s            1               28             28            28              93.324 ns        93.324 ns       93.324 ns
krnl_Tree_1   krnl_Tree    evaluateSplit                  undef           undef          undef         undef           undef            undef           undef
krnl_Tree_1   krnl_Tree    train                          undef           undef          undef         undef           undef            undef           undef
krnl_Tree_1   krnl_Tree    krnl_Tree                      undef           undef          undef         undef           undef            undef           undef

Area Information
Compute Unit  Kernel Name  Module Name                    FF     LUT    DSP  BRAM  URAM
------------  -----------  -----------------------------  -----  -----  ---  ----  ----
krnl_Tree_1   krnl_Tree    p_updateAttributeRange         1405   4154   0    0     0
krnl_Tree_1   krnl_Tree    p_updateQuantiles              3826   5954   0    0     0
krnl_Tree_1   krnl_Tree    p_getSampleCountDistribuition  2331   3999   0    0     0
krnl_Tree_1   krnl_Tree    pow_generic_float_s            4030   3161   0    1     0
krnl_Tree_1   krnl_Tree    evaluateSplit                  9698   12949  0    1     0
krnl_Tree_1   krnl_Tree    train                          23297  35617  0    1     0
krnl_Tree_1   krnl_Tree    krnl_Tree                      26991  43330  0    5     0
-------------------------------------------------------------------------------



```



#### Script for ssh connection


- Up until now I've been using my gnomo, to bounce the ssh session to the specs lab subnetwork. 
- This script provides a way of adding the direct route from the IP that is attributed from the VPN service (inside the FEUP's network) to the specs lab IP, via the private networks gateway. 

```sh
# Start snx ...
specs_ip="10.227.118.0/24"
vpn_device="tunsnx"
vpn_ip=$(ip --brief addr show dev $vpn_device | awk '{print $3}')
sudo ip route add $specs_ip via "$vpn_ip" dev $vpn_device
```


### Future work 

- Try other pragma in update (pipeline)
- Proceed work with if implementation? 
- Figure out why sometimes undef appears in kernel table and why some functions NodeData_update__attributes don't appear. 
- Search for xillinx max/min implementations 
- Run the overall hardware to see the real improvements 
- Figure out what other topics to tackle