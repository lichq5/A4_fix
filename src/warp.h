#ifndef _WARP_H_
#define _WARP_H_

#include "public.h"

CImg<double> getProject_coff(CImg<double> vertex, double tarVer[][2]) {
	double srcVer[4][2];
	tarVer[0][0] = tarVer[0][1] = 0;
	tarVer[1][0] = 594;
	tarVer[1][1] = 0;
	tarVer[2][0] = 594;
	tarVer[2][1] = 841;
	tarVer[3][0] = 0;
	tarVer[3][1] = 841;

	double width_x1,width_y1,width_x2,width_y2,width_x11,width_y11,width_x22,width_y22;
	width_x1=width_y1=width_x2=width_y2=width_x11=width_y11=width_x22=width_y22 = 0;
	double temp_dis1,temp_dis2;
	temp_dis1 = temp_dis2 = 0;
	double min_arr[6], min_copy[6];
	
		min_arr[0] = sqrt( pow((vertex(0,0,0,0)-vertex(1,0,0,0)), 2) + pow((vertex(0,0,0,1)-vertex(1,0,0,1)),2) );
		min_copy[0] = min_arr[0];
		min_arr[1] = sqrt( pow((vertex(0,0,0,0)-vertex(2,0,0,0)), 2) + pow((vertex(0,0,0,1)-vertex(2,0,0,1)),2) );
		min_copy[1] = min_arr[1];
		min_arr[2] = sqrt( pow((vertex(0,0,0,0)-vertex(3,0,0,0)), 2) + pow((vertex(0,0,0,1)-vertex(3,0,0,1)),2) );
		min_copy[2] = min_arr[2];
		min_arr[3] = sqrt( pow((vertex(1,0,0,0)-vertex(2,0,0,0)), 2) + pow((vertex(1,0,0,1)-vertex(2,0,0,1)),2) );
		min_copy[3] = min_arr[3];
		min_arr[4] = sqrt( pow((vertex(1,0,0,0)-vertex(3,0,0,0)), 2) + pow((vertex(1,0,0,1)-vertex(3,0,0,1)),2) );
		min_copy[4] = min_arr[4];
		min_arr[5] = sqrt( pow((vertex(2,0,0,0)-vertex(3,0,0,0)), 2) + pow((vertex(2,0,0,1)-vertex(3,0,0,1)),2) );
		min_copy[5] = min_arr[5];
		sort(min_arr, min_arr+6);
		for (int i = 0; i < 6; i++) {
			int p = 0;
			int q = 0;
			if (i == 0) {
				p = 0;
				q = 1;
			}
			else if (i == 1) {
				p = 0;
				q = 2;
			} else if (i == 2) {
				p = 0;
				q = 3;
			} else if (i == 3) {
				p = 1;
				q = 2;
			} else if (i == 4) {
				p = 1;
				q = 3;
			} else {
				p = 2;
				q = 3;
			}

			if (min_copy[i] == min_arr[0]) {
				width_x1 = vertex(p,0,0,0);
				width_y1 = vertex(p,0,0,1);
				width_x2 = vertex(q,0,0,0);
				width_y2 = vertex(q,0,0,1);
			}
			if (min_copy[i] == min_arr[1]) {
				width_x11 = vertex(p,0,0,0);
				width_y11 = vertex(p,0,0,1);
				width_x22 = vertex(q,0,0,0);
				width_y22 = vertex(q,0,0,1);
			}
		}

	double tempx,tempy;

	//判断A4是倾向横斜摆放还是竖斜摆放
	if ( abs(width_y1-width_y2) > abs(width_x1-width_x2) ) { 
		if (width_x1 + width_x2 > width_x11+width_x22) {  //横斜放的话，选择左边或靠上的直线作为起始边,x之和较小的为左边直线
			tempx = width_x1;
			tempy = width_y1;
			width_x1 = width_x11;
			width_y1 = width_y11;
			width_x11 = tempx;
			width_y11 = tempy;

			tempx = width_x2;
			tempy = width_y2;
			width_x2 = width_x22;
			width_y2 = width_y22;
			width_x22 = tempx;
			width_y22 = tempy;
		}
		if (width_y1 < width_y2) {     //起始角点为左下角的角点。（y大的）
			tempx = width_x1;
			tempy = width_y1;
			width_x1 = width_x2;
			width_y1 = width_y2;
			width_x2 = tempx;
			width_y2 = tempy;
			}
	} else {  //如果是竖斜摆放的画，起始角点为左上角的角点。（x小的）
		if (width_y1 + width_y2 > width_y11+width_y22) {  //选择靠上的直线作为起始边,y之和较小的为上边直线
			tempx = width_x1;
			tempy = width_y1;
			width_x1 = width_x11;
			width_y1 = width_y11;
			width_x11 = tempx;
			width_y11 = tempy;

			tempx = width_x2;
			tempy = width_y2;
			width_x2 = width_x22;
			width_y2 = width_y22;
			width_x22 = tempx;
			width_y22 = tempy;
		}
		if (width_x1 > width_x2) {   //起始角点为左上角的角点。（x小的）
			tempx = width_x1;
			tempy = width_y1;
			width_x1 = width_x2;
			width_y1 = width_y2;
			width_x2 = tempx;
			width_y2 = tempy;
		}
	}
	
	srcVer[0][0] = width_x1;
	srcVer[0][1] = width_y1;
	srcVer[1][0] = width_x2;
	srcVer[1][1] = width_y2;
	temp_dis1 = sqrt( pow((width_x2-width_x11), 2) + pow((width_y2-width_y11),2) );
	temp_dis2 = sqrt( pow((width_x2-width_x22), 2) + pow((width_y2-width_y22),2) );
	//判断另一条平行或接近平行的边的点是否与前一条边的结束点四邻接
	if (temp_dis1 < temp_dis2) {
		srcVer[2][0] = width_x11;
		srcVer[2][1] = width_y11;
		srcVer[3][0] = width_x22;
		srcVer[3][1] = width_y22;
	} else {
		srcVer[2][0] = width_x22;
		srcVer[2][1] = width_y22;
		srcVer[3][0] = width_x11;
		srcVer[3][1] = width_y11;
	}

	for (int i = 0; i < 4; i++) {
		cout << "src: " << srcVer[i][0] << " " << srcVer[i][1] << endl;
		cout << "tar: " << tarVer[i][0] << " " << tarVer[i][1] << endl;
	}
	/*-----------投影变换矩阵--------------*/
	CImg<double> Pro_Matrix(8,8,1,1);
	for (int p = 0; p < 4; p++) {
		int i = p*2;
		Pro_Matrix(0,i,0,0) = tarVer[p][0];
		Pro_Matrix(1,i,0,0) = tarVer[p][1];
		Pro_Matrix(2,i,0,0) = 1;
		Pro_Matrix(3,i,0,0) = 0;
		Pro_Matrix(4,i,0,0) = 0;
		Pro_Matrix(5,i,0,0) = 0;
		Pro_Matrix(6,i,0,0) = -tarVer[p][0]*srcVer[p][0];
		Pro_Matrix(7,i,0,0) = -tarVer[p][1]*srcVer[p][0];

		int j = i+1;
		Pro_Matrix(0,j,0,0) = 0;
		Pro_Matrix(1,j,0,0) = 0;
		Pro_Matrix(2,j,0,0) = 0;
		Pro_Matrix(3,j,0,0) = tarVer[p][0];
		Pro_Matrix(4,j,0,0) = tarVer[p][1];
		Pro_Matrix(5,j,0,0) = 1;
		Pro_Matrix(6,j,0,0) = -tarVer[p][0]*srcVer[p][1];
		Pro_Matrix(7,j,0,0) = -tarVer[p][1]*srcVer[p][1];
	}
	
	Pro_Matrix.invert(false);
	CImg<double> t(1,8,1,1);
	int count = 0;
	for (int i = 0; i < 4; i++) {
		cout << srcVer[i][0] << " " << srcVer[i][1] << endl;
		t(0,count++,0,0) = srcVer[i][0];
		t(0,count++,0,0) = srcVer[i][1];
	}
	for (int i = 0; i < 8; i++)
		cout << t(0,i,0,0) << endl;

	CImg<double> coeff(Pro_Matrix*t);
	cout << coeff.width() << " " <<  coeff.height() << endl;
	
	return coeff;
}

