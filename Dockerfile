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
	libboost-system-dev

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
