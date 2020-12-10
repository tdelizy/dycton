#!/usr/bin/python


# This file is part of the Dycton project scripts.
# This software aims to provide an environment for Dynamic Heterogeneous Memory 
# Allocation for embedded devices study.

# Copyright (C) 2019  Tristan Delizy, CITI Lab, INSA de Lyon

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.

# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.



from __future__ import print_function
import os, sys, getopt, time, math, datetime, subprocess

from decimal import *


import numpy as np

app = ""


def usage():
  print("usage:")
  print("./heap_validation [-h, --help] file_1.log file_2.log")
  print("-h, --help: printing help")
  print("this scripts check that two execution heap logs are equivalent")
  print("meaning that the same objects (same size, same alloc site)")
  print("have been allocated and freed in the same order, and accessed")
  print("the same number of time (R/W)")




def main():
  global app

  date_start = str(datetime.datetime.now())

  print("validating that heap object log from 2 different")
  print("execution have taken the same placement decisions.")
  print("==================================================")
  print("author: T. Delizy (delizy.tristan@gmail.com)")
  print(date_start)
  log1 = ""
  log2 = ""

# command line arguments processing
  try:
    opts, args = getopt.getopt(sys.argv[1:], 'h', ['help'])
  except getopt.GetoptError as err:# print help information and exit:
    print(str(err))  # will print something like "option -a not recognized"
    print("command line argument parsing error")
    usage()
    sys.exit(1)

  for o, a in opts:
    if o in ("-h", "--help"):
      print("invoking help")
      usage()
      sys.exit(0)

  if len(args) != 2:
    print("You need to provide exactly 2 files to compare.")
    usage()
    sys.exit(1)
  
  log1 = args[0]
  log2 = args[1]

  print("heap log 1:", log1)
  print("heap log 2:", log2)
  print("- - - - - - - - - - - - - - - - - - - - - - - - - ")


# log parsing 
  objects_log1 = np.loadtxt(log1, delimiter=';', dtype=int, converters={8: lambda s: int(s, 16)})
  objects_log2 = np.loadtxt(log2, delimiter=';', dtype=int, converters={8: lambda s: int(s, 16)})

#addr;size;malloc_date_cycles;lifespan_cycles;r_count;w_count;alloc_order;free_order;alloc_site
  trim1 = objects_log1[:,[0,1,4,5,6,7,8]]
  trim2 = objects_log2[:,[0,1,4,5,6,7,8]]

  ret = np.array_equal(trim1,trim2)

  print("are the heap logs the same ?", ret)

  if ret == False:
    print("saving the trimmed logs and calling diff.")
    log1_trim = log1.split(".log")[0]+"_trim.csv"
    log2_trim = log2.split(".log")[0]+"_trim.csv"
    np.savetxt(log1_trim, trim1, fmt='%d',delimiter=";")
    np.savetxt(log2_trim, trim2, fmt='%d',delimiter=";")

    subprocess.call(["meld", log1_trim, log2_trim])


  print("done. exiting...")
  exit(0)








if __name__ == "__main__":
    main()

