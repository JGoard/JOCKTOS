# JOCKTOS: A ground-up RTOS for ARM-CortexM4 Chipsets
This project was created for "fun" by Joshua Goard and Nicholas Schneider, both former colleagues from the Joint NC State University - UNC Asheville Mechatronics Program.
It's intended for those who would wish to see the source code of a functioning RTOS and make changes themselves, or to use it in their own projects with a source binary 
and trest it as an API.

## How it's Made
**Tech Used:** C, ARM Assm., Dockerfiles, VSCode, openOCD, GDB, QEMU

Building JOCKTOS from scratch was both a challenge and a passion project. We aimed to create a minimalist, efficient, and robust RTOS tailored to the ARM Cortex-M4 architecture. Starting with the core kernel, we wrote everything in C with critical sections in ARM Assembly for optimal performance. We used QEMU for initial testing and debugging, with openOCD and GDB to deploy and debug on actual hardware. Dockerfiles were included to standardize the development environment, ensuring that anyone can replicate our setup with ease.

We wanted JOCKTOS to be more than just a learning tool—it's a practical RTOS ready for deployment in real-world embedded systems. The project embodies our dedication to low-level programming and system design, reflecting countless hours of debugging, optimizing, and refining.

## Optimizations
Throughout the development of JOCKTOS, we've made several key optimizations to enhance both the performance of the RTOS and the efficiency of our workflow:

* Context Switching: Optimized the context-switching mechanism to reduce overhead, ensuring that task switching is as fast as possible.
* Memory Management: Implemented a custom memory allocator that balances speed and memory usage, minimizing fragmentation.
* Interrupt Handling: Streamlined interrupt handling routines to reduce latency and prioritize critical tasks effectively.
* Development Environment: Dockerized the entire development environment, allowing for consistent builds and easy collaboration across different systems.
* I/O Handler: Custom built hardware for ease of creating a list of I/O and using as needed.

## Lessons Learned:
Creating JOCKTOS was a deep dive into the intricacies of real-time systems. Here are some of the lessons we learned along the way:

* Timing is Everything: Understanding and controlling timing at the microsecond level is crucial in RTOS development.
* Debugging Low-Level Code: Low-level debugging requires patience and precision; the smallest errors can have significant impacts.
* Modularity: Designing the RTOS with modularity in mind made it easier to optimize and expand the system.

## Examples:
To get started with JOCKTOS, check out the 'c_based_scheduler branch' and clone it to your local pc. 
With Docker, running a basic emulation of the board we are using should be seamless.

## Installation & Setup
To get started with JOCKTOS, follow these steps:

1. **Clone the Repository**:
   ```bash
   git clone https://github.com/JGoard/jocktos.git
   cd jocktos
   docker build -t jocktos:latest .

## License
JOCKTOS is licensed under the MIT License.  See the [LICENSE](https://opensource.org/license/mit) for more information

## Credits & Acknowledgments
We would like to thank the ARM developer community, as well as the contributors to open-source tools like QEMU, GDB, and openOCD, for their invaluable resources and support.
