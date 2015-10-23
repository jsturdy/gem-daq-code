#!/bin/bash/
cd gemtreewriter
make all -j8
cd ../dqm-root/
make all -j8
cd ../
