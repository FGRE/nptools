#!/bin/bash
for file in nss/*.nsb ; do
	./nsbparse2 $1 "$file" "${file:4}"
done
