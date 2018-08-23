#!/usr/bin/python

import getopt, sys, subprocess, datetime


def main():
  # start by printing the date
  date_start = str(datetime.datetime.now())
  print date_start

  try:
    opts, args = getopt.getopt(sys.argv[1:], "hi::", ["help"])
  except getopt.GetoptError as err:# print help information and exit:
    print str(err)  # will print something like "option -a not recognized"
    sys.exit(2)
  input_path=""

  for o, a in opts:
    # print "option=", o, " arg=", a
    if o in ("-i"):
      input_path = a
      print "input path:", input_path
    elif o in ("-h", "--help"):
      print "-i path : input path to result folder (all file inside will be read as results"
      sys.exit()
    else:
      print "unhandled option :", o, a
      sys.exit(1)

  if(input_path==""):
    print "ERROR: no input path! (-iinput_path)"
    sys.exit(1)

  # get the result file liste (its called "ls")
  p = subprocess.Popen(["ls", input_path], stdout=subprocess.PIPE)
  p.wait()
  if p.returncode:
    print "ERROR when trying to list the result files.", xp_name
    sys.exit(1)

  with open(input_path+"../results/result_synthesis.csv", 'w') as outfile:
    outfile.write("#experience;return_code;final cycles;allocator cycles\n")
    for line in p.stdout:
      name = line.split('.')[0]
      # print "line from ls:", line
      # print name

      output = []
      with open(input_path+line.split("\n")[0]) as resfile: # strip line ending
        for resline in resfile:
        	if "[helper]" in resline:
        	  return_code = resline.split(" = ")[1][0:-1]
        	  # print "\treturn code =", return_code
        	if "final cycle count" in resline:
        	  final_cycle = resline.split(": ")[1][0:-2]
        	  # print "\tfinal cycles =", final_cycle
        	if "total allocator cycle count" in resline:
        	  alloc_cycle = resline.split(": ")[1].split(" ")[0]
        	  # print "\talloc cycles", alloc_cycle

      print "adding", name
      csvline = name+";"+return_code+";"+final_cycle+";"+alloc_cycle
      print "[CSVLINE]",csvline
      outfile.write(csvline+"\n")

  sys.exit(0)








if __name__ == "__main__":
    main()

