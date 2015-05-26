#!/usr/local/bin/python2.7

import numpy as np
import os
from scipy import io as sio

f=open('p-field_data_filename.txt','r')
g=os.popen("wc -l p-field_data_filename.txt| awk '{print $1}'")
plen=g.read()
plen=int(plen[0:(len(plen)-1)])
for row_ind in range(0,plen):
	pf_filename=f.readline()
	pf_filename=pf_filename[0:(len(pf_filename)-1)]
	data=np.load(pf_filename)
	xsize=int(np.sqrt(len(data)))
	ysize=xsize
	data=data.reshape((xsize,ysize))
	tstep=pf_filename[3:(len(pf_filename)-4)]
	out_fname='phase_'+str(tstep)+'.txt'
	np.savetxt(out_fname,data)
	out_fname='data_'+str(tstep)+'.mat'
	sio.savemat(out_fname,dict(data=data),oned_as='row')

f=open('o-field_data_filename.txt','r')
g=os.popen("wc -l o-field_data_filename.txt| awk '{print $1}'")
plen=g.read()
plen=int(plen[0:(len(plen)-1)])
for row_ind in range(0,plen):
	of_filename=f.readline()
	of_filename=of_filename[0:(len(of_filename)-1)]
	data=np.load(of_filename)
	xsize=int(np.sqrt(len(data)))
	ysize=xsize
	data=data.reshape((xsize,ysize))
	tstep=of_filename[3:(len(of_filename)-4)]
	out_fname='orientation_'+str(tstep)+'.txt'
	np.savetxt(out_fname,data)


