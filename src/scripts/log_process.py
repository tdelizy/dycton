#!/usr/bin/python

import os, sys, getopt, time, math

from datetime import datetime, timedelta
from decimal import *

import matplotlib.pyplot as plt
import matplotlib.colors as colors
import matplotlib.ticker as ticker
import matplotlib.patches as patches

import numpy as np

log_temporal_resolution = -1
log_spatial_resolution = -1
heap_base = -1

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



def data_cut(mem, threshold):
  ind = np.argsort(mem[:,2]); #order access by @
  cuts = []
  data = mem[ind]
  for index, access in enumerate(data[2:]):
    if data[index+1][2] - data[index][2] > threshold:
      cuts.append(index+1)
  mem_cuts = np.vsplit(data, cuts)
  return mem_cuts



def plot_scatter_RW_access(mem, amap):
  ind = np.argsort(mem[:,2]); #order access by @
  r_points =np.array([])
  w_points =np.array([])
  data = mem[ind]
  print data
  cur_addr = 0
  r_count = 0
  w_count = 0
  for index, access in enumerate(data[2:]):
    if cur_addr != access[2]:
      r_points = np.append(r_points, r_count)
      w_points = np.append(w_points,w_count)
      cur_addr = access[2]
      r_count=0
      w_count=0
    if access[1]>0:
      w_count+=1
    else:
      r_count+=1

  plt.close('all')
  f, ax = plt.subplots(1, 1)

  #plotting the graph
  ax.scatter(r_points,w_points, marker='+')

  # label work
  ax.set_xlabel('Read count', fontsize=16)
  ax.set_ylabel('Write count', fontsize=16)
  ax.set_title("R/W scatter by address")

  #show it to the world
  plt.show()

def plot_simple_alloc_boxes(allocs):
  global heap_base

  plt.close('all')
  f, ax = plt.subplots(1, 1)

  # plotting the graph
  for i, a in enumerate(allocs):
    # matplotlib.patches.Rectangle(xy, width, height, angle=0.0, **kwargs)
    ax.add_patch(patches.Rectangle((a[2], a[0] ), a[3], a[1], facecolor='steelblue', edgecolor='black',linewidth=0.1))

  #limits
  #compute the highest object top
  highest_heap_obj = allocs[np.where((allocs[:,0]+allocs[:,1]) == max(allocs[:,0]+allocs[:,1]))[0][0]]
  #compute the latest object end
  latest_heap_obj = allocs[np.where((allocs[:,2]+allocs[:,3]) == max(allocs[:,2]+allocs[:,3]))[0][0]]

  max_x_val = latest_heap_obj[2] + latest_heap_obj[3]
  max_y_val = highest_heap_obj[0] + highest_heap_obj[1]
  min_y_val = min(allocs[:,0])

  ax.set_xlim([0, max_x_val])
  ax.set_ylim([min_y_val, max_y_val])
  tick_range_y =  int(pow(16, math.floor(math.log((max_y_val - min_y_val),16))))
  tick_range_x =  int(pow(10, math.floor(math.log(max_x_val, 10)+0.5))/4)
  ax.get_yaxis().set_major_locator(ticker.MultipleLocator(tick_range_y))
  ax.get_yaxis().get_major_formatter().set_useOffset(False)
  ax.get_yaxis().set_major_formatter(ticker.FormatStrFormatter("0x%X"))
  ax.get_xaxis().set_major_locator(ticker.MultipleLocator(tick_range_x))

  # label work
  ax.set_xlabel('Time (cycles)', fontsize=16)
  ax.set_ylabel('Heap address', fontsize=16)
  ax.set_title("Heap occupation ")

  #show it to the world
  # plt.savefig('heap_occupation.png', dpi=990)
  plt.show()

  print "Heap occupation in function of time saved"
  # sys.exit(0)



