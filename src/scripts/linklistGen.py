#!/usr/bin/python

import sys, getopt
import random



def main():
  # command line arguments processing
  try:
    opts, args = getopt.getopt(sys.argv[1:], ":", [""])
  except getopt.GetoptError as err:# print help information and exit:
    print str(err)  # will print something like "option -a not recognized"
    sys.exit(2)

  if len(args)!=2:
    print "usage : \n./linklistGen.py length baseaddr\n with length the number of nodes in the linked list and baseaddr, the target address of the first list element in the simulator."
    sys.exit(2)

  size = int(args[0]) #get the number of nodes
  base_addr = int(args[1]) #get the base address
  sizeof_int = 4 # ye, we're writings uint32_t in memory so pointing to next node adds 4 to @

  addr = []

  for i in range(0,size-1):
    addr.append(base_addr+(i+1)*2*sizeof_int)

  random.shuffle(addr)

  for i in range(0,size-1):
    print random.randint(0,69535)
    print addr[i]

  print 0
  print 0

  sys.exit()





if __name__ == "__main__":
    main()