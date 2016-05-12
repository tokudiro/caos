#!/bin/bash

../caos -s SampleB.caos
../caos SampleA.caos
gcc SampleA.c SampleB.c Sample.c
