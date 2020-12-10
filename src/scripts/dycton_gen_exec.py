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
import getopt, sys, subprocess, datetime, time

sys.path.append('../../')


def usage():
  print("\n================================================================================")
  print("Usage")
  print("================================================================================")
  print("-o \"path\": experiment folder")
  print("-a : only copy to destination generated architecture description files")
  print("-x : only copy to destination generated executables")
  print("note : if you specify neither '-a' nor '-x' both will be generated and moved into destination folder")
  print("exiting...")


# executables generation script
def main():
  # start by printing the date
  date = str(datetime.datetime.now())
  print("Executables and memory architecture description files generation for Dycton")
  print("====================================================================================================")
  print("author: T. Delizy (delizy.tristan@gmail.com)")
  print(date)

  print("> command line arguments processing ... ", end=' ')
  try:
    opts, args = getopt.getopt(sys.argv[1:], 'o:hax', ['help'])
    # print "opts :", opts
    # print "args :", args
  except getopt.GetoptError as err:# print help information and exit:
    print(str(err))  # will print something like "option -a not recognized"
    sys.exit(2)

  arch_desc_only = False
  exec_only = False
  res_folder = ""

  for o, a in opts:
    if o in ("-o"):
      res_folder = a
    elif o in ("-h", "--help"):
      usage()
      sys.exit(0)
    elif o in ("-a"):
      arch_desc_only = True
    elif o in ("-x"):
      exec_only = True

  if res_folder == "" :
    print("\n/!\\no destination folder provided (try with -o xp_folder_path)")
    print("exiting...")
    exit(1)

  if arch_desc_only and exec_only :
    print("\n/!\\'-a' and '-x' option are exclusives.")
    print("exiting...")
    exit(1)


  print("done.\n")
  print("fucking res fucking folder")
  print(res_folder)
  subprocess.call(["ls", res_folder])
  subprocess.call(["mkdir", "emb_softs"], cwd = res_folder)
  subprocess.call(["mkdir", "simulators"], cwd = res_folder)
  subprocess.call(["mkdir", "archi_desc"], cwd = res_folder)


  # preparing subfolder results
  # if folder already exists, ignore
  # try:
  #   subprocess.call(["mkdir", "emb_softs"], cwd = res_folder)
  # except:
  #   pass
  # try:
  #   subprocess.call(["mkdir", "simulators"], cwd = res_folder)
  # except:
  #   pass
  # try:
  #   subprocess.call(["mkdir", "archi_desc"], cwd = res_folder)
  # except:
  #   pass


  try:
    import xp_config as xp
    print("import from config file OK.")
  except:
    print("CONFIG IMPORT FAILED !")
    sys.exit(1)

  target_sw = xp.target_sw
  target_hw = xp.target_hw
  if any(x in xp.target_strats for x in ["profile", "profile-enhanced", "profile-ilp"]):
    target_datasets = [0,1,2,3,4,5,6,7]
  else:
    target_datasets = xp.target_datasets

  print("target_sw", target_sw)
  print("target_hw", target_hw)


  # build execs :
  for sw in target_sw:
    soft_cmd = "DY_SOFT="+sw
    sim_name = sw+"_sim.x"
    soft_name = sw+"_soft.out"
    print("")
    print("")
    print("building,", soft_cmd)
    p = subprocess.Popen(["make", "-B", soft_cmd], cwd="../platform_tlm/iss/")
    p.wait()
    if p.returncode:
      print("ERROR in simulator compilation, aborting")
      sys.exit(1)

    if exec_only == False:
      for hw in target_hw:
        for d in target_datasets:
          desc_name = sw+"_arch"+hw+"_d"+str(d)+".desc"
          desc_name_50 = sw+"_arch"+hw+"_d"+str(d)+"_50p.desc"
          desc_name_85 = sw+"_arch"+hw+"_d"+str(d)+"_85p.desc"

          # generate architecture description file associated
          p = subprocess.Popen(["./run.x", "-c", "-a", hw, "-d", str(d)], cwd="../platform_tlm/iss/")
          time.sleep(2)
          p.kill()

          # retrieve the architecture description file
          p = subprocess.Popen(["mv", "../platform_tlm/logs/memory_architecture", res_folder+"archi_desc/"+desc_name])
          p.wait()
          if p.returncode:
            print("ERROR unable to move memory_architecture to", res_folder+"archi_desc/"+desc_name)
            sys.exit(1)

          # build the 85% architecture description file
          if hw not in ["0", "6"]:
            with open(res_folder+"/archi_desc/"+desc_name, "r") as infile:
              with open(res_folder+"/archi_desc/"+desc_name_85, "w") as outfile:
                for line in infile:
                  if line.split(":")[0] == "HEAP_0":
                    newline = line.split(":")[0]
                    newline += ":"+line.split(":")[1]+":"
                    newline += str(int(round(int(line.split(":")[2])*0.85)))
                    newline += ":"+line.split(":")[3]+":"+line.split(":")[4]
                    outfile.write("%s" % newline )
                  else:
                    outfile.write("%s" % line )

          # build the 50% architecture description file
          if hw not in ["0", "6"]:
            with open(res_folder+"/archi_desc/"+desc_name, "r") as infile:
              with open(res_folder+"/archi_desc/"+desc_name_50, "w") as outfile:
                for line in infile:
                  if line.split(":")[0] == "HEAP_0":
                    newline = line.split(":")[0]+":"+line.split(":")[1]+":"+str(int(round(int(line.split(":")[2])*0.5)))+":"+line.split(":")[3]+":"+line.split(":")[4]
                    outfile.write("%s" % newline )
                  else:
                    outfile.write("%s" % line )

    if arch_desc_only == False:
      # retrieve the simulator exec
      p = subprocess.Popen(["mv", "../platform_tlm/iss/run.x", res_folder+"/simulators/"+sim_name])
      p.wait()
      if p.returncode:
        print("ERROR unable to move run.x to", res_folder+"/simulators"+sim_name)
        sys.exit(1)

      # retrieve the embedded software
      p = subprocess.Popen(["mv", "../platform_tlm/software/a.out", res_folder+"/emb_softs/"+soft_name])
      p.wait()
      if p.returncode:
        print("ERROR unable to move a.out to", res_folder+"/simulators"+soft_name)
        sys.exit(1)

      # retrieve the symbol table for profile computation
      p = subprocess.Popen(["cp", "../platform_tlm/software/"+sw+"/cross/symbol_table.txt", res_folder+"emb_softs/"+sw])
      p.wait()
      if p.returncode:
        print("ERROR unable to move symbol table to", res_folder+"emb_softs/"+soft_name)
        sys.exit(1)

  print("generation successfully completed, exiting ...")
  sys.exit(0)








if __name__ == "__main__":
    main()
