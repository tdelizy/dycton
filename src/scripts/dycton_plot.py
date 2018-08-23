#!/usr/bin/python

import os, sys, getopt, time, math

from datetime import datetime, timedelta
from decimal import *

import matplotlib.pyplot as plt
import matplotlib.colors as colors
import matplotlib.ticker as ticker
import matplotlib.patches as patches
import matplotlib.lines as lines

import numpy as np

sys.path.append('../../')


# output format : paper or curves for general usage
# paper_output = True
paper_output = True

def robust_max(index, *args):
  to_max=[]
  for arg in args:
      try:
        to_max.append(arg[index])
      except:
        pass
  if len(to_max) == 0:
    print "ERROR: none of arguments is valid."
    sys.exit(1)
  return max(to_max)


def main():
  # command line arguments processing
  try:
    opts, args = getopt.getopt(sys.argv[0:], "", [""])
  except getopt.GetoptError as err:# print help information and exit:
    print str(err)  # will print something like "option -a not recognized"
    sys.exit(2)


  if(len(args)>1):
    print "args", args
    xp_folder = args[-1]
    res_folder = xp_folder+"offline_placements/"
  else:
    # create folder for result simulators and embedded softwares
    res_folder = "offline_placement_generation_"+date
  
  verbose = False
  input_csv = args[-1]

  try:
    import xp_config as xp
    print "import from config file OK."
  except:
    print "CONFIG IMPORT FAILED !"
    sys.exit(1)
    
  target_sw = xp.target_sw
  target_hw = xp.target_hw
  target_strats = xp.target_strats


  # construct x ticks and label
  arch_ref = [0,5,10,25,50,75,100]
  # arch_ref = [100,75,50,25,10,5,0]
  arch = []

  if paper_output:
    arch_names_ref = ["A6\n0% ","A5\n5%","A4\n  10%","A3\n25%","A2\n50%","A1\n75%","A0\n100%"]
  else:
    arch_names_ref = ["A6\n0%","A5\n5%","A4\n10%","A3\n25%","A2\n50%","A1\n75%","A0\n100%"]
  arch_names = []

  for hw in target_hw:
    arch.append(arch_ref[len(arch_ref)-int(hw)-1])
    arch_names.append(arch_names_ref[len(arch_ref)-int(hw)-1])

  print "arch = ", arch
  print "arch names = ", arch_names




  print "=================================="
  print "parsing allocation log"
  print "=================================="
  # sorted by request date (generated in that order)
  data = np.genfromtxt(input_csv, dtype=None, delimiter=';', names=True)

  # print data

  x = arch
  # target_sw = ["h263"]
  # target_sw = ["jpeg", "json_parser", "dijkstra", "jpg2000" , "h263"]



  for i, sw in enumerate(target_sw):
    print "sw=", sw
    y_speedup_baseline = []
    idx_baseline = []
    y_speedup_ilp =[]
    idx_ilp = []
    y_speedup_ilp_50p =[]
    idx_ilp_50p = []
    y_speedup_ilp_85p =[]
    idx_ilp_85p = []
    y_speedup_nofrag =[]
    idx_nofrag = []
    y_speedup_density =[]
    idx_density = []
    y_speedup_density_50p =[]
    idx_density_50p = []
    y_speedup_density_85p =[]
    idx_density_85p = []
    for j in range(len(data)):
      if data['experience'][j] == sw+"_arch6_run_baseline":
      	ref_arch6 = data[j]['final_cycles']
      	# print "REFERENCE EXEC TIME =", ref_arch6
      if sw in data['experience'][j] and "baseline" in data['experience'][j]:
      	idx_baseline.append(j)
      if sw in data['experience'][j] and "nofrag" in data['experience'][j]:
      	idx_nofrag.append(j)
      if sw in data['experience'][j] and "ilp" in data['experience'][j]:
      	idx_ilp.append(j)
      if sw in data['experience'][j] and "ilp_50" in data['experience'][j]:
      	idx_ilp_50p.append(j)
      if sw in data['experience'][j] and "ilp_85" in data['experience'][j]:
      	idx_ilp_85p.append(j)
      if sw in data['experience'][j] and "density" in data['experience'][j]:
      	idx_density.append(j)
      if sw in data['experience'][j] and "density_50" in data['experience'][j]:
      	idx_density_50p.append(j)
      if sw in data['experience'][j] and "density_85" in data['experience'][j]:
      	idx_density_85p.append(j)

    if "ilp_upper_bound" in target_strats:
      idx_ilp = [val for val in idx_ilp if val not in idx_nofrag]
    if "ilp_50" in target_strats:
      idx_ilp = [val for val in idx_ilp if val not in idx_ilp_50p]
    if "ilp_85" in target_strats:
      idx_ilp = [val for val in idx_ilp if val not in idx_ilp_85p]
    if "density_50" in target_strats:
      idx_density = [val for val in idx_density if val not in idx_density_50p]
    if "density_85" in target_strats:
      idx_density = [val for val in idx_density if val not in idx_density_85p]


    # architectures are ordered from A0 to A6, but we want to display from 0% of fast memory to 100%
    # i.e. from A6 to A0, so we reverse the lists:
    if "baseline" in target_strats:
      for t in data[idx_baseline]['final_cycles']:
        y_speedup_baseline.append((float(ref_arch6)-float(t))/float(ref_arch6)*100)
      # y_speedup_baseline = y_speedup_baseline[::-1]
    if "ilp_upper_bound" in target_strats:
      for t in data[idx_nofrag]['final_cycles']:
        y_speedup_nofrag.append((float(ref_arch6)-float(t))/float(ref_arch6)*100)
      # y_speedup_nofrag = y_speedup_nofrag[::-1]
    if "ilp" in target_strats:
      for t in data[idx_ilp]['final_cycles']:
        y_speedup_ilp.append((float(ref_arch6)-float(t))/float(ref_arch6)*100)
      # y_speedup_ilp = y_speedup_ilp[::-1]
    if "ilp_50" in target_strats:
      for t in data[idx_ilp_50p]['final_cycles']:
        y_speedup_ilp_50p.append((float(ref_arch6)-float(t))/float(ref_arch6)*100)
      # y_speedup_ilp_50p = y_speedup_ilp_50p[::-1]
    if "ilp_85" in target_strats:
      for t in data[idx_ilp_85p]['final_cycles']:
        y_speedup_ilp_85p.append((float(ref_arch6)-float(t))/float(ref_arch6)*100)
      # y_speedup_ilp_85p = y_speedup_ilp_85p[::-1]
    if "density" in target_strats:
      for t in data[idx_density]['final_cycles']:
        y_speedup_density.append((float(ref_arch6)-float(t))/float(ref_arch6)*100)
      # y_speedup_density = y_speedup_density[::-1]
    if "density_50" in target_strats:
      for t in data[idx_density_50p]['final_cycles']:
        y_speedup_density_50p.append((float(ref_arch6)-float(t))/float(ref_arch6)*100)
      # y_speedup_density_50p = y_speedup_density_50p[::-1]
    if "density_85" in target_strats:
      for t in data[idx_density_85p]['final_cycles']:
        y_speedup_density_85p.append((float(ref_arch6)-float(t))/float(ref_arch6)*100)
      # y_speedup_density_85p = y_speedup_density_85p[::-1]


    # ilp strategy as max(ilp, ilp85%, ilp50%)
    y_speedup_ilp_merged = []
    for i in range(max(len(y_speedup_ilp), max(len(y_speedup_ilp_50p), len(y_speedup_ilp_85p)))):
      y_speedup_ilp_merged.append(robust_max(i, y_speedup_ilp,y_speedup_ilp_50p,y_speedup_ilp_85p))

    # density strategy as max(density, density85%, density50%)
    y_speedup_density_merged = []
    for i in range(max(len(y_speedup_density), max(len(y_speedup_density_50p), len(y_speedup_density_85p)))):
      y_speedup_density_merged.append(robust_max(i, y_speedup_density,y_speedup_density_50p,y_speedup_density_85p))


    print "=================================="
    print "plotting"
    print "=================================="
    script_dpi = 80
    plt.close('all')

    if paper_output:
      f, ax = plt.subplots(1, 1, figsize=(500/script_dpi, 350/script_dpi), dpi=script_dpi)
      line_w = 1
    else:
      f, ax = plt.subplots(1, 1, figsize=(1000/script_dpi, 700/script_dpi), dpi=script_dpi)
      line_w = 2


    # speedup
    if "baseline" in target_strats:
      ax.plot(x, y_speedup_baseline, color='blue', marker='v', lw=line_w, clip_on=False, label="Fast First")
    if "ilp_upper_bound" in target_strats:
      ax.plot(x[1:-1], y_speedup_nofrag, color='black', marker='*', markersize=12, lw=line_w*2, linestyle='--', clip_on=False, label="Upper Bound")
    if "ilp" in target_strats or "ilp_50" in target_strats or "ilp85" in target_strats:
      ax.plot(x[1:-1], y_speedup_ilp_merged, color='red', marker='o', lw=line_w, clip_on=False, label="ILP")
    if "density" in target_strats or "density_50" in target_strats or "density85" in target_strats:
      ax.plot(x[1:-1], y_speedup_density_merged, color='green', marker='d', lw=line_w, clip_on=False, label="Density")

    for a in arch:
      ax.plot([a,a] , [-100, 100], lw=0.5, linestyle='--',color='grey', zorder=0)


    plt.xticks(arch, arch_names)
    ax.set_xlim([0, 100])
    ax.set_ylim([min(0, min(y_speedup_baseline)), max(y_speedup_baseline)])

    ax.tick_params(axis='both', which='major', labelsize=11)

    if paper_output:
      ax.legend(loc='lower right', bbox_to_anchor=(1, 0.04), ncol=2, fontsize=11)
    else:
      ax.legend(loc='lower right', bbox_to_anchor=(1, 0.04), ncol=2, fontsize=14)

    # label work
    if paper_output:
      ax.set_xlabel('Fraction of heap in fast memory', fontsize=13)
      ax.set_ylabel('Speedup (%)', fontsize=13)
    else:
      ax.set_xlabel('Fraction of heap in fast memory', fontsize=16)
      ax.set_ylabel('Speedup (%)', fontsize=16)

    # ax2.set_ylabel('Time in allocator (%)', fontsize=16)
    if sw == "json_parser":
      ax.set_title("json")
    else:
      ax.set_title(sw)

    f.tight_layout()

    plt.show()



  sys.exit()




if __name__ == "__main__":
    main()

