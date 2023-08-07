#!/bin/sh
# https://discussion.fedoraproject.org/t/which-kind-of-dependencies-suggested-recommended-does-dnf-install/74111/2
echo 'install_weak_deps=False' >>/etc/dnf/dnf.conf

dnf -y install \
    bison \
    ccache \
    cmake \
    diffutils \
    findutils \
    flex \
    gcc \
    gcc-c++ \
    git \
    libpcap-devel \
    make \
    nodejs-devel \
    openssl \
    openssl-devel \
    procps-ng \
    python3 \
    python3-devel \
    python3-pip \
    sqlite \
    swig \
    which \
    zlib-devel

pip3 install websockets junit2html
