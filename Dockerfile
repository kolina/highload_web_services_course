FROM ubuntu:14.04

RUN apt-get update && apt-get install -y \
    build-essential \
    git \
    debhelper \
    automake1.9 \
    autotools-dev \
    libboost-dev \
    libboost-thread-dev \
    libfcgi-dev \
    libxml2-dev \
    libboost-regex-dev \
    libtool \
    libssl-dev \
    autoconf-archive \
    pkg-config \
    automake \
    libboost-system-dev \
    libsasl2-dev \
    wget \
    software-properties-common

RUN add-apt-repository ppa:george-edison55/cmake-3.x

RUN apt-get update && \
    apt-get install -y cmake

RUN cd /tmp && \
    git clone https://github.com/lmovsesjan/Fastcgi-Daemon.git && \
    cd Fastcgi-Daemon && \
    dpkg-buildpackage -rfakeroot && \
    cd .. && \
    sudo dpkg -i \
         ./fastcgi-daemon2-init_2.10-29_amd64.deb \
         ./fastcgi-daemon2_2.10-29_amd64.deb \
         ./libfastcgi-daemon2-dev_2.10-29_amd64.deb \
         ./libfastcgi-daemon2_2.10-29_amd64.deb \
         ./libfastcgi2-syslog_2.10-29_amd64.deb \
    && cd / && rm -rf /tmp/Fastcgi-Daemon

RUN mkdir /tmp/mongoc-build && \
    cd /tmp/mongoc-build && \
    wget https://github.com/mongodb/mongo-c-driver/releases/download/1.5.1/mongo-c-driver-1.5.1.tar.gz && \
    tar xzf mongo-c-driver-1.5.1.tar.gz && \
    cd mongo-c-driver-1.5.1 && \
    ./configure && \
    make && \
    make install && \
    rm -rf /tmp/mongoc-build

RUN mkdir /tmp/mongocxx-build && \
    cd /tmp/mongocxx-build && \
    git clone https://github.com/mongodb/mongo-cxx-driver.git --branch releases/stable --depth 1 && \
    cd mongo-cxx-driver/build && \
    cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local .. && \
    make && \
    make install && \
    rm -rf /tmp/mongocxx-build

RUN ldconfig

COPY src /src

RUN mkdir /tmp/mail-app-build && \
    cd /tmp/mail-app-build && \
    cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/app /src && \
    make && \
    make install && \
    rm -rf /tmp/mail-app-build
