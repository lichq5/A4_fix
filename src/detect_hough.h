#ifndef _DETECT_HOUGH_H_
#define _DETECT_HOUGH_H_

#include "public.h"


/*----------refer to the website http://blog.csdn.net/kezunhai/article/details/11620357 ------------*/
CImg<double> NoneMaxSpress(CImg<double> magnitude, CImg<double> gradientX, CImg<double> gradientY) {
	double weight;
	double g1, g2, g3, g4;    //四个梯度方向的比较点
	double mag1, mag2, mag3;
	double gx, gy;
	int w = magnitude.width();
	int h = magnitude.height();
	int z = magnitude.depth();
	int v = magnitude.spectrum();
	CImg<double> NMSuppress(w,h,z,v);
	NMSuppress.fill(0);
	cimg_forXYZC(NMSuppress, x,y,z,c) {
		//假设图片边界不可能为边缘
		if (x == 0 || x == w-1 || y == 0 || y == h-1)
			continue;
		//梯度为0，不可能为边缘
		if (magnitude(x,y,z,c) == 0)
			NMSuppress(x,y,z,c) = 0;
		else {
			// the magnitude of the current position
			mag2 = magnitude(x,y,z,c);
			gx = fabs(gradientX(x,y,z,c));
			gy = fabs(gradientY(x,y,z,c));
			//如果方向导数y分量比x分量大，说明导数趋向y方向
			if (gy > gx) {
				weight = fabs(gx/gy);
				g2 = magnitude(x,y-1,z,c);
				g4 = magnitude(x,y+1,z,c);
				//如果符号相同
				if (gx*gy > 0) {
					g1 = magnitude(x-1,y-1,z,c);
					g3 = magnitude(x+1,y+1,z,c);
				} else {
					g1 = magnitude(x+1,y-1,z,c);
					g3 = magnitude(x-1,y+1,z,c);
				}
			} else {
				weight = fabs(gy/gx);
				g2 = magnitude(x+1,y,z,c);
				g4 = magnitude(x-1,y,z,c);
				if (gx*gy > 0) {
					g1 = magnitude(x+1,y+1,z,c);
					g3 = magnitude(x-1,y-1,z,c);
				} else {
					g1 = magnitude(x-1,y-1,z,c);
					g3 = magnitude(x+1,y+1,z,c);
				}
			}
			//按权值计算亚像素点的梯度
			mag1 = weight*g1 + (1-weight)*g2;
			mag3 = weight*g3 + (1-weight)*g4;

			if (mag2 > mag1 && mag2 >= mag3)
				NMSuppress(x,y,z,c) = mag2;
			else
				NMSuppress(x,y,z,c) = 0;
		}
	}

	return NMSuppress;
}

CImg<double> HysteresisThreshd(CImg<double> NMSupress, double high, double low) {
	int w = NMSupress.width();
	int h = NMSupress.height();
	int z = NMSupress.depth();
	int v = NMSupress.spectrum();
	cimg_forXYZC(NMSupress, x,y,z,c) {
		//假设图片边界不可能为边缘
		if (x == 0 || x == w-1 || y == 0 || y == h-1)
			continue;
		//看周围8个相邻像素的梯度是否>low
		if (NMSupress(x,y,z,c) < low)
			NMSupress(x,y,z,c) = 0;
		else if (NMSupress(x,y,z,c) < high) {
			if (NMSupress(x-1,y-1,z,c) > low)
				NMSupress(x,y,z,c) = high;
			else if (NMSupress(x,y-1,z,c) > low)
				NMSupress(x,y,z,c) = high;
			else if (NMSupress(x+1,y-1,z,c) > low)
				NMSupress(x,y,z,c) = high;
			else if (NMSupress(x-1,y,z,c) > low)
				NMSupress(x,y,z,c) = high;
			else if (NMSupress(x+1,y,z,c) > low)
				NMSupress(x,y,z,c) = high;
			else if (NMSupress(x-1,y+1,z,c) > low)
				NMSupress(x,y,z,c) = high;
			else if (NMSupress(x,y+1,z,c) > low)
				NMSupress(x,y,z,c) = high;
			else if (NMSupress(x+1,y+1,z,c) > low)
				NMSupress(x,y,z,c) = high;
		}
	}
	return NMSupress;
}

