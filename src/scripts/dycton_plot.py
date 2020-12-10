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
import os, sys, getopt, time, math, subprocess, pprint

from datetime import datetime, timedelta
from decimal import *

import matplotlib.pyplot as plt
import matplotlib.colors as colors
import matplotlib.ticker as ticker
import matplotlib.patches as patches
import matplotlib.lines as lines

import numpy as np

# plot visual scheme definition
# c = color
# m = marker
# l = label
plot_config = { "baseline":{"c":'blue', "m":'v', "l":'Fast First', "ls":"-"},
                "ilp_upper_bound":{"c":'black', "m":'*', "l":'Upper Bound', "ls":"--"},
                "ilp":{"c":'red', "m":'o', "l":'ILP', "ls":"-"},
                "den":{"c":'green', "m":'d', "l":'Freq. acces / octet', "ls":"-"},
                "profile":{"c":'darkorange', "m":'x', "l":'Alloc Site Profile', "ls":"-"},
                "profile-enhanced":{"c":'purple', "m":'x', "l":'Alloc Site Enhanced', "ls":"-"},
                "profile-ilp":{"c":'cyan', "m":'o', "l":'Alloc Site ILP Profile', "ls":"-"}}

def usage():
  print("\n================================================================================")
  print("Usage")
  print("================================================================================")
  print("-i \"filepath\": input csv file path")
  print("-p : paper output (smaller image size, increased text size.")
  print("plotting whats described in the xp_config.py in current directory, fallback to the xp_config.py present in .. otherwise. ")
  print("exiting...")


def stack_dict_search(d, value, default=None):
    # try to find a value inside nested dicts, return default (None) if not found, return corresponding key if found
    stack = [iter(d.items())]
    while stack:
        for k, v in stack[-1]:
            if isinstance(v, dict):
                stack.append(iter(v.items()))
                break
            elif v == value:
                return k
        else:
            stack.pop()
    return default



prp = pprint.PrettyPrinter(indent=0)

def robust_max(index, *args):
  to_max=[]
  for arg in args:
      try:
        to_max.append(arg[index])
      except:
        pass
  if len(to_max) == 0:
    print("\n/!\\ ERROR: none of arguments is valid.")
    sys.exit(1)
  return max(to_max)


