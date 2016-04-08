#include "detect_hough.h"
#include "warp.h"

#include "public.h"

int main() {
	CImg<unsigned char> testImg("..\\DataSet\\6.jpg");
	CImg<unsigned char> testCopy(testImg);
	int w = testImg.width();
	int h = testImg.height();
	int z = testImg.depth();
	int v = testImg.spectrum();
	/*---------高斯模糊---------*/
	testImg.blur(3);
	/*---------RGB转GREY-------*/
	CImg<double> testImg2grey(w,h,z,1);
	cimg_forXYZC(testImg2grey, x,y,z,c) {
		testImg2grey(x,y,z,c) = testImg(x,y,z,0)*0.299 +
		testImg(x,y,z,1)*0.587 + testImg(x,y,z,2)*0.114;
	}

	/*---------用sobel算子分别求x方向,y方向的梯度---------------*/
	CImgList<double> img_gradient = testImg2grey.get_gradient(0, 2);
	
 	CImg<double> img_grad_magnitude(w,h,z,1);
	CImg<double> img_NMSppress(w,h,z,1);
	cimg_forXYZC(img_grad_magnitude, x,y,z,c) {
		img_grad_magnitude(x,y,z,c) = sqrt(img_gradient[0](x,y,z,c)*img_gradient[0](x,y,z,c)
			+ img_gradient[1](x,y,z,c)*img_gradient[1](x,y,z,c));
	}
	//img_grad_magnitude.display("magnitude");

	/*--------用梯度方向和非极大值抑制去掉冗余信息--------------*/
	img_NMSppress = NoneMaxSpress(img_grad_magnitude, img_gradient[0], img_gradient[1]);
	//img_NMSppress.display("NoneMS");
	/*-------------Hytheresis thresholding-----------------*/
	CImg<double> img_hytheshold(w,h,z,1);
	img_hytheshold = HysteresisThreshd(img_NMSppress, 130, 60); //130,60;130,80
	//img_hytheshold.display("edge");


	/*------------ Hough transfer and draw vertex--------------------*/
	CImg<double> vertex = HoughTransf(img_hytheshold, 0.4);//0.4,0.5
	//HoughImg.save_bmp("..\\Result\\result.bmp");
	warping(vertex, testCopy).save_jpeg("..\\DataSet\\result.jpeg");
	return 0;
}