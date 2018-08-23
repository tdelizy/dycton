#!/usr/bin/python
import getopt, sys, subprocess, datetime, time

# get xp configuration 
import xp_config as xp


def main():
  # start by printing the date
  date = str(datetime.datetime.now())

  # Get memory latencies from address_map.h
  amap_lines = []
  with open("src/platform_tlm/address_map.h") as f:
    content = f.readlines()
  for line in content:
  	if "//" not in line and "#define MEM_" in line:
  		amap_lines.append(line[0:-1])

  # Xp description
  print "Dycton XP preparation script"
  print "============================"
  print "author: T. Delizy (tristan.delizy@insa-lyon.fr)"
  print date
  print "\n\nConfiguration:"
  print "=============="
  print "targeted softwares:"
  for s in xp.target_sw:
    print "\t",s
  print "targeted architectures:"
  print "\t",
  for h in xp.target_hw[0:-1]:
  	print h+",",
  print xp.target_hw[-1]
  print "targeted strategies for dynamic memory allocation:"
  for s in xp.target_strats:
    print "\t",s
  print "memory latencies (cycles) - src/platform_tlm/address_map.h:"
  for l in amap_lines:
  	print "\t",l

  print "\n\nStarting Xp preparation"
  print "======================="
  
  # Xp data folder creation
  print "> creating xp folder:",
  sys.stdout.flush()
  xp_folder = "dycton_xp_"+date+"/"
  log_folder = xp_folder+"logs/"
  print xp_folder, "...",
  subprocess.call(["mkdir", xp_folder])
  subprocess.call(["mkdir", log_folder])
  print "done."


  # Xp executables and architecture descriptors generation 
  print "> generating architecture descriptors and executable files ...",
  sys.stdout.flush()
  with open(log_folder+"exec_gen.log", 'w+') as outfile:
    p = subprocess.Popen(["python", "dycton_gen_exec.py", "../../"+xp_folder], cwd="src/scripts/", stdout=outfile, stderr=subprocess.STDOUT)
    p.wait()
    if p.returncode:
      print "\nERROR in executables generation, see log in", log_folder+"exec_gen.log"
      sys.exit(1)
  print "done."

  # offline placement generation
  print "> compute heap placements for offline strategies ...",
  sys.stdout.flush()
  with open(log_folder+"placement_gen.log", 'w+') as outfile:
    p = subprocess.Popen(["python", "dycton_gen_placement.py", "../../"+xp_folder], cwd="src/scripts/", stdout=outfile, stderr=subprocess.STDOUT)
    p.wait()
    if p.returncode:
      print "\nERROR in placement generation, see log in", log_folder+"placement_gen.log"
      sys.exit(1)
  print "done."

  # prepare sim env
  print "> prepare clean simulation environment ...",
  sys.stdout.flush()
  # create folders and copy input files
  subprocess.call(["mkdir", xp_folder+"clean_env"
    , xp_folder+"clean_env/iss"
    , xp_folder+"clean_env/logs"
    , xp_folder+"clean_env/software"])
  if "dijkstra" in xp.target_sw:
    subprocess.call(["mkdir", xp_folder+"clean_env/software/dijkstra"])
    subprocess.call(["cp", "src/platform_tlm/software/dijkstra/input.dat",
      "src/platform_tlm/software/dijkstra/LICENSE", 
      xp_folder+"clean_env/software/dijkstra/"])
  if "h263" in xp.target_sw:
    subprocess.call(["mkdir", xp_folder+"clean_env/software/h263"])
    subprocess.call(["cp", "src/platform_tlm/software/h263/COPYING",
      "src/platform_tlm/software/h263/input_base_4CIF_0to8.yuv",
      "src/platform_tlm/software/h263/input_base_4CIF_96bps.263",
      "src/platform_tlm/software/h263/input_small.yuv", 
      xp_folder+"clean_env/software/h263/"])
  if "jpeg" in xp.target_sw:
    subprocess.call(["mkdir", xp_folder+"clean_env/software/jpeg"])
    subprocess.call(["cp", "src/platform_tlm/software/jpeg/LICENSE",
      "src/platform_tlm/software/jpeg/input_large.jpg",
      "src/platform_tlm/software/jpeg/input_small.jpg",
      "src/platform_tlm/software/jpeg/input_large.ppm",
      "src/platform_tlm/software/jpeg/input_small.ppm", 
      xp_folder+"clean_env/software/jpeg/"])
  if "jpg2000" in xp.target_sw:
    subprocess.call(["mkdir", xp_folder+"clean_env/software/jpg2000"])
    subprocess.call(["cp", "src/platform_tlm/software/jpg2000/LICENSE", 
      xp_folder+"clean_env/software/jpg2000/"])
    subprocess.call(["cp", "src/platform_tlm/software/jpg2000/input_base_4CIF.pnm",
      "src/platform_tlm/software/jpg2000/input_base_4CIF.ppm",
      "src/platform_tlm/software/jpg2000/input_small.pnm", 
      xp_folder+"clean_env/software/jpg2000/"])
  if "json_parser" in xp.target_sw:
    subprocess.call(["mkdir", xp_folder+"clean_env/software/json_parser"
      , xp_folder+"clean_env/software/json_parser/datasets"])
    subprocess.call(["cp", "src/platform_tlm/software/json_parser/datasets/walking_dead_short.json", 
      xp_folder+"clean_env/software/json_parser/datasets/"])
  print "done."
  
  # setup run script and embedd configuration file
  print "> embedding run script and configuration file ...",  
  sys.stdout.flush()
  subprocess.call(["cp", "xp_config.py", "src/scripts/dycton_run_xp.py", xp_folder])
  print "done."
  print ""
  print ""
  print "Summary"
  print "======="
  print "preparation completed without errors"
  print "to run dycton simulation:"
  print "\t- go to", xp_folder
  print "\t- if running on another machine give execution rights to dycton_run_xp.py"
  print "\t- run dycton_run_xp.py script"
  print ""
  print "results will be stored inside xp folder."
  print "exiting..."
  sys.exit(0)








if __name__ == "__main__":
    main()



