#!/bin/bash

rm -f /tmp/procs_swap_cmdlines.out /tmp/procs_swap.out

for process in /proc/*/status
do
        awk '/VmSwap|^Pid|Name/{printf $2 " " $3}END{ print ""}' $process
done | sort -k 3 -n -r | head -5 2> /dev/null > /tmp/procs_swap.out


for pids in $(cat /tmp/procs_swap_pids.out)
do
        printf "$(awk '/VmSwap|^Pid|Name/{printf $2 " " $3}END{ print ""}' /proc/$pids/status) $(cat /proc/$pids/cmdline)" >> /tmp/procs_swap_cmdlines.out
        echo >> /tmp/procs_swap_cmdlines.out
        echo >> /tmp/procs_swap_cmdlines.out
done