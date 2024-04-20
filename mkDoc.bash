#!/bin/bash

doxygen doc/codeDoc/code.dox
cd doc/codeDoc/doxygen/latex
make
cp refman.pdf ../../.

