#!/usr/bin/env python3

import subprocess
import os
import sys

def usage():
    print(sys.argv[0] + ' [options]')
    print('     -run        run tests')
    print('     -rec        record tests')
    print('     -h or -help display help to options')

def compare(path1, path2):
    file1 = open(path1)
    file2 = open(path2)
    ret = True
    if file1.read() != file2.read():
        ret = False
    file1.close()
    file2.close()
    return ret

def rec():
    for _, _, files in os.walk('./tests'):
        for file in files:
            if file[len(file) - 5:] == 'corth':
                file = './tests/' + file
                returnCode = subprocess.run(['./corth', file]).returncode
                if returnCode != 0:
                    print('./corth ' + file + ' ERROR ' + str(returnCode) + "\n\n")
                    os.system('echo ' + '\'ERROR\' ' + '> ' + file.removesuffix('corth') + 'fasm.txt')
                    continue
                file = file.removesuffix('corth')
                file = file + 'fasm'
                subprocess.run(['fasm', '-m', '1000000', file])
                subprocess.run(['touch', file + '.txt'])
                os.system('./' + file.removesuffix('.fasm') + ' > ' + file + '.txt')

def test():
    some_fail = False
    fail_files = []
    number_of_tests = 0
    for _, _, files in os.walk('./tests'):
        for file in files:
            if file[len(file) - 5:] == 'corth':
                number_of_tests = number_of_tests + 1
                file = './tests/' + file
                if subprocess.run(['./corth', file]).returncode != 0:
                    os.system('echo ' + '\'ERROR\' ' + '> ' + file.removesuffix('corth') + 'fasm.test.txt')
                    if compare(file.removesuffix('corth') + 'fasm.test.txt', file.removesuffix('corth') + 'fasm.txt') == False:
                        fail_files.append(file)
                        some_fail = True
                        print('\nTEST FAIL ' + file + '\n')
                    continue
                file = file.removesuffix('corth')
                file = file + 'fasm'
                subprocess.run(['fasm','-m', '1000000', file])
                subprocess.run(['touch', file + '.test.txt'])
                os.system('./' + file.removesuffix('.fasm') + ' > ' + file + '.test.txt')
                if compare(file + '.test.txt', file + '.txt') == False:
                    fail_files.append(file)
                    some_fail = True
                    print('\nTEST FAIL ' + file + '\n')
    if some_fail:
        print("\n\nFail:")
        for i in fail_files:
            print(i)
    else:
        print(f'\n\nAll {number_of_tests} tests pass')
                
if __name__ == '__main__':
    if len(sys.argv) != 2:
        usage()
    elif sys.argv[1] == '-run':
        test()
    elif sys.argv[1] == '-rec':
        rec()
    elif sys.argv[1] == '-h' or sys.argv[1] == '-help':
        usage()
    else:
        usage()
