#!/bin/bash
for file in nss/*.nsb ; do
    ./nsbparse "$file" $1
done
