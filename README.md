# JOCKTOS: A ground-up RTOS for ARM-CortexM4 Chipsets
<div align="center">
<img src="https://cdn.rawgit.com/sindresorhus/awesome/d7305f38d29fed78fa85652e3a63e154dd8e8829/media/badge.svg" alt="Awesome Badge"/>
<a href="https://saythanks.io/to/erwin.lejeune15%40gmail.com?style=flat-square"><img src="https://img.shields.io/badge/Say%20Thanks-!-1EAEDB.svg?style=flat-square" alt="Thx Badge"/></a>
<img src="https://img.shields.io/static/v1?label=%F0%9F%8C%9F&message=If%20Useful&style=style=flat&color=BC4E99" alt="Star Badge"/></a><br>

<a href="https://github.com/JGoard/JOCKTOS/stargazers"><img src="https://img.shields.io/github/stars/JGoard/JOCKTOS" alt="Stars Badge"/></a>
<a href="https://github.com/JGoard/JOCKTOS/members"><img src="https://img.shields.io/github/forks/JGoard/JOCKTOS" alt="Forks Badge"/></a>
<a href="https://github.com/JGoard/JOCKTOS/pulls"><img src="https://img.shields.io/github/issues-pr/JGoard/JOCKTOS" alt="Pull Requests Badge"/></a>
<a href="https://github.com/JGoard/JOCKTOS/issues"><img src="https://img.shields.io/github/issues/JGoard/JOCKTOS" alt="Issues Badge"/></a>
<a href="https://github.com/JGoard/JOCKTOS/contributors"><img alt="GitHub contributors" src="https://img.shields.io/github/contributors/JGoard/JOCKTOS"></a>
<a href="https://github.com/JGoard/JOCKTOS/blob/master/LICENSE"><img src="https://img.shields.io/github/license/JGoard/JOCKTOS" alt="License Badge"/></a>
<div align="left">

Welcome to JOCKTOS! This project was created for "fun" by Joshua Goard and Nicholas Schneider, both former colleagues from the Joint NC State University - UNC Asheville Mechatronics Program. JOCKTOS is a fully functional, custom-built RTOS designed for ARM Cortex-M4 chipsets. Whether you're curious about the internals of a real-time operating system, want to make modifications, or need a solid foundation for your embedded project, JOCKTOS is here for you.

Our end-goal has two objectives...
1. By creating a simpler RTOS, this may help any end-user who would like to learn more about the intricacies of what a basic tasking system should look like.
2. Allow users to either build the source code, or to use a pre-built library with binaries for them to use as an API within their own projects.

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
To get started with JOCKTOS, follow this procedure:

### Tool Installation
* If using a Windows Host...
     * Ensure that you have [WSL](https://learn.microsoft.com/en-us/windows/wsl/install) downloaded and enabled for use with docker
     * [Docker](https://www.docker.com/)
     * [Visual Studio Code](https://code.visualstudio.com/)
     * [usbipd](https://github.com/dorssel/usbipd-win) is needed in order to bind usb devices to be used through WSL.
        * Follow these instructions or else debugging through the docker container will not work as intended. You will still be able to compile software, and use QEMU for container emulation 
* If using a Linux Host...
     * [Docker](https://www.docker.com/)
     * [Visual Studio Code](https://code.visualstudio.com/)
    
### Steps for Cloning and Compiling Software
1. **Clone the Repository**:
   ```bash
   git clone https://github.com/JGoard/jocktos.git
   cd jocktos
   docker build -t jocktos:latest .
This will build you the latest jocktos image located in the root directory of the docker file.

2. **Open Project in VSCode and Configure USB Device**:
   * Open the project within VSCode, and install all recommended extensions.
   * If using a Windows Host: Use the 'Attach USB Nucleo' task to have usbipd bind to your usb device for use with a container/wsl. The VID:PID combo may differ depending on your Cortex-M4 device.

3. **Use Devcontainers to launch new Container**
   * Select the bottom left remote window button.
   * Select 'Reopen in Container' from the drop down menu
   * The window should reload and the container should not be launching/
4. **Compile and Debug SW within docker container**
   * Wait a few minutes after attaching VSCode to the docker container for proper extension initializations, reload the window if prompted for the cortex debugger.
5. **Now you are ready to go!**
   * Run the compile debug task to ensure that your enviroment is indeed setup correctly

Please Note: USBIPD and WSL are still very finicky. If you disconnect the USB device, you will need to relaunch your container, and rebind on the host enviroment if you are using windows.

## License
JOCKTOS is licensed under the MIT License.  See the [LICENSE](https://opensource.org/license/mit) for more information

## Credits & Acknowledgments
We would like to thank the ARM developer community, as well as the contributors to open-source tools like QEMU, GDB, and openOCD, for their invaluable resources and support.
