import os
import re
import time

def main():
    start_time = time.time()
    disks = (os.popen('fsutil fsinfo drives').read()).split()[1:]
    dirs = list()
    for d in disks:
        dirs.append(list(os.walk(d)))
        pass
    end_rime = time.time()
    print(end_rime - start_time)



if __name__ == '__main__':
    main()