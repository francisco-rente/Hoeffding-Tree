[Desktop Entry]
Encoding=UTF-8
Type=Application
Name=Vitis 2020.2
Comment=Vitis 2020.2
Icon=/tools/Xilinx/Vitis/2020.2/doc/images/ide_icon.png
Exec=faketime '2020-12-24 00:00:00' /tools/Xilinx/Vitis/2020.2/bin/vitis 
Name[en_US]=vitis 2020.2
Comment[en_US.UTF-8]=Vitis 2020.2


Possible work: 
1. evaluate split latency 
2. Optimize "auxilliar functions" : 
    - NodeData_getSampleCountDistribuition__quantiles
    - NodeData_update__attributes
    - BinaryTree_sortSample__nodes


Pipelining a loop with II="N" means that HLS should try to finish one iteration of the loop every N clock cycles. Normally you'd set II=1, which means that it finishes one loop iteration every clock cycle. Resource count often does not increase by much, and may even decrease - the same hardware is being used but it's being utilized more of the time. Instead of a RAM with the source data being read every fifth cycle (and the other four cycles being used for processing), it's being read every cycle while processing continues for the previous four elements. As a result, for many processing tasks pipelining is a very good approach. However, it has a limitation: you can't pipeline to less than one clock cycle per iteration, so the absolute minimum time taken for a loop with M iterations is M cycles (normally it's a little bit more as starting and stopping the pipeline takes time).

Unrolling a loop with factor="N" means that HLS should create N copies of the processing hardware and run them in parallel. This has the advantage that it is possible to finish lots of iterations in every single clock cycle - I've had a block that did 54 iterations of a loop in one cycle. The disadvantage is that it takes a lot of hardware. If you're going to process 30 elements from an array at once, you need to be able to read 30 elements at once. Since Xilinx block RAMs have a maximum of two ports, this implies that you're going to need at least 15 block RAMs in parallel. If you don't have that, then HLS will construct a huge state machine so that your processing hardware gets sequential access to the RAM, which essentially drops you back to the un-unrolled performance while using 30 times as much hardware. As a result, unrolling only makes sense when you have (or can make) data structures that work with it. Other key requirements are that the extra hardware it generates (N sets of the processing hardware) is justified by the performance gain.

An example of where unrolling really helps is if you've got a 72-bit RAM which stores nine 8-bit values per element, and you want to sum those values. Doing this with a normal loop or pipelined loop will result in HLS reading each element nine times, using a mux to select the relevant 8 bits every time, and sequentially feeding them through one adder. On the other hand, unrolling it completely will result in HLS reading each element once and using eight adders to do the additions simultaneously. This eliminates the mux (each adder reads from a constant location in the 72-bit RAM output bus), simplifies the state machine, and gives roughly nine times the performance - with the only tradeoff being that it uses more adders. Since adders are very plentiful on Xilinx FPGAs, this is probably a worthwhile tradeoff.



Suport links 
- https://github.com/Xilinx/Vitis_Accel_Examples/blob/master/cpp_kernels/README.md