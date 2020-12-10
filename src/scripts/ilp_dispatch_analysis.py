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

import matplotlib.pyplot as plt
import matplotlib.colors as colors
import matplotlib.ticker as ticker
import matplotlib.patches as patches

from matplotlib.lines import Line2D   

import numpy as np

from collections import defaultdict


app = ""
dataset = -1


def usage():
  print("usage:")
  print("-i \"filepath\": input heap objects log file path")
  print("-s \"filepath\": symbol table file path")
  print("-m \"filepath\": memory architecture description file path")
  print("This scritp analyse from heap execution log how each allocation site have been dispatched")
  print("compute dispatch decisions pre-fallback mechanism")
  print("exiting...")




def main():
  global app
  global dataset

  date_start = str(datetime.datetime.now())

  print("Post mortem dispatch by allocation site analysis")
  print("==================================================")
  print("author: T. Delizy (delizy.tristan@gmail.com)")
  print(date_start)

  log_file = ""
  symbol_table_file = ""
  memory_arch_file = ""

# command line arguments processing
  try:
    opts, args = getopt.getopt(sys.argv[1:], 'rpi:s:m:', ['help'])
  except getopt.GetoptError as err:# print help information and exit:
    print(str(err))  # will print something like "option -a not recognized"
    print("command line argument parsing error")
    usage()
    sys.exit(1)

  for o, a in opts:
    if o in ("-i"):
      log_file = a
    elif o in ("-h", "--help"):
      print("invoking help")
      usage()
      sys.exit(0)
    elif o in ("-s"):
      symbol_table_file = a
    elif o in ("-m"):
      memory_arch_file = a
  if log_file == "" or symbol_table_file == "" or memory_arch_file == "":
    print("error in file path !")
    usage()
    sys.exit(1)    

  print("log file:", log_file)
  print("symbol table:", symbol_table_file)
  print("memory architecture description:", memory_arch_file)
  print("- - - - - - - - - - - - - - - - - - - - - - - - - ")

# loading memory architecture latencies
  memory_arch = np.loadtxt(memory_arch_file, delimiter=':', dtype=int, usecols=range(1,5))
  print("Memory arch :\n",memory_arch)
  m_fast_rlat = memory_arch[0][2]
  m_fast_wlat = memory_arch[0][3]
  m_slow_rlat = memory_arch[1][2]
  m_slow_wlat = memory_arch[1][3]

  print("Fast memory read latency :",m_fast_rlat)
  print("Fast memory write latency :",m_fast_wlat)
  print("Slow memory read latency :",m_slow_rlat)
  print("Slow memory write latency :",m_slow_wlat)

  if (m_fast_rlat == m_slow_rlat) and (m_fast_wlat == m_slow_wlat):
    print("ERROR : fast and slow have same characteristics, aborting...")
    exit(2)

  print("- - - - - - - - - - - - - - - - - - - - - - - - - ")
# detecting target dataset
  if "/_d" in log_file:
    dataset = int(log_file.split("/_d")[1].split("/")[0])
    print("target dataset :", dataset)


# log parsing 
  objects_log = np.loadtxt(log_file, delimiter=';', dtype=int, converters={8: lambda s: int(s, 16)})

