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
  print("-r: result synthesis, only plot access frequency per byte summary graphs")
  print("-p: profile run, generate profile, do not print graphs")
  print("-i \"filepath\": input heap objects log file path")
  print("-s \"filepath\": symbol table file path")
  print("-m \"filepath\": memory architecture description file path")
  print("")
  print("This script takes a reference execution as input (heap objects log and symbol table file)")
  print("and estimate hotness of objects using access frequency per byte.")
  print("From there it can plot this information correlated to objects size or allocation site")
  print("this computation depends on targetted memory technologies so we also need a memory description file")
  print("This script is also used with the -p option to generate profile for experiments")
  print("exiting...")




def main():
  global app
  global dataset

  date_start = str(datetime.datetime.now())

  print("ploting execution objects features versus different metrics")
  print("==================================================")
  print("author: T. Delizy (delizy.tristan@gmail.com)")
  print(date_start)
  profile_run = False
  result_run = False
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
    elif o in ("-p"):
      profile_run = True
      print("profile run.")
    elif o in ("-r"):
      result_run = True
      print("result synthesis run.")
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
  objects_data = np.array([[0,0,0,0,0,0,0,0,0,0]], dtype = float)

  for index, alloc in enumerate(objects_log) :
    # "addr;size;malloc_date_cycles;lifespan_cycles;r_count;w_count;alloc_order;free_order"

    # SIZE
    size = alloc[1]

    # NB READS
    nb_r = alloc[4]

    # NB WRITES
    nb_w = alloc[5]

    # NB READ + WRITES
    nb_rw = nb_r + nb_w
    
    # LIFESPAN
    lifespan =  alloc[3]*10**-6

    # DENSITY V1 (allocation lifespan as time)
    if alloc[1] == 0 or alloc[3] == 0:
      den_v1 = float(0)
    else:
      den_v1 = float(alloc[4]+alloc[5])/float(alloc[1]*alloc[3]*10**-6)

    # NB ALLOC DURING LIFESPAN
    after_alloc = objects_log[np.where(objects_log[:,6] > alloc[6])[0]][:,6]
    before_free = objects_log[np.where(objects_log[:,6] < alloc[7])[0]][:,6]
    overlaping_allocations = np.intersect1d(before_free, after_alloc, assume_unique=True)
    nb_alloc_lifespan = 1 + len(overlaping_allocations) # taking into account self to avoid dividing by 0
    
    # DENSITY V2 (alloction overlap as time)
    if alloc[1] == 0:
      den_v2 = float(0)
    else:
      den_v2 = float(alloc[4]+alloc[5])/float(alloc[1]*nb_alloc_lifespan)

    # DENSITY V2 CONSIDERING ONLY READS
    # if alloc[1] == 0:
    #   den_v2 = float(0)
    # else:
    #   den_v2 = float(alloc[4])/float(alloc[1]*nb_alloc_lifespan)

    # DENSITY V2 CONSIDERING ONLY WRITES
    # if alloc[1] == 0:
    #   den_v2 = float(0)
    # else:
    #   den_v2 = float(alloc[5])/float(alloc[1]*nb_alloc_lifespan)

    # DENSITY V3 (R/W sensitive in function of memory technologies characteristics)
    if alloc[1] == 0:
      den_v3 = float(0)
    else:
      den_v3 = float(alloc[4]*(m_slow_rlat - m_fast_rlat)+alloc[5]*(m_slow_wlat - m_fast_wlat))/float(alloc[1]*nb_alloc_lifespan)


    # ALLOC_SITE ADDR
    alloc_site = alloc[8]
    # print "alloc_site =", hex(alloc_site)

    # construct data for ploting
    if size > 0:
      objects_data = np.vstack((objects_data, [size, nb_r, nb_w, nb_rw, lifespan, den_v1, nb_alloc_lifespan, den_v2, alloc_site, den_v3]))

  objects_data = np.delete(objects_data, 0,0)

  # plot configuration
  features = ["size", "alloc_site", "all"]
  feature = "all"

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

  print("app =", app)

  alloc_site_list = np.unique(objects_data[:,8]).astype(int)

  big_step_x = 1.5
  small_step_x = 1
  site_x = 0

  site_data = np.array([[0,0,0,0]], dtype=object)

  previous_symbol = "00init_script"

  print("alloc_site_list", end=' ')
  print(alloc_site_list)

  for site_addr in alloc_site_list:
    print("alloc site = ", str(hex(site_addr))[:-1])
    pathname = os.path.dirname(sys.argv[0])  
    print("pathname=", pathname)
    print("current dir", os.getcwd())
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
      print("[", hex(site_addr)[:-1], "]", line)
      func_name = line.split(" ")[0]
      offset = int(line.split(" ")[1][:-1])
      if "ERROR" in line:
        print("ERROR when retrieving symbol for address", str(site_addr)[:-1])
        sys.exit(1)
    if previous_symbol == "00init_script":
      site_x = 1
      previous_symbol = func_name
    else:
      if previous_symbol == func_name:
        site_x += small_step_x
      else:
        site_x += big_step_x
        previous_symbol = func_name
    
    site_data = np.vstack((site_data, [int(site_addr), site_x, func_name, offset]))

  site_data = np.delete(site_data, 0,0) 

  if profile_run == False:

    if result_run:
      plot_synthesis_size(objects_data, site_data)
      plot_synthesis_alloc_site(objects_data, site_data, small_step_x, big_step_x)

    else:

      if feature != "alloc_site":
        plot_size_feature(objects_data)
    
      if feature != "size":
        plot_site_feature(objects_data, site_data, small_step_x, big_step_x)

  else:
    compute_profile(objects_data, site_data)

  print("done. exiting...")
  exit(0)





