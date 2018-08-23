#!/usr/bin/python

import getopt, sys, subprocess, datetime, os

# ATTENTION : ABSOLUTE PATHS HERE !!!
# ALSO ATTENTION : END WITH / !!!

def main():
  # start by printing the date
  date_start = str(datetime.datetime.now())
  print date_start

  # get xp configuration 
  try:
    import xp_config as xp
    print "config file loaded."
  except:
    print "dycton run xp error: cannot load xp_config.py"
    print "exiting..."
    sys.exit(1)

  # xp paths
  root_xp_data = "./"
  root_res = root_xp_data + "results_raw/"
  root_exec = root_xp_data + "xp_run/"

  target_sw = xp.target_sw
  target_hw = xp.target_hw
  target_strats = xp.target_strats

  subprocesses = []

  nice_arg = "-12" # a bit nice (useless for local execution)

  subprocess.call(["mkdir", root_exec])
  subprocess.call(["mkdir", root_res])


  # SETUP
  for sw in target_sw:
    for hw in target_hw:

      xp_name = sw+"_arch"+hw+"_run"
      xp_root = root_exec+xp_name+"/"

      # setup clean exec environment
      subprocess.call(["mkdir", root_exec+xp_name])
      p = subprocess.Popen(["cp", "-a", root_xp_data+"clean_env/.", xp_root])
      p.wait()
      if p.returncode:
        print "ERROR in clean environment setup for xp", xp_name
        sys.exit(1)

      # setup simulator executable
      p = subprocess.Popen(["cp", root_xp_data+"simulators/"+sw+"_arch"+hw+"_sim.x", xp_root+"iss/"])
      p.wait()
      if p.returncode:
        print "ERROR can't copy simulator"
        sys.exit(1)

      # chmod +x simulator
      print "CHMOD:", xp_root+"iss/"+sw+"_arch"+hw+"_sim.x"
      subprocess.call(["chmod", "+x", xp_root+"iss/"+sw+"_arch"+hw+"_sim.x"])

      # setup embedded software
      p = subprocess.Popen(["cp", root_xp_data+"emb_softs/"+sw+"_arch"+hw+"_soft.out", xp_root+"software/a.out"])
      p.wait()
      if p.returncode:
        print "ERROR can't copy embedded software"
        sys.exit(1)


  # SETUP FOR ILP_NOFRAG EXECUTION
  if "ilp_upper_bound" in target_strats:
    for sw in target_sw:
      xp_name = sw+"_all_arch_run"
      xp_root = root_exec+xp_name+"/"
  
      # setup clean exec environment
      subprocess.call(["mkdir", root_exec+xp_name])
      p = subprocess.Popen(["cp", "-a", root_xp_data+"clean_env/.", xp_root])
      p.wait()
      if p.returncode:
        print "ERROR in clean environment setup for xp", xp_name
        sys.exit(1)
  
      # setup simulator executable
      p = subprocess.Popen(["cp", root_xp_data+"simulators/"+sw+"_arch_nofrag_sim.x", xp_root+"iss/"])
      p.wait()
      if p.returncode:
        print "ERROR can't copy simulator"
        sys.exit(1)
  
      # chmod +x simulator
      print "CHMOD:", xp_root+"iss/"+sw+"_arch_nofrag_sim.x"
      subprocess.call(["chmod", "+x", xp_root+"iss/"+sw+"_arch_nofrag_sim.x"])
  
      # setup embedded software ex: jpeg_arch_nofrag_soft.out
      p = subprocess.Popen(["cp", root_xp_data+"emb_softs/"+sw+"_arch_nofrag_soft.out", xp_root+"software/a.out"])
      p.wait()
      if p.returncode:
        print "ERROR can't copy embedded software"
        sys.exit(1)

  # RUN XP 
  for sw in target_sw:
    for hw in target_hw:
      xp_name = sw+"_arch"+hw+"_run"
      xp_root = root_exec+xp_name+"/"

      # XP RUN START BASELINE
      if "baseline" in target_strats:
        with open(root_res+xp_name+"_baseline.result", 'w+') as outfile:
          p = subprocess.Popen(["nice", nice_arg, "./"+sw+"_arch"+hw+"_sim.x", "-c"], cwd=xp_root+"iss/", stdout=outfile, stderr=subprocess.STDOUT)
          subprocesses.append(p)


      # XP RUN START ILP & DENSITY
      if hw not in ["0", "6"]: # architectures 0 and 6 only have one heap so we can't evaluate these here
        # run experience : ilp
        if "ilp" in target_strats:
          with open(root_res+xp_name+"_ilp.result", 'w+') as outfile:
            p = subprocess.Popen(["nice", nice_arg, "./"+sw+"_arch"+hw+"_sim.x", "-c", "-soracle", "-f../../../offline_placements/"+sw+"_arch"+hw+"_ilp.placement"], cwd=xp_root+"iss/", stdout=outfile, stderr=subprocess.STDOUT)
            subprocesses.append(p)

        if "ilp_50" in target_strats:
          with open(root_res+xp_name+"_ilp_50.result", 'w+') as outfile:
            p = subprocess.Popen(["nice", nice_arg, "./"+sw+"_arch"+hw+"_sim.x", "-c", "-soracle", "-f../../../offline_placements/"+sw+"_arch"+hw+"_ilp_50p.placement"], cwd=xp_root+"iss/", stdout=outfile, stderr=subprocess.STDOUT)
            subprocesses.append(p)

        if "ilp_85" in target_strats:
          with open(root_res+xp_name+"_ilp_85.result", 'w+') as outfile:
            p = subprocess.Popen(["nice", nice_arg, "./"+sw+"_arch"+hw+"_sim.x", "-c", "-soracle", "-f../../../offline_placements/"+sw+"_arch"+hw+"_ilp_85p.placement"], cwd=xp_root+"iss/", stdout=outfile, stderr=subprocess.STDOUT)
            subprocesses.append(p)

          # run experience : density
        if "density" in target_strats:
          with open(root_res+xp_name+"_density.result", 'w+') as outfile:
            p = subprocess.Popen(["nice", nice_arg, "./"+sw+"_arch"+hw+"_sim.x", "-c", "-soracle", "-f../../../offline_placements/"+sw+"_arch"+hw+"_density.placement"], cwd=xp_root+"iss/", stdout=outfile, stderr=subprocess.STDOUT)
      	    subprocesses.append(p)

        if "density_50" in target_strats:
          with open(root_res+xp_name+"_density_50.result", 'w+') as outfile:
            p = subprocess.Popen(["nice", nice_arg, "./"+sw+"_arch"+hw+"_sim.x", "-c", "-soracle", "-f../../../offline_placements/"+sw+"_arch"+hw+"_density_50p.placement"], cwd=xp_root+"iss/", stdout=outfile, stderr=subprocess.STDOUT)
            subprocesses.append(p)

        if "density_85" in target_strats:
          with open(root_res+xp_name+"_density_85.result", 'w+') as outfile:
            p = subprocess.Popen(["nice", nice_arg, "./"+sw+"_arch"+hw+"_sim.x", "-c", "-soracle", "-f../../../offline_placements/"+sw+"_arch"+hw+"_density_85p.placement"], cwd=xp_root+"iss/", stdout=outfile, stderr=subprocess.STDOUT)
            subprocesses.append(p)

  # XP RUN START ILP_UPPER_BOUND
  if "ilp_upper_bound" in target_strats:
    for sw in target_sw:
      xp_name = sw+"_all_arch_run"
      xp_root = root_exec+xp_name+"/"
      for hw in target_hw:
        if hw not in ["0", "6"]:
          with open(root_res+xp_name+"_ilp_nofrag_arch_"+hw+".result", 'w+') as outfile:
            p = subprocess.Popen(["nice", nice_arg, "./"+sw+"_arch_nofrag_sim.x", "-c", "-soracle", "-f../../../offline_placements/"+sw+"_arch"+hw+"_ilp.placement"], cwd=xp_root+"iss/", stdout=outfile, stderr=subprocess.STDOUT)
            subprocesses.append(p)

  # WAIT XP END
  exit_codes = [p.wait() for p in subprocesses]

  for c in exit_codes:
    if c!=0 :
      print "ERROR in run, abording"
      sys.exit(1)

  # CLEANUP
  for sw in target_sw:
    for hw in target_hw:
      xp_name = sw+"_arch"+hw+"_run"
      xp_root = root_exec+xp_name+"/"
      p = subprocess.Popen(["rm","-r" , xp_root])
      p.wait()
      if p.returncode:
        print "ERROR can't clean up"
        sys.exit(1)
    # ilp upper_bound cleanup
    if "ilp_upper_bound" in target_strats:
      p = subprocess.Popen(["rm","-r" , root_exec+sw+"_all_arch_run/"])
      p.wait()
      if p.returncode:
        print "ERROR can't clean up"
        sys.exit(1)

  sys.exit(0)








if __name__ == "__main__":
    main()

