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
import getopt, sys, subprocess, datetime, os

import numpy as np

import pprint

# ATTENTION : ABSOLUTE PATHS HERE !!!
# ALSO ATTENTION : PATHS END WITH / !!!

# xp paths
root_xp_data = "./"
root_res = root_xp_data + "results_raw/"
root_exec = root_xp_data + "xp_run/"
xp_logs = root_res+"logs/"

nice_arg = "-12" # a bit nice (useless for local execution)

# get xp configuration
try:
  import xp_config as xp
  print("config file loaded.")
except:
  print("dycton run xp error: cannot load xp_config.py")
  print("exiting...")
  sys.exit(1)


def prepare_run(sw, hw, dataset, strat, length=0):
  if length>0:
    run_name = sw+"_arch"+hw+"_d"+str(dataset)+"_"+strat+"_"+str(int(length))+"_run"
  else:
    run_name = sw+"_arch"+hw+"_d"+str(dataset)+"_"+strat+"_run"
  run_path = root_exec+run_name+"/"

  # setup clean exec environment
  subprocess.call(["mkdir", root_exec+run_name])
  p = subprocess.Popen(["cp", "-a", root_xp_data+"clean_env/.", run_path])
  p.wait()
  if p.returncode:
    print("ERROR in clean environment setup for xp", run_name)
    sys.exit(1)

  #setup log folder
  subprocess.call(["mkdir", xp_logs+run_name])

  # setup simulator executable
  p = subprocess.Popen(["cp", root_xp_data+"simulators/"+sw+"_sim.x", run_path+"iss/"])
  p.wait()
  if p.returncode:
    print("ERROR can't copy simulator")
    sys.exit(1)

  # chmod +x simulator
  subprocess.call(["chmod", "+x", run_path+"iss/"+sw+"_sim.x"])

  # setup embedded software
  p = subprocess.Popen(["cp", root_xp_data+"emb_softs/"+sw+"_soft.out", run_path+"software/a.out"])
  p.wait()
  if p.returncode:
    print("ERROR can't copy embedded software")
    sys.exit(1)

  # elaborate simulator command line
  if strat in ["profile", "profile-enhanced"]:
    if length < 1:
      cmd_line = ["nice", nice_arg, "./"+sw+"_sim.x", "-c", "-a", hw, "-d", str(dataset), "-s", strat, "-p", str(int(xp.profile_static_opt[sw][hw]))]
    else:
      cmd_line = ["nice", nice_arg, "./"+sw+"_sim.x", "-c", "-a", hw, "-d", str(dataset), "-s", strat, "-p", str(int(length))]
  elif strat in ["ilp", "density"]:
    cmd_line = ["nice", nice_arg, "./"+sw+"_sim.x", "-c", "-a", hw, "-d", str(dataset), "-soracle", "-f../../../offline_placements/"+sw+"_arch"+hw+"_d"+str(dataset)+"_"+strat+".placement"]
  elif strat in ["ilp_50", "ilp_85", "density_50", "density_85"]:
    cmd_line = ["nice", nice_arg, "./"+sw+"_sim.x", "-c", "-a", hw, "-d", str(dataset), "-soracle", "-f../../../offline_placements/"+sw+"_arch"+hw+"_d"+str(dataset)+"_"+strat+"p.placement"]
  elif strat in ["ilp_upper_bound"]:
    cmd_line = ["nice", nice_arg, "./"+sw+"_sim.x", "-c", "-a", "-2", "-d", str(dataset), "-soracle", "-f../../../offline_placements/"+sw+"_arch"+hw+"_d"+str(dataset)+"_ilp.placement"]
  elif strat in ["profile-ilp"]:
    cmd_line = ["nice", nice_arg, "./"+sw+"_sim.x", "-c", "-a", hw, "-d", str(dataset), "-s", strat]
  else:
    cmd_line = ["nice", nice_arg, "./"+sw+"_sim.x", "-c", "-a", hw, "-d", str(dataset)]

  # return dict describing the run
  return {'name' : run_name, 'cmd' : cmd_line, 'path': run_path}




