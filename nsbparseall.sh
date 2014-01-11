#!/bin/bash
for file in nss/*.nsb ; do
	./nsbparse $1 "$file"
done
