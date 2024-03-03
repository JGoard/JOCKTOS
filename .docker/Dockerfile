FROM ubuntu:23.10

RUN apt-get update && \
    apt-get install -y \
    curl \
    wget \
    qemu-system \
    build-essential \
    gcc-arm-none-eabi \
    gdb-arm-none-eabi 

RUN ln -s /usr/bin/gdb-multiarch /usr/bin/arm-none-eabi-gdb

WORKDIR /app

CMD ["tail", "-f", "/dev/null"]