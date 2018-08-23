#!/usr/bin/python

import os, sys, getopt, time, math

from datetime import datetime, timedelta
from decimal import *

import matplotlib.pyplot as plt
import matplotlib.colors as colors
import matplotlib.ticker as ticker
import matplotlib.patches as patches

import numpy as np


_plot_number = 0

class Memory_range:
  def __init__(self, name, offset, size):
    self.name = name
    self.offset = offset
    self.size = size


def usage():
  print "usage : use it.\n"

def file_len(fname):
  i=-1
  with open(fname) as f:
    for i, l in enumerate(f):
      pass
  return i + 1




def map_range(low, high, amap):
  low_match = False
  high_match = False
  res = []
  for i, m in enumerate(amap):
    if m.offset < high and (m.offset + m.size)> low:
      print "appending", m.name
      res.append(m)
  return res

def plot_simple_alloc_boxes(allocs):

  obj_lifespan = np.array([])
  obj_size = np.array([])


  # computing access count to each object
  for i, a in enumerate(allocs):
    # selecting memory accesses with @ inside the allocated memory zone during object lifetime
    obj_lifespan = np.append(obj_lifespan, a[3]-a[1])
    obj_size = np.append(obj_size, (a[2]-a[0])/4)
    # print ">> [", i, "]\t\t", obj_lifespan[i], "\t\t", obj_size[i]


  plt.close('all')
  f, ax = plt.subplots(1, 1)

  # plotting the graph
  for i, a in enumerate(allocs):
    ax.add_patch(patches.Rectangle((a[1], a[0] ), a[3]-a[1], a[2]-a[0], facecolor='steelblue', edgecolor='black'))


  #limits
  min_x_val = 0
  max_x_val = max(allocs[:,3])

  min_y_val = min(allocs[:,0])
  max_y_val = max(allocs[:,2])

  ax.set_xlim([0, max_x_val])
  ax.set_ylim([min_y_val, max_y_val])
  tick_range_y =  int(pow(16, math.floor(math.log((max_y_val - min_y_val),16))))
  tick_range_x =  int(pow(10, math.floor(math.log(max_x_val, 10)+0.5))/4)
  ax.get_yaxis().set_major_locator(ticker.MultipleLocator(tick_range_y))
  ax.get_yaxis().get_major_formatter().set_useOffset(False)
  ax.get_yaxis().set_major_formatter(ticker.FormatStrFormatter("0x%X"))
  ax.get_xaxis().set_major_locator(ticker.MultipleLocator(tick_range_x))



  # label work
  ax.set_xlabel('Time (microseconds)', fontsize=16)
  ax.set_ylabel('Heap address', fontsize=16)
  ax.set_title("Heap occupation")

  # adjust margins
  plt.subplots_adjust(left=0.18)

  #show it to the world
  plt.savefig('heap_occupation.png', dpi=330)
  plt.show()

  print "Heap occupation in function of time saved"
  # sys.exit(0)


def main():
  # command line arguments processing
  try:
    # opts, args = getopt.getopt(sys.argv[1:-1], "hV:", ["help"])
    # ==> this breaks everything previously using this script and should not be done this way
    # most importantly this modification should have been notified in commit message of 25/11/2017 13:09:53 +0100.
    # see below for clean implementation
    opts, args = getopt.getopt(sys.argv[1:], "thV:", ["help", "title"])
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
    elif o in ("-t", "--title"):
      print "plot title set to:", a
      plot_title = a
    else:
      print "unhandled option :", o, a
      sys.exit(2)


  # parsing
  print "=================================="
  print "parsing files names"
  print "=================================="
  print "args = ", args
  for f in args:
    print "considering argumet", f
    if len(os.path.basename(f).split(".")) > 1 and os.path.basename(f).split(".")[1]=="log":
      l=file_len(f)
      print f, l
      if(l>0):
        mem_accesses_file_name = f
    elif os.path.basename(f) == "address_map":
      address_map_file_name = f
    elif os.path.basename(f) == "malloc_trace":
      alloc_log_file_name = f
    else :
      print "what is", f, "?(ignored)"

  print "=================================="
  print "parsing allocation log"
  print "=================================="

  # sorted by request date (generated in that order)
  alloc_log = np.loadtxt(alloc_log_file_name, delimiter=':', dtype=int)

  # removing the timed events (for now we don't need them here)
  indices_to_clean = np.where(alloc_log[:,0]==2)[0]
  alloc_log = np.delete(alloc_log, indices_to_clean, 0)

  allocations = np.array([[0,0,0,0]])
  thrash_list = []

  for index, alloc in enumerate(alloc_log) :
    if index in thrash_list:
      # print "index", index, "already taken into account, skipping line."
      continue
    if alloc[0] == 2:
      print "ERROR : allocation list not correctly cleaned from timed events"
      exit(0)
    if alloc[0] == 0:
      print "WARNING : freeing unallocated address !"
      print "index =", index
      print "alloc line =", alloc
      # sys.exit(0)
      print "(SKIPPED)"
      continue # may be an issue ?? well free can be called with anything, valid or not

    # gather allocation infos
    ad = alloc[3]
    size = alloc[4]
    malloc_date = alloc[1]

    # find corresponding deallocation
    same_ad_lines = np.where(alloc_log[:,3]==ad)[0] # indexes corresponding to the same address

    try:
      free_idx_idx = np.where(same_ad_lines>index)[0][0] # the first one with index greated than current iteration
      free_idx = same_ad_lines[free_idx_idx] #the actual index in alloc_log
      free_date = alloc_log[free_idx][1]

      # thrash that line
      thrash_list.append(free_idx)
    except:
      free_date = alloc_log[-1][1] #if the memory is not freed, we assume the end of the execution is the free
      pass

    # construct allocated object descriptor
    obj_desc = np.array( [ad, malloc_date, ad+size, free_date], dtype=int)

    # construct allocation object
    allocations = np.vstack((allocations, obj_desc))
  allocations = np.delete(allocations, 0, 0)

  print "=================================="
  print "plotting"
  plot_simple_alloc_boxes(allocations)



  sys.exit()




if __name__ == "__main__":
    main()

