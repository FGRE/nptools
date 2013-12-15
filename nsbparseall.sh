#!/bin/bash
for file in nss/*.nsb ; do
	./nsbparse -l "$file"
done
