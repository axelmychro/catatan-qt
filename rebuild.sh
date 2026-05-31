#!/bin/sh

rm -fr ./build/
cmake -S . -B ./build/Desktop_Debug/
ln -sfn ./build/Desktop_Debug/compile_commands.json .