CImg<double> warping(CImg<double> vertex, CImg<unsigned char> testImg) {
	double tarVer[4][2] = {0};
	CImg<double> coeff = getProject_coff(vertex,  tarVer);

	CImg<unsigned char> target(595, 842, 1, 3);
	target.fill(0);
	double a = coeff(0,0,0,0);
	double b = coeff(0,1,0,0);
	double c = coeff(0,2,0,0);
	double d = coeff(0,3,0,0);
	double e = coeff(0,4,0,0);
	double f = coeff(0,5,0,0);
	double g = coeff(0,6,0,0);
	double h = coeff(0,7,0,0);
	for (int i = 0; i < 8; i++)
		cout << coeff(0,i,0,0) << endl;
	double u, v, dis_a, dis_b;
	int i, j;
	//testImg.display();
	cimg_forXYZC(target, x,y,z,p) {
		u = (a*x+b*y+c)/(g*x+h*y+1);
		v = (d*x+e*y+f)/(g*x+h*y+1);
	
		i = floor(u);
		j = floor(v);
		dis_a = u-i;
		dis_b = v-j;
		if (u >= 0 && u < testImg.width()-1 && v >= 0 && v < testImg.height()-1) {
		target(x, y, z, p) = (1-dis_a)*(1-dis_b)*testImg(i,j,z,p) + 
			(1-dis_a)*dis_b*testImg(i,j+1,z,p) + dis_a*(1-dis_b)*testImg(i+1,j,z,p) + 
			dis_a*dis_b*testImg(i+1,j+1,z,p);
		}
	}
	target.display();
	return target;
}
#endif