def plot_scatter_RW_obj(allocs):
  print "plot_scatter_RW_obj"

  plt.close('all')
  f, ax = plt.subplots(1, 1)

  obj_reads = allocs[:,4]
  obj_writes = allocs[:,5]

  # plotting the graph
  ax.scatter(obj_reads,obj_writes, s=20, lw=0.1, color='black', alpha=0.3)

  ax.set_xlim([0, max(obj_reads)])
  ax.set_ylim([0, max(obj_writes)])

  # label work
  ax.set_xlabel("read count by object", fontsize=16)
  ax.set_ylabel("write count by object", fontsize=16)
  ax.set_title("R/W scatter by heap object with score")

  # plt.savefig('plot_scatter_read_write_object_with classes.png', dpi=660)
  #show it to the world
  plt.show()

def plot_alloc_boxes(allocs, score):
  print "plot_alloc_boxes"



  plt.close('all')
  f, ax = plt.subplots(1, 1)

  # plotting the graph
  for i, a in enumerate(allocs):
    # matplotlib.patches.Rectangle(xy, width, height, angle=0.0, **kwargs)
    ax.add_patch(patches.Rectangle((a[2], a[0] ), a[3], a[1], facecolor=(score[i], 0, 1-score[i]), edgecolor='black',linewidth=0.2))

  #limits
  #compute the highest object top
  highest_heap_obj = allocs[np.where((allocs[:,0]+allocs[:,1]) == max(allocs[:,0]+allocs[:,1]))[0][0]]
  #compute the latest object end
  latest_heap_obj = allocs[np.where((allocs[:,2]+allocs[:,3]) == max(allocs[:,2]+allocs[:,3]))[0][0]]

  max_x_val = latest_heap_obj[2] + latest_heap_obj[3]
  max_y_val = highest_heap_obj[0] + highest_heap_obj[1]
  min_y_val = min(allocs[:,0])

  ax.set_xlim([0, max_x_val])
  ax.set_ylim([min_y_val, max_y_val])
  tick_range_y =  int(pow(16, math.floor(math.log((max_y_val - min_y_val),16))))
  tick_range_x =  int(pow(10, math.floor(math.log(max_x_val, 10)+0.5))/4)
  ax.get_yaxis().set_major_locator(ticker.MultipleLocator(tick_range_y))
  ax.get_yaxis().get_major_formatter().set_useOffset(False)
  ax.get_yaxis().set_major_formatter(ticker.FormatStrFormatter("0x%X"))
  ax.get_xaxis().set_major_locator(ticker.MultipleLocator(tick_range_x))

  # label work
  ax.set_xlabel('Time (cycles)', fontsize=16)
  ax.set_ylabel('Heap address', fontsize=16)
  ax.set_title("Heap occupation with access pressure")

  # plt.savefig('heap_occupation_with_preasure.png', dpi=990)
  #show it to the world
  plt.show()
  # sys.exit(0)


