#!/bin/sh
ls -l |awk '$9~/pf/ && $9~/.npy/ {print $9}' -> p-field_data_filename.txt
ls -l |awk '$9~/of/ && $9~/.npy/ {print $9}' -> o-field_data_filename.txt

python2.7 proc.py
/usr/local/MATLAB/R2014a/bin/matlab   -nodesktop -nosplash -r "watershed_script; exit"; reset
g++ orientation_defect_identifier.cpp -ltiff
./a.out
rm *.mat *.txt

for i in radius_density*
do
	#echo $i
	a=`echo "${i//[!0-9]}"`
	gnuplot -e "file='${i}'; number='${a}'"  fit_and_plot_density_png.plg
done
