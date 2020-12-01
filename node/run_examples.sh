#!/bin/sh
for i in `ls examples | grep js`;
do
 echo "================= Run $i ===============";
 case=$i npm run test;
done