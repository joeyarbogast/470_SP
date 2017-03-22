#!/bin/bash
module load mpi
cd serial
make
cd ../par
make
cd ../
