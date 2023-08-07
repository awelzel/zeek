#!/bin/sh
apk add --no-cache \
    bash \
    bison \
    bsd-compat-headers \
    ccache \
    cmake \
    curl \
    diffutils \
    flex-dev \
    musl-fts-dev \
    g++ \
    git \
    libpcap-dev \
    linux-headers \
    make \
    openssh-client \
    openssl-dev \
    procps \
    py3-pip \
    python3 \
    python3-dev \
    swig \
    tar \
    zlib-dev

pip3 install websockets junit2html
