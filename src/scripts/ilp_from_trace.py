#!/usr/bin/python

import os, sys, getopt, time, math, subprocess

import numpy as np

import cplex

def x(val):
    return ("x"+str(val))

def whatsalive(t_ref, objects):
    alive_objects = objects[np.where((objects[:, 2]<=t_ref) & (objects[:, 3]>=t_ref))]
    return alive_objects

def file_len(fname):
    with open(fname) as f:
        for i, l in enumerate(f):
            pass
    return i + 1

def main():
  # command line arguments processing
  try:
    opts, args = getopt.getopt(sys.argv[1:], "n:hVi:a:", ["help", "no_ilp", "lpsolve", "overlap_density"])
  except getopt.GetoptError as err:# print help information and exit:
    print str(err)  # will print something like "option -a not recognized"
    sys.exit(2)

  verbose = False
  ilp = True
  uses_cplex = True
  overlap_density = False
  mem_arch_file_name = ""
  name = "noname.placement"
  prob_size = 500
  bigsize_threshold = 20000#1600
  smallsize_threshold = 512
  kpercentile = 80 # 100 - percentage of hottest objects placed directly in fast memory if they are smalls


  for o, a in opts:
    # print "option=", o, " arg=", a
    if o == "-V":
      print "script last modification : ", time.strftime('%m/%d/%Y-%H:%M', time.localtime(os.path.getmtime(sys.argv[0])))
      verbose = True
    elif o in ("-h", "--help"):
      print "-V : verbose"
      print "-i input : input heap object file path"
      print "-a mem_arch : memory architecture file path"
      print "-n name : output oracle file name"
      sys.exit()
    elif o in ("-n"):
      name = a
      print "output placement file name:", name
    elif o in ("-i"):
      print "input:", a
      alloc_log_file_name = a
    elif o in ("-a"):
      print "memory architecture:", a
      mem_arch_file_name = a
    elif o in ("--no_ilp"):
      print "no ilp used in oracle decisions"
      ilp = False
    elif o in ("--lpsolve"):
      print "using lpsolve instead of cplex"
      uses_cplex = False
    elif o in ("--overlap_density"):
      print "using density definition based on overlapping allocations"
      overlap_density = True
    else:
      print "unhandled option :", o, a
      sys.exit(1)


  if verbose:
    print "===================================================================="
    print "parsing memory architecture description file"
    print "===================================================================="
  if mem_arch_file_name == "":
    print "ERROR no memory architecture description file, aborting"
    sys.exit(1)
  mem_arch = np.loadtxt(mem_arch_file_name, delimiter=':', dtype=int, usecols=range(1,5))

  if len(mem_arch) > 2:
    print "ERROR this script doen't support yep more than 2 memory banks"
    sys.exit(1)

  m0 = mem_arch[0]
  m1 = mem_arch[1]

  # if verbose:
  print "memory architectures containing", len(mem_arch), "heaps."
  print mem_arch

  if verbose:
    print "===================================================================="
    print "parsing allocation log"
    print "===================================================================="
  # sorted by request date (generated in that order)
  alloc_log = np.loadtxt(alloc_log_file_name, delimiter=';', dtype=int)

  obj_count=0
  objects = np.array([[0,0,0,0,0,0,0,0]], dtype = float)
  score_computation = np.array(0)
  i = 0

  for index, alloc in enumerate(alloc_log) :
    # "addr;size;malloc_date_cycles;lifespan_cycles;r_count;w_count;alloc_order;free_order"
    # gather allocation infos
    size = alloc[1]
    ta = alloc[6]
    tf = alloc[7]
    nbr = alloc[4]
    nbw = alloc[5]

    #computing score for greedy offline heuristic (if no ilp)
    if not ilp:
      if(alloc[1]*alloc[3])!=0:
      	if overlap_density:
      	  after_alloc = alloc_log[np.where(alloc_log[:,6] > alloc[6])[0]][:,6]
      	  before_free = alloc_log[np.where(alloc_log[:,6] < alloc[7])[0]][:,6]
      	  # overlaping_allocations = [val for val in after_alloc if val in before_free]
      	  overlaping_allocations = np.intersect1d(before_free, after_alloc, assume_unique=True)
      	  malloc_overlap = 1 + len(overlaping_allocations)
          obj_score = float(alloc[4]+alloc[5])/float(alloc[1]*malloc_overlap) # OVERLAP BASED DENSITY
        else:
          obj_score = float(alloc[4]+alloc[5])/float(alloc[1]*alloc[3]) # DENSITY
        # obj_score = float(alloc[1]) # TAILLE
        # obj_score = float(alloc[4]+alloc[5]) # ABSOLUT R/W COUNT
      else:
        obj_score = 0
        print "glitched object detected at line", i
      score_computation = np.vstack((score_computation, [obj_score]))

    # construct allocated object descriptor
    if ilp and uses_cplex:
      obj_desc = np.array( [i, size, ta, tf, nbr, nbw, 0, 2])
    else:
      obj_desc = np.array( [i, size, ta, tf, nbr, nbw, obj_score, 3])
    i+=1
    # add to array
    objects = np.vstack((objects, obj_desc))


  objects = np.delete(objects, 0, 0)
  obj_count = len(objects)

  #computing score for greedy offline heuristic (if no ilp)
  if not ilp:
    # max_score = max(score_computation)
    # print "max_score =", max_score

    # for index, obj in enumerate(objects) :
    #   obj[6] = score_computation[index]
    ind_score_order = np.argsort(objects[:,6]);
    obj_score_ordered = objects[ind_score_order]


  # by default we use an ILP to decide what object should be in the fast memory
  # this require to select interresting big objects from heap trace, construct a ilp problem from them
  # solve it (curretly using lp-solve) and reimpact placement decision to objects allocation
  if ilp:
    if verbose:
      print "===================================================================="
      print "objective function"
      print "===================================================================="
    if uses_cplex:
      ilp_objects = objects
      objective_func = "Minimize\nobj:"
      const = 0
      for i,obj in enumerate(ilp_objects):
        coef = obj[4].astype(np.int64)*(m0[2]-m1[2])+obj[5].astype(np.int64)*(m0[3]-m1[3])
        const += obj[4].astype(np.int64)*m1[2] + obj[5].astype(np.int64)*m1[3]
        if coef < 0:
          coef_str = " - "+'{:d}'.format(-coef)
        else:
          coef_str = " + "+'{:d}'.format(coef)

        line = coef_str+" "+x(i)
        objective_func+= line
        if i%10 == 0:
            objective_func+="\n"
      const_str = " + "+'{:d}'.format(const)
      objective_func+= const_str+"\n"

    else: #USING LPSOLVE
      objective_func = "min: "
      for i,obj in enumerate(ilp_objects):
        line = str(obj[4].astype(np.int64)*(m0[2]-m1[2])+obj[5].astype(np.int64)*(m0[3]-m1[3])) \
        +x(i)+"+"+str(obj[4].astype(np.int64)*m1[2] + obj[5].astype(np.int64)*m1[3])
        objective_func+= line
        if i < len(ilp_objects)-1:
          objective_func+="+"
      objective_func+=";"
    if verbose:
      print "===================================================================="
      print "object variable constraints"
      print "===================================================================="
    if not uses_cplex: #USING LPSOLVE
      var_borne_sup = ""
      var_borne_inf = ""
      for i,obj in enumerate(ilp_objects):
        var_borne_sup += x(i) + " <= 1;\n"
        var_borne_inf += x(i) + " >= 0;\n"

    if verbose:
      print "===================================================================="
      print "temporal checking constraints"
      print "===================================================================="
      print "alloc", ilp_objects[:,2].flatten().astype(np.int64)
      print "frees", ilp_objects[:,3].flatten().astype(np.int64)
    considered_allocs = np.array([[0,0,0,0,0,0,0,0]], dtype = float)
    for i,obj in enumerate(ilp_objects):
      try:
        previous_action = ilp_objects[np.where(ilp_objects[:,2]==(obj[3]-1))[0][0]]
        considered_allocs = np.vstack((considered_allocs, previous_action))
      except:
        pass
    considered_allocs = np.delete(considered_allocs, 0, 0)
    if verbose:
      print "end of loop, considered allocs count:", len(considered_allocs)
    size_constraints = ""

    if uses_cplex:
      size_constraints += "Subject To\n"
      for c,alloc in enumerate(considered_allocs):
        constraint = ""
        alive_objects = whatsalive(alloc[2], ilp_objects)
        for i,obj in enumerate(alive_objects):
          constraint += str(obj[1].astype(np.int64)) + x(obj[0].astype(np.int64))
          if i % 10 == 0:
            constraint += "\n"
          if i < len(alive_objects)-1:
            constraint += " + "
        constraint += "<=" + str(m0[1]) + "\n"
        size_constraints += constraint
        if verbose and c % (1+len(considered_allocs)/100) == 0:
          print "constraint ", (c*100/len(considered_allocs)), "%"

    else: #USING LPSOLVE
      for alloc in considered_allocs:
        line = ""
        alive_objects = whatsalive(alloc[2], ilp_objects)
        # if verbose:
        # print "alive objects at that time :\n", alive_objects
        for i,obj in enumerate(alive_objects):
          line += str(obj[1].astype(np.int64)) + x(obj[0].astype(np.int64))
          if i < len(alive_objects)-1:
            line += " + "
        line += "<=" + str(m0[1]) + ";\n"
        size_constraints += line
        if verbose and c % (1+obj_count/100) == 0:
          print "constraint ", (c*100/obj_count), "%"


    if verbose:
      print "===================================================================="
      print "problem variable declaration as integer / binary"
      print "===================================================================="
    var_int_declatation = ""

    if uses_cplex:
      var_int_declatation += "Binary\n"
      for i,obj in enumerate(ilp_objects):
        var_int_declatation += x(i)+"\n"
      var_int_declatation += "End\n"

    else: #USING LPSOLVE
      for i,obj in enumerate(ilp_objects):
        var_int_declatation += "int "+x(i)+";\n"

    if verbose:
      print "===================================================================="
      print "printing output file \"problem.lp\""
      print "===================================================================="

    if uses_cplex:
      with open("problem.lp", "w") as problem_file:
        problem_file.write("%s" % objective_func )
        problem_file.write("%s" % size_constraints)
        problem_file.write("%s" % var_int_declatation)

    else: #USING LPSOLVE
      with open("problem.lp", "w") as problem_file:
        problem_file.write("%s" % objective_func )
        problem_file.write("%s" % var_borne_inf )
        problem_file.write("%s" % var_borne_sup)
        problem_file.write("%s" % size_constraints)
        problem_file.write("%s" % var_int_declatation)


    if verbose:
      print "===================================================================="
      print "calling solver on problem.lp"
      print "===================================================================="

    ilp_choices = []

    if uses_cplex:
      # Start Cplex, read the problem and solve it.
      cpx = cplex.Cplex()
      cpx.read('problem.lp')
      cpx.solve()

      # then retrieveing result and reimpacting it in the objects
      print "printing solutions:"
      ilp_values = np.array(cpx.solution.get_values())
      print "ilp return values :\n", ilp_values
      for i,obj in enumerate(objects):
        obj[7] = 1 - ilp_values[i]



    else: #USING LPSOLVE
      p = subprocess.Popen(['lp_solve', "problem.lp"], stdout=subprocess.PIPE)
      p.wait()
      if p.returncode:
        print "ERROR in lp_solve, aborting"
        sys.exit(1)

      l = 0
      mem = 0
      if verbose:
        print "printing output line by line"

      for line in p.stdout:
        if True:
          print "[",l,"]",line[0:-1],
          print "==> \"", line.split(" ")[-1][0:-1],"\""
        if l > 3:
          if int(line.split(" ")[-1][0:-1]) == 1:
            ilp_choices.append(mem)
          elif int(line.split(" ")[-1][0:-1]) == 0:
            ilp_choices.append(mem+1)
          else:
            print "ERROR reading lpsolve results, aborting"
            sys.exit(1)
        l += 1

      for i in range(len(ilp_choices)):
        original_idx = np.int(ilp_objects[i][0])
        objects[original_idx][7]=ilp_choices[i]


    print "* * * * * * * repartition des objets avant passe de selection :"
    print "\td office dans rapide:", len(np.where(objects[:,7]==0)[0])
    print "\td office dans lent:", len(np.where(objects[:,7]==1)[0])
    print "\tdestine a l ILP (should be 0!):", len(np.where(objects[:,7]==2)[0])
    print "\tnon decide:", len(np.where(objects[:,7]==3)[0])


  if not ilp or not uses_cplex:
    print "===================================================================="
    print "heap filling"
    print "===================================================================="
    already_in_fast = objects[np.where(objects[:,7]==0)[0]]
    size_limit = np.zeros(len(objects)*2)
    for i in range(len(already_in_fast)):
      # print "applying size in range [",already_in_fast[i][2], ", ",already_in_fast[i][3], "]"
      size_limit[np.int(already_in_fast[i][2]):np.int(already_in_fast[i][3])]+=already_in_fast[i][1]


    # try adding objects in fast memory in decreasing score order, until memory is "full"
    for i in reversed(range(obj_count)):

      real_index = np.int(obj_score_ordered[i][0])

      # don't consider already allocated objects
      if objects[real_index][7]!=3:
        continue;

      # try to add it in fast memory
      target_mem = 0
      obj_sz = objects[real_index][1]

      if max(size_limit[np.int(objects[real_index][2]):np.int(objects[real_index][3])]) + obj_sz > m0[1]:
        objects[real_index][7] = 1
      else:
        objects[real_index][7] = 0
        size_limit[np.int(objects[real_index][2]):np.int(objects[real_index][3])] += obj_sz


  print "* * * * * * * repartition des objets apres passe de selection :"
  print "\td office dans rapide:", len(np.where(objects[:,7]==0)[0])
  print "\td office dans lent:", len(np.where(objects[:,7]==1)[0])
  print "\tdestine a l ILP (should be 0!):", len(np.where(objects[:,7]==2)[0])
  print "\tnon decide:", len(np.where(objects[:,7]==3)[0])

  oracle_choices_ordered = []
  ind_alloc_order = np.argsort(objects[:,2]);

  with open(name, "w") as oracle_file:
    for c in objects[ind_alloc_order]:
      oracle_file.write("%s" % str(c[7].astype(int))+"\n" )

  res_file_len = file_len(name)

  if res_file_len != obj_count:
    print "ERROR : internal inconsistency, aborting"
    print "object count :", obj_count
    print "output oracle file length:", res_file_len
    sys.exit(1)

  if verbose:
    print "script end, returning."
  sys.exit(0)




if __name__ == "__main__":
    main()

