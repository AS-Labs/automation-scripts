#!/bin/bash

perf record -ag -F 99 -- sleep 5
perf script | c++filt | gprof2dot -f perf | dot -Tpng -o output.png
