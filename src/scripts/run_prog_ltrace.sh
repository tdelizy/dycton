#!/bin/bash


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



# script for running a program and plot its heap behavior
# >>>>>input : the command line to execute (for example "program arg1 arg2 ...")
# <<<<<output : a timestamped folder containing the plot of heap behavior and much logs

################################# command line args parsing
for arg in "$@"
do
 line=$line" "$arg
done

################################# target program execution with ltrace
#execute the command passed in parameter with ltrace of malloc() and free()
ltrace -r -o malloc_trace -e malloc -e free -e calloc $line
#execute once again for the summary of ltrace
ltrace -r -c -o ltrace_summary.log -e malloc -e free -e calloc $line
#log the command line at the end of summary
echo 'used command line for this run' >> ltrace_summary.log
echo $line >> ltrace_summary.log

#copy original malloc log file before transformation
cp malloc_trace ltrace_original.log

################################# log transformation for plotting allocation boxes
#remove last line of file (+++ exited (status 0) +++)
sed -i '$ d' malloc_trace

# transform "calloc(x, y)" into "malloc(z)" with z = x*y
sed -i -r 's/\(/\(:/g' malloc_trace
sed -i -r 's/\)/:\)/g' malloc_trace
sed -i -r 's/, /:/g' malloc_trace

gawk -i inplace 'BEGIN{FS=":";OFS=":";} {if($1 ~ /calloc/){ t = $2 * $3; $2 = t; $3 = ""; print; } else{print;}}' malloc_trace

sed -i -r 's/(calloc\()/malloc\(/g' malloc_trace
sed -i -r 's/://g' malloc_trace


#remove / substitute strings of character
sed -i -r 's/( [^ ]*)(free\()/:0:0:/g' malloc_trace
sed -i -r 's/( [^ ]*)(malloc\()/:1:0:/g' malloc_trace
sed -i -r 's/\) *= /:/g' malloc_trace
sed -i -r 's/<void>/0/g' malloc_trace
sed -i -r 's/ //g' malloc_trace



#swap column 1 and 2
gawk -i inplace 'BEGIN{FS=":";OFS=":";} { t = $1; $1 = $2; $2 = t; print; }' malloc_trace

#swap column 4 and 5 if necessary
gawk -i inplace 'BEGIN{FS=":";OFS=":";} {if($1==1){ t = $4; $4 = $5; $5 = t; print; } else{print;}}' malloc_trace

#remove the "0x" of column 4 and change base to decimal
gawk -i inplace 'BEGIN{FS=":";OFS=":";} {$4 = strtonum( $4 ); print;}' malloc_trace

#work on timestamps : converts to microseconds and accumulate relative timestamps
gawk -i inplace 'BEGIN{FS=":";OFS=":";} {sumTime+=$2*1000000; $2 = sumTime; print;}' malloc_trace

#deletes lines corresponding to "free(0)"
gawk -i inplace 'BEGIN{FS=":";OFS=":";} {if($4!=0){ print; }}' malloc_trace

################################# plot allocation boxes from ltrace log
#launch python script for plotting
python /data_ssd/simulateur/dycton/src/scripts/log_process_old.py malloc_trace

################################# put results in a timestamped folder
for arg in "$@"
do
  if [[ "$arg" == "$1" ]];
  then
    res_dir=$arg
  elif [ ${#arg} -lt 4 ];
  then
    res_dir=$res_dir" "$arg
  fi
done

res_dir=$(sed -r 's/[\.\/]+//g' <<< $res_dir)
res_dir=$(sed -r 's/[\%\ \\]+/_/g' <<< $res_dir)

res_dir=ltrace_log_$res_dir$(date +_%Y.%m.%d_%Hh%Mm%S)

mkdir $res_dir

mv malloc_trace $res_dir/malloc_trace
mv ltrace_summary.log $res_dir/ltrace_summary.log
mv ltrace_original.log $res_dir/ltrace_original.log
mv heap_occupation.png $res_dir/heap_occupation.png

echo end of script, results stored in $res_dir