def main():
  print("Dycton plot script")
  print("================================================================================")
  print("author: T. Delizy (delizy.tristan@gmail.com)")
  print(str(datetime.now()))
  print("")

  global plot_config

  print("> command line arguments processing ... ", end=' ')
  try:
    opts, args = getopt.getopt(sys.argv[1:], 'i:hp', ['help'])
    # print "opts :", opts
    # print "args :", args
  except getopt.GetoptError as err:# print help information and exit:
    print(str(err))  # will print something like "option -a not recognized"
    sys.exit(2)

  input_csv = ""
  paper_output = False

  for o, a in opts:
    if o in ("-i"):
      input_csv = a
    elif o in ("-h", "--help"):
      usage()
      sys.exit(0)
    elif o in ("-p"):
      paper_output = True

  if input_csv == "":
    print("\n/!\\no data provided (try with -i path_to_csv)")
    print("exiting...")
    exit(1)
  print("done.\n")


  print("> experiment configuration file loading ...", end=' ')
  try:
    sys.path.append('.')
    import xp_config as xp
  except:
    print("\nCONFIG IMPORT FAILED FROM CURRENT DIRECTORY!")
    print("trying to get config from ../../", end=' ')
    try:
      sys.path.append('../../')
      import xp_config as xp
    except:
      print("\n/!\\no config file could be loaded, aborting.")
      sys.exit(1)




  print("done.\n")

  print("> loaded config : ")
  print("\t Apps :", xp.target_sw)
  print("\t Archis : ", xp.target_hw)
  print("\t Strategies : ", xp.target_strats)
  print("\t Datasets : ", xp.target_datasets)


  print("> constructing result structure ... ", end=' ')
  results = {}
  for sw in xp.target_sw:
    results[sw] = {}
    for hw in xp.target_hw:
      results[sw][hw] = {}
      if hw in [ "0", "6"]:
        results[sw][hw]["baseline"] = {}
        for d in xp.target_datasets:
          results[sw][hw]["baseline"][d] = -1
          # add result baseline
          # print sw, "on arch", hw, "strategy baseline", "dataset", str(d)
      else :
        for strat in xp.target_strats:
            results[sw][hw][strat] = {}
            for d in xp.target_datasets:
              # add result for given strategy, hw, sw and dataset
              results[sw][hw][strat][d] = -1
              # print sw, "on arch", hw, "strategy", strat, "dataset", str(d)
  print("done.\n")

  print("================================================================================")
  print("data retrieving and preparation")
  print("================================================================================")
  print("> parsing allocation log ... ", end=' ')
  data = np.genfromtxt(input_csv, dtype=None, delimiter=';', names=True, encoding=None)
  print("done.\n")

  print("> start of parsed data:")
  print(data.dtype.names)
  print(data[1:min(len(data)-1, 6)])
  print("...\n")


  print("> populating result structure... ", end=' ')
  for j in range(len(data)):
    xp_name = data['experience'][j]
    cur_sw = xp_name.split("arch")[0].strip("_")
    cur_hw = xp_name.split("arch")[1].split("_")[0]
    cur_ds = int(xp_name.split("arch")[1].split("_")[1].strip("d"))
    cur_strat = xp_name.split("arch")[1].strip("_run").split(cur_hw+"_d"+str(cur_ds)+"_")[1]

    # debug output of every simulation ran
    # print(xp_name)
    # print("sw = ", cur_sw)
    # print("hw = ", cur_hw)
    # print("dataset = ", cur_ds)
    # print("strat = ", cur_strat)
    try:
      results[cur_sw][cur_hw][cur_strat][cur_ds] = int(data['final_cycles'][j])
    except:
      continue
      print("\n/!\\ data inconsistent with xp_config file !")
      print("exiting ...")
      exit(1)
  print("done.\n")


  for i, sw in enumerate(xp.target_sw):
    print("================================================================================")
    print("plotting", sw)
    print("================================================================================", "\n\n")

    arch_x_names = {"6":"0%", "5":"5%", "4":" 10%", "3":"25%", "2":"50%", "1":"75%", "0":"100%" }
    arch_x_values = {"6":0, "5":5, "4":10, "3":25, "2":50, "1":75, "0":100 }

    arch = []
    arch_names = []
    y_min = 100
    y_max = 0
    for hw in xp.target_hw:
      arch.append(arch_x_values[hw])
      arch_names.append(arch_x_names[hw])

    print("arch = ", arch)
    print("arch names = ", arch_names)
    print("strats =", xp.target_strats)


    # create plot
    script_dpi = 80
    if paper_output:
      f, ax = plt.subplots(1, 1, figsize=(500/script_dpi, 350/script_dpi), dpi=script_dpi)
      line_w = 1.2
      markersize = 7
      capsize = 4
      elw = 0.7
    else:
      f, ax = plt.subplots(1, 1, figsize=(1000/script_dpi, 700/script_dpi), dpi=script_dpi)
      line_w = 2
      markersize = 8
      capsize = 8
      elw = 1
    # plot strats
    # results[cur_sw][cur_hw][cur_strat][cur_ds]
    ilp_strats = []
    den_strats = []
    adjustment_x = -0.4
    adjustment_x_val = 0.2
    for s in xp.target_strats:
      x = []
      y = {"avg":[], "down":[], "up":[]}
      if s in ["baseline", "ilp_upper_bound", "profile", "profile-enhanced", "profile-ilp"]:
        for hw in sorted(results[sw].keys(), key=str.lower, reverse=True):
          if s in results[sw][hw]:
            x.append(arch_x_values[hw]+ adjustment_x)
            speedups = []
            for k,v in results[sw][hw][s].items():
              if k in xp.target_datasets:
                speedups.append((float(results[sw]["6"]["baseline"][k]) - float(v))/float(results[sw]["6"]["baseline"][k])*100)
            avg = sum(speedups)/ float(len(speedups))
            y["avg"].append(avg)
            y["down"].append(avg - min(speedups))
            y["up"].append(max(speedups) - avg)
            if min(speedups) < y_min:
              y_min = min(speedups)
            if max(speedups) > y_max:
              y_max = max(speedups)

        eb = ax.errorbar(x, y["avg"],
          yerr=[y["down"], y["up"]],
          color=plot_config[s]["c"],
          linestyle=plot_config[s]["ls"],
          marker=plot_config[s]["m"],
          markersize=markersize,
          capsize=capsize,
          lw=line_w,
          elinewidth=elw,
          clip_on=False,
          label=plot_config[s]["l"] )

        adjustment_x += adjustment_x_val

      elif "ilp" in s:
        ilp_strats.append(s)
      elif "den" in s:
        den_strats.append(s)

    if ilp_strats != []:
      x = []
      y = {"avg":[], "down":[], "up":[]}
      for hw in sorted(results[sw].keys(), key=str.lower, reverse=True):
        # print "HW=", hw
        speedups = []
        if ilp_strats[0] in results[sw][hw].keys():
          for d in results[sw][hw][ilp_strats[0]].keys():
            val = 0
            for s in ilp_strats:
              try:
                if results[sw][hw][s][d] > 0:
                  test = (float(results[sw]["6"]["baseline"][d]) - float(results[sw][hw][s][d]))/float(results[sw]["6"]["baseline"][d])*100
                else:
                  test = 0
              except:
                test = 0
                pass
              if test > val:
                val = test
            if d in xp.target_datasets:
              speedups.append(val)
          x.append(arch_x_values[hw]+ adjustment_x)
          avg = sum(speedups)/ float(len(speedups))
          y["avg"].append(avg)
          y["down"].append(avg - min(speedups))
          y["up"].append(max(speedups) - avg)
          if min(speedups) < y_min:
            y_min = min(speedups)
          if max(speedups) > y_max:
            y_max = max(speedups)
      eb = ax.errorbar(x, y["avg"],
        yerr=[y["down"], y["up"]],
        color=plot_config["ilp"]["c"],
        linestyle=plot_config["ilp"]["ls"],
        marker=plot_config["ilp"]["m"],
        markersize=markersize,
        capsize=capsize,
        lw=line_w,
        elinewidth=elw,
        clip_on=False,
        label=plot_config["ilp"]["l"])

      adjustment_x += adjustment_x_val


    if den_strats != []:
      x = []
      y = {"avg":[], "down":[], "up":[]}
      for hw in sorted(results[sw].keys(), key=str.lower, reverse=True):
        # print "HW=", hw
        speedups = []
        if den_strats[0] in results[sw][hw].keys():
          for d in results[sw][hw][den_strats[0]].keys():
            val = 0
            for s in den_strats:
              try:
                if results[sw][hw][s][d] > 0:
                  test = (float(results[sw]["6"]["baseline"][d]) - float(results[sw][hw][s][d]))/float(results[sw]["6"]["baseline"][d])*100
              except:
                test = 0
                pass
              if test > val:
                val = test
            if d in xp.target_datasets:
              speedups.append(val)
          x.append(arch_x_values[hw]+ adjustment_x)
          avg = sum(speedups)/ float(len(speedups))
          y["avg"].append(avg)
          y["down"].append(avg - min(speedups))
          y["up"].append(max(speedups) - avg)
          if min(speedups) < y_min:
            y_min = min(speedups)
          if max(speedups) > y_max:
            y_max = max(speedups)
      eb = ax.errorbar(x, y["avg"],
        yerr=[y["down"], y["up"]],
        color=plot_config["den"]["c"],
        linestyle=plot_config["den"]["ls"],
        marker=plot_config["den"]["m"],
        markersize=markersize,
        capsize=capsize,
        lw=line_w,
        elinewidth=elw,
        clip_on=False,
        label=plot_config["den"]["l"])

      adjustment_x += adjustment_x_val

    for a in arch:
      ax.plot([a,a] , [-100, 100], lw=0.5, linestyle='--',color='grey', zorder=0)

    plt.xticks(arch, arch_names)
    ax.set_xlim([-0.7, 100.7])
    ax.set_ylim([y_min - 0.5, y_max + 0.5])

    ax.tick_params(axis='both', which='major', labelsize=11)


    handles, labels = ax.get_legend_handles_labels()
    if len(xp.target_datasets) > 1:
      extraString = "(Average,\nmin and max)"
      handles.append(patches.Patch(color='none', label=extraString))
      ax.legend(handles=handles)


    if paper_output:
      ax.legend(loc='lower right', bbox_to_anchor=(1, 0), ncol=2, fontsize=11, handles=handles)
      ax.set_xlabel('Fraction of heap in fast memory', fontsize=13)
      ax.set_ylabel('Execution Time Reduction (%)', fontsize=13)
      title_sz = 13
    else:
      ax.legend(loc='lower right', bbox_to_anchor=(1, 0.04), ncol=2, fontsize=14, handles=handles)
      ax.set_xlabel('Fraction of heap in fast memory', fontsize=16)
      ax.set_ylabel('Execution Time Reduction (%)', fontsize=16)
      title_sz = 16



    if sw == "json_parser":
      ax.set_title("json", fontsize=title_sz)
    else:
      ax.set_title(sw, fontsize=title_sz)


    f.tight_layout()

    strat_string = ""
    dataset_string = ""
    arch_string = ""

    for s in xp.target_strats:
      if s == "baseline":
        strat_string+= "ff-"
      if s == "ilp":
        strat_string+= "ilp-"
      if s == "density":
        strat_string+= "freq-"
      if s == "profile":
        strat_string+= "prof-"
      if s == "profile-enhanced":
        strat_string+= "profEN-"
      if s == "profile-ilp":
        strat_string+= "profILP-"
    strat_string = strat_string[0:-1]

    for a in xp.target_hw:
      arch_string+= a+"-"
    arch_string = arch_string[0:-1]

    for d in xp.target_datasets:
      dataset_string+= str(d)+"-"
    dataset_string = dataset_string[0:-1]


    plt.savefig(sw+"_A("+arch_string+")D("+dataset_string+")S("+strat_string+").pdf", dpi=330)


    # show it to the world
    plt.show()


    # exit(0)



if __name__ == "__main__":
    main()
