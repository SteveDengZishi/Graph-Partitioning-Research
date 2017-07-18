#!/bin/bash
g++ -o Test Test.cpp -std=c++11
echo -e "g++ compiled Test.cpp successfully\n"

FileName="facebook_combined.txt"
shard=8

echo "running Test"
./Test > lp_ingred.txt << EOF
$FileName
$shard
EOF
