#!/bin/bash
set -e
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

if [ ! -d "$SCRIPT_DIR/thirdparty/imgui" ]; then
    echo "Cloning imgui ..."
    git clone --depth 1 https://github.com/ocornut/imgui.git "$SCRIPT_DIR/thirdparty/imgui"
fi

mkdir -p "$SCRIPT_DIR/build"
cd "$SCRIPT_DIR/build"
cmake ..
cmake --build . --config Release -j"$(nproc 2>/dev/null || echo 4)"

echo ""
echo "Build succeeded!"
