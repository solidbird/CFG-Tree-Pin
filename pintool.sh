#!/bin/bash

makefilePath=/home/ich/Desktop/pin/source/tools/ManualExamples

scriptPath=$(pwd)

cp $1.cpp $makefilePath
cd $makefilePath
make $1.test TARGET=intel64

cp $makefilePath/obj-intel64/$1.so $scriptPath