def plot_synthesis_size(objects_data, site_data):
  global app
  global dataset

  size_bar_freq_access = np.zeros(40)
  hist_size = np.zeros(40, dtype = int)

  max_x = max(objects_data[:,0])
  print("max", max_x)
  max_pow = math.ceil(math.log10(max_x))
  print("max_pow", max_pow)
  
  x_scale = np.logspace(0, max_pow, num=41)
  x_scale_diff = np.diff(np.logspace(0, max_pow, num=41))


  # np.vstack((objects_data, [size, nb_r, nb_w, nb_rw, lifespan, den_v1, nb_alloc_lifespan, den_v2, alloc_site, den_v3]))
  prev = 0
  for obj in objects_data:
    inds = np.argwhere(x_scale>obj[0])
    size_bar_freq_access[inds[0][0]-1]+=obj[9]
    if obj[0]> max_x/10:
      print("size", obj[0])
      print("target class", inds[0][0]-1)
      s = 0
      for test in range(inds[0][0]-1):
        s += x_scale_diff[test]
      print("somme en x", s)
    hist_size[inds[0][0]-1]+=1
    if hist_size[-1] > prev:
      print("last size class increase (", prev, "->", hist_size[-1], ")")
      print("target object")
      print(obj)
      prev = hist_size[-1]

  print(hist_size)

  for i in range(len(hist_size)):
      if hist_size[i] != 0 and size_bar_freq_access[i]!=0:
          size_bar_freq_access[i] = float(size_bar_freq_access[i])/float(hist_size[i])


  low = len(size_bar_freq_access)
  high = 0
  for i, size_class_val in enumerate(hist_size):
    if size_class_val > 0:
      if i < low:
        low = i
      if i > high:
        high = i

  plt.close('all')
  f, ax = plt.subplots(3, 1, sharex=True, figsize=(10, 5))
  f.subplots_adjust(hspace=0.1, top=0.95, bottom=0.1, left=0.18,right=0.98)

  data = objects_data[:,7]
  ax[0].grid(axis='x', which='major', linestyle='--', color='dimgrey')
  ax[0].scatter(objects_data[:,0],objects_data[:,7],s=50, marker='x', alpha=0.3, color='black')
  ax[0].set_ylabel("Objects\nAccess\nfreq/byte", rotation=0, horizontalalignment='right', verticalalignment='center')
  ax[0].set_yscale('symlog')
  ax[0].set_xscale('symlog')
  ax[0].set_ylim([0,10**math.ceil(math.log10(np.max(data)))])

  ax[1].grid(axis='x', which='major', linestyle='--', color='dimgrey')
  ax[1].bar(x_scale[:-1], size_bar_freq_access, log=True, width=x_scale_diff, ec="k", align="edge", facecolor='lightsteelblue')
  ax[1].set_ylabel("Access\nfreq/byte\n(average on\nsize class)", rotation=0, horizontalalignment='right', verticalalignment='center')
  ax[1].set_yscale('symlog')
  ax[1].set_xscale('symlog')
  ax[1].set_ylim([0,10**math.ceil(math.log10(np.max(size_bar_freq_access)))]) 



  data = hist_size
  ax[2].grid(axis='x', which='major', linestyle='--', color='dimgrey')
  ax[2].bar(x_scale[:-1], data, log=True, width=x_scale_diff, ec="k", align="edge", facecolor='lightsteelblue')
  ax[2].set_ylabel("Object count\nby size class", rotation=0, horizontalalignment='right', verticalalignment='center')
  ax[2].set_yscale('symlog')
  ax[2].set_xscale('symlog')
  ax[2].set_ylim([0,10**math.ceil(math.log10(np.max(data)))])  
  ax[2].set_xlabel("Size (Bytes)")

  max_class = min(high+2, len(x_scale)-1)
  min_class = max(low-1, 0)
  ax[2].set_xlim([x_scale[min_class],x_scale[max_class]])

  if dataset != -1:
    ax[0].set_title("Objects analysis by size classes ("+app+", dataset "+str(dataset)+")")
  else:
    ax[0].set_title("Objects analysis by size classes ("+app+")")

  if dataset != -1:
    plt.savefig(app + "_d" + str(dataset) + "_size_feature_synthesis.pdf", dpi=330)

  else:
    plt.savefig(app + "_size_feature_synthesis.pdf", dpi=330)
  
  # show it to the world
  # plt.show()


