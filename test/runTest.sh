#!/bin/bash

DIRS=`ls -l | grep "^d" | awk '{print $9}'`

for DIR in $DIRS; do
  if [[ -s $DIR/Makefile ]]; then
    make -C $DIR clean &>> /dev/null;
    make -C $DIR &>> /dev/null;
    TESTS=`ls $DIR/*Test`
    for TEST in $TESTS; do
      ./$TEST
    done
    make -C $DIR clean &>> /dev/null;
  fi
done
