#include <iostream>
#include <fstream>
#include <cmath>
#include <sstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <tiffio.h>
#include <fstream>

using namespace std;

#define f(i,a,b) for (int i=a;i<b;i++)
#define ABS(A) ((A)<0 ? (-1.0)*(A) : A)
#define angdiff(A,B) (ABS(A-B)<ABS(1.0-ABS(A-B)) ? A-B : ( (A-B)>0.0 ? (A-B)-1.0 : (ABS(A-B)<0.5 ? B-A : 1.0+(A-B)) )  )


int tsteps_reader()
{
	ifstream file;
	file.open("tsteps.txt");
	int ntsteps=-1;
	int tstep;
	while ( !file.eof() ){
		file>>tstep;
		ntsteps++;
	}
	file.close();
	return ntsteps;
}

void tsteps_reader(int *a, int l)
{
	ifstream file;
	file.open("tsteps.txt");

	for (int i=0;i<l;i++) file>>a[i];
	file.close();
}

void read_sizes(int &a,int &b)
{
	ifstream file;
	file.open("sizes.txt");
	file>>a;file>>b;
	file.close();
}


int main()
{
	int length=tsteps_reader();
	cout<<"length="<<length<<endl;
	int *tsteps;
	int l=tsteps_reader();
	tsteps=new int[l];
	tsteps_reader(tsteps,l);

	double **field,**rotation,**grains;
	int *xp,*xm,*yp,*ym;
	
	int L,xsize,ysize;
	read_sizes(xsize,ysize);
	xsize==ysize ? L=xsize : L=0;
	field=new double*[L];
	f(i,0,L) field[i]=new double[L];
	rotation=new double*[L];
	f(i,0,L) rotation[i]=new double[L];
	grains=new double*[L];
	f(i,0,L) grains[i]=new double[L];

	// set periodic boundary condition:
	xp=new int[L];
	xm=new int[L];
	yp=new int[L];
	ym=new int[L];
	f(i,0,L){xp[i]=i+1;xm[i]=i-1;yp[i]=i+1;ym[i]=i-1;}
	xp[L-1]=0;
	yp[L-1]=0;
	xm[0]=L-1;
	ym[0]=L-1;
    
    // ****************variables of the images output**************
	typedef unsigned char tRGBline[L][3];
  	tRGBline *myRGB;
  	TIFF *f;
  	myRGB = (tRGBline*)malloc(L*sizeof(tRGBline));
	// ****************variables of the images output**************
	
	double d_xpyp,d_xp,d_xpym,d_ym,d_xmym,d_xm,d_xmyp,d_yp;
	double sum_of_grad;
    
    int index;

    ofstream out_defnum;
    char out_defnum_file[50];
    sprintf(out_defnum_file,"ori_defect_number.dat");
    out_defnum.open(out_defnum_file);
    out_defnum<<"tsteps \t num_of_right \t num_of_left \t num_of_ori_def_trijunction \t num_of_ori_def_at_GB"<<endl;


    f(t,0,length)
	{
	// read in data file:
	ifstream in;
	char filename[50];
	sprintf(filename,"orientation_%07d.txt",tsteps[t]);
	in.open(filename);
	cout<<filename<<endl;
	cout<<"Read in starts"<<endl;
	f(i,0,L){f(j,0,L){in>>field[i][j];}}
	f(i,L-5,L){f(j,L-5,L){cout<<field[i][j]<<"\t";}cout<<endl;}
	cout<<"Read in ends"<<endl;
	
	// now read in the grains data which was gained, from the watershed algorithm
	ifstream in2;
	char filename2[50];
	sprintf(filename2,"grains_data_%d.txt",tsteps[t]);
	in2.open(filename2);
	cout<<"Reading grains data file started!"<<endl;
	f(i,0,L){f(j,0,L){in2>>grains[i][j];}}
	f(i,L-5,L){f(j,L-5,L){cout<<grains[i][j]<<'\t';}cout<<endl;}

	int num_of_left=0;
	int num_of_right=0;
	int zero=0;

	double total_sum=0.0;

	// write out the number of defects:
	double num_of_ori_def_trijunction=0.0;
	double num_of_ori_def_at_GB=0.0;

	f(i,0,L)
	{
		f(j,0,L)
		{
			
		sum_of_grad=0.0;
		d_xpyp=field[xp[i]][yp[j]];
		d_xpym=field[xp[i]][ym[j]];
		d_xmyp=field[xm[i]][yp[j]];
		d_xmym=field[xm[i]][ym[j]];
		d_xp=field[xp[i]][j];
		d_xm=field[xm[i]][j];
		d_ym=field[i][ym[j]];
		d_yp=field[i][yp[j]];
				
		sum_of_grad=angdiff(d_ym,d_xmym)+angdiff(d_xpym,d_ym)+angdiff(d_xp,d_xpym)+angdiff(d_xpyp,d_xp)+angdiff(d_yp,d_xpyp)+angdiff(d_xmyp,d_yp)+angdiff(d_xm,d_xmyp)+angdiff(d_xmym,d_xm);
		// this sum should be either -1.0 or 0.0 or +1.0
		total_sum+=sum_of_grad;
		rotation[i][j]=sum_of_grad;
		if (fabs(sum_of_grad+1.0)>1.0e-4 && fabs(sum_of_grad)>1.0e-4 && fabs(sum_of_grad-1.0)>1.0e-4) 
			{cout<<"Something is wrong at "<<i<<" : "<<j<<" coordinates the sum is: "<<sum_of_grad<<endl;
			 cout<<d_xmyp<<"\t"<<d_yp<<"\t"<<d_xpyp<<"\n"
	        	     <<d_xm<<"\t"<<field[i][j]<<"\t"<<d_xp<<"\n"
			     <<d_xmym<<"\t"<<d_ym<<"\t"<<d_xpym<<"\n"<<endl;
			}
			if (fabs(sum_of_grad+1.0)<1.0e-3) {sum_of_grad=-1.0;num_of_right++;}
			else if (fabs(sum_of_grad)<1.0e-3) {sum_of_grad=0.0;zero++;}
			else if (fabs(sum_of_grad-1.0)<1.0e-3) {sum_of_grad=1.0;num_of_left++;}
			int trijunction_or_boundary=0;

			if (sum_of_grad!=0)
			{
				int nn[27]={grains[xp[xp[xp[i]]]][ym[ym[ym[j]]]],grains[xp[xp[xp[i]]]][ym[ym[j]]],grains[xp[xp[xp[i]]]][ym[j]],
							grains[xp[xp[xp[i]]]][j],grains[xp[xp[xp[i]]]][yp[j]],grains[xp[xp[xp[i]]]][yp[yp[j]]],grains[xp[xp[xp[i]]]][yp[yp[yp[j]]]],
							grains[xp[xp[i]]][ym[ym[ym[j]]]],grains[xp[xp[i]]][yp[yp[yp[j]]]],
							grains[xp[i]][ym[ym[ym[j]]]],grains[xp[i]][yp[yp[yp[j]]]],
							grains[i][ym[ym[ym[j]]]],grains[i][yp[yp[yp[j]]]],
							grains[xm[i]][ym[ym[ym[j]]]],grains[xm[i]][yp[yp[yp[j]]]],
							grains[xm[xm[i]]][ym[ym[ym[j]]]],grains[xm[xm[i]]][yp[yp[yp[j]]]],
							grains[xm[xm[xm[i]]]][ym[ym[ym[j]]]],grains[xm[xm[xm[i]]]][ym[ym[j]]],grains[xm[xm[xm[i]]]][ym[j]],
							grains[xm[xm[xm[i]]]][j],grains[xm[xm[xm[i]]]][yp[j]],grains[xm[xm[xm[i]]]][yp[yp[j]]],grains[xm[xm[xm[i]]]][yp[yp[yp[j]]]]};
				int unn[2]={0,0};
				int ind_unn=0;
				f(ind_nn,0,27)
				{
					if (nn[ind_nn]!=0 && nn[ind_nn]!=unn[0] && nn[ind_nn]!=unn[1]) 
						{
							if (ind_unn==2){/*cout<<"trijunction detected"<<endl*/;trijunction_or_boundary=1; num_of_ori_def_trijunction+=1.0;break;}
							unn[ind_unn]=nn[ind_nn];
							ind_unn++;
						}
				}
			}

			if (sum_of_grad!=0 && trijunction_or_boundary==0){
				// cout<<"Possible orientation defect detected"<<endl;
				num_of_ori_def_at_GB+=1.0;
				// cout<<grains[xm[xm[i]]][yp[yp[j]]]<<"\t"<<grains[xm[i]][yp[yp[j]]]<<"\t"<<grains[i][yp[yp[j]]]
				// <<"\t"<<grains[xp[i]][yp[yp[j]]]<<"\t"<<grains[xp[xp[i]]][yp[yp[j]]]<<endl;
				// cout<<grains[xm[xm[i]]][yp[j]]<<"\t \t \t \t"<<grains[xp[xp[i]]][yp[j]]<<endl;
				// cout<<grains[xm[xm[i]]][j]<<"\t \t \t \t"<<grains[xp[xp[i]]][j]<<endl;
				// cout<<grains[xm[xm[i]]][ym[j]]<<"\t \t \t \t"<<grains[xp[xp[i]]][ym[j]]<<endl;
				// cout<<grains[xm[xm[i]]][ym[ym[j]]]<<"\t"<<grains[xm[i]][ym[ym[j]]]<<"\t"<<grains[i][ym[ym[j]]]
				// <<"\t"<<grains[xp[i]][ym[ym[j]]]<<"\t"<<grains[xp[xp[i]]][ym[ym[j]]]<<endl;
				// cout<<"-------------------------------------------------------------------"<<endl;

			}
			// if (sum_of_grad!=0) cout<<"i:j:="<<i<<":"<<j<<" - sum_of_grad:"<<sum_of_grad<<endl;
			sum_of_grad!=0 ? index=int(sum_of_grad/fabs(sum_of_grad))+1 : index=1;
			myRGB[j][i][0]=0.0;
			myRGB[j][i][1]=0.0;
			myRGB[j][i][2]=0.0;
	
			myRGB[j][i][index]=255.0;

			if (sum_of_grad!=0 && trijunction_or_boundary==0){
							myRGB[j][i][0]=255.0;
							myRGB[j][i][1]=255.0;
							myRGB[j][i][2]=0.0;
			}
				

			}
		}	
    cout<<"num_of_ori_def_at_GB="<<num_of_ori_def_at_GB<<"\t num_of_ori_def_trijunction="<<num_of_ori_def_trijunction<<endl;  
    cout<<"Percentage of defects at GB-s: "<<100*num_of_ori_def_at_GB/(num_of_ori_def_trijunction+num_of_ori_def_at_GB)<<endl;
	cout<<"total_sum: "<<total_sum<<endl;
	// **********write out the image********************
	char filename_out[50];
	sprintf(filename_out,"ori_defect_map_%03d.tiff",tsteps[t]);

	if((f=TIFFOpen(filename_out,"w")) == NULL)
    abort();
  
    TIFFSetField(f, TIFFTAG_IMAGEWIDTH, (uint32)L);
    TIFFSetField(f, TIFFTAG_IMAGELENGTH, (uint32)L);
    TIFFSetField(f, TIFFTAG_SAMPLESPERPIXEL, 3);
    TIFFSetField(f, TIFFTAG_BITSPERSAMPLE, 8,8,8);
    TIFFSetField(f, TIFFTAG_COMPRESSION, 8);
    TIFFSetField(f, TIFFTAG_PHOTOMETRIC, 2);
    TIFFSetField(f, TIFFTAG_ROWSPERSTRIP, 1);
    TIFFSetField(f, TIFFTAG_XRESOLUTION, 1.0);
    TIFFSetField(f, TIFFTAG_YRESOLUTION, 1.0);
    TIFFSetField(f, TIFFTAG_RESOLUTIONUNIT, 1);
    TIFFSetField(f, TIFFTAG_PLANARCONFIG, 1);	

    for(int x=0; x<L; x++){
	if(TIFFWriteScanline(f,myRGB[x],x,0)<0){
	  	TIFFClose(f);
		return 1;}}

	TIFFClose(f);
	

	cout<<" left: "<<num_of_left<<"\n right: "<<num_of_right<<"\n zero: "<<zero<<endl;
	

	char filename_out2[50];
	sprintf(filename_out2,"ori_defect_map_%03d.dat",tsteps[t]);
	ofstream out;
	out.open(filename_out2);
	f(i,0,L){f(j,0,L){out<<rotation[i][j]<<"\t";}out<<endl;}

	out_defnum<<tsteps[t]<<"\t"<<num_of_right<<"\t"<<num_of_left<<"\t"<<num_of_ori_def_trijunction<<"\t"<<num_of_ori_def_at_GB<<endl;

	} 
	out_defnum.close();

	free(myRGB);
	return 0;
}