FROM alpine:3.23.2

WORKDIR /lupinho

RUN apk update && apk add --no-cache \
    cmake \
    make \
    libuv-dev \
    libc-dev \
    readline-dev \
    gcc \
    g++ \
    git \
    openssl-dev \
    wget \
    curl \
    unzip \
    musl-dev \
    bash

# Install Lua 5.1
RUN wget https://www.lua.org/ftp/lua-5.1.5.tar.gz && \
    tar -xzf lua-5.1.5.tar.gz && \
    cd lua-5.1.5 && \
    make linux && \
    make install

# Install LuaRocks
RUN wget https://luarocks.org/releases/luarocks-3.10.0.tar.gz && \
    tar -xzf luarocks-3.10.0.tar.gz && \
    cd luarocks-3.10.0 && \
    ./configure && \
    make && \
    make install

# Install Lua dependencies
RUN luarocks install luasec 1.3.0-1 && \
    luarocks install luasocket 3.1.0-1 && \
    luarocks install luafilesystem 1.8.0-1 && \
    luarocks install luv 1.51.0-1 && \
    luarocks install busted && \
    luarocks install lua-cjson && \
    luarocks install luabitop && \
    luarocks install argparse

COPY . /lupinho/