def plot_synthesis_alloc_site(objects_data, site_data, small_step_x, big_step_x):
  global app
  global dataset


  fn_pos = []
  fn_group = []
  for fn in np.unique(site_data[:,2]):
    same_fn_sites = site_data[np.where(site_data[:,2] == fn)]
    fn_pos.append(min(same_fn_sites[:,1].astype(float))+(max(same_fn_sites[:,1].astype(float)) - min(same_fn_sites[:,1].astype(float)))/2.0)
    if len(same_fn_sites) > 1:
      fn_group.append((max(same_fn_sites[:,1].astype(float)) - min(same_fn_sites[:,1].astype(float)))/2.0)
    else:
      fn_group.append(0)

  max_x = max(objects_data[:,0])
  
  x_ticks = []
  x_ticks_values = []

  w = small_step_x
  x_bar = site_data[:,1].astype(float)

  alloc_site_count = len(np.unique(objects_data[:,8]))

  scatter_x = np.zeros(len(objects_data))

  bar_access_freq_per_byte = np.zeros(alloc_site_count)

  hist_alloc = np.zeros(alloc_site_count)

  # size, nb_r, nb_w, nb_rw, lifespan, den_v1, nb_alloc, den_v2
  for i, site in enumerate(site_data):
    indices = np.where(objects_data[:,8].astype(int) == int(site[0]))
    objs = objects_data[indices]
    
    scatter_x[indices] = float(site[1])

    x_ticks.append(site[3])
    x_ticks_values.append(float(site[1]))

    for o in objs:
      bar_access_freq_per_byte[i]+=o[7]
      
      hist_alloc[i]+=1


  hot_fix_by_app = []
  if app == "h263":
    hot_fix_by_app.append(0.00003)
    hot_fix_by_app.append(1)
  elif app == "jpeg":
    hot_fix_by_app.append(0.02)
    hot_fix_by_app.append(1)
  elif app == "json_parser":
    hot_fix_by_app.append(0.0007)
    hot_fix_by_app.append(0.982)
  elif app == "jpg2000":
    hot_fix_by_app.append(0.0002)
    hot_fix_by_app.append(1)
  elif app == "dijkstra":
    hot_fix_by_app.append(0.000025)
    hot_fix_by_app.append(1)
  if app == "ecdsa":
    hot_fix_by_app.append(0.00003)
    hot_fix_by_app.append(1)



  for i in range(len(hist_alloc)):
      if hist_alloc[i] != 0 and bar_access_freq_per_byte[i]!=0:
          bar_access_freq_per_byte[i] = float(bar_access_freq_per_byte[i])/float(hist_alloc[i])


  plt.close('all')
  f, ax = plt.subplots(3, 1, sharex=True, figsize=(10, 7.5))
  f.subplots_adjust(hspace=0.1, top=0.95, bottom=0.3, left=0.18,right=0.98)


  data = objects_data[:,7]
  ax[0].scatter(scatter_x,data,s=40, marker='x', alpha=0.5, color='black')
  ax[0].set_ylabel("Access\nfreq/byte", rotation=0, horizontalalignment='right', verticalalignment='center')
  ax[0].set_yscale('symlog')
  ax[0].set_ylim([0,10**math.ceil(math.log10(np.max(data)))  ])

  ax[1].bar(x_bar, bar_access_freq_per_byte, width=w, ec="k", align="center", facecolor='lightsteelblue')
  ax[1].set_ylabel("Access\nfreq/byte\n(average\non alloc site)", rotation=0, horizontalalignment='right', verticalalignment='center')
  ax[1].set_yscale('symlog')
  ax[1].set_ylim([0,10**math.ceil(math.log10(np.max(bar_access_freq_per_byte)))])  


  data = hist_alloc
  ax[2].bar(x_bar, data, width=w, ec="k", align="center", facecolor='lightsteelblue')
  ax[2].set_ylabel("Object count\nby allocation\nsite", rotation=0, horizontalalignment='right', verticalalignment='center')
  ax[2].set_yscale('symlog')
  ax[2].set_ylim([0,10**math.ceil(math.log10(np.max(data)))])  
  ax[2].set_xlim([0,np.max(scatter_x)+small_step_x])


  plt.xticks(x_ticks_values)
  plt.setp(ax[2].get_xticklabels(), visible=False)

  pos = ax[2].get_position()
  for i, fn in enumerate(np.unique(site_data[:,2])):
    f.text(pos.x0+float(fn_pos[i]/(max(fn_pos)+small_step_x)*(pos.width)*hot_fix_by_app[1]), pos.y0-0.015, fn, fontsize=10, horizontalalignment='center', verticalalignment='top', rotation=90)
    if fn_group[i] != 0:
      ax[2].plot([float(fn_pos[i]-fn_group[i]*1.1),float(fn_pos[i]+fn_group[i]*1.1)],[-1*hot_fix_by_app[0]*max(data),-1*hot_fix_by_app[0]*max(data)], color='black', clip_on=False, lw=1)



  if dataset != -1:
    ax[0].set_title("Objects analysis by allocation site ("+app+", dataset "+str(dataset)+")")
  else:
    ax[0].set_title("Objects analysis by allocation site ("+app+")")
  


  if dataset != -1:
    plt.savefig(app + '_d' + str(dataset) + '_alloc_site_feature_synthesis.pdf', dpi=330)

  else:
    plt.savefig(app + '_alloc_site_feature_synthesis.pdf', dpi=330)


  # show it to the world
  # plt.show()



