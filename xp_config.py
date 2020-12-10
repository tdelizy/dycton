#!/usr/bin/python

# Dycton experiments parameters
#==============================
# this file is written in python and used in the xp_preparation and xp_run scripts
# for more details see documentation

# what applications will be executed
# target_sw = ["json_parser", "dijkstra", "jpg2000" , "h263", "ecdsa"]
from __future__ import print_function
target_sw = ["json_parser", "dijkstra", "jpg2000" , "h263", "ecdsa"]

# on which architecture the application will be executed (for heap only)
# arch.       0		1		2		3		4		5		6
# % fast mem  100%	75%		50%		25%		10%		5%		0%
# % slow mem  0%	25%		50%		75%		90%		95%		100%
# target_hw = ["0", "1", "2", "3", "4", "5", "6"]
target_hw = ["0", "4", "5", "6"]

# targeted datasets (src/platform_tlm/software/$target_software/dataset/)
# target_datasets = [0, 1, 2, 3, 4, 5, 6, 7]
target_datasets = [0, 1]

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

# target_strats = ["baseline", "ilp", "ilp_upper_bound","ilp_50", "ilp_85", "density", "density_50", "density_85", "profile", "profile-enhanced", "profile-ilp"]
target_strats = ["baseline", "ilp", "ilp_upper_bound","ilp_50", "ilp_85", "density", "density_50", "density_85"]

# when exploring the static profile length influence or dynamic start value influence and you want more points
profile_high_resolution = False

# explore the influence on performance profile length
profile_explo = True

# optimal values by architecture/application couple
# results for fast=1/3 slow=2/30d .
# /!\ STUB VALUES FOR PROFILES STRAT DEV
profile_static_json_parser = { "1":8, "2":7, "3":6, "4":5, "5":5}
profile_static_dijkstra = { "1":3, "2":3, "3":3, "4":3, "5":1}
profile_static_jpg2000 = { "1":25, "2":25, "3":18, "4":7, "5":7}
profile_static_h263 = { "1":19, "2":18, "3":16, "4":15, "5":15}
profile_static_ecdsa = {"1":1, "2":1, "3":1, "4":1, "5":1}
profile_static_opt = {"json_parser":profile_static_json_parser, "dijkstra":profile_static_dijkstra, "jpg2000":profile_static_jpg2000, "h263":profile_static_h263, "ecdsa":profile_static_ecdsa}

# save all the heap log generated
save_heap_logs = True


def main():
  print("THIS FILE IS NOT INTENDED FOR EXECUTION.")
  print("please read the documentation and edit this file to configure experience preparation and run.")
  print("dumping current configuration:")
  print("target sw")
  print(target_sw)
  print("target hw")
  print(target_hw)
  print("target strats")
  print(target_strats)
  print("target datasets")
  print(target_datasets)
  print("profile_high_resolution: ", profile_high_resolution)
  print("profile_explo: ", profile_explo)
  print("")
  print("exiting...")
  exit(1)

if __name__ == "__main__":
    main()
