#!/usr/bin/python

import getopt, sys, subprocess, datetime, time

sys.path.append('../../')

# executables generation script
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
    res_folder = args[-1]
  else:
    # create folder for result simulators and embedded softwares
    res_folder = "exec_gen_"+date
    subprocess.call(["mkdir", res_folder])

  subprocess.call(["mkdir", "emb_softs", "simulators", "archi_desc"], cwd = res_folder)



  try:
    import xp_config as xp
    print "import from config file OK."
  except:
    print "CONFIG IMPORT FAILED !"
    sys.exit(1)
    
  target_sw = xp.target_sw
  target_hw = xp.target_hw

  print "target_sw", target_sw
  print "target_hw", target_hw


  # build execs :
  for sw in target_sw:
    for hw in target_hw:
      arch_cmd = "DY_ARCH="+hw
      soft_cmd = "DY_SOFT="+sw
      sim_name = sw+"_arch"+hw+"_sim.x"
      soft_name = sw+"_arch"+hw+"_soft.out"
      desc_name = sw+"_arch"+hw+".desc"
      desc_name_50 = sw+"_arch"+hw+"_50p.desc"
      desc_name_85 = sw+"_arch"+hw+"_85p.desc"
      print ""
      print ""
      print "building", arch_cmd,",", soft_cmd
      p = subprocess.Popen(["make", "-B", arch_cmd, soft_cmd], cwd="../platform_tlm/iss/")
      p.wait()
      if p.returncode:
        print "ERROR in simulator compilation, aborting"
        sys.exit(1)

      # generate architecture description file associated
      p = subprocess.Popen(["./run.x", "-c"], cwd="../platform_tlm/iss/")
      time.sleep(2)
      p.kill()

      # retrieve the architecture description file
      p = subprocess.Popen(["mv", "../platform_tlm/logs/memory_architecture", res_folder+"archi_desc/"+desc_name])
      p.wait()
      if p.returncode:
        print "ERROR unable to move memory_architecture to", res_folder+"archi_desc/"+desc_name
        sys.exit(1)

      # build the 85% architecture description file
      if hw not in ["0", "6"]:
        with open(res_folder+"/archi_desc/"+desc_name, "r") as infile:
          with open(res_folder+"/archi_desc/"+desc_name_50, "w") as outfile:
            for line in infile:
              if line.split(":")[0] == "HEAP_0":
              	newline = line.split(":")[0]+":"+line.split(":")[1]+":"+str(int(round(int(line.split(":")[2])*0.85)))+":"+line.split(":")[3]+":"+line.split(":")[4]
              	outfile.write("%s" % newline )
              else:
              	outfile.write("%s" % line )

      # build the 50% architecture description file
      if hw not in ["0", "6"]:
        with open(res_folder+"/archi_desc/"+desc_name, "r") as infile:
          with open(res_folder+"/archi_desc/"+desc_name_85, "w") as outfile:
            for line in infile:
              if line.split(":")[0] == "HEAP_0":
              	newline = line.split(":")[0]+":"+line.split(":")[1]+":"+str(int(round(int(line.split(":")[2])*0.5)))+":"+line.split(":")[3]+":"+line.split(":")[4]
              	outfile.write("%s" % newline )
              else:
              	outfile.write("%s" % line )


      # retrieve the simulator exec
      p = subprocess.Popen(["mv", "../platform_tlm/iss/run.x", res_folder+"/simulators/"+sim_name])
      p.wait()
      if p.returncode:
        print "ERROR unable to move run.x to", res_folder+"/simulators"+sim_name
        sys.exit(1)

      # retrieve the embedded software
      p = subprocess.Popen(["mv", "../platform_tlm/software/a.out", res_folder+"/emb_softs/"+soft_name])
      p.wait()
      if p.returncode:
        print "ERROR unable to move a.out to", res_folder+"/simulators"+soft_name
        sys.exit(1)


  # build execs for ilp_nofrag
  for sw in target_sw:
    arch_cmd = "DY_ARCH=-2"
    soft_cmd = "DY_SOFT="+sw
    sim_name = sw+"_arch_nofrag_sim.x"
    soft_name = sw+"_arch_nofrag_soft.out"
    print ""
    print ""
    print "building", arch_cmd,",", soft_cmd
    p = subprocess.Popen(["make", "-B", arch_cmd, soft_cmd], cwd="../platform_tlm/iss/")
    p.wait()
    if p.returncode:
      print "ERROR in simulator compilation, aborting"
      sys.exit(1)

    # retrieve the simulator exec
    p = subprocess.Popen(["mv", "../platform_tlm/iss/run.x", res_folder+"/simulators/"+sim_name])
    p.wait()
    if p.returncode:
      print "ERROR unable to move run.x to", res_folder+"/simulators"+sim_name
      sys.exit(1)

    # retrieve the embedded software
    p = subprocess.Popen(["mv", "../platform_tlm/software/a.out", res_folder+"/emb_softs/"+soft_name])
    p.wait()
    if p.returncode:
      print "ERROR unable to move a.out to", res_folder+"/simulators"+soft_name
      sys.exit(1)

  print "generation successfully completed, exiting ..."
  sys.exit(0)








if __name__ == "__main__":
    main()

