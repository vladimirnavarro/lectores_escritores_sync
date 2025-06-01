#!/bin/bash

echo "version,readers,writers,time" > resultados.csv

for version in le_mutex_cond le_busy_wait le_barrier; do
  for r in 1 4 2 ; do
    for w in 1 3 2; do
      echo "Ejecutando $version con $r lectores y $w escritores..."
      tiempo=$(./bin/$version $r $w | grep -i "Total time" | awk '{print $(NF-1)}')
      echo "$version,$r,$w,$tiempo" >> resultados.csv
    done
  done
done
