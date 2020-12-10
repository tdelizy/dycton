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
import os, sys, getopt, math

from datetime import datetime, timedelta
from decimal import *

import matplotlib.pyplot as plt
import matplotlib.colors as colors
import matplotlib.ticker as ticker
import matplotlib.patches as patches

import numpy as np

# multi_dataset representation
dataset_look_and_feel = {
  "_d0_":{"color":"blue" , "linestyle": '-', "marker": "o", "fillstyle": "none"},
  "_d1_":{"color":"blue" , "linestyle": '--', "marker": "o", "fillstyle": "full"},
  "_d2_":{"color":"red" , "linestyle": '-', "marker": "s", "fillstyle": "none"},
  "_d3_":{"color":"red" , "linestyle": '--', "marker": "s", "fillstyle": "full"},
  "_d4_":{"color":"green" , "linestyle": '-', "marker": "^", "fillstyle": "none"},
  "_d5_":{"color":"green" , "linestyle": '--', "marker": "^", "fillstyle": "full"},
  "_d6_":{"color":"black" , "linestyle": '-', "marker": "v", "fillstyle": "none"},
  "_d7_":{"color":"black" , "linestyle": '--', "marker": "v", "fillstyle": "full"}
}


def usage():
  print("usage:")
  print("-a \"arch\": memory configuration, acceptable values in [1,5]")
  print("-i \"filepath\": input csv file path")
  print("-t \"target_application\": target application name.")
  print("-d \"dataset\": target dataset execution, if none, will print all datasets aviable")
  print("--dyn-init-explo: (/!\\workaround) plotting the execution time of application with dynamic profile length strategy in function of the initial value")
  print("--profile-enhanced: exploration for enhanced static profile strategy.")
  print("")
  print("example: ")
  print("./profile_len_perf.py -a 5 -i input.csv -r ref.csv -t h263")
  print("exiting...")
  exit(1)



