#!/usr/bin/python

# Dycton experiments parameters
#==============================
# this file is written in python and used in the xp_preparation and xp_run scripts
# for more details see documentation

# what applications will be executed
# target_sw = ["jpeg", "json_parser", "dijkstra", "jpg2000" , "h263"]
target_sw = ["jpeg", "json_parser", "dijkstra", "jpg2000" , "h263"]

# on which architecture the application will be executed (for heap only)
# running architecture 6 as reference is mandatory
# arch.       0		1		2		3		4		5		6
# % fast mem  100%	75%		50%		25%		10%		5%		0%
# % slow mem  0%	25%		50%		75%		90%		95%		100%
# target_hw = ["0", "1", "2", "3", "4", "5", "6"] 
target_hw = ["0", "1", "2", "3", "4", "5", "6"] 

# what strategy for dynamic memory allocation will be used
#	baseline: "fast first", always try to allocate in fast, if no place, allocate in slow
#
#	ilp: offline ilp precomputed placement ignoring fragmentation, online strategy applies 
#		placement, allocate in slow if no place in fast
#
#	ilp_upper_bound: same as ilp but executed on infinite size memory banks: applies placement
#		without failing due to heap fragmentation
#
#	ilp_50: offline placement ilp precomputed on heap 50% smaller to compensate for fragmentation
#
#	ilp_85: offline placement ilp precomputed on heap 15% smaller to compensate for fragmentation
#
#	density: offline precomputed placement ignoring fragmentation, online strategy applies 
#		placement, allocate in slow if no place in fast
#
#	density_50: offline placement precomputed on heap 50% smaller to compensate for fragmentation
#
#	density_85: offline placement precomputed on heap 15% smaller to compensate for fragmentation
#
# for more details about strategies refer to documentation / paper

# target_strats = ["baseline", "ilp", "ilp_upper_bound","ilp_50", "ilp_85", "density", "density_50", "density_85"]
target_strats = ["baseline", "ilp", "ilp_upper_bound","ilp_50", "ilp_85", "density", "density_50", "density_85"]



def main():
  print "THIS FILE IS NOT INTENDED FOR EXECUTION."
  print "please read the documentation and edit this file to configure experience preparation and run."
  print "dumping current configuration:"
  print "target sw"
  print target_sw
  print "target hw"
  print target_hw
  print "target strats"
  print target_strats
  print ""
  print "exiting..."
  exit(1)

if __name__ == "__main__":
    main()



