#!/bin/bash
set -e

PROJECT_ROOT="$HOME/Programmer/QtProjects/NoLaPad"
LOG_DIR="$PROJECT_ROOT/tests/log"

QT_PATH=$HOME/Qt/6.9.3/gcc_64
export LD_LIBRARY_PATH="${QT_PATH}/lib:${LD_LIBRARY_PATH}"

export CMAKE_CXX_COMPILER_LAUNCHER=ccache
export CMAKE_C_COMPILER_LAUNCHER=ccache

cd "$PROJECT_ROOT"
mkdir -p "$LOG_DIR"

echo "--- Preparing build directory ---"
mkdir -p build
cd build

echo "--- Configuring CMake ---"
cmake .. \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_PREFIX_PATH=$QT_PATH

echo "--- Cleaning stale coverage data (.gcda) ---"
find . -name "*.gcda" -delete 2>/dev/null || true

echo "--- Building all targets ---"
cmake --build . --parallel $(nproc)

echo ""
echo "============================================================"
echo "  RUNNING ALL TESTS VIA CTEST"
echo "============================================================"

ANY_FAILED=0
if ! ctest -j$(nproc) --output-on-failure --verbose; then
    echo ""
    echo "  ✗ SOME TESTS FAILED"
    ANY_FAILED=1
fi

echo ""
echo "============================================================"
echo "  TEST COVERAGE REPORT"
echo "============================================================"

gcovr -r .. \
    --merge-mode-functions=merge-use-line-min \
    --gcov-ignore-parse-errors=negative_hits.warn \
    --gcov-ignore-errors=no_working_dir_found \
    -e ".*/JUCE/.*" \
    -e ".*/tests/.*" \
    -e ".*/build/.*" \
    -e ".*_autogen/.*" \
    -e ".*/nlohmann/.*" \
    -e ".*/Qt/.*" \
    -e ".*/usr/.*" \
    --print-summary \
    --txt "$LOG_DIR/coverage_summary.txt" \
    --html-details "$LOG_DIR/coverage_report.html"

echo ""
echo "--- Coverage saved to: $LOG_DIR ---"

if [ -f "$LOG_DIR/coverage_summary.txt" ]; then
    cat "$LOG_DIR/coverage_summary.txt"
else
    echo "⚠ Coverage summary not found."
fi

exit $ANY_FAILED
