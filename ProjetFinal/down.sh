#!/bin/bash
cd VM1
echo "DOWN VM1"
vagrant halt
echo "VM1 morte"
cd ..

cd VM3
echo "DOWN VM3"
vagrant halt
echo "VM3 morte"
cd ..

cd VM1-6
echo "DOWN VM1-6"
vagrant halt
echo "VM1-6 morte"
cd ..

cd VM2-6
echo "DOWN VM2-6"
vagrant halt
echo "VM2-6 morte"
cd ..

cd VM3-6
echo "DOWN VM3-6"
vagrant halt
echo "VM3-6 morte"
cd ..