# objects ranking / analysis
  objects_data = np.array([[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]], dtype = float)

  for index, alloc in enumerate(objects_log) :
    # "addr;size;malloc_date_cycles;lifespan_cycles;r_count;w_count;alloc_order;free_order"

    # ADDR
    addr = alloc[0]

    # ALLOC DATE CYCLES
    alloc_date = alloc[2]

    # SIZE
    size = alloc[1]

    # NB READS
    nb_r = alloc[4]

    # NB WRITES
    nb_w = alloc[5]

    # NB READ + WRITES
    nb_rw = nb_r + nb_w
    
    # LIFESPAN
    lifespan =  alloc[3]

    free_date = alloc_date + lifespan

    # NB ALLOC DURING LIFESPAN
    after_alloc = objects_log[np.where(objects_log[:,6] > alloc[6])[0]][:,6]
    before_free = objects_log[np.where(objects_log[:,6] < alloc[7])[0]][:,6]
    overlaping_allocations = np.intersect1d(before_free, after_alloc, assume_unique=True)
    nb_alloc_lifespan = 1 + len(overlaping_allocations) # taking into account self to avoid dividing by 0
    
    if alloc[1] == 0:
      den_v3 = float(0)
    else:
      if len(memory_arch) >1:
        den_v3 = float(alloc[4]*(m_slow_rlat - m_fast_rlat)+alloc[5]*(m_slow_wlat - m_fast_wlat))/float(alloc[1]*nb_alloc_lifespan)
      else :
        den_v3 = den_v2

    # ALLOC_SITE ADDR
    alloc_site = alloc[8]
    # print "alloc_site =", hex(alloc_site)

    # is the allocation fallbacked ?
    fallback = alloc[9]

    # allocation and free order
    alloc_order = alloc[6]
    free_order = alloc[7]


    # construct data for ploting
    objects_data = np.vstack((objects_data, [ addr, 
                                              size, 
                                              alloc_date, 
                                              lifespan, 
                                              nb_r, 
                                              nb_w, 
                                              nb_rw, 
                                              0, 
                                              nb_alloc_lifespan, 
                                              0, 
                                              alloc_site, 
                                              den_v3, 
                                              fallback, 
                                              alloc_order,
                                              free_order, 
                                              free_date]))

  objects_data = np.delete(objects_data, 0,0)

  if "json_parser" in symbol_table_file:
    app = "json_parser"
  elif "dijkstra" in symbol_table_file:
    app = "dijkstra"
  elif "jpg2000" in symbol_table_file:
    app = "jpg2000"
  elif "h263" in symbol_table_file:
    app = "h263"
  elif "ecdsa" in symbol_table_file:
    app = "ecdsa"
  elif "jpeg" in symbol_table_file:
    app = "jpeg"
  else:
    print("application not recognized, see obj_analysis.py")
    sys.exit(1)

  print("application :", app)

  alloc_site_list = np.unique(objects_data[:,10]).astype(int)

  site_data = np.array([[0,0,0]], dtype=object)


  for site_addr in alloc_site_list:
    # print "alloc site = ", str(hex(site_addr))[:-1]
    pathname = os.path.dirname(sys.argv[0])  
    # print "pathname=", pathname
    # print "current dir", os.getcwd()
    try : 
      p = subprocess.Popen([pathname+'/symfinder.py', symbol_table_file, str(hex(site_addr))[:-1]], stdout=subprocess.PIPE)
    except:
      p = subprocess.Popen([os.getcwd()+'/symfinder.py', symbol_table_file, str(hex(site_addr))[:-1]], stdout=subprocess.PIPE)
      pass
    p.wait()
    if p.returncode:
      print("ERROR when retrieving symbol for address", hex(site_addr)[:-1])
      sys.exit(1)

    for line in p.stdout:
      # print "[", hex(site_addr)[:-1], "]", line
      func_name = line.split(" ")[0]
      offset = int(line.split(" ")[1][:-1])
      if "ERROR" in line:
        print("ERROR when retrieving symbol for address", str(site_addr)[:-1])
        sys.exit(1)
    
    site_data = np.vstack((site_data, [int(site_addr), func_name, offset]))

  site_data = np.delete(site_data, 0,0) 


  print("\n\nAlloc. Site \tNb Obj\t% Disp. Fast (% Fb)\t% Disp. Slow (% Fb)\tFunc Name & Offset")

  for site in site_data:
    # print "site", site
    indices = np.where(objects_data[:,10].astype(int) == int(site[0]))
    objs = objects_data[indices]
    nb_obj = len(objs)
    # print "Nb_Obj =", nb_obj
    fallback_objs = objs[np.where(objs[:,12].astype(int) == 1)]
    # print "nb fallback", len(fallback_objs)
    no_fallback_objs = objs[np.where(objs[:,12].astype(int) == 0)]

    fallback_in_fast = len(fallback_objs[np.where(fallback_objs[:,0].astype(int) < int(memory_arch[1][0]))])
    # print "fallback in fast count", fallback_in_fast
    fallback_in_slow = len(fallback_objs[np.where(fallback_objs[:,0].astype(int) >= int(memory_arch[1][0]))])
    # print "fallback in slow count", fallback_in_slow

    alloc_in_fast = len(no_fallback_objs[np.where(no_fallback_objs[:,0].astype(int) < int(memory_arch[1][0]))])
    # print "alloc in fast count", alloc_in_fast
    alloc_in_slow = len(no_fallback_objs[np.where(no_fallback_objs[:,0].astype(int) >= int(memory_arch[1][0]))])
    # print "alloc in slow count", alloc_in_slow

    fallback_in_fast_p = fallback_in_fast / float(nb_obj)*100
    fallback_in_slow_p = fallback_in_slow / float(nb_obj)*100
    dispatch_in_fast_p = (alloc_in_fast + fallback_in_slow)/float(nb_obj)*100
    dispatch_in_slow_p = (alloc_in_slow + fallback_in_fast)/float(nb_obj)*100

    print("[",site[0],"]\t"+str(nb_obj)+"\t", "%.1f" % dispatch_in_fast_p , "("+"%.1f" % fallback_in_slow_p+")\t\t", "%.1f" % dispatch_in_slow_p, "("+"%.1f" % fallback_in_fast_p+")\t\t", str(site[1])+"+"+str(hex(int(site[2])))[:-1])
 
  print("done. exiting...")
  exit(0)



if __name__ == "__main__":
    main()

