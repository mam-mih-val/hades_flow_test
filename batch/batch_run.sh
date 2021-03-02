#!/bin/bash

format='+%Y/%m/%d-%H:%M:%S'

date $format

job_num=$(($SLURM_ARRAY_TASK_ID))

filelist=$lists_dir/$(ls $lists_dir | sed "${job_num}q;d")

cd $output_dir
mkdir -p $job_num
cd $job_num

while read line; do
    echo $line >> list.txt
done < $filelist
echo >> list.txt

source /etc/profile.d/modules.sh
module use /cvmfs/it.gsi.de/modulefiles/
module load compiler/gcc/9

echo "loading " $ownroot
source $ownroot

echo "executing $build_dir/test -i list.txt -t hades_analysis_tree -n 1 -o out.root --output-tree-name """

$build_dir/test -i list.txt -t hades_analysis_tree -n 1 -o out.root --output-tree-name ""

date $format
echo JOB FINISHED!