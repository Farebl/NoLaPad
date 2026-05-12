#!/bin/bash
set -e

echo "==> Installing NoLaPad build dependencies (Qt6 must be installed separately)..."

sudo apt update
sudo apt install -y \
    cmake \
    ninja-build \
    git \
    g++ \
    libcurl4-openssl-dev \
    libmp3lame-dev \
    libasound2-dev \
    libfreetype6-dev \
    libfontconfig1-dev \
    libx11-dev \
    libxrandr-dev \
    libxinerama-dev \
    libxcursor-dev \
    libxcomposite-dev

echo ""
echo "==> Done!"
echo "    Make sure Qt6 is installed and Qt6_DIR is set, then:"
echo "    cmake -B build -DCMAKE_BUILD_TYPE=Release"
echo "    cmake --build build -j\$(nproc)"
echo "    ./build/NoLaPad"
