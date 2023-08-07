#!/bin/sh
apt-get update && apt-get --no-install-recommends -y install \
    bison \
    bsdmainutils \
    ccache \
    cmake \
    curl \
    flex \
    g++ \
    gcc \
    git \
    libfl-dev \
    libkrb5-dev \
    libnode-dev \
    libpcap-dev \
    libssl-dev \
    libuv1-dev \
    make \
    python3 \
    python3-dev \
    python3-pip \
    python3-websockets \
    sqlite3 \
    swig \
    wget \
    xz-utils \
    zlib1g-dev

pip3 install --break-system-packages junit2html