def compute_profile(objects_data, site_data):
  global app
  profile = np.zeros((len(site_data), 4), dtype=object)
  
  profile[:,0] = site_data[:,0]
  profile[:,2] = site_data[:,2]


  total_allocated_bytes = np.sum(objects_data[:,0])
  cumul = 0

  median = np.median(objects_data[:,9])
  print("objects frequency per byte median value =", median)
  print("total allocated bytes =", total_allocated_bytes)

  for site in profile:
    indices = np.where(objects_data[:,8].astype(int) == int(site[0]))
    objs = objects_data[indices]

    # metric = average density V3 for objects by site
    site[1] = np.sum(objs[:,9])/float(len(objs))

    # proportion of allocated bytes of total for site (in %)
    site[3] = np.sum(objs[:,0])/float(total_allocated_bytes)*100

    cumul += site[3]
    print("cumul % bytes allocated =", cumul)
  
  profile = profile[profile[:, 1].astype(float).argsort()]

  print("profile:")
  # print profile

  with open(app+"_alloc_site.profile", "w") as outfile:
    for line in reversed(profile):
      print(line)
      # logline = line[0]+"\n"
      logline = str(line[0])+"\t"+str(line[1])+"\t"+str(line[3])+"\t("+line[2]+")\n"
      outfile.write("%s" % logline )
  



