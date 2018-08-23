#!/usr/bin/python

import sys, getopt
from random import randint



def main():
  # command line arguments processing
  try:
    opts, args = getopt.getopt(sys.argv[1:], ":", [""])
  except getopt.GetoptError as err:# print help information and exit:
    print str(err)  # will print something like "option -a not recognized"
    sys.exit(2)

  if(len(args)>1):
    print "too many args, just give a size of the targetted square matrix"
    sys.exit(2)

  s = int(args[0]) #get the matrix size and generates 2xnxn lines with one random integer

  for i in range(0, (2*s*s)):
    print randint(0,69535)

  sys.exit()





if __name__ == "__main__":
    main()