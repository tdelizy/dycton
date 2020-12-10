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

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
from __future__ import print_function
import os, sys, getopt, time, math, subprocess

from datetime import datetime, timedelta
from decimal import *

import matplotlib.pyplot as plt
import matplotlib.colors as colors
import matplotlib.ticker as ticker
import matplotlib.patches as patches
from matplotlib.colors import LinearSegmentedColormap
import matplotlib as mpl

import numpy as np

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
log_temporal_resolution = -1
log_spatial_resolution = -1
heap_base = -1
save_name = "heap_occupation.png"

class Memory_range:
  def __init__(self, name, offset, size, rlat, wlat):
    self.name = name
    self.offset = int(offset)
    self.size = int(size)
    self.rlat = int(rlat)
    self.wlat = int(wlat)


# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
def usage():
  print("usage : provide execution log files as arguments\n")
  print("options:")
  print("\t-t app : allow to specify target application \n\t (in jpg2000, ecdsa, h263, json_parser, dijkstra and jpeg)")
  print("\t-s symbol table file : will generate a color map by allocation site to color objects")
  print("\t-c : plot the occupation of heap without objects hotness information display")
  print("\t-h : this")
  print("\t-V : verbose, prints more")
  print("\nscript for plotting the heap occupation (memory space in function of time) of an execution")
  print("\nexample usage (from repository iss folder, where you just ran a simulation):")
  print("  python ../../scripts/log_process.py ../logs/*")


# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
def to_hex(x, pos):
    return '0x%x' % int(x)


# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
def plot_simple_alloc_boxes(allocs, mem_arch):
  global save_name
  plt.close('all')
  f, ax = plt.subplots(1, 2, sharey=True, gridspec_kw = {'width_ratios':[7, 1]},figsize=(10, 10))
  # plotting the graph
  # [addr, size, alloc_date, lifespan, nb_r, nb_w, nb_rw, den_v1, nb_alloc_lifespan, den_v2, alloc_site, den_v3]
  for i, a in enumerate(allocs):
    ax[0].add_patch(patches.Rectangle((a[2], a[0] ), a[3], a[1], facecolor='lightsteelblue', edgecolor='black',linewidth=0.1))

  #limits
  #compute the highest object top
  highest_heap_obj = allocs[np.where((allocs[:,0]+allocs[:,1]) == max(allocs[:,0]+allocs[:,1]))[0][0]]
  #compute the latest object end
  latest_heap_obj = allocs[np.where((allocs[:,2]+allocs[:,3]) == max(allocs[:,2]+allocs[:,3]))[0][0]]

  max_x_val = max(allocs[:,15])
  max_y_val = highest_heap_obj[0] + highest_heap_obj[1]
  min_y_val = min(allocs[:,0])

  # generating memory zones separation lines and legend
  for i, m in enumerate(mem_arch):
    y_label = min((m.offset+m.size), max_y_val)-50
    description = m.name + "\n  R lat. " + str(m.rlat)+ "\n  W lat. " + str(m.wlat)
    ax[0].plot((0,max_x_val),(m.offset,m.offset), linestyle='--', color='black')
    if(i != len(mem_arch)-1):
      ax[1].add_patch(patches.Rectangle((0, m.offset), 1, (mem_arch[i+1].offset-m.offset), facecolor='white', edgecolor='black'))
    else:
      ax[1].add_patch(patches.Rectangle((0, m.offset), 1, m.size, facecolor='white', edgecolor='black'))
    ax[1].patch.set_facecolor('grey')
    ax[1].text(0.03, y_label, description, fontsize=10, verticalalignment='top')
    ax[1].set_xticks([])
    ax[1].set_yticks([])

  ax[0].set_xlim([0, max_x_val])
  ax[0].set_ylim([min_y_val, max_y_val])
  tick_range_y =  int(pow(16, math.floor(math.log((max_y_val - min_y_val)/2,16))))
  tick_range_x =  int(pow(10, math.floor(math.log(max_x_val, 10)+0.5))/4)
  ax[0].get_yaxis().set_major_locator(ticker.MultipleLocator(tick_range_y))
  ax[0].get_yaxis().get_major_formatter().set_useOffset(False)
  fmt = ticker.FuncFormatter(to_hex)
  ax[0].get_yaxis().set_major_formatter(fmt)
  ax[0].get_xaxis().set_major_locator(ticker.MultipleLocator(tick_range_x))
  # label work
  ax[0].set_xlabel('Time (cycles)', fontsize=16)
  ax[0].set_ylabel('Heap address', fontsize=16)

  #show it to the world
  plt.savefig(save_name, dpi=480)
  plt.show()


# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
def get_cmap(n, name='nipy_spectral_r'):
    '''Returns a function that maps each index in 0, 1, ..., n-1 to a distinct
    RGB color; the keyword argument name must be a standard mpl colormap name.'''
    return plt.cm.get_cmap(name, n)

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
def plot_alloc_boxes_by_site(allocs, mem_arch, symbol_table_file):
  global save_name

  plt.close('all')
  f, ax = plt.subplots(1, 2, sharey=True, gridspec_kw = {'width_ratios':[7, 1]},figsize=(15, 10))

  cmap_size = len(np.unique(allocs[:,10]))
  print("cmap_size =", cmap_size)
  cmap_site_map = {}

  print(np.unique(allocs[:,10]).argsort())
  print(np.unique(allocs[:,10])[np.unique(allocs[:,10]).argsort()])

  for i, site_addr in enumerate(np.unique(allocs[:,10])[np.unique(allocs[:,10]).argsort()].astype(int)):
    print("alloc site = ", str(hex(site_addr)))
    pathname = os.path.dirname(sys.argv[0])
    print("pathname = "+pathname)
    try :
      p = subprocess.Popen([pathname+'/symfinder.py', symbol_table_file, str(hex(site_addr))], stdout=subprocess.PIPE,encoding='utf8')
    except:
      p = subprocess.Popen([os.getcwd()+'/symfinder.py', symbol_table_file, str(hex(site_addr))], stdout=subprocess.PIPE, stderr=subprocess.STDOUT,encoding='utf8')
      pass
    p.wait()
    if p.returncode:
      print("returncode :", p.returncode)
      print("ERROR when retrieving symbol for address", hex(site_addr))
      sys.exit(1)

    for line in p.stdout:
      print(line)
      func_name = line.split(" ")[0]
      offset = int(line.split(" ")[1])
      legend_text = func_name+" +"+str(hex(int(offset)))
      if legend_text[0] == "_":
        legend_text = " "+legend_text
      if "ERROR" in line:
        print("ERROR when retrieving symbol for address", str(site_addr))
        sys.exit(1)

    cmap_site_map[site_addr] = {"index": i, "legend_text": legend_text}

  my_cmap = get_cmap(cmap_size);
  lp = lambda i: patches.Patch(facecolor=my_cmap(cmap_site_map[i]["index"]), edgecolor='b', label=cmap_site_map[i]["legend_text"])
  handles = [lp(i) for i in np.unique(allocs[:,10])[np.unique(allocs[:,10]).argsort()]]

  # plotting the graph
  for i, a in enumerate(allocs):
    ax[0].add_patch(patches.Rectangle((a[2], a[0] ), a[3], a[1], facecolor=my_cmap(cmap_site_map[a[10]]["index"]), edgecolor='black',linewidth=0.2))

  #limits
  #compute the highest object top
  highest_heap_obj = allocs[np.where((allocs[:,0]+allocs[:,1]) == max(allocs[:,0]+allocs[:,1]))[0][0]]
  max_x_val = max(allocs[:,15])
  max_y_val = highest_heap_obj[0] + highest_heap_obj[1]
  min_y_val = min(allocs[:,0])

  # generating memory zones separation lines and legend
  for i, m in enumerate(mem_arch):
    y_label = min((m.offset+m.size), max_y_val)-50
    description = m.name + "\n  R lat. " + str(m.rlat)+ "\n  W lat. " + str(m.wlat)
    ax[0].plot((0,max_x_val),(m.offset,m.offset), linestyle='--', color='black')
    if(i != len(mem_arch)-1):
      ax[1].add_patch(patches.Rectangle((0, m.offset), 1, (mem_arch[i+1].offset-m.offset), facecolor='white', edgecolor='black'))
    else:
      ax[1].add_patch(patches.Rectangle((0, m.offset), 1, m.size, facecolor='white', edgecolor='black'))
    ax[1].patch.set_facecolor('grey')
    ax[1].text(0.03, y_label, description, fontsize=10, verticalalignment='top')
    ax[1].set_xticks([])
    ax[1].set_yticks([])

  ax[0].set_xlim([0, max_x_val])
  ax[0].set_ylim([min_y_val, max_y_val])
  tick_range_y =  int(pow(16, math.floor(math.log((max_y_val - min_y_val)/2,16))))
  tick_range_x =  int(pow(10, math.floor(math.log(max_x_val, 10)+0.5))/4)
  ax[0].get_yaxis().set_major_locator(ticker.MultipleLocator(tick_range_y))
  ax[0].get_yaxis().get_major_formatter().set_useOffset(False)
  fmt = ticker.FuncFormatter(to_hex)
  ax[0].get_yaxis().set_major_formatter(fmt)
  ax[0].get_xaxis().set_major_locator(ticker.MultipleLocator(tick_range_x))
  # label work
  ax[0].set_xlabel('Time (cycles)', fontsize=16)
  ax[0].set_ylabel('Heap address', fontsize=16)
  ax[0].set_title("Heap occupation ")
  plt.legend(handles=handles[0:10], loc='center left', bbox_to_anchor=(1, 0.5), fontsize=14)

  #show it to the world
  f.tight_layout()
  f.subplots_adjust(wspace=0, right = 0.7)
  plt.savefig(save_name, dpi=330)
  plt.show()


# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
def plot_alloc_boxes(allocs, mem_arch):
  global save_name
  print("plot_alloc_boxes")
  plt.close('all')
  f, ax = plt.subplots(2, 2, gridspec_kw = {'width_ratios':[20, 5],'height_ratios':[20, 1]}, sharey='row', figsize=(12, 10))
  f.delaxes(ax[1,1])
  max_den = np.max(allocs[:,11], axis=0)
  log_score = np.log(1+allocs[:,11])
  min_log_score = log_score.min()
  max_log_score = log_score.max()
  b = 0.8 #color base factor

  # plotting the graph
  for i, a in enumerate(allocs):
    c = (log_score[i]-min_log_score)/(max_log_score-min_log_score)
    if a[12] == 0:
      ax[0,0].add_patch(patches.Rectangle((a[2], a[0] ), a[3], a[1], facecolor=(b*(1-c),b*(1-c),b+0.1), edgecolor='none'))
    else:
      ax[0,0].add_patch(patches.Rectangle((a[2], a[0] ), a[3], a[1], facecolor=(b+0.1,b*(1-c),b*(1-c)), edgecolor='none'))

  #limits
  #compute the highest object top
  highest_heap_obj = allocs[np.where((allocs[:,0]+allocs[:,1]) == max(allocs[:,0]+allocs[:,1]))[0][0]]
  max_x_val = int(max(allocs[:,15]))
  max_y_val = int(highest_heap_obj[0] + highest_heap_obj[1])
  min_y_val = int(mem_arch[0].offset)

  # generating memory zones separation lines and legend
  for i, m in enumerate(mem_arch):
    y_label = min((m.offset+m.size), max_y_val)-50
    description = m.name + "\n  R lat. " + str(m.rlat)+ "  W lat. " + str(m.wlat)
    ax[0,0].plot((0,max_x_val),(m.offset,m.offset), linestyle='--', color='black')
    if(i != len(mem_arch)-1):
      ax[0,1].add_patch(patches.Rectangle((0, m.offset), 1, (mem_arch[i+1].offset-m.offset), facecolor='white', edgecolor='black'))
    else:
      ax[0,1].add_patch(patches.Rectangle((0, m.offset), 1, m.size, facecolor='white', edgecolor='black'))
    ax[0,1].patch.set_facecolor('grey')
    ax[0,1].text(0.03, y_label, description, fontsize=12, verticalalignment='top')
    ax[0,1].set_xticks([])
    ax[0,1].set_yticks([])

  tick_range_y =  int(pow(16, math.floor(math.log((max_y_val - min_y_val)/2,16))))
  tick_range_x =  int(pow(10, math.floor(math.log(max_x_val, 10)+0.5))/4)
  ax[0,0].get_yaxis().set_major_locator(ticker.MultipleLocator(tick_range_y))
  ax[0,0].get_yaxis().get_major_formatter().set_useOffset(False)
  fmt = ticker.FuncFormatter(to_hex)
  ax[0,0].get_yaxis().set_major_formatter(fmt)
  ax[0,0].get_xaxis().set_major_locator(ticker.MultipleLocator(tick_range_x))
  # label work
  ax[0,0].set_xlabel('Time (cycles)', fontsize=16)
  ax[0,0].set_ylabel('Heap address', fontsize=16)

  ax[1,1].set_xticks([])
  ax[1,1].set_yticks([])
  ax[1,0].set_xticks([])
  ax[1,0].set_yticks([])
  cdict1 = {'red':  ((0.0, 1.0, 1.0),
                    (1.0, 0.0, 0.0)),
            'green':((0.0, 1.0, 1.0),
                    (1.0, 0.0, 0.0)),
            'blue': ((0.0, 1.0, 1.0),
                    (1.0, 0.9, 0.9))
        }

  cmap_grad_test = LinearSegmentedColormap('cmap_grad_test', cdict1)
  plt.register_cmap(cmap=cmap_grad_test)

  ax[0,0].set_xlim([0, max_x_val])
  ax[0,0].set_ylim([min_y_val, max_y_val])

  norm = mpl.colors.Normalize(vmin=0, vmax=max_x_val)
  grad_freq =  mpl.colorbar.ColorbarBase(ax[1,0], norm = norm, ticks=[], cmap=cmap_grad_test, orientation='horizontal')
  ax[1,0].set_xlabel("less frequency per byte -> more frequency per byte", fontsize=12)

  parch_ok = patches.Patch(facecolor=(0,0,1), edgecolor='b', label="Allocation in \ndestination heap")
  parch_fallback = patches.Patch(facecolor=(1,0,0), edgecolor='b', label="Fallback to \nthe other heap")

  handles = [parch_ok, parch_fallback]
  ax[1,0].legend(handles=handles, loc='center left', bbox_to_anchor=(1, 0.5), fontsize=12)

  #show it to the world
  plt.savefig(save_name, dpi=480)
  f.tight_layout()
  f.subplots_adjust(wspace=0, right = 1)
  plt.show()


# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
def main():
  global log_temporal_resolution
  global log_spatial_resolution
  global heap_base
  global save_name
  nocolor = False
  alloc_site_info = False
  app = ""
  symbol_table_file = ""

  # command line arguments processing
  try:
    opts, args = getopt.getopt(sys.argv[1:], 's:t:hVc', ['help'])
  except getopt.GetoptError as err:# print help information and exit:
    print(str(err))  # will print something like "option -a not recognized"
    sys.exit(2)

  verbose = False

  for o, a in opts:
    # print "option=", o, " arg=", a
    if o == "-V":
      print("script last modification : ", time.strftime('%m/%d/%Y-%H:%M', time.localtime(os.path.getmtime(sys.argv[0]))))
      verbose = True
    elif o == "-t":
      app = a
    elif o in ("-h", "--help"):
      usage()
      sys.exit()
    elif o == "-c":
      nocolor = True
    elif o == "-s":
      alloc_site_info = True
      symbol_table_file = a
    else:
      print("unhandled option :", o, a)
      sys.exit(2)

  print("nocolor = ", nocolor)

  # parsing
  if verbose:
    print("==================================")
    print("parsing files names")
    print("==================================")
    print("args = ", args)
  for f in args:
    if verbose:
      print("considering argument", f)
    if os.path.basename(f) == "mem_access.log":
        mem_accesses_file_name = f
    elif os.path.basename(f) == "memory_architecture":
      mem_arch_file_name = f
    elif os.path.basename(f) == "heap_objects.log":
      alloc_log_file_name = f
    elif verbose:
      print("what is", f, "? (ignored)")

  if verbose:
    print("==================================")
    print("constructing platform address map")
    print("==================================")