def plot_size_feature(objects_data):
  global app
  size_bar_nb_r = np.zeros(40)
  size_bar_nb_w = np.zeros(40)
  size_bar_nb_rw = np.zeros(40)
  size_bar_lifespan = np.zeros(40)
  size_bar_nb_alloc = np.zeros(40)
  size_bar_den_v1 = np.zeros(40)
  size_bar_den_v2 = np.zeros(40)

  hist_size = np.zeros(40)

  max_x = max(objects_data[:,0])
  max_pow = math.ceil(math.log10(max_x))  
  
  x_scale = np.logspace(0, max_pow, num=41)
  x_scale_diff = np.diff(np.logspace(0, max_pow, num=41))

  # size, nb_r, nb_w, nb_rw, lifespan, den_v1, nb_alloc, den_v2
  for obj in objects_data:
  
    inds = np.argwhere(x_scale[:-1]>obj[0])
    size_bar_nb_r[inds[0][0]-1]+=obj[1]
    size_bar_nb_w[inds[0][0]-1]+=obj[2]
    size_bar_nb_rw[inds[0][0]-1]+=obj[3]
    size_bar_lifespan[inds[0][0]-1]+=obj[4]
    size_bar_nb_alloc[inds[0][0]-1]+=obj[6]
    size_bar_den_v1[inds[0][0]-1]+=obj[5]
    size_bar_den_v2[inds[0][0]-1]+=obj[7]

    hist_size[inds[0][0]-1]+=1


  for i in range(len(hist_size)):
      if hist_size[i] != 0 and size_bar_den_v1[i]!=0:
          print("[", i, "]count =", hist_size[i], "| den_v1 (cumul) =", size_bar_den_v1[i], "|avg =", float(size_bar_den_v1[i])/float(hist_size[i]))
          size_bar_den_v1[i] = float(size_bar_den_v1[i])/float(hist_size[i])
          size_bar_den_v2[i] = float(size_bar_den_v2[i])/float(hist_size[i])
      elif hist_size[i] + size_bar_den_v1[i] !=0:
          print("[", i, "]count =", hist_size[i], "| den_v1 (cumul) =", size_bar_den_v1[i], "PROBLEEEEEEM")
      else:
          print("[", i, "]count =", hist_size[i])

  low = len(size_bar_nb_rw)
  high = 0
  for i, size_class_val in enumerate(hist_size):
    if size_class_val > 0:
      if i < low:
        low = i
      if i > high:
        high = i


  # plot the graphs
  plt.close('all')
  # f, ax = plt.subplots(1, 1)
  
  f, ax = plt.subplots(8, 1, sharex=True, figsize=(10, 10))
  f.subplots_adjust(hspace=0.1, top=0.95, bottom=0.1, left=0.18,right=0.98)
  
  # ax.scatter(objects_data[:,0],objects_data[:,4], marker='+')
  # ax.set_ylabel("Read\nCount")
  
  data = objects_data[:,1]
  ax[0].scatter(objects_data[:,0],data,s=50, marker='x', alpha=0.5)
  ax[0].set_ylabel("Read\nCount", rotation=0, horizontalalignment='right', verticalalignment='center')
  ax[0].set_yscale('symlog')
  ax[0].set_xscale('symlog')
  ax[0].set_ylim([0,10**math.ceil(math.log10(np.max(data)))])
  
  data = objects_data[:,2]
  ax[1].scatter(objects_data[:,0],data,s=50, marker='x', alpha=0.5)
  ax[1].set_ylabel("Write\nCount", rotation=0, horizontalalignment='right', verticalalignment='center')
  ax[1].set_yscale('symlog')
  ax[1].set_xscale('symlog')
  ax[1].set_ylim([0,10**math.ceil(math.log10(np.max(data)))])
  
  data = objects_data[:,3]
  ax[2].scatter(objects_data[:,0],data,s=50, marker='x', alpha=0.5)
  ax[2].set_ylabel("Access\nCount", rotation=0, horizontalalignment='right', verticalalignment='center')
  ax[2].set_yscale('symlog')
  ax[2].set_xscale('symlog')
  ax[2].set_ylim([0,10**math.ceil(math.log10(np.max(data)))])
  
  data = objects_data[:,4]
  ax[3].scatter(objects_data[:,0],data,s=50, marker='x', alpha=0.5)
  ax[3].set_ylabel("Lifespan\n(10^6 Cycles)", rotation=0, horizontalalignment='right', verticalalignment='center')
  ax[3].set_yscale('symlog')
  ax[3].set_xscale('symlog')
  ax[3].set_ylim([0,10**math.ceil(math.log10(np.max(data)))])
  
  
  data = objects_data[:,6]
  ax[4].scatter(objects_data[:,0],data,s=50, marker='x', alpha=0.5)
  ax[4].set_ylabel("Allocation Count\nDuring Lifespan\n", rotation=0, horizontalalignment='right', verticalalignment='center')
  ax[4].set_yscale('symlog')
  ax[4].set_xscale('symlog')
  ax[4].set_ylim([0,10**math.ceil(math.log10(np.max(data)))])
  
  data = objects_data[:,5]
  ax[5].scatter(objects_data[:,0],data,s=50, marker='x', alpha=0.5)
  ax[5].set_ylabel("Density v1", rotation=0, horizontalalignment='right', verticalalignment='center')
  ax[5].set_yscale('symlog')
  ax[5].set_xscale('symlog')
  ax[5].set_ylim([0,10**math.ceil(math.log10(np.max(data)))])

  
  data = objects_data[:,7]
  ax[6].scatter(objects_data[:,0],objects_data[:,7],s=50, marker='x', alpha=0.5)
  ax[6].set_ylabel("Density v2", rotation=0, horizontalalignment='right', verticalalignment='center')
  ax[6].set_yscale('symlog')
  ax[6].set_xscale('symlog')
  ax[6].set_ylim([0,10**math.ceil(math.log10(np.max(data)))])
  
  data = hist_size
  ax[7].bar(x_scale[:-1], data, log=True, width=x_scale_diff, ec="k", align="edge")
  ax[7].set_ylabel("Histogram", rotation=0, horizontalalignment='right', verticalalignment='center')
  ax[7].set_yscale('symlog')
  ax[7].set_xscale('symlog')
  ax[7].set_ylim([0,10**math.ceil(math.log10(np.max(data)))])  
  ax[7].set_xlabel("Size (Bytes)")

  ax[7].set_xlim([x_scale[low-1],x_scale[high+2]])

  ax[0].set_title("Objects in function of Size")
  
  # show it to the world
  plt.show()
  plt.savefig(app+'size_scatter.png', dpi=330)


  # plot the graphs
  plt.close('all')
  f, ax = plt.subplots(7, 1, sharex=True, figsize=(10, 10))
  f.subplots_adjust(hspace=0.1, top=0.95, bottom=0.1, left=0.18,right=0.98)

  max_x = max(objects_data[:,0])

  ax[0].bar(x_scale[:-1], size_bar_nb_r, log=True, width=x_scale_diff, ec="k", align="edge")
  ax[0].set_ylabel("Read Count\n(Sum on Size Class)", rotation=0, horizontalalignment='right', verticalalignment='center')
  ax[0].set_yscale('symlog')
  ax[0].set_xscale('symlog')
  ax[0].set_ylim([0,10**math.ceil(math.log10(np.max(size_bar_nb_r)))])  
  
  ax[1].bar(x_scale[:-1], size_bar_nb_w, log=True, width=x_scale_diff, ec="k", align="edge")
  ax[1].set_ylabel("Write Count\n(Sum on Size Class)", rotation=0, horizontalalignment='right', verticalalignment='center')
  ax[1].set_yscale('symlog')
  ax[1].set_xscale('symlog')
  ax[1].set_ylim([0,10**math.ceil(math.log10(np.max(size_bar_nb_w)))])  
  
  ax[2].bar(x_scale[:-1], size_bar_nb_rw, log=True, width=x_scale_diff, ec="k", align="edge")
  ax[2].set_ylabel("Access Count\n(Sum on Size Class)", rotation=0, horizontalalignment='right', verticalalignment='center')
  ax[2].set_yscale('symlog')
  ax[2].set_xscale('symlog')
  ax[2].set_ylim([0,10**math.ceil(math.log10(np.max(size_bar_nb_rw)))])  
  
  ax[3].bar(x_scale[:-1], size_bar_lifespan, log=True, width=x_scale_diff, ec="k", align="edge")
  ax[3].set_ylabel("Lifespan\n(10^6 Cycles\n(Sum on Size Class)", rotation=0, horizontalalignment='right', verticalalignment='center')
  ax[3].set_yscale('symlog')
  ax[3].set_xscale('symlog')
  ax[3].set_ylim([0,10**math.ceil(math.log10(np.max(size_bar_lifespan)))])  
  
  ax[4].bar(x_scale[:-1], size_bar_nb_alloc, log=True, width=x_scale_diff, ec="k", align="edge")
  ax[4].set_ylabel("Allocation Count\nDuring Lifespan\n(Sum on Size Class)", rotation=0, horizontalalignment='right', verticalalignment='center')
  ax[4].set_yscale('symlog')
  ax[4].set_xscale('symlog')
  ax[4].set_ylim([0,10**math.ceil(math.log10(np.max(size_bar_nb_alloc)))])  
  
  ax[5].bar(x_scale[:-1], size_bar_den_v1, log=True, width=x_scale_diff, ec="k", align="edge")
  ax[5].set_ylabel("Density v1\n(Average on\nSize Class)", rotation=0, horizontalalignment='right', verticalalignment='center')
  ax[5].set_yscale('symlog')
  ax[5].set_xscale('symlog')
  ax[5].set_ylim([0,10**math.ceil(math.log10(np.max(size_bar_den_v1)))])  
  
  ax[6].bar(x_scale[:-1], size_bar_den_v2, log=True, width=x_scale_diff, ec="k", align="edge")
  ax[6].set_ylabel("Density v2\n(Average on\nSize Class)", rotation=0, horizontalalignment='right', verticalalignment='center')
  ax[6].set_yscale('symlog')
  ax[6].set_xscale('symlog')
  ax[6].set_ylim([0,10**math.ceil(math.log10(np.max(size_bar_den_v2)))])  

  ax[6].set_xlabel("Size (Bytes)")
  ax[6].set_xlim([x_scale[low-1],x_scale[high+2]])
  
  ax[0].set_title("Objects in function of Size")

  #show it to the world
  plt.show()
  plt.savefig(app+'size_bar.png', dpi=330)



