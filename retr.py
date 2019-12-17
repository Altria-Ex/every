import os
import time

from utils import *

def main():
    _, files = disk_search()
    build_index(files)
    print(len(files))


if __name__ == '__main__':
    main()