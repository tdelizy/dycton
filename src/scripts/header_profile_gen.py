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



import os, sys, getopt, datetime

import numpy as np

end_of_macro_line = "\\\n"
static_preamble = "//automatically generated during each build, please see src/scripts/header_profile_gen.py"
alloc_and_fallback = "if(l < prof_len){\\\n\
\talloc_context = &(multi_heap_ctx[0]);\\\n\
\treturnPointer = newlib_malloc_r(_impure_ptr, sz);\\\n\
\tif (!returnPointer) {\\\n\
\t\tALLOC_INTERNAL_FAIL(0);\\\n\
\t\talloc_context = &(multi_heap_ctx[1]);\\\n\
\t\treturnPointer = newlib_malloc_r(_impure_ptr, sz);\\\n\
\t}\\\n\
}else{\\\n\
\talloc_context = &(multi_heap_ctx[1]);\\\n\
\treturnPointer = newlib_malloc_r(_impure_ptr, sz);\\\n\
\tif (!returnPointer) {\\\n\
\t\tALLOC_INTERNAL_FAIL(0);\\\n\
\t\talloc_context = &(multi_heap_ctx[0]);\\\n\
\t\treturnPointer = newlib_malloc_r(_impure_ptr, sz);\\\n\
\t}\\\n\
}\\\n\
}while(0)"
alloc_and_fallback_no_variable_len = "if(l < 0xFFFF){\\\n\
\talloc_context = &(multi_heap_ctx[0]);\\\n\
\treturnPointer = newlib_malloc_r(_impure_ptr, sz);\\\n\
\tif (!returnPointer) {\\\n\
\t\tALLOC_INTERNAL_FAIL(0);\\\n\
\t\talloc_context = &(multi_heap_ctx[1]);\\\n\
\t\treturnPointer = newlib_malloc_r(_impure_ptr, sz);\\\n\
\t}\\\n\
}else{\\\n\
\talloc_context = &(multi_heap_ctx[1]);\\\n\
\treturnPointer = newlib_malloc_r(_impure_ptr, sz);\\\n\
\tif (!returnPointer) {\\\n\
\t\tALLOC_INTERNAL_FAIL(0);\\\n\
\t\talloc_context = &(multi_heap_ctx[0]);\\\n\
\t\treturnPointer = newlib_malloc_r(_impure_ptr, sz);\\\n\
\t}\\\n\
}\\\n\
}while(0)\n\n"



app = ""

def usage():
  print "\n================================================================================"
  print "Usage"
  print "================================================================================"
  print "pass profiles to aggregate to generate header"
  print "caution : profiles names containing \"ilp\" will be used to enhance profile and generates ilp-profile"
  print "only files.profiles taken into account, but all of them, no check on xp_config"
  print "-h --help : this."
  print "exiting..."


