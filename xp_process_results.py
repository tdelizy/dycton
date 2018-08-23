#!/usr/bin/python

import getopt, sys, subprocess, datetime, os

def main():
  # start by printing the date
  date_start = str(datetime.datetime.now())

  print "Dycton XP result processing script"
  print "=================================="
  print "author: T. Delizy (tristan.delizy@insa-lyon.fr)"
  print date_start
  print ""

  # command line arguments processing
  try:
    opts, args = getopt.getopt(sys.argv[0:], "", [""])
  except getopt.GetoptError as err:# print help information and exit:
    print str(err)  # will print something like "option -a not recognized"
    sys.exit(2)

  if(len(args)>1):
    print "target experience folder:", args[-1]
    if args[-1][-1] == "/":
      xp_folder = args[-1]
    else:
      xp_folder = args[-1]+"/"
  else:
    print "ERROR: you must provide the target experience folder in parameter."
    print "exiting..."
    sys.exit(1)

  # get xp configuration 
  try:
    import xp_config as xp
    print "config file loaded."
  except:
    print "dycton run xp error: cannot load xp_config.py"
    print "exiting..."
    sys.exit(1)
  print ""

  # create result folder
  subprocess.call(["mkdir", xp_folder+"results"])

  # result synthesis into .csv
  print "> result synthesis...",
  sys.stdout.flush()
  with open(xp_folder+"logs/result_synthesis.log", 'w+') as outfile:
    p = subprocess.Popen(["python", "dycton_result_synthesis.py", "-i../../"+xp_folder+"results_raw/"], cwd="src/scripts/", stdout=outfile, stderr=subprocess.STDOUT)
    p.wait()
    if p.returncode:
      print "\nERROR in result synthesis, see log in", xp_folder+"logs/result_synthesis.log"
      sys.exit(1)
  print "done."


  # xp graph plot
  print "> plotting graphs...",
  sys.stdout.flush()
  with open(xp_folder+"logs/plot.log", 'w+') as outfile:
    p = subprocess.Popen(["python", "dycton_plot.py", "../../"+xp_folder+"results/result_synthesis.csv"], cwd="src/scripts/", stdout=outfile, stderr=subprocess.STDOUT)
    p.wait()
    if p.returncode:
      print "\nERROR in graph plot, see log in", xp_folder+"logs/plot.log"
      sys.exit(1)
  print "done."

  print "finished without error, exiting..."
  sys.exit(0)






if __name__ == "__main__":
    main()



