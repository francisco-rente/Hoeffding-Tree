# Week 2 Report 

## Tasks accomplished this week 

- Setup of the working environment, 3 section directories, including the cpp files for the tree, correctly linking necessary files, choosing the right platform (U250). 
- General understanding of the IDE and statistics. 
- General understanding of compilation stages. 


### Quirks and setbacks: 
- Symbolic links in the IDE don't work as expected, although the IDE correctly detects them, the compilation requires actual links (used for the tree definition file in host and kernel directories).
- License issue: current calendar had to be set back 2 years in order to compile https://support.xilinx.com/s/question/0D52E00006uxy49SAA/vivado-fails-to-export-ips-with-the-error-message-bad-lexical-cast-source-type-value-could-not-be-interpreted-as-target?language=ja
- Some tests didn't pass.
- Workspace setup cumbersome. 
- Choose new project -> u250 -> hello world for template
- Host -> host cpp Tree cpp (symbolic link) 
- Kernels -> HTree cpp, typechooser cpp, Tree cpp 

### Topics 

- How the fpga is programmed: bit stream updates and modifies LUT tables that each logic unit uses to compute results based on input. 
- The compilation has two main stages, placing of each logic section and routing, based on the floor planning algorithm. The compilation time is proportional to the amount of available space. 
- Host program setups the buffers that are migrated to the fpga and then retrieved with the results with OCL_check(args) and enqueue migrate.
- Host <- HW link -> Kernel. 
- Companies producing FPGAs and their marketshare (such as Altera and Xillinx). 
- VLSI design software and steps of ASIC production. 

### Notes for future developmments 

- Change allocation method, to correct align warning with align_allocator.
- Link summary of HW: pratically all information that is needed for a analysis - kernel estimates and guidance, but takes longer). Compile summary of kernel (less info, __enough for the job__). 
- 3 options: SW/CPU compilation, Kernel compilation and HW link, all necessary for HW execution. 
- Save the compile logs and statistics on submit, use Vitis analyser for checking progress (vitis ide has links that go directly to them)
- Useful attributes: latency, LTU lookups, pipeline efficiency. 
- Slack metric: Time that can be paralelized, if negative the clock had to be diminished during execution (bad performance), if positive, it can use some of computing space to other tasks.- Compile/Build kernels (HW mode) 
- Kernel guidance and HLS syntesis (Vitis analyser) -> Compile summary. 

## Study

- Pragmas 
- Dot product example


## Further research 

- See HLS IDE import configurations. 
This program shows the user a task schedule graph, providing clearer interpretation of clock cycles and paralelism. 
- Export compile and runtime info (log commit and archive summary).

