import sys
import subprocess
import datetime

# 1 000 000 000 = 10^10 // 8 processors = 12 seconden
repetition = 10;
executable = '';

for p in [1, 2, 4, 8]:
    for iterations in [100, 1000, 10000]:
        executable = "./main " + str(p) + " " + str(iterations)
        for repeat in range(0, repetition):
            process = subprocess.Popen(executable, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
            retval = process.wait()