def plot_selective_boxes(objects, scores) :
  plt.close('all')
  f, ax = plt.subplots(1, 1)

  # we want the 10% objects with the highest score
  # we can't assume the scores to be normalized
  score_percentage_threshold = 0.5
  print "threshold =", score_percentage_threshold

  ordered_scores = scores[np.argsort(scores)];

  score_threshold_val = ordered_scores[int(round(len(ordered_scores)*score_percentage_threshold))]
  print "score_threshold_val=", score_threshold_val
  idx_score = np.where(scores > score_threshold_val)
  print "idx_score", idx_score
  print "selected scores", scores[idx_score]

  print "len scores", len(scores)
  print "len selec",  len(scores[idx_score])

  print "max =", max(scores[idx_score])
  print "min =", min(scores[idx_score])




  size_percentage_threshold = 0.9

  ordered_sizes = objects[np.argsort(objects[:,1])]

  size_threshold_val = ordered_sizes[int(round(len(ordered_sizes)*size_percentage_threshold))][1]
  print "size_threshold_val=", size_threshold_val
  idx_size = np.where(objects[:,1] > size_threshold_val)
  print "idx_size", idx_size
  print "selected objects", objects[idx_size]

  print "len objects", len(objects)
  print "len selec",  len(objects[idx_size])

  print "max =", max(objects[idx_size][:,1])
  print "min =", min(objects[idx_size][:,1])

  # plotting the graph
  for i, a in enumerate(objects):
    # matplotlib.patches.Rectangle(xy, width, height, angle=0.0, **kwargs)
    # if i in idx_size[0] and i in idx_score[0]:
    #   ax.add_patch(patches.Rectangle((a[2], a[0] ), a[3], a[1], facecolor='red', edgecolor='none'))
    # elif i in idx_size[0]:
    #   ax.add_patch(patches.Rectangle((a[2], a[0] ), a[3], a[1], facecolor='green', edgecolor='none'))
    if i in idx_score[0]:
      ax.add_patch(patches.Rectangle((a[2], a[0] ), a[3], a[1], facecolor='red', edgecolor='none'))
    else:
      ax.add_patch(patches.Rectangle((a[2], a[0] ), a[3], a[1], facecolor='grey', edgecolor='none',alpha=0.5))


  #limits
  #compute the highest object top
  highest_heap_obj = objects[np.where((objects[:,0]+objects[:,1]) == max(objects[:,0]+objects[:,1]))[0][0]]
  #compute the latest object end
  latest_heap_obj = objects[np.where((objects[:,2]+objects[:,3]) == max(objects[:,2]+objects[:,3]))[0][0]]

  max_x_val = latest_heap_obj[2] + latest_heap_obj[3]
  max_y_val = highest_heap_obj[0] + highest_heap_obj[1]
  min_y_val = min(objects[:,0])

  ax.set_xlim([0, max_x_val])
  ax.set_ylim([min_y_val, max_y_val])
  tick_range_y =  int(pow(16, math.floor(math.log((max_y_val - min_y_val),16))))
  tick_range_x =  int(pow(10, math.floor(math.log(max_x_val, 10)+0.5))/4)
  ax.get_yaxis().set_major_locator(ticker.MultipleLocator(tick_range_y))
  ax.get_yaxis().get_major_formatter().set_useOffset(False)
  ax.get_yaxis().set_major_formatter(ticker.FormatStrFormatter("0x%X"))
  ax.get_xaxis().set_major_locator(ticker.MultipleLocator(tick_range_x))

  # label work
  ax.set_xlabel('Time (cycles)', fontsize=16)
  ax.set_ylabel('Heap address', fontsize=16)
  ax.set_title("Heap occupation with access pressure")

  # plt.savefig('heap_occupation_with_preasure.png', dpi=990)
  #show it to the world
  plt.show()
  # sys.exit(0)

def plot_alloc_boxes_with_access(allocs, mem, score):
  global log_temporal_resolution
  global log_spatial_resolution
  global heap_base
  print "plot_alloc_boxes_with_access"

  obj_access_count = np.array([])
  obj_lifespan = np.array([])
  obj_size = np.array([])
  obj_score = np.array([])

  plt.close('all')
  f, ax = plt.subplots(1, 1)

  # plotting the graph
  for i, a in enumerate(allocs):
    # matplotlib.patches.Rectangle(xy, width, height, angle=0.0, **kwargs)
    # ax.add_patch(patches.Rectangle((a[2], a[0] ), a[3], a[1], facecolor=(score[i], 0, 1-score[i], 0.5), edgecolor='black', linewidth=0.2))
    ax.add_patch(patches.Rectangle((a[2], a[0] ), a[3], a[1], facecolor='none', edgecolor=(0,0,0,1), linewidth=1))


  print "log temporal resoltion =", log_temporal_resolution, "cycles"
  print "log spatial resoltion =", log_spatial_resolution, "bytes"

  #plot accesses on top of allocations
  for i, line in enumerate(mem):
    if line[1]==0 and line[2]==0:
        c = 'white'
    if line[1] > 0 and line[2] > 0:
        c = 'purple'
    elif line[2] > 0:
        c = 'red'
    else:
        c = 'blue'
    # print "box color =", box_color
    # matplotlib.patches.Rectangle(xy, width, height, angle=0.0, **kwargs)
    ax.add_patch(patches.Rectangle((line[0], line[3] ), log_temporal_resolution, log_spatial_resolution, facecolor=c, edgecolor='none'))

  #limits
  #compute the highest object top
  highest_heap_obj = allocs[np.where((allocs[:,0]+allocs[:,1]) == max(allocs[:,0]+allocs[:,1]))[0][0]]
  #compute the latest object end
  latest_heap_obj = allocs[np.where((allocs[:,2]+allocs[:,3]) == max(allocs[:,2]+allocs[:,3]))[0][0]]

  max_x_val = latest_heap_obj[2] + latest_heap_obj[3]
  max_y_val = highest_heap_obj[0] + highest_heap_obj[1]
  min_y_val = min(allocs[:,0])

  ax.set_xlim([0, max_x_val])
  ax.set_ylim([min_y_val, max_y_val])
  tick_range_y =  int(pow(16, math.floor(math.log((max_y_val - min_y_val),16))))
  tick_range_x =  int(pow(10, math.floor(math.log(max_x_val, 10)+0.5))/4)
  ax.get_yaxis().set_major_locator(ticker.MultipleLocator(tick_range_y))
  ax.get_yaxis().get_major_formatter().set_useOffset(False)
  ax.get_yaxis().set_major_formatter(ticker.FormatStrFormatter("0x%X"))
  ax.get_xaxis().set_major_locator(ticker.MultipleLocator(tick_range_x))

  # label work
  ax.set_xlabel('simulated time (cycles)', fontsize=16)
  ax.set_ylabel('heap address', fontsize=16)
  ax.set_title("Heap occupation and accesses")
  # ax.legend(handles, labels)

  # plt.savefig('heap_occupation_with_accesses.png', dpi=990)
  #show it to the world
  plt.show()



