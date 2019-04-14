#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;




void transformRot(vector<float> xi, vector<float> yi,vector<float> x,vector<float> y,vector<float> *xt,vector<float> *yt)
{
	Mat T(2,3,CV_32FC1);
	float cxxx = 0;
	float cxxy = 0;
	float rxxx = 0;
	float rxxy = 0;
	printf("%i\n",xi.size());
	Mat A(xi.size(),4,CV_32FC1);
	Mat b(xi.size(),1,CV_32FC1);

	Mat xx(3,xi.size(),CV_32FC1);
	Mat rx(2,xi.size(),CV_32FC1);

	/*center the values*/
	cxxx = cxxy =rxxx = rxxy = 0;
	for (int i = 0;i<xi.size();i++)
	{
		cxxx += x[i]; 	
		cxxy += y[i]; 	
		rxxx += xi[i]; 	
		rxxy += yi[i]; 	
	}
	cxxx = cxxx/xi.size();
	cxxy = cxxy/xi.size();
	rxxx = rxxx/xi.size();
	rxxy = rxxy/xi.size();
	for (int i = 0;i<xi.size();i++){
		x[i] = x[i]-cxxx;
		y[i] = y[i]-cxxy;
		xi[i] = xi[i]-rxxx;
		yi[i] = yi[i]-rxxy;
	}

	/*caltulate trf matrix*/
	for (int i = 0;i<xi.size();i++){
		A.at<float>(i,0)=yi[i]*x[i]-xi[i]*y[i];
		A.at<float>(i,1)=yi[i]*y[i]+xi[i]*x[i];
		A.at<float>(i,2)=yi[i];
		A.at<float>(i,3)=-xi[i];
	}
	A = (A.t()*A);
	cv::SVD svdMat(A);
	cout << svdMat.w << endl; 	//eigenvalues indicate solution quality
	cv::SVD::solveZ(A,b);

	//normalize vector so that it points in a correct direction
	if (b.at<float>(0,0) < 0) b=-b;
	float a0 = b.at<float>(0,0);
	float a1 = b.at<float>(1,0);
	//normalize vector so that first two coefs for cos and sin of a rotation matrix 
	b = b/sqrt(a0*a0+a1*a1);

	//construct the transformation matrix
	a0 = b.at<float>(0,0);
	a1 = b.at<float>(1,0);
	float tx = b.at<float>(2,0);
	float ty = b.at<float>(3,0);
	T.at<float>(0,0) = a0; 
	T.at<float>(0,1) = a1; 
	T.at<float>(1,0) = -a1; 
	T.at<float>(1,1) = a0; 
	T.at<float>(0,2) = tx-cxxx+a0*rxxx-a1*rxxy; 
	T.at<float>(1,2) = ty-cxxy+a1*rxxx+a0*rxxy;
	cout << T << endl;

	//transform the points
	for (int i = 0;i<xi.size();i++)
	{
		xx.at<float>(0,i)=x[i]+cxxx;
		xx.at<float>(1,i)=y[i]+cxxy;
		xx.at<float>(2,i)=1;
		rx.at<float>(0,i)=xi[i]+rxxx;
		rx.at<float>(1,i)=yi[i]+rxxy;
	}
	Mat ttx = T*xx;
	Mat e = ttx-rx;

	//calculate error
	float err = 0;
	for (int i = 0;i<xi.size();i++){
		xt->push_back(ttx.at<float>(0,i));
		yt->push_back(ttx.at<float>(1,i));
	       	err += sqrt(e.at<float>(0,i)*e.at<float>(0,i)+e.at<float>(1,i)*e.at<float>(1,i));
	}
	printf("Error: %f\n",err/xi.size());
}

float dist(float x,float y, float rx,float ry)
{
	float dx = x-rx;
	float dy = y-ry;
	return sqrt(dx*dx+dy*dy);
}

int main(int argc,char* argv[])
{
	/*read input*/
	vector<float> camX,camY,radX,radY,radC,lasX,lasY,lasC,radTX,radTY,lasTX,lasTY; 
	if(argc<3)
	{
		fprintf(stderr,"usage: %s referencePoints.txt points.txt \n",argv[0]); 
		return 0;
	}

	ifstream inFile(argv[1]);
	ifstream outFile(argv[2]);
	float cx,cy,rx,ry,rc,lx,ly,lc;
	string ret;

	while(inFile >> ret >> cx >> cy >> rx >>ry >> rc >> lx >> ly >> lc)
	{
		camX.push_back(cx);
		camY.push_back(cy);
		radTX.push_back(rx);
		radTY.push_back(ry);
		radC.push_back(rc);
		lasTX.push_back(lx);
		lasTY.push_back(ly);
		lasC.push_back(lc);
	}
	/*perform transformations*/
	transformRot(camX,camY,radTX,radTY,&radX,&radY);
	transformRot(camX,camY,lasTX,lasTY,&lasX,&lasY);
	float wr,wl,kfX,kfY,radD,lasD,kfD,sfD,sfX,sfY;
	radD=lasD=sfD=kfD=0;	
	for (int i = 0;i<camX.size();i++){
		wr = radC[i];
		wl = lasC[i];
		kfX = (radX[i]*wr+lasX[i]*wl)/(wr+wl);
		kfY = (radY[i]*wr+lasY[i]*wl)/(wr+wl);
		if (wr > wl) {
			sfX = radX[i];
			sfY = radY[i];
		}else{
			sfX = lasX[i];
			sfY = lasY[i];
		}
		lasD += dist(lasX[i],lasY[i],camX[i],camY[i]);
		radD += dist(radX[i],radY[i],camX[i],camY[i]);
		kfD += dist(kfX,kfY,camX[i],camY[i]);
		sfD += dist(sfX,sfY,camX[i],camY[i]);
		printf("Las/Rad/KF/SF %f %f %f %f\n",dist(lasX[i],lasY[i],camX[i],camY[i]),dist(radX[i],radY[i],camX[i],camY[i]),dist(kfX,kfY,camX[i],camY[i]),dist(sfX,sfY,camX[i],camY[i]));
	}
	lasD = lasD/camX.size();
	radD = radD/camX.size();
	sfD = sfD/camX.size();
	kfD = kfD/camX.size();
	printf("Sum Las/Rad/KF/SF %f %f %f %f\n",lasD,radD,kfD,sfD);

	return 0;
}