# loading memory architecture latencies
  memory_arch = np.loadtxt(mem_arch_file_name, delimiter=':', dtype=object)
  print(memory_arch)

  path = os.path.dirname(os.path.abspath(alloc_log_file_name))
  print("path :", path)
  if "jpg2000" in path or "jpeg2000" in path:
    app = "jpg2000"
  elif "ecdsa" in path:
    app = "ecdsa"
  elif "h263" in path:
    app = "h263"
  elif "json_parser" in path:
    app = "json_parser"
  elif "dijkstra" in path:
    app = "dijkstra"
  elif "jpeg" in path:
    app = "jpeg"
  if alloc_site_info == True and app == "":
    path += os.path.dirname(os.path.abspath(symbol_table_file))
    if "jpg2000" in path or "jpeg2000" in path:
      app = "jpg2000"
    elif "ecdsa" in path:
      app = "ecdsa"
    elif "h263" in path:
      app = "h263"
    elif "json_parser" in path:
      app = "json_parser"
    elif "dijkstra" in path:
      app = "dijkstra"
    elif "jpeg" in path:
      app = "jpeg"
  try:
    print("path.split(app)[-1]", path.split(app)[-1])
    print("")
    print("path.split(app)[-1].split(_run)[-1]", path.split(app)[-1].split("_run")[0])
    print("")
    if nocolor == True:
      save_name = app + "_heap_usage_no_freq.pdf"
    elif alloc_site_info == True:
      save_name = app + "_heap_usage_alloc_site.pdf"
    else:
      save_name = app + "_heap_usage.pdf"
  except:
    save_name = "heap_usage.pdf"

  print("figure name =", save_name)
  mem_arch = []
  addr_map_file = open(mem_arch_file_name,'r')
  addr_map_lines = addr_map_file.readlines()
  addr_map_file.close()
  if len(addr_map_lines) > 1:
    for i, line in enumerate(addr_map_lines):
      mem_arch.append(Memory_range(memory_arch[i][0], memory_arch[i][1], memory_arch[i][2], memory_arch[i][3], memory_arch[i][4]))
      if verbose:
        print(mem_arch[-1].name, mem_arch[-1].offset, mem_arch[-1].size, mem_arch[-1].rlat, mem_arch[-1].wlat)
  else:
    print("memarch[0]")
    print(memory_arch)
    mem_arch.append(Memory_range(memory_arch[0], memory_arch[1], memory_arch[2], memory_arch[3], memory_arch[4]))

  heap_base = mem_arch[0].offset

  if heap_base == -1:
    print("unable to locate segment \"heap\" in address map file, exiting.")
    exit(2)
  elif verbose:
      print("heap base set at", heap_base)

  if verbose:
    print("==================================")
    print("parsing allocation log")
    print("==================================")
# log parsing
  objects_log = np.loadtxt(alloc_log_file_name, delimiter=';', dtype=int, converters={8: lambda s: int(s, 16)})

# objects ranking / analysis
  objects_data = np.array([[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]], dtype = float)

  avg_den = 0

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
    # /!\ OUTDATED
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

    # /!\ OUTDATED
    # DENSITY V2 (alloction overlap as time)
    if alloc[1] == 0:
      den_v2 = float(0)
    else:
      den_v2 = float(alloc[4]+alloc[5])/float(alloc[1]*nb_alloc_lifespan)

    # DENSITY V3 (R/W sensitive in function of memory technologies characteristics)
    if alloc[1] == 0:
      den_v3 = float(0)
    else:
      if len(mem_arch) >1:
        den_v3 = float(alloc[4]*(mem_arch[1].rlat - mem_arch[0].rlat)+alloc[5]*(mem_arch[1].wlat - mem_arch[0].wlat))/float(alloc[1]*nb_alloc_lifespan)
      else :
        den_v3 = den_v2

    avg_den += den_v3

    # ALLOC_SITE ADDR
    alloc_site = alloc[8]
    # print "alloc_site =", hex(alloc_site)

    # is the allocation fallbacked ?
    fallback = alloc[9]

    # allocation and free order
    alloc_order = alloc[6]
    free_order = alloc[7]


    # construct data for ploting
    objects_data = np.vstack((objects_data, [ 	addr,
    											size,
    											alloc_date,
    											lifespan,
    											nb_r,
    											nb_w,
    											nb_rw,
    											den_v1,
    											nb_alloc_lifespan,
    											den_v2,
    											alloc_site,
    											den_v3,
    											fallback,
    											alloc_order,
    											free_order,
                                            free_date]))

  objects_data = np.delete(objects_data, 0,0)

  avg_den = float(avg_den)/len(objects_data)

  print("average object density of the application :", avg_den)
  print("caution : this is related to memory technologies latencies.")

  print("objects_data length =", len(objects_data))

  if verbose:
    print("==================================")
    print("plotting")
    print("==================================")
  if nocolor == True:
    plot_simple_alloc_boxes(objects_data, mem_arch)
  elif alloc_site_info == True:
    plot_alloc_boxes_by_site(objects_data, mem_arch, symbol_table_file)
  else:
    plot_alloc_boxes(objects_data, mem_arch)
  sys.exit(0)



# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
if __name__ == "__main__":
    main()
