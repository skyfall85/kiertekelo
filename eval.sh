#!/bin/sh
ls -l |awk '$9~/pf/ && $9~/.npy/ {print $9}' -> p-field_data_filename.txt
ls -l |awk '$9~/of/ && $9~/.npy/ {print $9}' -> o-field_data_filename.txt

python2.7 proc.py
/usr/local/MATLAB/R2014a/bin/matlab   -nodesktop -nosplash -r "watershed_script; exit"; reset
g++ orientation_defect_identifier.cpp -ltiff
./a.out


mkdir density
for i in radius_density*
do
	a=`echo "${i//[!0-9]}"`
	gnuplot -e "file='${i}'; number='${a}'"  plot_and_fit_density_png.plg
	mv $i density
done

mkdir defect
for i in ori_def*
do
	mv $i defect
done

rm *.mat *.txt
rm a.out
rm fit*
