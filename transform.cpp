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
	Mat A(xi.size(),4,CV_32FC1);
	Mat b(xi.size(),1,CV_32FC1);
	Mat T(2,3,CV_32FC1);

	Mat xx(3,xi.size(),CV_32FC1);
	Mat rx(2,xi.size(),CV_32FC1);

	for (int i = 0;i<xi.size();i++){
		A.at<float>(i,0)=yi[i]*x[i]-xi[i]*y[i];
		A.at<float>(i,1)=yi[i]*y[i]+xi[i]*x[i];
		A.at<float>(i,2)=yi[i];
		A.at<float>(i,3)=-xi[i];
		xx.at<float>(0,i)=x[i];
		xx.at<float>(1,i)=y[i];
		xx.at<float>(2,i)=1;
		rx.at<float>(0,i)=xi[i];
		rx.at<float>(1,i)=yi[i];
	}
	A = (A.t()*A);
	cv::SVD svdMat(A);
	cout << svdMat.u << endl;
	cout << svdMat.w << endl;
	cout << svdMat.vt << endl;
	cv::SVD::solveZ(A,b);
	if (b.at<float>(0,0) < 0) b=-b;
	float a0 = b.at<float>(0,0);
	float a1 = b.at<float>(1,0);
	b = b/sqrt(a0*a0+a1*a1);
	a0 = b.at<float>(0,0);
	a1 = b.at<float>(1,0);
	float tx = b.at<float>(2,0);
	float ty = b.at<float>(3,0);
	T.at<float>(0,0) = a0; 
	T.at<float>(0,1) = a1; 
	T.at<float>(1,0) = -a1; 
	T.at<float>(1,1) = a0; 
	T.at<float>(0,2) = tx; 
	T.at<float>(1,2) = ty; 
	cout << T << endl;
 	Mat e = T*xx-rx;
	float err = 0;
	for (int i = 0;i<xi.size();i++)
	{
		err += sqrt(e.at<float>(0,i)*e.at<float>(0,i)+e.at<float>(1,i)*e.at<float>(1,i));
	}
	printf("%f\n",err/xi.size());
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
	ifstream outFile(argv[3]);
	float a,b;

	while(inReferenceFile >> a >> b)
	{
		refX.push_back(a);
		refY.push_back(b);

	}
	while(inFile >> a >> b)
	{
		x.push_back(a+atof(argv[4]));
		y.push_back(b);
	}
	hom = transform(refX,refY,x,y);
	cout << "Mat ";
	for( int i=0;i<hom.size();i++)cout<< hom[i] << " ";
	cout << endl;
	return 0;
}

