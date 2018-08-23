#!/usr/bin/python

import getopt, sys, subprocess, datetime

sys.path.append('../../')

# offline placement strategy generation
# host : pc

def main():
  # start by printing the date
  date = str(datetime.datetime.now())
  print date

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
  
  subprocess.call(["mkdir", res_folder])
  subprocess.call(["pwd"])

  try:
    import xp_config as xp
    print "import from config file OK."
  except:
    print "CONFIG IMPORT FAILED !"
    sys.exit(1)
    
  target_sw = xp.target_sw
  target_hw = xp.target_hw
  target_strats = xp.target_strats

  print "target_sw", target_sw
  print "target_hw", target_hw
  print "target_strats", target_strats

  # build offline placement solutions
  for sw in target_sw:
    for hw in target_hw:
      if hw not in {"0", "6"}:
        desc_name = sw+"_arch"+hw+".desc"
        desc_name_frag50 = sw+"_arch"+hw+"_50p.desc"
        desc_name_frag85 = sw+"_arch"+hw+"_85p.desc"
        print "building offline solution for", sw," on architecture", hw
  
        # ILP offline placement
        if "ilp" in target_strats or "ilp_upper_bound" in target_strats:
          p = subprocess.Popen(["./ilp_from_trace.py", "-ireference_executions/"+sw+"/heap_objects.log", "-a"+xp_folder+"archi_desc/"+desc_name, "-n"+sw+"_arch"+hw+"_ilp.placement"])
          p.wait()
          if p.returncode:
            print "ERROR in ilp placement construction"
            sys.exit(1)
  
        if "ilp_50" in target_strats:
          # ILP offline placement with fragmentation compensation 50%
          p = subprocess.Popen(["./ilp_from_trace.py", "-ireference_executions/"+sw+"/heap_objects.log", "-a"+xp_folder+"archi_desc/"+desc_name_frag50, "-n"+sw+"_arch"+hw+"_ilp_50p.placement"])
          p.wait()
          if p.returncode:
            print "ERROR in ilp_50 placement construction"
            sys.exit(1)
    
        if "ilp_85" in target_strats:
          # ILP offline placement with fragmentation compensation 85%
          p = subprocess.Popen(["./ilp_from_trace.py", "-ireference_executions/"+sw+"/heap_objects.log", "-a"+xp_folder+"archi_desc/"+desc_name_frag85, "-n"+sw+"_arch"+hw+"_ilp_85p.placement"])
          p.wait()
          if p.returncode:
            print "ERROR in ilp_85 placement construction"
            sys.exit(1)
    
        if "density" in target_strats:
          # density offline placement
          p = subprocess.Popen(["./ilp_from_trace.py", "-ireference_executions/"+sw+"/heap_objects.log", "--no_ilp", "--overlap_density", "-a"+xp_folder+"archi_desc/"+desc_name, "-n"+sw+"_arch"+hw+"_density.placement"])
          p.wait()
          if p.returncode:
            print "ERROR in density placement construction"
            sys.exit(1)
    
        if "density_50" in target_strats:
          # density offline placement with fragmentation compensation 50%
          p = subprocess.Popen(["./ilp_from_trace.py", "-ireference_executions/"+sw+"/heap_objects.log", "--no_ilp", "--overlap_density", "-a"+xp_folder+"archi_desc/"+desc_name_frag50, "-n"+sw+"_arch"+hw+"_density_50p.placement"])
          p.wait()
          if p.returncode:
            print "ERROR in density_50 placement construction"
            sys.exit(1)
    
        if "density_85" in target_strats:
          # density offline placement with fragmentation compensation 85%
          p = subprocess.Popen(["./ilp_from_trace.py", "-ireference_executions/"+sw+"/heap_objects.log", "--no_ilp", "--overlap_density", "-a"+xp_folder+"archi_desc/"+desc_name_frag85, "-n"+sw+"_arch"+hw+"_density_85p.placement"])
          p.wait()
          if p.returncode:
            print "ERROR in density_85 placement construction"
            sys.exit(1)

  for sw in target_sw:
    for hw in target_hw:
      if hw not in {"0", "6"}:
        # retrieve the resulting placements
        if "ilp" in target_strats or "ilp_upper_bound" in target_strats:
          subprocess.call(["mv", sw+"_arch"+hw+"_ilp.placement", res_folder])
        if "ilp_50" in target_strats:
          subprocess.call(["mv", sw+"_arch"+hw+"_ilp_50p.placement", res_folder])
        if "ilp_85" in target_strats:
          subprocess.call(["mv", sw+"_arch"+hw+"_ilp_85p.placement", res_folder])
        if "density" in target_strats:
          subprocess.call(["mv", sw+"_arch"+hw+"_density.placement", res_folder])
        if "density_50" in target_strats:
          subprocess.call(["mv", sw+"_arch"+hw+"_density_50p.placement", res_folder])
        if "density_85" in target_strats:
          subprocess.call(["mv", sw+"_arch"+hw+"_density_85p.placement", res_folder])



  print "placement generation done, exiting..."
  sys.exit(0)





if __name__ == "__main__":
    main()