def main():
  global app

  date_start = str(datetime.datetime.now())
  profiles = []
  profiles_ilp = []
  profile_header = ""

  print "Allocation site profile header generation for Dycton"
  print "===================================================================================================="
  print "author: T. Delizy (delizy.tristan@gmail.com)"
  print date_start

  # command line arguments processing
  try:
    opts, args = getopt.getopt(sys.argv[1:], 'h', ['help'])
    # print "opts =", opts
    # print "args =", args
  except getopt.GetoptError as err:# print help information and exit:
    print str(err)  # will print something like "option -a not recognized"
    usage()
    exit(2)

  for o, a in opts:
    # print "o=", o
    # print "a=", a
    if o in ("-h", "--help"):
      print "invoking help"
      usage()
      sys.exit(0)

  for a in args:
    if ".profile" not in a:
      print "what is", a, "(ignored)"
    else:
      if "ilp" in a:
        profiles_ilp.append(a)
      else:
        profiles.append(a)
  
  if profiles == []:
    print "you must provide input profile file, exiting..."
    exit(1)
  if "json_parser" in profiles[0]:
    app = "json_parser"
  elif "dijkstra" in profiles[0]:
    app = "dijkstra"
  elif "jpg2000" in profiles[0]:
    app = "jpg2000"
  elif "h263" in profiles[0]:
    app = "h263"
  elif "ecdsa" in profiles[0]:
    app = "ecdsa"
  elif "jpeg" in profiles[0]:
    app = "jpeg"
  else:
    print "application not recognized, see header_profile_gen.py"
    sys.exit(1)

  profile_header = app+"_alloc_site_profile.h"
  
  print "\n\ttarget application:", app
  print "\n\toutput header:", profile_header
  print "\n\tinput profiles (", len(profiles), ")"
  print profiles
  print "\n\tinput ilp profiles (", len(profiles_ilp), ")"
  print profiles_ilp
  print "- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - \n\n"

  profile_max_len = 0
  alloc_site_dict = {}
  alloc_site_dict_ilp = {}
  site_addr_list = []
  logline = ""

  print "> Reading input profiles into a dict...",
  sys.stdout.flush()
  # 4196644 2.668425558610061 15.972159613408104  (get_quoted_string)
  for p in profiles:
    with open(p, "r") as infile:
      for line in infile:
        ad = int(line.split("\t")[0])
        den = float(line.split("\t")[1])
        if alloc_site_dict.has_key(ad):
          alloc_site_dict[ad]["list"].append(den)
        else:
          alloc_site_dict[ad] = {"list" :[den], "metric": 0, "index":0}

  profile_max_len = len(alloc_site_dict.keys())

  print "done."
  print "> Reading ilp profiles into a dict...",
  sys.stdout.flush()
  # 4196644 2.668425558610061 15.972159613408104  (get_quoted_string)
  for p in profiles_ilp:
    with open(p, "r") as infile:
      arch = p.split("_arch")[1].split("_")[0]
      for line in infile:
        ad = int(line.split(";")[0])
        alloc_percent = float(line.split(";")[1].strip("\n"))
        if alloc_site_dict_ilp.has_key(arch):
          if alloc_site_dict_ilp[arch].has_key(ad):
            alloc_site_dict_ilp[arch][ad]["list"].append(alloc_percent)
          else:
            alloc_site_dict_ilp[arch][ad] = {"list" :[alloc_percent], "90percentile": 0, "10percentile": 0}
        else:
          alloc_site_dict_ilp[arch] = {ad: {"list" :[alloc_percent], "90percentile": 0, "10percentile": 0}}

  arch_covered = alloc_site_dict_ilp.keys()
  arch_stubs = [x for x in ["1", "2", "3", "4", "5"] if x not in arch_covered]

  print "done."
  print "> Aggregate profiles,",
  # # max density on different datasets profiles
  # print "metric: max...",
  # for k, v in alloc_site_dict.iteritems():
  #   v["metric"] = np.max(v["list"])

  # # median density on different datasets profiles
  # print "metric: median...",
  # for k, v in alloc_site_dict.iteritems():
  #   v["metric"] = np.median(v["list"])

  # # average density on different datasets profiles
  # print "metric: average...",
  # for k, v in alloc_site_dict.iteritems():
  #   v["metric"] = np.average(v["list"])

  # max density on different datasets profiles
  print "metric: 90 percentile...",
  for k, v in alloc_site_dict.iteritems():
    v["metric"] = np.percentile(v["list"], 90)
  print "done."

  print "\tmax length of profile :", profile_max_len
  print "\tordered aggregated alloc site frequencies per byte:"

  for i, site in enumerate(sorted(alloc_site_dict.items(), key=lambda x: x[1]["metric"], reverse=True)):
    print "\t",site[0], ":", site[1]["metric"]
    site_addr_list.append(site[0])
    alloc_site_dict[site[0]]["index"] = i

  print "> aggregate ilp profiles,",
  for a in alloc_site_dict_ilp.keys():
    for k, v in alloc_site_dict_ilp[a].iteritems():
      v["90percentile"] = np.percentile(v["list"], 90)
      v["10percentile"] = np.percentile(v["list"], 10)

  print "done."

  good_list_by_archi = {}
  bad_list_by_archi = {}
  enhanced_profile_by_archi = {}
  ilp_profile_by_archi = {}
  for a in alloc_site_dict_ilp.keys():
    print "  Architecture", a, "- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -"
    good_list_by_archi[a] = []
    bad_list_by_archi[a] = []
    enhanced_profile_by_archi[a] = []
    ilp_profile_by_archi[a] = []
    # for k, v in alloc_site_dict_ilp[a].iteritems():
    #   print "\tsite :", k
    #   print "\t\tlist :", v["list"]
    #   print "\t\tup value :", v["90percentile"]
    #   print "\t\tlow value :", v["10percentile"]

    # for each architecture, extract "100%" list (good) and "0%" list (bad)
    for k, v in alloc_site_dict_ilp[a].iteritems():
      if v["90percentile"] == 100.0:
        good_list_by_archi[a].append(k)
      if v["10percentile"] == 0.0:
        bad_list_by_archi[a].append(k)

    # order good list site by access frequency per byte : 
    good_list_by_archi[a].sort(key=lambda x: alloc_site_dict[x]["metric"], reverse=True)


    print "\tgood list (100% up value) ", len(good_list_by_archi[a]), ":"
    print "\t\t",good_list_by_archi[a]
    print "\tbad list (0% low value) ", len(bad_list_by_archi[a]), ":"
    print "\t\t",bad_list_by_archi[a]
    print "\tintersection"
    print "\t\t",[x for x in good_list_by_archi[a] if x in bad_list_by_archi[a]]

    # enhance profile with ilp info by architecture
    # meaning that:
    #   - sites wich have up value to 100% are at start of profile
    #   - sites wich have low value to 0% are out of profile
    # starting from the 100% list, then adding stuf if not in bad list
    enhanced_profile_by_archi[a] = list(good_list_by_archi[a])
    for s in site_addr_list : 
      if s not in enhanced_profile_by_archi[a] and s not in bad_list_by_archi[a]:
        enhanced_profile_by_archi[a].append(s)

    # caution, profile can't be empty : if so, taking the highest site in original profile
    if len(enhanced_profile_by_archi[a]) == 0:
      enhanced_profile_by_archi[a].append(site_addr_list[0])

    print "\tenhanced profile (", len(enhanced_profile_by_archi[a]), ")"
    print "\t\t",enhanced_profile_by_archi[a]

    # construct a profile with ilp info by architecture
    # meaning that:
    #   - sites wich have up value to 100% are in the profile
    #   - thats all
    ilp_profile_by_archi[a] = list(good_list_by_archi[a])

    # caution, profile can't be empty : if so, taking the highest site in original profile
    if len(ilp_profile_by_archi[a]) == 0:
      ilp_profile_by_archi[a].append(site_addr_list[0])
 
    print "\tilp profile (", len(ilp_profile_by_archi[a]), ")"
    print "\t\t",ilp_profile_by_archi[a]



  print "> Writing result into header file...",
