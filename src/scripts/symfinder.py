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
import os, sys, getopt, time

import numpy as np

def usage():
	print("usage : ")
	print("./symfinder.py FILEPATH ADDRESS")
	print("FILEPATH: the path to the symbol table in textformat (objdump -t target.out > symtable.txt)")
	print("ADDRESS: the code address to link with a function name /!\\ INTERPRETED AS HEXADECIMAL\n")
	print("standard output: the function name and the hexadecimal offset from the function start or \"ERROR ERROR\"")


def main():
  # command line arguments processing
  try:
    opts, args = getopt.getopt(sys.argv[0:], "h", ["help"])
  except getopt.GetoptError as err:# print help information and exit:
    print(str(err))  # will print something like "option -a not recognized"
    usage()
    exit(2)

  for o, a in opts:
    #print "option=", o, " arg=", a
    if o in ("-h", "--help"):
      usage()
      sys.exit()
    else:
      print("unhandled option :", o, a)
      print("\n\n")
      usage()
      sys.exit(2)


  if len(args)==3:
    # print "args", args
    file = args[1]
    addr = int(args[2], 16)
  else:
    usage()
    exit(2)

  # print "file =", file
  # print "addr =", addr, "//", addr

  i=0
  ok=0

  with open(file, "r") as infile:
    for line in infile:
      i+=1
      try:
      	cur_sym_addr = int(line.split(" ")[0],16)
      	section = line.split(".")[1].split("\t")[0]
      	symbol_name = line.split(" ")[-1][:-1]
      	size = int(line.split(" ")[-2].split("\t")[-1], 16)
      	if section == "text" and addr >= cur_sym_addr and addr < (cur_sym_addr+size):
      	  print(symbol_name, (addr - cur_sym_addr))
      	  ok = 1
      except:
      	pass
  if ok == 0:
    print("ERROR ERROR")
    exit(1)
  else:
    exit(0)




if __name__ == "__main__":
	main()