def main():
  # start by printing the date
  date_start = str(datetime.now())

  # CONFIG FOR NOW
  arch=-1
  app = ""
  ref_csv = ""
  dyn_init_explo = False
  prof_enhanced = False
  dataset=-1

  print("Dycton profile length evaluation script")
  print("========================================")
  print("author: T. Delizy (delizy.tristan@gmail.com)")
  print(date_start)
  print("")

  # command line arguments processing
  try:
    opts, args = getopt.getopt(sys.argv[1:], 'a:i:r:t:d:h', ['help', 'profile-enhanced'])
  except getopt.GetoptError as err:# print help information and exit:
    print(str(err))  # will print something like "option -a not recognized"
    print("command line argument parsing error")
    usage()
    sys.exit(1)

  for o, a in opts:
    if o in ("-i"):
      input_csv = a
    elif o in ("-h", "--help"):
      usage()
      sys.exit(0)
    elif o in ("-a"):
      arch = int(a)
    elif o in ("-t"):
      app = a
    elif o in ("-d"):
      dataset = int(a)
    elif o in ("--profile-enhanced"):
      prof_enhanced = True

  if app not in ["h263", "dijkstra", "json_parser", "jpg2000", "ecdsa"]:
    print("application unrecognized, exiting...")
    exit(2)
  if arch not in range(1,6):
    print("architecture unrecognized, exiting...")
    exit(2)
  if input_csv == "":
  	print("please provide input csv file with -i, exiting...")
  	exit(2)

  print("input csv:", input_csv)
  print("target application:", app)
  print("target architecture:", arch)
  print("- - - - - - - - - - - - - - - - - - - - - - - - - ")

  print("==================================")
  print("parsing input values")
  print("==================================")
  data = np.genfromtxt(input_csv, dtype=None, delimiter=';', names=True)

  # selecting target dataset
  # highest_heap_obj = allocs[np.where((allocs[:,0]+allocs[:,1]) == max(allocs[:,0]+allocs[:,1]))[0][0]]

  if dataset == -1:
    print("target datasets =", "all")
  else:
    print("target dataset =", "d"+str(dataset))
  print("data[:0]", data['experience'])

  if dataset != -1:
    print("were", np.where(np.char.find(data['experience'],"_d"+str(dataset))>=0))

    data = data[np.where(np.char.find(data['experience'],"_d"+str(dataset))>=0)]

    print("dataset filter:")
    print(data)

  print("np.where(data['return_code'])", np.where(data['return_code'])[0])
  print("len(np.where(data['return_code'])", len(np.where(data['return_code'])[0]))
  if len(np.where(data['return_code'])[0]):
    print("errors : ", np.where(data['return_code']!= 0))
    print("aborting...")
    exit(1)


  if dataset == -1:
    uniques_dset_str = []
    extract_dset = lambda i: "_d"+data[i]['experience'].split("_d")[1].split("_")[0]+"_"
    [uniques_dset_str.append(extract_dset(i)) for i in range(len(data)) if extract_dset(i) not in uniques_dset_str]

    print("uniques_dset_str\n", uniques_dset_str)
  else:
    uniques_dset_str = ["_d"+str(dataset)+"_"]


  # horrible multi dataset refactor
  ref_ilp_raw = {}
  ref_ilp = {}
  ref_density_raw = {}
  ref_density = {}
  ref_upper_bound = {}
  ref_baseline = {}
  time_ref = {}
  ref_profile_dyn = {}

  list_len_for = {}
  speedup_for = {}
  fallback_for = {}
  list_len_switch = {}
  speedup_switch = {}
  fallback_switch = {}
  for_count = {}
  max_len = {}
  switch_count = {}

  baseline = {}
  ilp = {}
  upper_bound = {}
  density = {}
  profile_dyn = {}


  for dataset_str in uniques_dset_str:

    print("working on dataset", dataset_str)
    # print "data[0]", data[0]['experience']
    # print "test ?", "yes" if dataset_str in data[0]['experience'] else "no"
    # print "np.where(dataset_str in data['experience'])", np.where(np.char.find(data['experience'],dataset_str)>=0)
    # print "filtered data:\n", data[np.where(np.char.find(data['experience'],dataset_str)>=0)[0]]


    ref_ilp_raw[dataset_str] = []
    ref_ilp[dataset_str]  = -1
    ref_density_raw[dataset_str]  = []
    ref_density[dataset_str]  = -1
    ref_upper_bound[dataset_str]  = -1
    ref_baseline[dataset_str]  = -1
    time_ref[dataset_str]  = -1
    ref_profile_dyn[dataset_str]  = -1

    for d in data[np.where(np.char.find(data['experience'],dataset_str)>=0)[0]]:

      if "arch6" in d[0] and app in d[0]:
        time_ref[dataset_str] = d[2]
        print("time_ref = ", time_ref[dataset_str])
      if ("arch"+str(arch) in d[0] and app in d[0]) or ("arch_"+str(arch) in d[0] and app in d[0]):
        if "baseline" in d[0]:
          ref_baseline[dataset_str] = d[2]
          print("arch"+str(arch), "baseline:", ref_baseline[dataset_str])
        if "ilp" in d[0] and not prof_enhanced:
          if "ilp_upper_bound" in d[0]:
            ref_upper_bound[dataset_str] = d[2]
          else:
            ref_ilp_raw[dataset_str].append(d[2])
        if "density" in d[0]:
          ref_density_raw[dataset_str].append(d[2])
        elif prof_enhanced:
            if "profile-ilp" in d[0]:
              ref_profile_dyn[dataset_str] = d[2]
            print("arch"+str(arch), "profile-ilp:", ref_profile_dyn[dataset_str])


    if len(ref_ilp_raw[dataset_str]) > 0:
      ref_ilp[dataset_str] = min(ref_ilp_raw[dataset_str])

    if len(ref_density_raw[dataset_str]) > 0:
      ref_density[dataset_str] = min(ref_density_raw[dataset_str])


    if ref_baseline[dataset_str] < 0 or time_ref[dataset_str] < 0:
      print("Error in reference retrieving (we need baseline and reference time), exiting...")
      print("dataset :", dataset_str)
      print("ref_ilp =", ref_ilp[dataset_str])
      print("ref_ilp_raw =", ref_ilp_raw[dataset_str])
      print("ref_density =", ref_density[dataset_str])
      print("ref_density_raw =", ref_density_raw[dataset_str])
      print("ref_baseline =", ref_baseline[dataset_str])
      print("ref_upper_bound =", ref_upper_bound[dataset_str])
      print("time_ref =", time_ref[dataset_str])
      print("profile_dyn =", ref_profile_dyn[dataset_str])
      exit(2)


    arch_data = []
    for d in data[np.where(np.char.find(data['experience'],dataset_str)>=0)[0]]:
    	if "arch"+str(arch) in d[0] and app in d[0]:
    		arch_data.append( d)

    list_len_for[dataset_str] = []
    speedup_for[dataset_str] = []
    fallback_for[dataset_str] = []
    list_len_switch[dataset_str] = []
    speedup_switch[dataset_str] = []
    fallback_switch[dataset_str] = []
    for_count[dataset_str] = 0
    max_len[dataset_str] = 0
    switch_count[dataset_str] = 0
    for d in arch_data:
      if prof_enhanced:
        if "profile-enhanced" in d[0]:
          prof_len = int(d[0].split("profile-enhanced_")[1].split("_run")[0])
          list_len_switch[dataset_str].append(prof_len)
          if prof_len > max_len[dataset_str]:
            max_len[dataset_str] = prof_len
          speedup_switch[dataset_str].append((1-float(d[2])/time_ref[dataset_str])*100)
          fallback_switch[dataset_str].append(float(d[4]))
          switch_count[dataset_str]+=1
      else:
        if "profile" in d[0]:
          prof_len = int(d[0].split("profile_")[1].split("_run")[0])
          list_len_switch[dataset_str].append(prof_len)
          if prof_len > max_len[dataset_str]:
            max_len[dataset_str] = prof_len
          speedup_switch[dataset_str].append((1-float(d[2])/time_ref[dataset_str])*100)
          fallback_switch[dataset_str].append(float(d[4]))
          switch_count[dataset_str]+=1

    baseline[dataset_str] = (1-float(ref_baseline[dataset_str])/time_ref[dataset_str])*100
    if ref_ilp[dataset_str] > 0:
      ilp[dataset_str] = (1-float(ref_ilp[dataset_str])/time_ref[dataset_str])*100
    if ref_density[dataset_str] > 0:
      density[dataset_str] = (1-float(ref_density[dataset_str])/time_ref[dataset_str])*100
    if ref_upper_bound[dataset_str] >0:
      upper_bound[dataset_str] = (1-float(ref_upper_bound[dataset_str])/time_ref[dataset_str])*100
    if ref_profile_dyn[dataset_str] > 0:
      profile_dyn[dataset_str] = (1-float(ref_profile_dyn[dataset_str])/time_ref[dataset_str])*100


    if len(list_len_for[dataset_str])> 0:
      list_len_for[dataset_str] = np.array(list_len_for[dataset_str])
      speedup_for[dataset_str] = np.array(speedup_for[dataset_str])
      fallback_for[dataset_str] = np.array(fallback_for[dataset_str])
      indices_for = np.argsort(list_len_for[dataset_str])
      list_len_for[dataset_str] = list_len_for[dataset_str][indices_for]
      speedup_for[dataset_str] = speedup_for[dataset_str][indices_for]
      fallback_for[dataset_str] = fallback_for[dataset_str][indices_for]

    if len(list_len_switch[dataset_str])> 0:
      list_len_switch[dataset_str] = np.array(list_len_switch[dataset_str])
      speedup_switch[dataset_str] = np.array(speedup_switch[dataset_str])
      fallback_switch[dataset_str] = np.array(fallback_switch[dataset_str])
      indices_switch = np.argsort(list_len_switch[dataset_str])
      list_len_switch[dataset_str] = list_len_switch[dataset_str][indices_switch]
      speedup_switch[dataset_str] = speedup_switch[dataset_str][indices_switch]
      fallback_switch[dataset_str] = fallback_switch[dataset_str][indices_switch]


  print("==================================")
  print("plotting")
  print("==================================")

  plt.close('all')
  f, ax1 = plt.subplots(1, 1, figsize=(14, 10))



  # ax1.plot((1, max_len), (baseline,baseline), lw=1, color='blue')
  # ax1.text( max(for_count,switch_count)-0.5,baseline, "baseline speedup", horizontalalignment='right', color='blue')

  # if dataset != -1:
  #   ax2 = ax1.twinx()  # instantiate a second axes that shares the same x-axis

  for dataset_str in uniques_dset_str:
    if not prof_enhanced:
      if len(list_len_switch[dataset_str])> 0:
        ax1.plot(list_len_switch[dataset_str], speedup_switch[dataset_str], lw=1.5,
          color=dataset_look_and_feel[dataset_str]["color"],
          linestyle= dataset_look_and_feel[dataset_str]["linestyle"],
          marker= dataset_look_and_feel[dataset_str]["marker"],
          fillstyle=dataset_look_and_feel[dataset_str]["fillstyle"],
          label=" "+dataset_str+" Profile Speedup")
    else:
      ax1.plot(list_len_switch[dataset_str], speedup_switch[dataset_str], lw=1.5,
        color=dataset_look_and_feel[dataset_str]["color"],
        linestyle= dataset_look_and_feel[dataset_str]["linestyle"],
        marker= dataset_look_and_feel[dataset_str]["marker"],
        fillstyle=dataset_look_and_feel[dataset_str]["fillstyle"],
        label=" "+dataset_str+" Profile-enhanced Speedup")

  if dataset != -1:
    if ref_ilp[dataset_str] > 0:
      ax1.plot((1, max_len[dataset_str]), (ilp[dataset_str],ilp[dataset_str]), lw=1, color='red')
      ax1.text( 1.5,ilp[dataset_str], "ilp speedup", color='red', verticalalignment='top')

    if ref_density[dataset_str] > 0:
      ax1.plot((1, max_len[dataset_str]), (density[dataset_str],density[dataset_str]), lw=1, color='green')
      ax1.text( 1.5,density[dataset_str], "density speedup", color='green', verticalalignment='top')

    if ref_upper_bound[dataset_str] >0:
      ax1.plot((1, max_len[dataset_str]), (upper_bound[dataset_str],upper_bound[dataset_str]), lw=1, color='darkred')
      ax1.text( 1.5,upper_bound[dataset_str], "upper bound", color='darkred')


    if ref_profile_dyn[dataset_str] > 0:
      ax1.plot((1, max_len[dataset_str]), (profile_dyn[dataset_str],profile_dyn[dataset_str]), lw=1, color='black')
      ax1.text( 1.5,profile_dyn[dataset_str], "Dynamic profile length", color='black')
    if ref_profile_dyn > 0:
      ax1.plot((1, max_len[dataset_str]), (profile_dyn[dataset_str],profile_dyn[dataset_str]), lw=1, color='black')
      ax1.text( max(for_count[dataset_str],switch_count[dataset_str])-0.5,profile_dyn[dataset_str], "ILP profile", horizontalalignment='right', color='black')


  ax1.set_xlim([1, max(max_len.values())])
  ax1.set_xlabel('Length of Allocation Site List')
  ax1.set_ylabel('Speedup against Reference Execution (%)')
  size = f.get_size_inches()*f.dpi
  ax1.set_title("Influence of profile length for "+app+" on architecture "+str(arch))
  ax1.legend(loc='lower left', bbox_to_anchor=(1, 0), ncol=1)
  f.subplots_adjust(top=0.9, right=0.8)




  if dataset == -1:
    plt.savefig(app+"_arch"+str(arch)+"_all_datasets_profile_length_influence.pdf", dpi=330)
  else:
    plt.savefig(app+"_arch"+str(arch)+"_d"+str(dataset)+"_profile_length_influence.pdf", dpi=330)



  plt.show()

  print("done... exiting.")
  exit(0)

















if __name__ == "__main__":
    main()
