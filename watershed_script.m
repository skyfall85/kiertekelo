disp('Start');
files=dir('data_*');
for file=files' 
	infile_name=file.name
	load(infile_name);
	dlmwrite('sizes.txt', size(data), 'delimiter', '\t');
	
	tstep=str2num(infile_name(6:(length(infile_name)-3)))
	disp('Infile ready');
	for th=[994]
		data(data>0.001*th)=1.0;

		[size_x,size_y]=size(data);

% data=horzcat(data,data,data);
% data=vertcat(data,data,data);
		lind=size_x-99;
		data1=horzcat(data(lind:size_x,lind:size_x),data(lind:size_x,:),data(lind:size_x,1:100));
		data2=horzcat(data(:,lind:size_x),data,data(:,1:100));
		data3=horzcat(data(1:100,lind:size_x),data(1:100,:),data(1:100,1:100));

		data=vertcat(data1,data2,data3);		

		grains=watershed(1.0-data);

% grains=grains(size_x+1:2*size_x,size_x+1:2*size_x);
 		grains=grains(101:(size_x+100),101:(size_x+100));
 		grains_filename=strcat('grains_data_',int2str(tstep),'.txt');

 		% data=reshape(grains,[size_x*size_y,1]);
 		filename=strcat('grain_size_',int2str(tstep),'.dat');
		dlmwrite(filename, grains, 'delimiter', '\n','-append');

		ind0=0;
		rand_ind=horzcat(ind0,randperm(max(max(grains))));
		grains_rand=reshape(rand_ind(grains+1),length(grains),length(grains));
		grains_rgb = label2rgb(grains_rand,'jet',[.5 .5 .5]);
		imshow(grains_rgb),title('Az algoritmus altal beazonositott szemcsek')
		plot_filename=strcat('ori_def_Identified_grains1_',int2str(tstep),'t.pdf');
		print('-dpdf','-r200',plot_filename);
		dlmwrite(grains_filename,grains_rand,'delimiter','\t');
		dlmwrite('tsteps.txt', int2str(tstep), 'delimiter', '','-append');

		grain_vec=grains(:)';
% 2: calculate the areas of the grains:
		max_id=max(grain_vec);
		binranges=1:max_id;
		grain_area=histc(double(grain_vec),binranges);
		grain_area=grain_area(grain_area>300);
		myformat='%5d %5d\n';
		fid=fopen('grain_number.dat','a');
		grain_number_vector(1)=tstep;
		grain_number_vector(2)=length(grain_area);
		fprintf(fid,myformat,grain_number_vector);
		fclose(fid);


% grain_area contains the area of the individual grains:
% 3: area->radius:
		grain_radiuses=sqrt(grain_area);
		mean_radius=mean(grain_radiuses);
		normed_radius=grain_radiuses/mean_radius;
		max(normed_radius)

 		num_of_breaks=20;
 		bp_x=[1:1:num_of_breaks];	
 		bp=min(normed_radius)+bp_x*(max(normed_radius)-min(normed_radius))/num_of_breaks;
 		dx=bp(2)-bp(1);
 		density=ones(1,(num_of_breaks-1));
 		for i = 2:num_of_breaks
  			density(i)=(length(normed_radius(normed_radius<bp(i)))-length(normed_radius(normed_radius<bp(i-1))))/(dx*length(normed_radius));
 		end
  		density(1)=length(normed_radius(normed_radius<bp(1)))/(dx*length(normed_radius));
  
  		out_data=zeros(length(density),2)
  		out_data(:,1)=bp
  		out_data(:,2)=density

 		out_name=strcat('radius_density_data',int2str(tstep),'.dat')
 		dlmwrite(out_name, [out_data], 'delimiter', '\t'); 
		

end
end