#!/bin/bash

echo "noderetry = $1" > imcycle.htc
echo "imcycle = $(( $1 + 1 ))" >> imcycle.htc

if [ $1 -gt 0 ]
then
    echo "copy_model = True" >> imcycle.htc
else
    echo "copy_model = False" >> imcycle.htc
fi
