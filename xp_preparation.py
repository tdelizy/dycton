#!/usr/bin/python
from __future__ import print_function
import getopt, sys, subprocess, datetime, time

import numpy as np

# get xp configuration
import xp_config as xp


def main():
  # start by printing the date
  date = str(datetime.datetime.now())
  date = date.replace(" ", "_") # we don't want spaces anywhere in sub path

  # Get memory latencies from address_map.h
  amap_lines = []
  with open("src/platform_tlm/address_map.h") as f:
    content = f.readlines()
  for line in content:
    if "//" not in line and "#define MEM_" in line:
      amap_lines.append(line[0:-1])
      if "MEM_FAST_RLAT" in line:
        m_fast_rlat = int(line.split("(")[1].split(")")[0])
      if "MEM_FAST_WLAT" in line:
        m_fast_wlat = int(line.split("(")[1].split(")")[0])
      if "MEM_SLOW_RLAT" in line:
        m_slow_rlat = int(line.split("(")[1].split(")")[0])
      if "MEM_SLOW_WLAT" in line:
        m_slow_wlat = int(line.split("(")[1].split(")")[0])


  # Xp description
  print("Dycton XP preparation script")
  print("============================")
  print("author: T. Delizy (delizy.tristan@gmail.com)")
  print(date)
  print("\n\nConfiguration:")
  print("==============")
  print("targeted softwares:")
  for s in xp.target_sw:
    print("\t",s)
  print("\ntargeted architectures:")
  print("\t", end=' ')
  for h in xp.target_hw[0:-1]:
  	print(h+",", end=' ')
  print(xp.target_hw[-1])
  print("\ntargeted strategies for dynamic memory allocation:")
  for s in xp.target_strats:
    print("\t",s)
  print("\ntarget datasets:")
  print("\t", end=' ')
  for d in xp.target_datasets[0:-1]:
    print(str(d)+",", end=' ')
  print(xp.target_datasets[-1])
  print("\nheap memory latencies (cycles) - src/platform_tlm/address_map.h:")
  for l in amap_lines:
  	print("\t",l)

  profile_target_datasets = []
  if any(x in xp.target_strats for x in ["profile", "profile-enhanced", "profile-ilp"]):
    profile_target_datasets = [x for x in [0, 1, 2, 3, 4, 5, 6, 7] if x not in xp.target_datasets]
    print("\nprofile will be constructed on following datasets:")
    print("\t", end=' ')
    for d in profile_target_datasets[0:-1]:
      print(str(d)+",", end=' ')
    print(profile_target_datasets[-1])

  print("\n\nStarting Xp preparation")
  print("=======================")


  # Xp data folder creation
  print("> creating xp folder:", end=' ')
  sys.stdout.flush()
  xp_folder = "dycton_xp_"+date+"/"
  log_folder = xp_folder+"logs/"
  ref_exec_folder = xp_folder+"ref_exec/"
  ref_subproc = []
  print(xp_folder, "...", end=' ')
  subprocess.call(["mkdir", xp_folder])
  subprocess.call(["mkdir", log_folder])
  subprocess.call(["mkdir", ref_exec_folder])
  subprocess.call(["mkdir", xp_folder+"results/"])
  print("done.")

  # generating experiment architecture description files needed for density computation
  with open(ref_exec_folder+"target_memory", 'w+') as outfile:
    logline = "HEAP_0:0:0:"+str(m_fast_rlat)+":"+str(m_fast_wlat)+"\n"
    outfile.write("%s" % logline )
    logline = "HEAP_1:0:0:"+str(m_slow_rlat)+":"+str(m_slow_wlat)+"\n"
    outfile.write("%s" % logline )


  # prepare sim env
  print("> prepare clean simulation environment ...", end=' ')
  sys.stdout.flush()
  # create folders and copy input files
  subprocess.call(["mkdir", xp_folder+"clean_env"
                          , xp_folder+"clean_env/iss"
                          , xp_folder+"clean_env/logs"
                          , xp_folder+"clean_env/software"])
  subprocess.call(["cp", "src/platform_tlm/rng_sim.txt"
                       , xp_folder+"clean_env/"])
  if "dijkstra" in xp.target_sw:
    subprocess.call(["mkdir", xp_folder+"clean_env/software/dijkstra"])
    subprocess.call(["cp", "src/platform_tlm/software/dijkstra/LICENSE"
                         , xp_folder+"clean_env/software/dijkstra/"])
    subprocess.call(["cp", "-R", "src/platform_tlm/software/dijkstra/datasets"
                               , xp_folder+"clean_env/software/dijkstra/"])

  if "h263" in xp.target_sw:
    subprocess.call(["mkdir", xp_folder+"clean_env/software/h263"])
    subprocess.call(["cp", "src/platform_tlm/software/h263/COPYING"
                         , xp_folder+"clean_env/software/h263/"])
    subprocess.call(["cp", "-R", "src/platform_tlm/software/h263/datasets"
                               , xp_folder+"clean_env/software/h263/"])

  if "jpg2000" in xp.target_sw:
    subprocess.call(["mkdir", xp_folder+"clean_env/software/jpg2000"])
    subprocess.call(["cp", "src/platform_tlm/software/jpg2000/LICENSE"
                         , xp_folder+"clean_env/software/jpg2000/"])
    subprocess.call(["cp", "-R", "src/platform_tlm/software/jpg2000/datasets"
                               , xp_folder+"clean_env/software/jpg2000/"])

  if "jpeg" in xp.target_sw:
    subprocess.call(["mkdir", xp_folder+"clean_env/software/jpeg"])
    subprocess.call(["cp", "-R", "src/platform_tlm/software/jpeg/datasets"
                               , xp_folder+"clean_env/software/jpeg/"])

  if "json_parser" in xp.target_sw:
    subprocess.call(["mkdir", xp_folder+"clean_env/software/json_parser"])
    subprocess.call(["cp", "-R", "src/platform_tlm/software/json_parser/datasets"
                               , xp_folder+"clean_env/software/json_parser/"])

  if "ecdsa" in xp.target_sw:
    subprocess.call(["mkdir", xp_folder+"clean_env/software/ecdsa"])


  print("done.")


  # generating reference executions for experiments
  if xp.target_strats != ["baseline"]:
    print("> generating reference executions ...", end=' ')
    sys.stdout.flush()
    with open(log_folder+"ref_exec.log", 'w+') as outfile:

      # one reference execution by dataset
      # if profile strategy is involved, we also need to execute reference for profile computation
      if any(x in xp.target_strats for x in ["profile", "profile-enhanced", "profile-ilp"]):
        ref_exec_datasets = [0, 1, 2, 3, 4, 5, 6, 7]
      else:
        ref_exec_datasets = xp.target_datasets

      # create sw folders in reference execution
      for sw in xp.target_sw:
        sw_ref = ref_exec_folder+sw+"/"

        subprocess.call(["mkdir", sw_ref])
        for i in ref_exec_datasets:
          subprocess.call(["mkdir", sw_ref+"_d"+ str(i)])

        # build simulators and embedded softwares
        p = subprocess.Popen(["make", "-B", "DY_SOFT="+sw], cwd="src/platform_tlm/iss/", stdout=outfile, stderr=subprocess.STDOUT)
        p.wait()
        if p.returncode:
          print("ERROR in simulator compilation for reference executions, aborting")
          sys.exit(1)

        ref_sw_tmp_run = sw_ref+"tmp_run/"
        subprocess.call(["mkdir", ref_sw_tmp_run])


        for i in ref_exec_datasets:
          # create temporary execution directpries
          dataset_string = "_d" + str(i)
          dataset_sw_tmp_run = ref_sw_tmp_run + dataset_string+"/"
          subprocess.call(["mkdir", dataset_sw_tmp_run])
          p = subprocess.Popen(["cp -R "+xp_folder+"clean_env/* "+dataset_sw_tmp_run], shell=True)
          p.wait()
          if p.returncode:
            print("ERROR in reference execution setup, aborting")
            sys.exit(1)

          # move simulator and embedded soft to the reference execution temporary folder
          p = subprocess.Popen(["cp", "src/platform_tlm/iss/run.x", dataset_sw_tmp_run+"iss/"+sw+"_sim.x"])
          p.wait()
          if p.returncode:
            print("ERROR unable to move run.x to", dataset_sw_tmp_run+"iss/")
            sys.exit(1)
          p = subprocess.Popen(["cp", "src/platform_tlm/software/a.out", dataset_sw_tmp_run+"software/"])
          p.wait()
          if p.returncode:
            print("ERROR unable to move run.x to", dataset_sw_tmp_run+"software/")
            sys.exit(1)

          # chmod +x simulator
          subprocess.call(["chmod", "+x", dataset_sw_tmp_run+"iss/"+sw+"_sim.x"])

          # run reference execution
          with open(sw_ref+dataset_string+"/"+sw+dataset_string+"_run.result", 'w+') as resfile:
            p = subprocess.Popen(["./"+sw+"_sim.x", "-c", "-a", "0", "-d", str(i) ], cwd=dataset_sw_tmp_run+"iss/", stdout=resfile, stderr=subprocess.STDOUT)
            ref_subproc.append(p)

        # move symbol table and disassembly to ref_exec result folder
        p = subprocess.Popen(["mv", "src/platform_tlm/software/"+sw+"/cross/symbol_table.txt", "src/platform_tlm/software/"+sw+"/cross/a.asm", sw_ref])
        p.wait()
        if p.returncode:
          print("ERROR unable to move symbol table and disassembly to", sw_ref)
          sys.exit(1)

      # wait xp_end
      exit_codes = [p.wait() for p in ref_subproc]
      for c in exit_codes:
        if c!=0 :
          print("ERROR in reference execution run, abording")
          sys.exit(1)

      # retrieve results
      for sw in xp.target_sw:
        sw_ref = ref_exec_folder+sw+"/"
        ref_sw_tmp_run = sw_ref+"tmp_run/"
        for i in ref_exec_datasets:
          # create temporary execution directpries
          dataset_string = "_d" + str(i)
          dataset_sw_tmp_run = ref_sw_tmp_run + dataset_string + "/"
          p = subprocess.Popen(["mv", dataset_sw_tmp_run+"logs/heap_objects.log", sw_ref + dataset_string])
          p.wait()
          if p.returncode:
            print("ERROR unable to retrieve reference execution logs from", dataset_sw_tmp_run+"logs/")
            sys.exit(1)
        # cleanup
        p = subprocess.Popen(["rm","-r" , ref_sw_tmp_run])
        p.wait()
        if p.returncode:
          print("ERROR can't clean up"+ref_sw_tmp_run)
          sys.exit(1)
    print("done.")



  # profile computation on half of datasets (datasets that are not target dataset)
  if any(x in xp.target_strats for x in ["profile", "profile-enhanced", "profile-ilp"]):
    print("> compute profiles for online strategy...", end=' ')
    sys.stdout.flush()
    # create profile folder
    profile_folder = xp_folder+"profiles/"
    subprocess.call(["mkdir", profile_folder])

    with open(log_folder+"profile_gen.log", 'w+') as outfile:
      for sw in xp.target_sw:
        subprocess.call(["mkdir", profile_folder+sw+"/"])
        for i in profile_target_datasets: # /!\ not target datasets !
          dataset_string = "_d" + str(i)
          # compute profiles from reference executions
          dataset_sw_ref = ref_exec_folder+sw+"/"+dataset_string+"/"
          p = subprocess.Popen(["python", "obj_analysis.py", "-p",
            "-i","../../"+dataset_sw_ref+"/heap_objects.log",
            "-s","../../"+ref_exec_folder+sw+"/symbol_table.txt",
            "-m","../../"+ref_exec_folder+"target_memory"
            ], cwd="src/scripts/", stdout=outfile, stderr=subprocess.STDOUT)
          p.wait()
          if p.returncode:
            print("\nERROR in profile computation, see log in", log_folder+"profile_gen.log")
            sys.exit(1)

          # move results to profile folder
          p = subprocess.Popen(["mv", "src/scripts/"+sw+"_alloc_site.profile", profile_folder+sw+"/"+sw+"_alloc_site"+dataset_string+".profile"])
          p.wait()
          if p.returncode:
            print("ERROR unable to retrieve profile files for application", sw)
            sys.exit(1)


    print("done.")



  # Xp architecture descriptors  generation
  print("> generating architecture descriptors files ...", end=' ')
  sys.stdout.flush()
  with open(log_folder+"arch_gen.log", 'w+') as outfile:
    p = subprocess.Popen(["python", "dycton_gen_exec.py", "-o../../"+xp_folder, "-a"], cwd="src/scripts/", stdout=outfile, stderr=subprocess.STDOUT)
    p.wait()
    if p.returncode:
      print("\nERROR in architecture descriptors generation, see log in", log_folder+"arch_gen.log")
      sys.exit(1)
  print("done.")


  # offline placement generation and ilp profile computation
  if any(x in xp.target_strats for x in ["ilp","ilp_50","ilp_85","ilp_upper_bound","density","density_50","density_85","profile" "profile-enhanced", "profile-ilp"]):
    print("> compute heap placements for offline strategies ...", end=' ')
    sys.stdout.flush()
    with open(log_folder+"placement_gen.log", 'w+') as outfile:
      p = subprocess.Popen(["python", "dycton_gen_placement.py", "../../"+xp_folder], cwd="src/scripts/", stdout=outfile, stderr=subprocess.STDOUT)
      p.wait()
      if p.returncode:
        print("\nERROR in placement generation, see log in", log_folder+"placement_gen.log")
        sys.exit(1)
    print("done.")


  # header generation
  if any(x in xp.target_strats for x in ["profile", "profile-enhanced", "profile-ilp"]):
    print("> generate headers from profiles...", end=' ')
    sys.stdout.flush()
    with open(log_folder+"header_gen.log", 'w+') as outfile:
      for sw in xp.target_sw:
        sw_profile_folder = profile_folder+sw+"/"

        # generate profile headers
        p = subprocess.Popen(["python header_profile_gen.py ../../"+profile_folder+sw+"/*.profile"], cwd="src/scripts/", stdout=outfile, stderr=subprocess.STDOUT, shell=True)
        p.wait()
        if p.returncode:
          print("\nERROR in header generation, see log in", log_folder+"profile_gen.log")
          sys.exit(1)

        # move results to profile folder
        p = subprocess.Popen(["cp", "src/scripts/"+sw+"_alloc_site_profile.h", sw_profile_folder])
        p.wait()
        if p.returncode:
          print("ERROR unable to retrieve profile files for application", sw)
          sys.exit(1)

        # update profile headers in repo
        p = subprocess.Popen(["cp", sw_profile_folder+sw+"_alloc_site_profile.h", "src/platform_tlm/software/"+sw+"/"])
        p.wait()
        if p.returncode:
          print("ERROR unable to copy new profile header to repo :",sw_profile_folder+sw+"_alloc_site_profile.h")
          sys.exit(1)

    print("done.")



  # Xp executables  generation
  print("> generating executable files ...", end=' ')
  sys.stdout.flush()
  with open(log_folder+"exec_gen.log", 'w+') as outfile:
    p = subprocess.Popen(["python", "dycton_gen_exec.py", "-o../../"+xp_folder, "-x"], cwd="src/scripts/", stdout=outfile, stderr=subprocess.STDOUT)
    p.wait()
    if p.returncode:
      print("\nERROR in executables generation, see log in", log_folder+"exec_gen.log")
      sys.exit(1)
  print("done.")


  # setup run script and embedd configuration file
  print("> embedding run script and configuration file ...", end=' ')
  sys.stdout.flush()
  subprocess.call(["cp", "xp_config.py", "src/scripts/dycton_run_xp.py", xp_folder])
  print("done.")
  print("")
  print("")
  print("Summary")
  print("=======")
  print("preparation completed without errors")
  print("to run dycton simulation:")
  print("\t- go to", xp_folder)
  print("\t- if running on another machine give execution rights to dycton_run_xp.py")
  print("\t- run dycton_run_xp.py script")
  print("")
  print("results will be stored inside xp folder.")
  print("exiting...")
  sys.exit(0)








if __name__ == "__main__":
    main()