# def plot_alloc_hist(allocs, mem, amap):
#   print "plot_alloc_hist"
#   print "memory accesses data RAW:"
#   print mem
#   # print allocs

#   obj_access_count = np.array([])
#   obj_lifespan = np.array([])
#   obj_size = np.array([])
#   obj_score = np.array([])

#   # computing access count to each object
#   for i, a in enumerate(allocs):
#     ac = len(np.where((mem[:,2]>=a[0]) & (mem[:,2]<=a[2]) & (mem[:,0]>=a[1]) & (mem[:,0]<=a[3]))[0])
#     ls =  a[3]-a[1]
#     sz = a[2]-a[0]
#     # selecting memory accesses with @ inside the allocated memory zone during object lifetime
#     if ls != 0 and sz != 0:
#       obj_access_count = np.append(obj_access_count, ac)
#       obj_lifespan = np.append(obj_lifespan, ls)
#       obj_size = np.append(obj_size, sz)
#       # print ">> [", i, "]\t", obj_access_count[i], "\t\t", obj_lifespan[i], "\t\t", obj_size[i], "\t\t", obj_access_count[i] / (obj_lifespan[i]*obj_size[i])
#     else:
#       print "alloc ignored (ac=", ac,", ls=", ls, "sz=", sz, ")"
#       obj_access_count = np.append(obj_access_count, 0)
#       obj_lifespan = np.append(obj_lifespan, 0)
#       obj_size = np.append(obj_size, 0)

#   for i in range(len(obj_access_count)):
#     obj_score = np.append(obj_score, obj_access_count[i]/( obj_lifespan[i]*obj_size[i] ))


#   obj_score_nrm = np.divide(obj_score, max(obj_score))

#   # object class separation computations
#   obj_nb = len(obj_access_count)
#   low_sep = obj_nb/3
#   high_sep = obj_nb*2/3

#   print "obj count =", obj_nb
#   print "low sep =", low_sep
#   print "high sep =", high_sep

#   partition = np.argpartition(obj_score_nrm, (low_sep, high_sep))
#   idx_low = partition[range(0,low_sep)]
#   idx_mid = partition[range(low_sep, high_sep)]
#   idx_high = partition[range(high_sep, obj_nb)]

#   obj_score_nrm_low = obj_score_nrm[idx_low]
#   obj_score_nrm_mid = obj_score_nrm[idx_mid]
#   obj_score_nrm_high = obj_score_nrm[idx_high]

#   partition = np.argpartition(obj_access_count, (low_sep, high_sep))
#   idx_low = partition[range(0,low_sep)]
#   idx_mid = partition[range(low_sep, high_sep)]
#   idx_high = partition[range(high_sep, obj_nb)]

#   obj_access_count_low = obj_access_count[idx_low]
#   obj_access_count_mid = obj_access_count[idx_mid]
#   obj_access_count_high = obj_access_count[idx_high]

