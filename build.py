#!/usr/bin/env python3

import subprocess
import sys

def run_echoed_cmd(args):
    print('[CMD] ' + args)
    args = args.split()
    if subprocess.run(args).returncode != 0:
        print("error")
        exit(1)

def main():
    run_echoed_cmd('gcc -c mem.c -o mem.o')
    run_echoed_cmd('gcc -c corth.c -o corth.o')
    run_echoed_cmd('gcc mem.o corth.o main.c -o corth -lm')
    run_echoed_cmd('gcc -g mem.c corth.c main.c -lm')

if __name__ == "__main__":
    main()
    if len(sys.argv) == 2 and sys.argv[1] == '-run':
        run_echoed_cmd('./corth std.corth')
        run_echoed_cmd('fasm -m 1000000 std.fasm')
        run_echoed_cmd('./std')

