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
    build-essential \
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

# install Debugging dependencies
# install OPENOCD Build dependancies and gdb
RUN apt-get install --no-install-recommends -y \
    libhidapi-hidraw0 \
    libusb-0.1-4 \
    libusb-1.0-0 \
    libhidapi-dev \
    libusb-1.0-0-dev \
    libusb-dev \
    libtool \
    make \
    automake \
    pkg-config \
    autoconf \
    stlink-tools \
    texinfo
    #build and install OPENOCD from repository
RUN cd /usr/src/ \
    && git clone https://gitlab.zapb.de/libjaylink/libjaylink.git \
    && cd libjaylink \
    && ./autogen.sh \
    && ./configure \
    && make \
    && make install \
    && cd /usr/src \
    && git clone --depth 1 https://github.com/xpack-dev-tools/openocd.git \
    && cd openocd && ./bootstrap && ./configure --enable-stlink --enable-jlink --enable-ftdi --enable-cmsis-dap && make -j"$(nproc)" && make install
    RUN rm -rf /usr/src/openocd \
    && rm -rf /var/lib/apt/lists/
    #OpenOCD talks to the chip through USB, so we need to grant our account access to the FTDI.
    RUN cp /usr/local/share/openocd/contrib/60-openocd.rules /etc/udev/rules.d/60-openocd.rules
    COPY openocd.cfg /usr/local/share/openocd/openocd.cfg  
    #OpenOCD talks to the chip through USB, so we need to grant our account access to the FTDI.
EXPOSE 3333
EXPOSE 4444
EXPOSE 6666

WORKDIR /JOCKTOS

CMD ["tail", "-f", "/dev/null"]


# $ openocd 
# -s "/usr/local/share/openocd/scripts" 
# -f "interface/stlink-v2.cfg" 
# -f "target/stm32f4x.cfg" 
# -c "program blinky.elf verify reset exit")