#   print "len(obj_access_count_low) =", len(obj_access_count_low)
#   print "len(obj_access_count_mid) =", len(obj_access_count_mid)
#   print "len(obj_access_count_high) =", len(obj_access_count_high)

#   partition = np.argpartition(obj_lifespan, (low_sep, high_sep))
#   idx_low = partition[range(0,low_sep)]
#   idx_mid = partition[range(low_sep, high_sep)]
#   idx_high = partition[range(high_sep, obj_nb)]

#   obj_lifespan_low = obj_lifespan[idx_low]
#   obj_lifespan_mid = obj_lifespan[idx_mid]
#   obj_lifespan_high = obj_lifespan[idx_high]

#   partition = np.argpartition(obj_size, (low_sep, high_sep))
#   idx_low = partition[range(0,low_sep)]
#   idx_mid = partition[range(low_sep, high_sep)]
#   idx_high = partition[range(high_sep, obj_nb)]

#   obj_size_low = obj_size[idx_low]
#   obj_size_mid = obj_size[idx_mid]
#   obj_size_high = obj_size[idx_high]


#   plt.close('all')
#   f, ax = plt.subplots(4, 1)

#   colors = ['red', 'tan', 'lime']
#   #create legend
#   handles = [patches.Rectangle((0,0),1,1,color=c,ec="k") for c in colors]
#   labels= ["1/3 lowest scores","1/3 medium scores", "1/3 highest scores"]

#   ax[0].hist([obj_score_nrm_low, obj_score_nrm_mid, obj_score_nrm_high], 30, histtype='bar',stacked=True, color=colors)
#   ax[0].set_xlabel('normalized score', fontsize=16)
#   ax[0].set_ylabel('count', fontsize=16)
#   ax[0].set_title("object score histogram")
#   ax[0].legend(handles, labels)

#   ax[1].hist([obj_access_count_low, obj_access_count_mid, obj_access_count_high], 30, histtype='bar',stacked=True, color=colors)
#   ax[1].set_xlabel('access counts', fontsize=16)
#   ax[1].set_ylabel('count', fontsize=16)
#   ax[1].set_title("access histogram")
#   ax[1].legend(handles, labels)

#   ax[2].hist([obj_lifespan_low, obj_lifespan_mid, obj_lifespan_high], 30, histtype='bar',stacked=True, color=colors)
#   ax[2].set_xlabel('lifespans (cycles)', fontsize=16)
#   ax[2].set_ylabel('count', fontsize=16)
#   ax[2].set_title("lifespan histogram")
#   ax[2].legend(handles, labels)

#   ax[3].hist([obj_size_low, obj_size_mid, obj_size_high], 30, histtype='bar',stacked=True, color=colors)
#   ax[3].set_xlabel('sizes (Bytes)', fontsize=16)
#   ax[3].set_ylabel('count', fontsize=16)
#   ax[3].set_title("size histogram")
#   ax[3].legend(handles, labels)


#   # plt.savefig('object_histograms.png', dpi=660)
#   #show it to the world
#   plt.show()
#   # sys.exit(0)