CImg<double> drawVertex(CImg<double> vertex, CImg<double> testImg) {
	for (int i = 0; i < 4; i++)
		for (int x = vertex(i,0,0,0)-10; x < vertex(i,0,0,0)+10; x++)
			for (int y = vertex(i,0,0,1)-10; y < vertex(i,0,0,1)+10; y++)
				testImg(x,y,0,0) = 255;

	testImg.display("draw vertex");
	return testImg;
}

CImg<double> HoughTransf(CImg<double> testImg, double threshold) {
	int w = testImg.width();
	int h = testImg.height();
	int z = testImg.depth();
	int c = testImg.spectrum();
	int centerX = w/2;
	int centerY = h/2;
	int hough_space = 1000;
	double hough_cell = PI/hough_space;
	int diagonalLine = (int)round(sqrt(w*w + h*h));
	CImg<double> houghImg(hough_space,diagonalLine*2,z,c);
	houghImg.fill(0);
	// start hough transform now...
	double theta = 0;
	int r = 0;
	cimg_forXYZC(testImg, x,y,z,c) {
		if (testImg(x,y,z,c) != 0) {
			for (int i = 0; i < hough_space; i++) {
				double theta = i*hough_cell;
				r = (int)((y-centerY)*sin(theta) - (x-centerX)*cos(theta));
				r += diagonalLine;  //从原点diagonalLine开始，上下对称
				if (r >= 0 && r < 2*diagonalLine)
					houghImg(i,r,z,c) = houghImg(i,r,z,c) + 1;
			}
		}
	}
	//houghImg.display("HOugh");

	int max_hough = 0;
	cimg_forXYZC (houghImg, x,y,z,c) {
		if (houghImg(x,y,z,c) > max_hough)
			max_hough = houghImg(x,y,z,c);
	}
	
	//transfer back to image pixels space from hough space...
	int hough_threshold = (int)round(threshold*max_hough);
	cimg_forXYZC (houghImg, x,y,z,c) {
		if (houghImg(x,y,z,c) < hough_threshold)
			houghImg(x,y,z,c) = 0;
	}
	//houghImg.display("hough");
	CImg<double> houghImg_copy(houghImg);
	houghImg_copy.sort(false);
	
	int pass = 0;
	//表示是否已经找到一条宽或长
	bool x_match = false;
	bool y_match = false;
	double xb1 = 0;
	double xb2 = 0;
	double yb1 = 0;
	double yb2 = 0;
	double xtheta1 = 0;
	double xtheta2 = 0;
	double ytheta1 = 0;
	double ytheta2 = 0;
	int r1 = 0;
	int r2 = 0;
	int cell1 = 0;
	int cell2 = 0;
	int count = 0;
	bool finish_x = false;
	bool finish_y = false;
	CImg<double> vertex(4,1,1,2);  //用于保存角点
	int v = 0;
	cimg_forXYZC(houghImg, hx,hy,z,c) {
		if (count == 4) break;
		if (houghImg(hx,hy,z,c) == houghImg_copy(pass,0,0,0)) {
			if (hx*180/hough_space > 45 && hx*180/hough_space <= 135) {
				if (!finish_x) {
				if (x_match) {
					xb2 = (hy-diagonalLine)/sin(hx*hough_cell);
					xtheta2 = hx*180/hough_space;
					cell2 = hx;
					double min_theta = xtheta1 > xtheta2? xtheta2 : xtheta1;
					double max_theta = xtheta1 > xtheta2? xtheta1 : xtheta2; 
					if (((min_theta+180-max_theta < 10 && xb1*xb2>0) ||abs(xtheta2- xtheta1) < 10) && abs(cell1-cell2) >= 5
						|| (xtheta1 == xtheta2 && cell1 == cell2 && xb1*xb2 < 0) ) { //>10
						for (int x = 0; x < w; x++) {
							int y = (int)((hy-diagonalLine+(x-centerX)*cos(hx*hough_cell))/sin(hx*hough_cell))+centerY;
							if (y < h && y >= 0) {
								if (testImg(x,y,0,0) == 255) {
									cout << "角点坐标为: (" << x << "," << y << ")" << endl;
									vertex(v,0,0,0) = x;
									vertex(v++,0,0,1) = y;
								}else
									testImg(x,y,0,0) = 255;
								//testImg(x,y,0,0) = 255;
							}
						}
						count++;
						//testImg.display();
						finish_x = true;
						//打印直线信息
						cout << "pass: " << pass << ", max: " << houghImg_copy(pass,0,0,0) << ", hx: " << hx
						<< ", hy: " << hy << ", theta: " << hx*180/hough_space << ", B: " << (hy-diagonalLine)/sin(hx*hough_cell)
						<< ", K: " << -sin(hx*hough_cell)/cos(hx*hough_cell) << endl;
					}
				
				} else {
					count++;
					x_match = true;
					cell1 = hx;
					xtheta1 = hx*180/hough_space;
					xb1 = (hy-diagonalLine)/sin(hx*hough_cell);
					for (int x = 0; x < w; x++) {
						int y = (int)((hy-diagonalLine+(x-centerX)*cos(hx*hough_cell))/sin(hx*hough_cell))+centerY;
						if (y < h && y >= 0) {
							if (testImg(x,y,0,0) == 255) {
								cout << "角点坐标为: (" << x << "," << y << ")" << endl;
								vertex(v,0,0,0) = x;
								vertex(v++,0,0,1) = y;
							} else
								testImg(x,y,0,0) = 255;
						}
					}
					//testImg.display();
					//打印直线信息
					cout << "pass: " << pass << ", max: " << houghImg_copy(pass,0,0,0) << ", hx: " << hx
						<< ", hy: " << hy << ", theta: " << hx*180/hough_space << ", B: " << (hy-diagonalLine)/sin(hx*hough_cell)
						<< ", K: " << -sin(hx*hough_cell)/cos(hx*hough_cell) << endl;
				}
				}
			} else {
				if (!finish_y) {
				if (y_match) {
					yb2 = (hy-diagonalLine)/sin(hx*hough_cell);
					ytheta2 = hx*180/hough_space;
					r2 = hy;
					double min_theta = ytheta1 > ytheta2? ytheta2 : ytheta1;
					double max_theta = ytheta1 > ytheta2? ytheta1 : ytheta2; 
					//如果角度基本互补，那么截距符号要一样，否则说明两条线基本重合,假设如果两条直线的夹角大于10，很有可能该直线不是要找的边缘
					if (((min_theta+180-max_theta < 10 && yb1*yb2 > 0) || (abs(ytheta2- ytheta1) < 10)) &&abs(r2-r1) > 10&& r2 > 0) {
						for (int y = 0; y < h; y++) {
							int x = (int)((-hy+diagonalLine+(y-centerY)*sin(hx*hough_cell))/cos(hx*hough_cell))+centerX;
							if (x < w && x >= 0) {
								if (testImg(x,y,0,0) == 255) {
									cout << "角点坐标为: (" << x << "," << y << ")" << endl;
									vertex(v,0,0,0) = x;
									vertex(v++,0,0,1) = y;
								} else
									testImg(x,y,0,0) = 255;
							}
						}
						count++;
						//testImg.display();
						finish_y = true;
						//打印直线信息
						cout << "pass: " << pass << ", max: " << houghImg_copy(pass,0,0,0) << ", hx: " << hx
						<< ", hy: " << hy << ", theta: " << hx*180/hough_space << ", B: " << (hy-diagonalLine)/sin(hx*hough_cell)
						<< ", K: " << -sin(hx*hough_cell)/cos(hx*hough_cell) << endl;
					}
				} else {
					count++;
					y_match = true;
					r1 = hy;
					ytheta1 = hx*180/hough_space;
					yb1 = (hy-diagonalLine)/sin(hx*hough_cell);
					for (int y = 0; y < h; y++) {
						int x = (int)((-hy+diagonalLine+(y-centerY)*sin(hx*hough_cell))/cos(hx*hough_cell))+centerX;
						if (x < w && x >= 0) {
							if (testImg(x,y,0,0) == 255) {
								cout << "角点坐标为: (" << x << "," << y << ")" << endl;
								vertex(v,0,0,0) = x;
								vertex(v++,0,0,1) = y;
							} else
								testImg(x,y,0,0) = 255;
						}
					}
					//testImg.display();
					//打印直线信息
					cout << "pass: " << pass << ", max: " << houghImg_copy(pass,0,0,0) << ", hx: " << hx
						<< ", hy: " << hy << ", theta: " << hx*180/hough_space << ", B: " << (hy-diagonalLine)/sin(hx*hough_cell)
						<< ", K: " << -sin(hx*hough_cell)/cos(hx*hough_cell) << endl;
					
				}
				}
			}
		hx = 0;
		hy = 0;
		pass++;
		}
	}
	testImg.display("line");

	//drawVertex(vertex,testImg);
	return vertex;
}

#endif