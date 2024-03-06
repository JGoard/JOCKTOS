FROM ubuntu:23.10

RUN apt-get update && \
    apt-get install -y \
    curl \
    wget \
    qemu-system \
    build-essential \
    gcc-arm-none-eabi

WORKDIR /app

CMD ["tail", "-f", "/dev/null"]