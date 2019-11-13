#!/bin/bash
cd VM1
echo "lancement VM1"
vagrant up > /dev/null 2>&1
echo "VM1 OK"
cd ..

cd VM3
echo "lancement VM3"
vagrant up > /dev/null 2>&1
echo "VM3 OK"
cd ..

cd VM1-6
echo "lancement VM1-6"
vagrant up > /dev/null 2>&1
echo "VM1-6 OK"
cd ..

cd VM2-6
echo "lancement VM2-6"
vagrant up > /dev/null 2>&1
echo "VM2-6 OK"
cd ..

cd VM3-6
echo "lancement VM3-6"
vagrant up > /dev/null 2>&1
echo "VM3-6 OK"
cd ..