# PREAMBLE ======================================================================================
  # write header
  with open(profile_header, "w") as outfile:
    # static preamblule
    outfile.write("%s" % static_preamble )
    outfile.write("%s" % "\n" )

    # define addresses
    for i, site in enumerate(site_addr_list):
      logline = "#define __APP_ALLOC_SITE_"+str(i)+" ("+str(site)+")\n"
      outfile.write("%s" % logline )

    # define the max profile length
    logline = "#define __APP_ALLOC_SITE_LENGTH ("+str(len(site_addr_list))+")\n"
    outfile.write("%s" % logline )

    # define guards (deactivated by default)
    logline = "// #define PROFILE_GARDS\n"
    outfile.write("%s" % "\n" )
    outfile.write("%s" % logline )
    logline = "#ifdef PROFILE_GARDS\n"
    outfile.write("%s" % logline )

    logline = "\t#define ASSERT_PROFILE_CONSISTENCY(a) do{ if(!("
    for i, site in enumerate(site_addr_list):
      logline += "a == __APP_ALLOC_SITE_"+str(i)
      if i < (len(site_addr_list)-1):
        logline += " || "
      else:
        logline += ")){print(\"ERROR : caller @ not in profile !!\");exit(a);} }while(0)\n"
    outfile.write("%s" % logline )
    logline = "#else\n"
    outfile.write("%s" % logline )
    logline = "\t#define ASSERT_PROFILE_CONSISTENCY(a) do{}while(0)\n"
    outfile.write("%s" % logline )
    logline = "#endif\n"
    outfile.write("%s" % logline )

