Embedded Image Processing Application on a Multi-core Platform with FPGA
======================================

This project is a comprehensive project with FPGA. We designed and implemented the FPGA hardware architecture using Qsys and Verilog.Also, we developed three image detection and processing applications that can run on different hardware resources:
1. on a single core without operating system i.e. Bare-metal;
2. on a single core using the real-time operating system MicroC/OS-II;
3. on the multiprocessor without operating system while satisfying design constraints on worst case throughput and memory footprint.

In addition to these, we also developed a debug mode for software testing.

## Contributors

* Jun Zhang
* Jaakko Laiho

## Contents 

This repository is organized as follows:

 * `app` contains source files for the applications, grouped in subfolders by project. It also contains build automation scripts for helping with the lab tasks.
 * `bsp` is an empty folder which will contain the generated Board Support Packages for different configurations.
 * `hardware` contains files describing the custom hardware platform. 
 * `model` contains the (ready-to-build) ForSyDe-Haskell project which represents the executable functional specification of the application that needs to be implemented on the given hardware platform.
 * `test` contains input images and scripts that help in generating input data, output plots and testing the implementations against the golden model.
 
For more detailed usage instructions, check the `README` files found in each folder.
