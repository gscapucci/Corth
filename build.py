#!/usr/bin/env python3


import subprocess
import sys

def run_echoed_cmd(args):
    print('[CMD] ' + args)
    process = subprocess.Popen(args, shell = True, stdout = subprocess.PIPE, stderr = subprocess.STDOUT)
    output = process.communicate()
    exitCode = process.returncode
    if exitCode != 0:
        if output[0] != None:
            print(output[0].decode("utf-8"))
        if output[1] != None:
            print(output[1].decode("utf-8"))
        exit(1)
    if output[0] != None:
        print(output[0].decode("utf-8"), end = '')
    if output[1] != None:
        print(output[1].decode("utf-8"), end = '')

def build():
    run_echoed_cmd('gcc -c mem.c -o mem.o')
    run_echoed_cmd('gcc -c corth.c -o corth.o')
    run_echoed_cmd('gcc mem.o corth.o main.c -o corth -lm')
    run_echoed_cmd('gcc -g mem.c corth.c main.c -lm')

def run():
    run_echoed_cmd('./corth std.corth')
    run_echoed_cmd('fasm -m 1000000 std.fasm')
    run_echoed_cmd('./std')

if __name__ == "__main__":
    if len(sys.argv) == 1:
        print('empty input')
        exit(1)
    if len(sys.argv) == 2:
        if sys.argv[1] == '-run' or sys.argv[1] == '-r':
            run()
            exit(0);
        if sys.argv[1] == '-build' or sys.argv[1] == '-b':
            build()
            exit(0);
    if len(sys.argv) == 3:
        if (sys.argv[1] == '-build' or sys.argv[1] == '-b') and (sys.argv[2] == '-run' or sys.argv[2] == '-r'):
            build()
            run()
            exit(0);
    else:
        print('invalid args: ' + str(sys.argv[1:]))
        exit(1)