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


def profile_len_explored(name):
  if "profile" not in name:
    return -3
  selstr = name.split("profile")[1]
  sel_nbr = [int(s) for s in selstr.split("_") if s.isdigit()]
  if len(sel_nbr) > 1:
    print("ambiguous profile exploration experiment run name :", name)
    return -2
  elif len(sel_nbr) == 0:
    return -1 # not a profile exploration
  else:
    return sel_nbr[0]


def usage():
  print("usage:")
  print("./result_synthesis.py -i result_folder_path")
  print("-i path : input path to result folder (all files inside will be read as results)")
  print("-h, --help: printing help")
  print("")
  print("constructs a CSV file containing information regarding experiment executions")
  print("each line describe one execution of the simulator (name, exec time, return value, ...)")
  print("for strategies implying profile exploration, best profile length is selected.")


def main():
  # start by printing the date
  date_start = str(datetime.datetime.now())
  error = 0
  print("Result synthesis (CSV) construction from experiment logs")
  print("========================================================")
  print("author: T. Delizy (delizy.tristan@gmail.com)")
  print(date_start)

  prof_explo_res = {}

  try:
    opts, args = getopt.getopt(sys.argv[1:], "hi::", ["help"])
  except getopt.GetoptError as err:# print help information and exit:
    print(str(err))  # will print something like "option -a not recognized"
    sys.exit(2)
  input_path=""

  for o, a in opts:
    # print "option=", o, " arg=", a
    if o in ("-i"):
      input_path = a
      print("input path:", input_path)
    elif o in ("-h", "--help"):
      usage()
      sys.exit()
    else:
      print("unhandled option :", o, a)
      sys.exit(1)

  if(input_path==""):
    print("ERROR: no input path! (-iinput_path)")
    sys.exit(1)

  # get the result file liste (its called "ls")
  p = subprocess.Popen(["ls", input_path], stdout=subprocess.PIPE, encoding="utf8")
  p.wait()
  if p.returncode:
    print("ERROR when trying to list the result files.", xp_name)
    sys.exit(1)

  with open(input_path+"../results/result_synthesis.csv", 'w+') as outfile:
    outfile.write("#experience;return_code;final cycles;allocator cycles;fallback\n")
    for line in p.stdout:
      if ".result" not in line:
        continue

      name = line.split("\n")[0].split('.result')[0]
      # print "line from ls:", line
      # print name

      output = []
      with open(input_path+line.split("\n")[0]) as resfile: # strip line ending
        return_code = "-1"
        final_cycle = "-1"
        alloc_cycle = "-1"
        alloc_fallback = "-1"

        for resline in resfile:
          alloc_error = False
          if "[helper]" in resline:
            return_code = resline.split(" = ")[1][0:-1]
            # print "\treturn code =", return_code
          if "final cycle count" in resline:
            final_cycle = resline.split(": ")[1][0:-2]
            # print "\tfinal cycles =", final_cycle
          if "total allocator cycle count" in resline:
            alloc_cycle = resline.split(": ")[1].split(" ")[0]
            # print "\talloc cycles", alloc_cycle
          if "malloc fallback" in resline:
            alloc_fallback = resline.split("(")[1].split("%")[0]
          if "outside of heap" in resline:
            alloc_error = True


      if return_code == "-1" or final_cycle == "-1" or alloc_cycle == "-1" or alloc_fallback == "-1":
        print("ERROR in data for", name, "see log")
        error +=1
      elif return_code != "0":
        print("ERROR in run for", name, "see log")
        error +=1
      elif alloc_error:
        print("ERROR in allocation for", name, "see log")
        error +=1
      else:
        print("\t", name, "ok.")

      csvline = name+";"+return_code+";"+final_cycle+";"+alloc_cycle+";"+alloc_fallback

      if "profile" in name:
        l = profile_len_explored(name)
        if l in {-2, -3}:
          error +=1
        ref = name.split("profile")[0]
        if ref not in prof_explo_res:
          prof_explo_res[ref] = {csvline}
        else:
          prof_explo_res[ref].add(csvline)
      else:
        # print "[CSVLINE]",csvline
        outfile.write(csvline+"\n")

    # add profile exploration results at the end
    for key in prof_explo_res:
      candidates = prof_explo_res[key]
      res = sys.maxsize - 1
      for s in candidates:
        if int(s.split(";")[1]) == 0 and int(s.split(";")[2]) < res:
          res = int(s.split(";")[2])
          csvline = key + "profile_run;"+';'.join(s.split(";")[1:])
      outfile.write(csvline+"\n")


  if error:
    print(error, "errors in experiment.")
  else:
    print("success : experiment run without errors.")
  sys.exit(error)








if __name__ == "__main__":
    main()
