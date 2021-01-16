#pragma once
#include<iostream>
#include<opencv2/opencv.hpp>
using namespace std;
using namespace cv;

enum GLCM_TYPE{
    HORIZATION=0,
    VERTICAL=1,
    ANGLE45=2,
    ANGLE135=3
};
//灰度级的压缩
Mat Compression(Mat img);
//灰度共生矩阵的提取
Mat GLCM(Mat img,int GLCM_TYPE);
//灰度共生矩阵的归一化
Mat Normalize(Mat glcm);
//特征量的计算
//ASM 能量（angular second moment)
double ASM(Mat glcm);
//对比度（contrast）
double CON(Mat glcm);
//逆差矩（inverse different moment）
double IDM(Mat glcm);
//熵（entropy）
double ENT(Mat glcm);
//自相关（correlation）
double COR(Mat glcm);