def plot_site_feature(objects_data, site_data, small_step_x, big_step_x):
  global app
  fn_pos = []
  fn_group = []
  for fn in np.unique(site_data[:,2]):
    same_fn_sites = site_data[np.where(site_data[:,2] == fn)]
    fn_pos.append(min(same_fn_sites[:,1].astype(float))+(max(same_fn_sites[:,1].astype(float)) - min(same_fn_sites[:,1].astype(float)))/2.0)
    if len(same_fn_sites) > 1:
      fn_group.append((max(same_fn_sites[:,1].astype(float)) - min(same_fn_sites[:,1].astype(float)))/2.0)
    else:
      fn_group.append(0)

  max_x = max(objects_data[:,0])
  
  x_ticks = []
  x_ticks_values = []

  w = small_step_x
  x_bar = site_data[:,1].astype(float)

  alloc_site_count = len(np.unique(objects_data[:,8]))

  scatter_x = np.zeros(len(objects_data))

  bar_nb_r = np.zeros(alloc_site_count)
  bar_nb_w = np.zeros(alloc_site_count)
  bar_nb_rw = np.zeros(alloc_site_count)
  bar_lifespan = np.zeros(alloc_site_count)
  bar_nb_alloc = np.zeros(alloc_site_count)
  bar_den_v1 = np.zeros(alloc_site_count)
  bar_den_v2 = np.zeros(alloc_site_count)

  hist_alloc = np.zeros(alloc_site_count)

  # size, nb_r, nb_w, nb_rw, lifespan, den_v1, nb_alloc, den_v2
  for i, site in enumerate(site_data):
    indices = np.where(objects_data[:,8].astype(int) == int(site[0]))
    objs = objects_data[indices]
    
    scatter_x[indices] = float(site[1])

    x_ticks.append(site[3])
    x_ticks_values.append(float(site[1]))

    for o in objs:
      bar_nb_r[i]+=o[1]
      bar_nb_w[i]+=o[2]
      bar_nb_rw[i]+=o[3]
      bar_lifespan[i]+=o[4]
      bar_nb_alloc[i]+=o[6]
      bar_den_v1[i]+=o[5]
      bar_den_v2[i]+=o[7]
      
      hist_alloc[i]+=1


  hot_fix_by_app = []
  if app == "h263":
    hot_fix_by_app.append(0.00003)
    hot_fix_by_app.append(0.004)
  elif app == "jpeg":
    hot_fix_by_app.append(0.02)
    hot_fix_by_app.append(0.02)
  elif app == "json_parser":
    hot_fix_by_app.append(0.001)
    hot_fix_by_app.append(0.022)
  elif app == "jpg2000":
    hot_fix_by_app.append(0.0002)
    hot_fix_by_app.append(0.004)
  elif app == "dijkstra":
    hot_fix_by_app.append(0.00005)
    hot_fix_by_app.append(0.1)
  if app == "ecdsa":
    hot_fix_by_app.append(0.00003)
    hot_fix_by_app.append(0.004)


  for i in range(len(hist_alloc)):
      if hist_alloc[i] != 0 and bar_den_v1[i]!=0:
          bar_den_v1[i] = float(bar_den_v1[i])/float(hist_alloc[i])
          bar_den_v2[i] = float(bar_den_v2[i])/float(hist_alloc[i])

  # plot the graphs
  plt.close('all')
  f, ax = plt.subplots(8, 1, sharex=True, figsize=(10, 10))
  f.subplots_adjust(hspace=0.1, top=0.95, bottom=0.25, left=0.18,right=0.98)

  max_x = max(objects_data[:,0])

  data = objects_data[:,1]
  ax[0].scatter(scatter_x,data,s=40, marker='x', alpha=0.5)
  ax[0].set_ylabel("Read\nCount", rotation=0, horizontalalignment='right', verticalalignment='center')
  ax[0].set_yscale('symlog')
  ax[0].set_ylim([0,10**math.ceil(math.log10(np.max(data)))  ])
  
  data = objects_data[:,2]
  ax[1].scatter(scatter_x,data,s=40, marker='x', alpha=0.5)
  ax[1].set_ylabel("Write\nCount", rotation=0, horizontalalignment='right', verticalalignment='center')
  ax[1].set_yscale('symlog')
  ax[1].set_ylim([0,10**math.ceil(math.log10(np.max(data)))  ])
  
  data = objects_data[:,3]
  ax[2].scatter(scatter_x,data,s=40, marker='x', alpha=0.5)
  ax[2].set_ylabel("Access\nCount", rotation=0, horizontalalignment='right', verticalalignment='center')
  ax[2].set_yscale('symlog')
  ax[2].set_ylim([0,10**math.ceil(math.log10(np.max(data)))  ])
  
  data = objects_data[:,4]
  ax[3].scatter(scatter_x,data,s=40, marker='x', alpha=0.5)
  ax[3].set_ylabel("Lifespan\n(10^6 Cycles)", rotation=0, horizontalalignment='right', verticalalignment='center')
  ax[3].set_yscale('symlog')
  ax[3].set_ylim([0,10**math.ceil(math.log10(np.max(data)))  ])
  
  data = objects_data[:,6]
  ax[4].scatter(scatter_x,data,s=40, marker='x', alpha=0.5)
  ax[4].set_ylabel("Allocation Count\nDuring Lifespan\n", rotation=0, horizontalalignment='right', verticalalignment='center')
  ax[4].set_yscale('symlog')
  ax[4].set_ylim([0,10**math.ceil(math.log10(np.max(data)))  ])
  
  data = objects_data[:,5]
  ax[5].scatter(scatter_x,data,s=40, marker='x', alpha=0.5)
  ax[5].set_ylabel("Density v1", rotation=0, horizontalalignment='right', verticalalignment='center')
  ax[5].set_yscale('symlog')
  ax[5].set_ylim([0,10**math.ceil(math.log10(np.max(data)))  ])

  data = objects_data[:,7]
  ax[6].scatter(scatter_x,data,s=40, marker='x', alpha=0.5)
  ax[6].set_ylabel("Density v2", rotation=0, horizontalalignment='right', verticalalignment='center')
  ax[6].set_yscale('symlog')
  ax[6].set_ylim([0,10**math.ceil(math.log10(np.max(data)))  ])

  data = hist_alloc
  ax[7].bar(x_bar, data, width=w, ec="k", align="center")
  ax[7].set_ylabel("histogram", rotation=0, horizontalalignment='right', verticalalignment='center')
  ax[7].set_yscale('symlog')
  ax[7].set_ylim([0,10**math.ceil(math.log10(np.max(data)))  ])

  ax[7].set_ylim([0,10**math.ceil(math.log10(np.max(data)))  ])  
  ax[7].set_xlim([0,np.max(scatter_x)+small_step_x])
  # ax[6].set_xlabel("allocation site")

  ax[0].set_title("Objects in function of Allocation Site")


  
  plt.xticks(x_ticks_values)
  plt.setp(ax[7].get_xticklabels(), visible=False)

  pos = ax[7].get_position()
  for i, fn in enumerate(np.unique(site_data[:,2])):
    f.text(pos.x0+float(fn_pos[i]/(max(fn_pos)+small_step_x)*(pos.width)), pos.y0-0.015, fn, fontsize=10, horizontalalignment='center', verticalalignment='top', rotation=90)
    if fn_group[i] != 0:
      ax[7].plot([float(fn_pos[i]-fn_group[i]*1.1),float(fn_pos[i]+fn_group[i]*1.1)],[-1*hot_fix_by_app[0]*max(data),-1*hot_fix_by_app[0]*max(data)], color='black', clip_on=False, lw=1)

  # show it to the world
  plt.show()
  plt.savefig(app+'alloc_site_scatter.png', dpi=330)

  # plot the graphs
  plt.close('all')
  # f, ax = plt.subplots(1, 1)
  
  f, ax = plt.subplots(7, 1, sharex=True, figsize=(10, 10))
  f.subplots_adjust(hspace=0.1, top=0.95, bottom=0.25, left=0.18,right=0.98)

  # w = (max(objects_data[:,8]) - min(objects_data[:,8]))/(len(alloc_site_list)*2)

  
  ax[0].bar(x_bar, bar_nb_r, width=w, ec="k", align="center")
  ax[0].set_ylabel("Read Count\n(Sum on Alloc Site)", rotation=0, horizontalalignment='right', verticalalignment='center')
  ax[0].set_yscale('symlog')
  ax[0].set_ylim([0,10**math.ceil(math.log10(np.max(bar_nb_r)))  ])
  
  ax[1].bar(x_bar, bar_nb_w, width=w, ec="k", align="center")
  ax[1].set_ylabel("Write Count\n(Sum on Alloc Site)", rotation=0, horizontalalignment='right', verticalalignment='center')
  ax[1].set_yscale('symlog')
  ax[1].set_ylim([0,10**math.ceil(math.log10(np.max(bar_nb_w)))  ])
  
  ax[2].bar(x_bar, bar_nb_rw, width=w, ec="k", align="center")
  ax[2].set_ylabel("Access Count\n(Sum on Alloc Site)", rotation=0, horizontalalignment='right', verticalalignment='center')
  ax[2].set_yscale('symlog')
  ax[2].set_ylim([0,10**math.ceil(math.log10(np.max(bar_nb_rw)))  ])
  
  ax[3].bar(x_bar, bar_lifespan, width=w, ec="k", align="center")
  ax[3].set_ylabel("Lifespan\n(10^6 Cycles\nsum on alloc site)", rotation=0, horizontalalignment='right', verticalalignment='center')
  ax[3].set_yscale('symlog')
  ax[3].set_ylim([0,10**math.ceil(math.log10(np.max(bar_lifespan)))  ])
  
  ax[4].bar(x_bar, bar_nb_alloc, width=w, ec="k", align="center")
  ax[4].set_ylabel("Allocation Count\nDuring Lifespan\n(Sum on Alloc Site)", rotation=0, horizontalalignment='right', verticalalignment='center')
  ax[4].set_yscale('symlog')
  ax[4].set_ylim([0,10**math.ceil(math.log10(np.max(bar_nb_alloc)))  ])
  
  ax[5].bar(x_bar, bar_den_v1, width=w, ec="k", align="center")
  ax[5].set_ylabel("Density v1\n(Average on\nAlloc Site)", rotation=0, horizontalalignment='right', verticalalignment='center')
  ax[5].set_yscale('symlog')
  ax[5].set_ylim([0,10**math.ceil(math.log10(np.max(bar_den_v1)))  ])
  
  ax[6].bar(x_bar, bar_den_v2, width=w, ec="k", align="center")
  ax[6].set_ylabel("Density v2\n(Average on\nAlloc Site)", rotation=0, horizontalalignment='right', verticalalignment='center')
  ax[6].set_yscale('symlog')
  ax[6].set_ylim([0,10**math.ceil(math.log10(np.max(bar_den_v2)))  ])  

  ax[6].set_xlim([0,np.max(x_bar)+small_step_x])
  
  plt.xticks(x_ticks_values)
  plt.setp(ax[6].get_xticklabels(), visible=False)

  pos = ax[6].get_position()
  for i, fn in enumerate(np.unique(site_data[:,2])):
    f.text(pos.x0+float(fn_pos[i]/(max(fn_pos)+small_step_x)*(pos.width)), pos.y0-0.01, fn, fontsize=10, horizontalalignment='center', verticalalignment='top', rotation=90)
    if fn_group[i] != 0:
      ax[6].plot([float(fn_pos[i]-fn_group[i]*1.1),float(fn_pos[i]+fn_group[i]*1.1)],[-1*hot_fix_by_app[1]*max(bar_den_v2),-1*hot_fix_by_app[1]*max(bar_den_v2)], color='black', clip_on=False, lw=1,)


  ax[0].set_title("Objects in function of Allocation Site")

  #show it to the world
  plt.show()
  plt.savefig(app+'alloc_site_bar.png', dpi=330)






if __name__ == "__main__":
    main()

