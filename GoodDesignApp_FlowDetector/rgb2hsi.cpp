/**
 * @file rgb2hsi.cpp
 * @brief convert coloring codings
 * @author Liu Yang, 912617287@qq.com, JeremyRobturtle@gmail.com
 * @version 
 * @date 2013-02-05
 */
#include <stdafx.h>
#include "rgb2hsi.h"
#include<math.h>

#define PI	(3.14159265359)

int rgb2hsi_h(int r, int g, int b){
	if(r==g && r==b) return 0;
	double tmp = (r-g)+(r-b);
	tmp /= 2*sqrt( double((r-g)*(r-g)+(r-b)*(g-b)) );
	tmp = acos(tmp);
	//convert to 360
	int degree = int(tmp*180/PI);
	if(g>=b)
		return degree;
	else
		return 360-degree;
}

double rgb2hsi_s(int r, int g, int b){
	int min = r<g?r:g;
	min = min<b?min:b;
	return 1 - 3*min/double(r+g+b);
}
