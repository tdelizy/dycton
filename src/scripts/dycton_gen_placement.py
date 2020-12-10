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
import getopt, sys, subprocess, datetime

sys.path.append('../../')


def usage():
  print("\n================================================================================")
  print("Usage")
  print("================================================================================")
  print("this script should not be called directly but throug the experiment preparation script")
  print("it relies on the ../../xp_config.py file and generates the needed placement files from")
  print("references executions for the whole experiments (calls ilp_from_trace.py)")
  print("its only parameter is the path to the experiment folder being generated")



# offline placement strategy generation
def main():
  # start by printing the date
  date = str(datetime.datetime.now())
  print("Placement for heterogeneous dynamic memory allocation offline solving for Dycton")
  print("====================================================================================================")
  print("author: T. Delizy (delizy.tristan@gmail.com)")
  print(date)

  # command line arguments processing
  try:
    opts, args = getopt.getopt(sys.argv[0:], "", [""])
  except getopt.GetoptError as err:# print help information and exit:
    print(str(err))  # will print something like "option -a not recognized"
    sys.exit(2)




  if(len(args)>1):
    print("args", args)
    xp_folder = args[-1]
    res_folder = xp_folder+"offline_placements/"
    profile_folder = xp_folder+"profiles/"
  else:
    # create folder for result simulators and embedded softwares
    res_folder = "offline_placement_generation_"+date
    profile_folder = "ilp_profile_folder_"+date
    subprocess.call(["mkdir", profile_folder])


  subprocess.call(["mkdir", res_folder])
  subprocess.call(["pwd"])

  try:
    import xp_config as xp
    print("import from config file OK.")
  except:
    print("CONFIG IMPORT FAILED !")
    sys.exit(1)


  print("target_sw", xp.target_sw)
  print("target_hw", xp.target_hw)
  print("target_strats", xp.target_strats)

  # if target strategies includes profile-based strategies, we need to solve ILP on the other half of datasets to generates ilp profiles
  if any(x in xp.target_strats for x in ["profile", "profile-enhanced", "profile-ilp"]):
    considered_datasets = [0,1,2,3,4,5,6,7]
  else:
    considered_datasets = xp.target_datasets

  # build offline placement solutions
  for sw in xp.target_sw:
    for hw in xp.target_hw:
      if hw not in {"0", "6"}:
        for d in considered_datasets:
          dstring = "_d"+str(d)
          desc_name = sw+"_arch"+hw+"_d"+str(d)+".desc"
          desc_name_frag50 = sw+"_arch"+hw+"_d"+str(d)+"_50p.desc"
          desc_name_frag85 = sw+"_arch"+hw+"_d"+str(d)+"_85p.desc"
          print("building offline solution for", sw,"on architecture", hw, "targetting dataset", d)

          # ILP offline placement
          if any(x in xp.target_strats for x in ["profile", "ilp", "ilp_upper_bound", "profile-enhanced", "profile-ilp"]):
            p = subprocess.Popen(["./ilp_from_trace.py", "-i"+xp_folder+"ref_exec/"+sw+"/"+dstring+"/heap_objects.log", "-a"+xp_folder+"archi_desc/"+desc_name, "-n"+sw+"_arch"+hw+dstring+"_ilp.placement"])
            p.wait()
            if p.returncode:
              print("ERROR in ilp placement construction")
              sys.exit(1)

          if any(x in xp.target_strats for x in ["ilp_50"]):
            # ILP offline placement with fragmentation compensation 50%
            p = subprocess.Popen(["./ilp_from_trace.py", "-i"+xp_folder+"ref_exec/"+sw+"/"+dstring+"/heap_objects.log", "-a"+xp_folder+"archi_desc/"+desc_name_frag50, "-n"+sw+"_arch"+hw+dstring+"_ilp_50p.placement"])
            p.wait()
            if p.returncode:
              print("ERROR in ilp_50 placement construction")
              sys.exit(1)

          if any(x in xp.target_strats for x in ["ilp_85"]):
            # ILP offline placement with fragmentation compensation 85%
            p = subprocess.Popen(["./ilp_from_trace.py", "-i"+xp_folder+"ref_exec/"+sw+"/"+dstring+"/heap_objects.log", "-a"+xp_folder+"archi_desc/"+desc_name_frag85, "-n"+sw+"_arch"+hw+dstring+"_ilp_85p.placement"])
            p.wait()
            if p.returncode:
              print("ERROR in ilp_85 placement construction")
              sys.exit(1)

          if "density" in xp.target_strats:
            # density offline placement
            p = subprocess.Popen(["./ilp_from_trace.py", "-i"+xp_folder+"ref_exec/"+sw+"/"+dstring+"/heap_objects.log", "--no_ilp", "-a"+xp_folder+"archi_desc/"+desc_name, "-n"+sw+"_arch"+hw+dstring+"_density.placement"])
            p.wait()
            if p.returncode:
              print("ERROR in density placement construction")
              sys.exit(1)

          if "density_50" in xp.target_strats:
            # density offline placement with fragmentation compensation 50%
            p = subprocess.Popen(["./ilp_from_trace.py", "-i"+xp_folder+"ref_exec/"+sw+"/"+dstring+"/heap_objects.log", "--no_ilp", "-a"+xp_folder+"archi_desc/"+desc_name_frag50, "-n"+sw+"_arch"+hw+dstring+"_density_50p.placement"])
            p.wait()
            if p.returncode:
              print("ERROR in density_50 placement construction")
              sys.exit(1)

          if "density_85" in xp.target_strats:
            # density offline placement with fragmentation compensation 85%
            p = subprocess.Popen(["./ilp_from_trace.py", "-i"+xp_folder+"ref_exec/"+sw+"/"+dstring+"/heap_objects.log", "--no_ilp", "-a"+xp_folder+"archi_desc/"+desc_name_frag85, "-n"+sw+"_arch"+hw+dstring+"_density_85p.placement"])
            p.wait()
            if p.returncode:
              print("ERROR in density_85 placement construction")
              sys.exit(1)

  for sw in xp.target_sw:
    for hw in xp.target_hw:
      if hw not in {"0", "6"}:
        for d in considered_datasets:
          dstring = "_d"+str(d)
          # retrieve the resulting placements
          if any(x in xp.target_strats for x in ["profile", "ilp", "ilp_upper_bound", "profile-enhanced", "profile-ilp"]):
            subprocess.call(["mv", sw+"_arch"+hw+dstring+"_ilp.placement", res_folder])
            subprocess.call(["mv", sw+"_arch"+hw+dstring+"_ilp.profile", profile_folder+sw+"/"])
          if any(x in xp.target_strats for x in ["ilp_50"]):
            subprocess.call(["mv", sw+"_arch"+hw+dstring+"_ilp_50p.placement", res_folder])
          if any(x in xp.target_strats for x in ["ilp_85"]):
            subprocess.call(["mv", sw+"_arch"+hw+dstring+"_ilp_85p.placement", res_folder])
          if any(x in xp.target_strats for x in ["density"]):
            subprocess.call(["mv", sw+"_arch"+hw+dstring+"_density.placement", res_folder])
          if any(x in xp.target_strats for x in ["density_50"]):
            subprocess.call(["mv", sw+"_arch"+hw+dstring+"_density_50p.placement", res_folder])
          if any(x in xp.target_strats for x in ["density_85"]):
            subprocess.call(["mv", sw+"_arch"+hw+dstring+"_density_85p.placement", res_folder])



  print("placement generation done, exiting...")
  sys.exit(0)





if __name__ == "__main__":
    main()
