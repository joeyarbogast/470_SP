#!/bin/bash
cd serial
make clean
make
cd ../par
make clean
make
cd ../
