#!/bin/bash
set -e
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

if [ ! -d "$SCRIPT_DIR/thirdparty/imgui" ]; then
    echo "Cloning imgui ..."
    git clone --depth 1 https://github.com/ocornut/imgui.git "$SCRIPT_DIR/thirdparty/imgui"
fi

mkdir -p "$SCRIPT_DIR/build"
cd "$SCRIPT_DIR/build"
cmake -DCMAKE_CXX_COMPILER=g++-13 -DCMAKE_C_COMPILER=gcc-13 ..
make -j"$(nproc)"

echo ""
echo "Build succeeded! Run: ./selection_demo/build/selection_demo"
