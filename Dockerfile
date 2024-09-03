FROM ubuntu:23.10

# update and install basic tools
RUN apt-get update && \
    apt-get install -y \
    curl \
    udev \
    wget \
    nano \
    build-essential \
    symlinks \
    expect \
    git 
# install emulator and C build tools
RUN apt-get install -y \
    qemu-system \
    gcc-arm-none-eabi \
    gdb-arm-none-eabi \
    libnewlib-arm-none-eabi



# install documentation tools   
RUN apt-get install -y \
    python3-sphinx \
    python3-sphinx-rtd-theme \
    python3-breathe \
    doxygen \
    graphviz \
    texlive-xetex \
    latexmk \
    fonts-freefont-otf \
    xindy
    
RUN apt-cache policy gcc-arm-none-eabi

# install make and cmake build dependancies
RUN apt-get install --no-install-recommends -y \
libhidapi-hidraw0 \
libusb-0.1-4 \
libusb-1.0-0 \
libhidapi-dev \
libusb-1.0-0-dev \
libusb-dev \
libftdi-dev \
libtool \
usbutils \
openocd \
qemu-system \
# install make tools
make \
cmake \
automake \
pkg-config \
autoconf \
texinfo 
# TODO: remove unneeded directories from installing in src if needed for other packages
RUN cd ..
#OpenOCD talks to the chip through USB, so we need to grant our account access to the FTDI.
# RUN cp /usr/local/share/openocd/contrib/60-openocd.rules /etc/udev/rules.d/60-openocd.rules 
RUN /lib/systemd/systemd-udevd --daemon && udevadm control --reload-rules

#OpenOCD talks to the chip through USB, so we need to grant our account access to the FTDI.
EXPOSE 3333
EXPOSE 4444
EXPOSE 6666
CMD ["tail", "-f", "/dev/null"]
