#!/bin/zsh

OUTPUT_DIR="run_tree/"

if [ ! -d "$OUTPUT_DIR" ]; then
    mkdir run_tree
fi

gcc -framework Cocoa -x objective-c -o $OUTPUT_DIR/app ./src/mac/mac_app.m
