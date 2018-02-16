#!/bin/bash

make clean;
make libgpio.a;
make dep;
make copy
ssh -p 5025 pi@132.227.102.36