# ALLOCATION SITE PROFILE =======================================================================
    # define switch case macro
    outfile.write("%s" % "\n" )
    logline = "#define GENERATED_SWITCH_PROFILE(addr, prof_len) do { " + end_of_macro_line
    outfile.write("%s" % logline )
    logline = "uint32_t l = 0xFFFF;" + end_of_macro_line
    outfile.write("%s" % logline )
    logline = "switch (addr) {" + end_of_macro_line
    outfile.write("%s" % logline )
    for i, site in enumerate(site_addr_list):
      logline = "case __APP_ALLOC_SITE_"+str(i)+":"+end_of_macro_line
      outfile.write("%s" % logline )
      logline = "\tl = "+str(i)+";"+ end_of_macro_line+"\tbreak;" + end_of_macro_line
      outfile.write("%s" % logline )

    #switch end
    logline = "}"+ end_of_macro_line
    outfile.write("%s" % logline )

    # check for profile length, macro static end
    logline = alloc_and_fallback
    outfile.write("%s" % logline )

# ENHANCED PROFILE BY ARCHITECTURE ==============================================================
    for a in alloc_site_dict_ilp.keys():
      # define switch case macro
      outfile.write("%s" % "\n\n" )
      logline = "#define GEN_SW_PROF_ENHANCED_A"+str(a)+"(addr, prof_len) do { " + end_of_macro_line
      outfile.write("%s" % logline )
      logline = "uint32_t l = 0xFFFF;" + end_of_macro_line
      outfile.write("%s" % logline )
      logline = "switch (addr) {" + end_of_macro_line
      outfile.write("%s" % logline )
      for i, site in enumerate(enhanced_profile_by_archi[a]):
        index = alloc_site_dict[site]["index"]
        logline = "case __APP_ALLOC_SITE_"+str(index)+":"+end_of_macro_line
        outfile.write("%s" % logline )
        logline = "\tl = "+str(i)+";"+ end_of_macro_line+"\tbreak;" + end_of_macro_line
        outfile.write("%s" % logline )
  
      #switch end
      logline = "}"+ end_of_macro_line
      outfile.write("%s" % logline )
  
      # check for profile length, macro static end
      logline = alloc_and_fallback
      outfile.write("%s" % logline )

# ILP PROFILE BY ARCHITECTURE ===================================================================
    for a in alloc_site_dict_ilp.keys():
      # define switch case macro
      outfile.write("%s" % "\n\n" )
      logline = "#define GEN_SW_PROF_ILP_A"+str(a)+"(addr) do { " + end_of_macro_line
      outfile.write("%s" % logline )
      logline = "uint32_t l = 0xFFFF;" + end_of_macro_line
      outfile.write("%s" % logline )
      logline = "switch (addr) {" + end_of_macro_line
      outfile.write("%s" % logline )
      for i, site in enumerate(ilp_profile_by_archi[a]):
        index = alloc_site_dict[site]["index"]
        logline = "case __APP_ALLOC_SITE_"+str(index)+":"+end_of_macro_line
        outfile.write("%s" % logline )
        logline = "\tl = "+str(i)+";"+ end_of_macro_line+"\tbreak;" + end_of_macro_line
        outfile.write("%s" % logline )
  
      #switch end
      logline = "}"+ end_of_macro_line
      outfile.write("%s" % logline )
  
      # check for profile length, macro static end
      logline = alloc_and_fallback_no_variable_len
      outfile.write("%s" % logline )

# ILP AND ENHANCE STUB FOR NOT COVERED ARCHITECTURES ============================================
    if len(arch_stubs) > 0:
      logline = "// we need to define these macro for every architecture, but simulation will fail for architectures whose profile is not computed.\n"
      outfile.write("%s" % logline )
    for a in arch_stubs:
      logline = "#define GEN_SW_PROF_ENHANCED_A"+str(a)+"(addr, prof_len) do { exit(8080); }while(0)\n"
      outfile.write("%s" % logline )
      logline = "#define GEN_SW_PROF_ILP_A"+str(a)+"(addr) do { exit(8080); }while(0)\n"
      outfile.write("%s" % logline )

  print "done."
  print "> End of script, exiting..."
  exit(0)




if __name__ == "__main__":
  main()

