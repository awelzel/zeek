#!/bin/sh
apt-get update && apt-get --no-install-recommends -y install \
    bc \
    bison \
    bsdmainutils \
    ccache \
    cmake \
    curl \
    flex \
    g++ \
    gcc \
    git \
    lcov \
    libfl-dev \
    libkrb5-dev \
    libmaxminddb-dev \
    libnode-dev \
    libpcap-dev \
    libssl-dev \
    libuv1-dev \
    make \
    python3 \
    python3-dev \
    python3-pip ruby \
    sqlite3 \
    swig \
    unzip \
    wget \
    zlib1g-dev

pip3 install websockets junit2html
gem install coveralls-lcov
