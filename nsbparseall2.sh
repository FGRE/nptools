#!/bin/bash
for file in nss/*.nsb ; do
    ./nsbparse2 "$file" "${file:4}"
done