def main():
  global log_temporal_resolution
  global log_spatial_resolution
  global heap_base
  # command line arguments processing
  try:
    opts, args = getopt.getopt(sys.argv[1:], "hV:", ["help"])
  except getopt.GetoptError as err:# print help information and exit:
    print str(err)  # will print something like "option -a not recognized"
    sys.exit(2)

  verbose = False

  for o, a in opts:
    #print "option=", o, " arg=", a
    if o == "-V":
      print "script last modification : ", time.strftime('%m/%d/%Y-%H:%M', time.localtime(os.path.getmtime(sys.argv[0])))
      verbose = True
    elif o in ("-h", "--help"):
      usage()
      sys.exit()
    else:
      print "unhandled option :", o, a
      sys.exit(2)

  # parsing
  if verbose:
    print "=================================="
    print "parsing files names"
    print "=================================="
    print "args = ", args
  for f in args:
    if verbose:
      print "considering argument", f
    if os.path.basename(f) == "mem_access.log":
        mem_accesses_file_name = f
    elif os.path.basename(f) == "memory_architecture":
      mem_arch_file_name = f
    elif os.path.basename(f) == "heap_objects.log":
      alloc_log_file_name = f
    elif verbose:
      print "what is", f, "? (ignored)"

  if verbose:
    print "=================================="
    print "constructing platform address map"
    print "=================================="
  mem_arch = []
  addr_map_file = open(mem_arch_file_name,'r')
  addr_map_lines = addr_map_file.readlines()
  addr_map_file.close()
  for line in addr_map_lines:
    mem_arch.append(Memory_range(line.split(":")[0], int(line.split(":")[1]), int(line.split(":")[2])))
    if verbose:
      print mem_arch[-1].name, mem_arch[-1].offset, mem_arch[-1].size

  heap_base = mem_arch[0].offset

  if heap_base == -1:
    print "unable to locate segment \"heap\" in address map file, exiting."
    exit(2)
  elif verbose:
      print "heap base set at", heap_base

  if verbose:
    print "=================================="
    print "parsing allocation log"
    print "=================================="
  # sorted by request date (generated in that order)
  alloc_log = np.loadtxt(alloc_log_file_name, delimiter=';', dtype=int)

  objects = np.array([[0,0,0,0,0,0]])

  for index, alloc in enumerate(alloc_log) :
    # gather allocation infos
    ad = alloc[0]
    size = alloc[1]
    malloc_date = alloc[2]
    lifespan = alloc[3]
    r_count = alloc[4]
    w_count = alloc[5]
    # construct allocated object descriptor
    obj_desc = np.array( [ad, size, malloc_date, lifespan, r_count, w_count], dtype=int)
    # add to array
    objects = np.vstack((objects, obj_desc))
  # substracting heap base to addresses for readability
  objects[:,0]-= heap_base
  objects = np.delete(objects, 0, 0)
  # order objects by allocation date
  ind = np.argsort(objects[:,2]);
  objects = objects[ind]

  # compute application memory footprint
  highest_heap_obj = objects[np.where((objects[:,0]+objects[:,1]) == max(objects[:,0]+objects[:,1]))[0][0]]
  app_mem_footprint = highest_heap_obj[0] + highest_heap_obj[1]
  print "FOOTPRINT=", app_mem_footprint

  if verbose:
    print "alloc log :"
    print objects

    print "=================================="
    print "score computation"
    print "=================================="
  obj_score = np.array([])
  obj_score_nrm = np.array([])

  for a in objects:
    if(a[1]*a[3])!=0:
      obj_score = np.append(obj_score,float(a[4]+a[5])/float(a[1]*a[3]))
    else:
      if verbose:
        print "object error :", a
      obj_score = np.append(obj_score,float(0))
      # exit(2)

  obj_score_nrm = np.divide(obj_score, max(obj_score))
  if verbose:
    print "=================================="
    print "parsing memory accesses file"
    print "=================================="
  # header_extraction = open(mem_accesses_file_name,'r')
  # first_lines = []
  # buf = header_extraction.readline()
  # while buf[0]=="#" and len(first_lines)<15:
  #   first_lines.append(buf)
  #   buf = header_extraction.readline()
  # log_temporal_resolution = first_lines[1].split('=')[1]
  # log_spatial_resolution = first_lines[2].split('=')[1]
  # if verbose:
  #   print "log temporal resoltion =", log_temporal_resolution, "cycles"
  #   print "log spatial resoltion =", log_spatial_resolution, "bytes"
  # mem_access = np.loadtxt(mem_accesses_file_name, delimiter=';', dtype=int) #should be faster
  # # substracting heap base to the accesses for readability
  # mem_access[:,3]-= heap_base

  if verbose:
    print "=================================="
    print "basic stats extractions"
    print "=================================="
    print "simulated time run duration (cycles):", mem_access[-1][0]
    print "program R/W ratio: (TODO)"


    print "=================================="
    print "plotting"
    print "=================================="
  # plot_scatter_RW_obj(objects)
  plot_simple_alloc_boxes(objects)
  # plot_selective_boxes(objects, obj_score_nrm)
  # plot_alloc_boxes(objects, obj_score_nrm)
  # plot_alloc_boxes_with_access(objects, mem_access, obj_score_nrm)

  # plot_alloc_hist(allocations, mem_access, address_map)



  sys.exit(app_mem_footprint)




if __name__ == "__main__":
    main()

