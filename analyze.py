#!/usr/bin/python3
import sys

with open(sys.argv[1]) as fd:
    data = fd.read()

nums = list(map(int, data.split()))
print("min max mean:")
print(min(nums), max(nums), sum(nums) / len(nums))
