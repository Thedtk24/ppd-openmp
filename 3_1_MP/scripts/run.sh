#!/bin/bash

EXEC=./bin/3_1_1
REPORT=reports/report.csv

mkdir -p reports

echo "num_threads,time_kernel,time_reference" > $REPORT

SIZES=(8192)

for threads in {1..8}; do
    export OMP_NUM_THREADS=$threads
    export OMP_DYNAMIC=FALSE
    export OMP_NESTED=FALSE
    for N in "${SIZES[@]}"; do
        echo "Running mode=3 (BOTH) N=$N threads=$threads..."
        $EXEC 3 $N >> $REPORT
    done
done

echo "Rapport généré dans $REPORT"
