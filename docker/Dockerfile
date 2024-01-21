# Base image
FROM --platform=amd64 mbentley/debian:stretch

# Development package dependencies
RUN apt-get update && \
    apt-get install -y \
        build-essential \
        libreadline-dev \
        wget \
        git \
        vim

# Tango package dependencies
RUN apt-get update && \
    apt-get install -y \
        pkg-config \
        zlib1g-dev \
        libomniorb4-dev \
        libcos4-dev \
        libzmq3-dev \
        omniidl

# Retrieve Rontec Tango device server
RUN git -C /tmp clone https://github.com/gabadinho/RontecTangoDS.git

# Compile Tango
RUN wget -P /tmp/ https://github.com/tango-controls/TangoSourceDistribution/releases/download/9.3.4/tango-9.3.4.tar.gz && \
    mkdir -p /usr/local/tango && \
    tar xfz /tmp/tango-9.3.4.tar.gz -C /usr/local/tango && \
    cd /usr/local/tango/tango-9.3.4 && \
    ./configure --enable-java=no --enable-mariadb=no --enable-dbserver=no --enable-dbcreate=no --prefix=/usr/local && \
    make && \
    make install

# Set TANGO database server port
ENV TANGO_HOST=localhost:10000
ENV LD_LIBRARY_PATH=/usr/local/lib

# Open a shell
CMD ["/bin/bash"]

