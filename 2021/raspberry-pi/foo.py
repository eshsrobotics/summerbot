#!/usr/bin/env python

import argparse

foo = argparse.ArgumentParser(prog="foo", description="Something")

foo.add_argument("--uche", type=int, metavar='N', help="The string --Uche is going to be used to test the function.")

args = foo.parse_args()

print(args.uche*args.uche*args.uche)