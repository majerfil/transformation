#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include "sysmat.h"

using namespace std;


vector<float> refX,refY,x,y,hom; 



vector<float> transform(vector<float> xi, vector<float> yi,vector<float> x,vector<float> y)
{
	//STrackedObject r[4];
	//STrackedObject o[4];
	/*specific to the pheromone system - compensates the fact, that the calibration patterns are displayed in a lower position than the robots
	  assumes that the camera above the field centre*/
	//float iX = dimX/cameraHeight*robotHeight/2;
	//float iY = dimY/cameraHeight*robotHeight/2;

	//float iX = dimX/(inp[0].x+inp[1].x+inp[2].x+inp[3].x)*4*off;
	//float iY = dimY/(inp[0].x+inp[1].x+inp[2].x+inp[3].x)*4*off;
	/*		

			r[0].x = robotRadius+iX;
			r[0].y = robotRadius+iY;
			r[1].x = dimX-robotRadius-iX;
			r[1].y = robotRadius+iY;
			r[2].x = robotRadius+iX;
			r[2].y = dimY-robotRadius-iY;
			r[3].x = dimX-robotRadius-iX;
			r[3].y = dimY-robotRadius-iY;
			for (int i = 0;i<4;i++){
			o[i].x = -inp[i].y/inp[i].x;
			o[i].y = -inp[i].z/inp[i].x;
			}
	 */
	double est[2*xi.size()][8];
	double vec[2*xi.size()][1];
	REAL det;
	for (int i = 0;i<xi.size();i++){
		est[2*i][0]=-1*x[i];
		est[2*i][1]=-1*y[i];
		est[2*i][2]=-1;
		est[2*i][3]=0;
		est[2*i][4]=0;
		est[2*i][5]=0;
		est[2*i][6]=xi[i]*x[i];
		est[2*i][7]=xi[i]*y[i];
		est[2*i+1][0]=0;
		est[2*i+1][1]=0;
		est[2*i+1][2]=0;
		est[2*i+1][3]=-1*x[i];
		est[2*i+1][4]=-1*y[i];
		est[2*i+1][5]=-1;
		est[2*i+1][6]=yi[i]*x[i];
		est[2*i+1][7]=yi[i]*y[i];
		vec[2*i][0]=xi[i];
		vec[2*i+1][0]=yi[i];
	}
	MATINV(8,1,est,vec,&det);
	for (int i = 0;i<8;i++)  hom.push_back(vec[i][0]);
	hom.push_back(1);
	//transformType = TRANSFORM_2D;
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

