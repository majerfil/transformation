#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;


vector<float> refX,refY,x,y,hom; 



vector<float> transform(vector<float> xi, vector<float> yi,vector<float> x,vector<float> y)
{
	printf("%i\n",xi.size());
	Mat A(2*xi.size(),8,CV_32FC1);
	Mat b(2*xi.size(),1,CV_32FC1);
	Mat xx;

	for (int i = 0;i<xi.size();i++){
		A.at<float>(2*i,0)=-x[i];
		A.at<float>(2*i,1)=-y[i];
		A.at<float>(2*i,2)=1;
		A.at<float>(2*i,3)=0;
		A.at<float>(2*i,4)=0;
		A.at<float>(2*i,5)=0;
		A.at<float>(2*i,6)=-xi[i]*x[i];
		A.at<float>(2*i,7)=-xi[i]*y[i];
		A.at<float>(2*i+1,0)=0;
		A.at<float>(2*i+1,1)=0;
		A.at<float>(2*i+1,2)=0;
		A.at<float>(2*i+1,3)=-x[i];
		A.at<float>(2*i+1,4)=-y[i];
		A.at<float>(2*i+1,5)=1;
		A.at<float>(2*i+1,6)=-yi[i]*x[i];
		A.at<float>(2*i+1,7)=-yi[i]*y[i];

		b.at<float>(2*i,0)=-xi[i];
		b.at<float>(2*i+1,0)=-yi[i];
	}
	cout << A << endl;
	solve(A,b,xx,DECOMP_SVD);
	cout << xx << endl;
	//MATINV(8,1,est,vec,&det);
	//for (int i = 0;i<8;i++)  hom.push_back(vec[i][0]);
	//hom.push_back(1);
	//transformType = TRANSFORM_2D;*/
	return hom;
}

int main(int argc,char* argv[])
{
	if(argc<3)
	{
		fprintf(stderr,"usage: %s referencePoints.txt points.txt \n",argv[0]); 
		return 0;
	}

	ifstream inReferenceFile(argv[1]);
	ifstream inFile(argv[2]);
	float a,b;

	while(inReferenceFile >> a >> b)
	{
		refX.push_back(a);
		refY.push_back(b);

	}
	while(inFile >> a >> b)
	{
		x.push_back(a);
		y.push_back(b);
	}
	hom = transform(refX,refY,x,y);
	cout << "Mat ";
	for( int i=0;i<hom.size();i++)cout<< hom[i] << " ";
	cout << endl;
	return 0;
}

