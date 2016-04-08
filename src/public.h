#ifndef _PUBLIC_H_
#define _PUBLIC_H_

#include <iostream>
#include "CImg.h"
#include <cmath>
#include <algorithm>

#define H  1
#define PI 3.1415926

using namespace std;
using namespace cimg_library;

int round(double x) {
	double temp = floor(x)+0.5;
	return x >= temp? ceil(x) : floor(x);
}
#endif