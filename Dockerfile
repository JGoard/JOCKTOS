FROM ubuntu:23.10

# update and install basic tools
RUN apt-get update && \
    apt-get install -y \
    curl \
    wget \
    nano

# install emulator and C build tools
RUN apt-get install -y \
    qemu-system \
    build-essential \
    gcc-arm-none-eabi

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


WORKDIR /JOCKTOS

CMD ["tail", "-f", "/dev/null"]
