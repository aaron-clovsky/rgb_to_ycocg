#!/usr/bin/env bash

if ! command -v cc >/dev/null 2>&1; then
  echo "C compiler not installed, please install one"
  exit 2
fi

if ! command -v iverilog >/dev/null 2>&1; then
  echo "iverilog is not available, please install iverilog"
  exit 2
fi

if ! command -v vvp >/dev/null 2>&1; then
  echo "vvp is not available, please install iverilog"
  exit 2
fi

set -e

cd "$(dirname "$0")"

make clean >/dev/null 2>&1
make

echo finish | vvp sim_ycocg > test_v.txt

./rgb_to_ycocg allYCoCg > test_c.txt

if diff test_v.txt test_c.txt > /dev/null; then
    echo "Verilog Matches C"
    rm -f test_v.txt
    rm -f test_c.txt
else
    echo "!!! Verilog DOES NOT MATCH C !!!"
fi

