#!/bin/zsh

OUTPUT_DIR="run_tree/"

if [ ! -d "$OUTPUT_DIR" ]; then
    mkdir run_tree
fi

gcc -framework Cocoa -framework OpenGL -x objective-c -Wno-deprecated-declarations -o $OUTPUT_DIR/app ./src/mac/mac_app.m
