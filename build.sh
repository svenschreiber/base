#!/bin/zsh

OUTPUT_DIR="run_tree/"

if [ ! -d "$OUTPUT_DIR" ]; then
    mkdir run_tree
fi

FLAGS="-D_GNU_SOURCE -fPIC -fpermissive" 
WARNINGS="-Wno-write-strings -Wno-deprecated-declarations -Wno-comment -Wno-switch -Wno-null-dereference -Wno-tautological-compare -Wno-unused-result -Wno-missing-declarations -Wno-nullability-completeness"

clang $=WARNINGS -DBUILD_MACOS -framework Cocoa -framework OpenGL -xobjective-c $=FLAGS -o $OUTPUT_DIR/app ./src/app.c

