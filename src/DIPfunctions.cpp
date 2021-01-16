#include"DIPfunctions.h"
using namespace cv;
using namespace std;

//灰度级的压缩
Mat Compression(Mat img) {
    for (int i = 0; i < img.rows; i++) {
        for (int j = 0; j < img.cols; j++) {
            img.at<uchar>(i, j) /= 16;
        }
    }
    return img;
}
//灰度共生矩阵的提取
Mat GLCM(Mat img,int GLCM_TYPE){
    Mat result = Mat::zeros(16,16,CV_32S);
    switch (GLCM_TYPE){
    //垂直
    case GLCM_TYPE::VERTICAL :{
        for(int i=0;i<img.rows-1;i++){
            for(int j=0;j<img.cols;j++){
                int p0 = img.at<uchar>(i,j);
                int p1 = img.at<uchar>(i+1,j);
                result.at<int>(p0,p1)++;
                result.at<int>(p1,p0)++;
            }
        }
        return result;
    }break;
    //水平
    case GLCM_TYPE::HORIZATION :{
        for(int i=0;i<img.rows;i++){
            for(int j=0;j<img.cols-1;j++){
                int p0 = img.at<uchar>(i,j);
                int p1 = img.at<uchar>(i,j+1);
                result.at<int>(p0,p1)++;
                result.at<int>(p1,p0)++;
            }
        }
        return result;
    }break;
    //45度角
    case GLCM_TYPE::ANGLE45 :{
        for(int i=0;i<img.rows-1;i++){
            for(int j=0;j<img.cols-1;j++){
                int p0 = img.at<uchar>(i,j);
                int p1 = img.at<uchar>(i+1,j+1);
                result.at<int>(p0,p1)++;
                result.at<int>(p1,p0)++;
            }
        }
        return result;
    }break;
    //135度角
    case GLCM_TYPE::ANGLE135 :{
        for(int i=0;i<img.rows-1;i++){
            for(int j=1;j<img.cols;j++){
                int p0 = img.at<uchar>(i,j);
                int p1 = img.at<uchar>(i+1,j-1);
                result.at<int>(p0,p1)++;
                result.at<int>(p1,p0)++;
            }
        }
        return result;
    }break;
    default:
        break;
    }
    return result;
}
//灰度共生矩阵的归一化
Mat Normalize(Mat glcm){
    glcm.convertTo(glcm,CV_32F);
    return (1.0/sum(glcm)[0])*glcm;
}
//特征量的计算

//ASM 能量（angular second moment)
double ASM(Mat glcm){
    double ans = sum(glcm.mul(glcm))[0];
    return ans;
}

//对比度（contrast）
double CON(Mat glcm){
    double ans = 0;
    for(int i =0;i<16;i++){
        for(int j = 0; j<16;j++){
            ans += pow((i-j),2)*double(glcm.at<float>(i,j));
        }
    }
    return ans;
}

//逆差矩（inverse different moment）
double IDM(Mat glcm){
    double ans = 0;
    for(int i =0;i<16;i++){
        for(int j = 0; j<16;j++){
            ans += double(glcm.at<float>(i,j))/(1+pow((i-j),2));
        }
    }
    return ans;
}

//熵（entropy）
double ENT(Mat glcm){
    double ans = 0;
    for(int i =0;i<16;i++){
        for(int j = 0; j<16;j++){
            ans += double(glcm.at<float>(i,j))*log2(double(glcm.at<float>(i,j))+1e-8);
        }
    }
    return -ans;
}

//自相关（correlation）
double COR(Mat glcm) {
    float u1 = 0, u2 = 0, sigma1 = 0, sigma2 = 0; float f3 = 0; float temp[3];
    //u1和u2:
    for (int i = 0; i < 16; i++) {
        temp[0] = 0; temp[1] = 0;
        for (int j = 0; j < 16; j++) {
            temp[0] += glcm.at<float>(i, j);
            temp[1] += glcm.at<float>(j, i);
        }
        u1 += temp[0] * i;
        u2 += temp[1] * i;
    }
    //sigma1(即sigma1的平方）&sigma2(即sigma2的平方）&f3:
    for (int i = 0; i < 16; i++) {
        temp[2] = 0;
        for (int j = 0; j < 16; j++) {
            temp[0] += glcm.at<float>(i, j);
            temp[1] += glcm.at<float>(j, i);
            temp[2] += glcm.at<float>(i, j) * i * j;
        }
        sigma1 += temp[0] * (i - u1) * (i - u1);
        sigma2 += temp[1] * (i - u2) * (i - u2);
    }
    //计算f3：
    f3 = (temp[2] - u1 * u2) / (sigma1 * sigma2);
    return double(f3);
}
