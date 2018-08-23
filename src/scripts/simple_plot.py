#!/usr/bin/python

import os, sys, getopt, time, math

from datetime import datetime, timedelta
from decimal import *

import matplotlib.pyplot as plt
import matplotlib.colors as colors
import matplotlib.ticker as ticker
import matplotlib.patches as patches

import numpy as np



def main():
  # command line arguments processing
  try:
    opts, args = getopt.getopt(sys.argv[0:], "hV:", ["help"])
  except getopt.GetoptError as err:# print help information and exit:
    print str(err)  # will print something like "option -a not recognized"
    sys.exit(2)

  verbose = False

  for o, a in opts:
    #print "option=", o, " arg=", a
    if o == "-V":
      print "script last modification : ", time.strftime('%m/%d/%Y-%H:%M', time.localtime(os.path.getmtime(sys.argv[0])))
    elif o in ("-h", "--help"):
      usage()
      sys.exit()
    else:
      print "unhandled option :", o, a
      sys.exit(2)

  # parsing
  print "=================================="
  print "parsing file name"
  print "=================================="
  print "args = ", args
  for f in args:
      data_file_name = f

  print "=================================="
  print "parsing allocation log"
  print "=================================="
  # sorted by request date (generated in that order)
  data = np.loadtxt(data_file_name, delimiter=':', dtype=int)

  print data

  x = []
  y = []

  for line in data:
    x.append(line[0])
    y.append(line[1])

  print x
  print y
  print "=================================="
  print "plotting"
  print "=================================="
  plt.close('all')
  f, ax = plt.subplots(1, 1)

  # ax.scatter(x,y)
  ax.plot(x, y, lw=1, color='black', marker='+')
  # print "max x =", max(data[0:])
  # print "max y =", max(data[1:])
  ax.set_xlim([0, max(x)])
  ax.set_ylim([0, max(y)])

  ax.get_yaxis().get_major_formatter().set_useOffset(False)
  ax.get_yaxis().set_major_formatter(ticker.FormatStrFormatter("%d"))

  # label work
  ax.set_xlabel('Heap Memory Latency (cycles)', fontsize=16)
  ax.set_ylabel('Exec Time (cycles)', fontsize=16)
  ax.set_title("Heap Memory Latency Influence on Program Execution Time")

  plt.show()
  sys.exit()




if __name__ == "__main__":
    main()