def main():
  # start by printing the date
  date_start = str(datetime.datetime.now())
  print("Dycton experiment run script")
  print("================================================================================")
  print("author: T. Delizy (delizy.tristan@gmail.com)")
  print(date_start)

  subprocess.call(["mkdir", root_exec])
  subprocess.call(["mkdir", root_res])
  subprocess.call(["mkdir", xp_logs])

  run_list_by_app = {"json_parser":[], "dijkstra":[], "jpg2000":[], "h263":[], "ecdsa":[], "jpeg":[]}

  # iterate on the different application sequentially to avoid taking to much ressources
  for sw in xp.target_sw:
    # setup :
    # - creates an environment for every run of the target app / arch / strat (and multiple for profile strategies exploration)
    # - populates a dict describing each run with run name, execution path and simulator command line
    for hw in xp.target_hw:
      for d in xp.target_datasets:
        if hw in [ "0", "6"] and "baseline" in xp.target_strats:
          # only execute baseline on singe heap architectures
          run_list_by_app[sw].append(prepare_run(sw, hw, d, "baseline"))
        else:
          for strat in xp.target_strats:
            # we want to launch multiple runs with the same sw, hw and strat and with different profile length
            if strat in ["profile", "profile-enhanced"] and xp.profile_explo == True:
              # get the profile max length for this application
              max_len = 0
              profile_len_list = []
              with open(root_xp_data+"profiles/"+sw+"/"+sw+"_alloc_site_profile.h") as profile:
                content = profile.readlines()
                for line in content:
                  if "__APP_ALLOC_SITE_LENGTH" in line:
                    max_len = int(line.split("__APP_ALLOC_SITE_LENGTH")[1].strip(' ()\t\n\r'))
              if xp.profile_high_resolution :
                # every profile length for low values (< 20)
                profile_len_list = np.arange(1.0, min(20, max_len-1), 1.0)
                print("profile len list (1)")
                print(profile_len_list)
                if max_len > 20:
                  # ten profile lengths after that
                  interval = float(max(1,float(max_len-20)/10))
                  end_of_list = np.arange(20, max_len-1, interval)
                  end_of_list = [int(round(l)) for l in end_of_list]
                  print("end_of_list (2)")
                  print(end_of_list)
                  profile_len_list = np.concatenate((profile_len_list, end_of_list), 0)
                  print("profile len list (3)")
                  print(profile_len_list)
              else:
                # 10 profile lengths tested
                interval = max(1,float(max_len)/10)
                profile_len_list = np.arange(1, max_len-1, interval)
              # always plot the lmast point
              np.append(profile_len_list, max_len)
              # round the floats to integer values
              profile_len_list = [int(round(l)) for l in profile_len_list]
              for l in profile_len_list:
                # prepare the runs for the different profile lengths
                run_list_by_app[sw].append(prepare_run(sw, hw, d, strat, l))
            else:
              # prepare the run for the giventarget soft, architecture and strategy
              run_list_by_app[sw].append(prepare_run(sw, hw, d, strat))


    # run all the "runs" preparated for the current software target
    for run in run_list_by_app[sw]:
      with open(root_res+run['name']+".result", 'w+') as outfile:
        p = subprocess.Popen(run['cmd'], cwd=run['path']+"iss/", stdout=outfile, stderr=subprocess.STDOUT)
        run['process'] = p

    # wait end of simulations
    for run in run_list_by_app[sw]:
      run['ret'] = run['process'].wait()
      if run['ret']!= 0:
        print("ERROR in", run['name'])

    # retrieve logs and cleanup
    for run in run_list_by_app[sw]:
      # if specified in xp_config.py save execution logs
      if xp.save_heap_logs:
        p = subprocess.Popen(["cp","-r" , run['path']+"logs/", xp_logs+run['name']+"/"])
        p.wait()
        if p.returncode:
          print("ERROR can't retrieve logs for", run['name'])
          sys.exit(1)
      if "h263" in run['name']:
        p = subprocess.Popen(["cp" , run['path']+"software/h263/datasets/output.263", xp_logs+run['name']+"/"])
        p.wait()
        if p.returncode:
          print("ERROR can't retrieve output.263", run['name'])
          sys.exit(1)

      # delete run environment
      p = subprocess.Popen(["rm","-r" , run['path']])
      p.wait()
      if p.returncode:
        print("ERROR can't clean up", run['name'])

  print("end of experiment run, exiting...")
  print("printing the run dict:")
  pp = pprint.PrettyPrinter(indent=2)
  pp.pprint(run_list_by_app)
  sys.exit(0)





if __name__ == "__main__":
    main()
