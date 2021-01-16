#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "DIPfunctions.h"
#include "DIPfunctions.cpp"

#include <QDebug>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <fstream>

using namespace std;
using namespace cv;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::SHOW(Mat image){
    //显示图片
    QImage Image = QImage((const unsigned char*)(image.data),image.cols,image.rows,image.step,QImage::Format_RGB888);
    QPixmap pixmap = QPixmap::fromImage(Image.scaled(ui->graphicsView->width()-10,ui->graphicsView->height()-10));
    QGraphicsScene* scene = new QGraphicsScene;
    scene->addPixmap(pixmap);
    ui->graphicsView->setScene(scene);
    ui->graphicsView->show();
}
//打开按钮
void MainWindow::on_action_triggered()
{
    //获取文件名
    QString str = QFileDialog::getOpenFileName(this,tr("open picture"),"C:",tr("pic(*jpg *png *bmp *raw)"));
    if(str.isEmpty()){
        qDebug()<<"open pic error\n";
        return;
    }
    QFileInfo info(str);
    string filename = str.toUtf8().data();
    //jpg&png图像显示
    if(info.suffix()=="jpg"||info.suffix()=="png"||info.suffix()=="bmp"){
        this->img = imread(filename);
        if(img.empty()){
            qDebug()<<"open pic error\n";
            return;
        }
        this->width=img.cols;
        this->height=img.rows;
        cvtColor(img,img,COLOR_BGR2RGB);
    }
    //raw图像显示
    else if(info.suffix()=="raw"){
        bool ok;
        this->width=QInputDialog::getInt(this,tr("input pic'width"),tr("input pic'width"),0,0,10000,1,&ok);
        if(ok)qDebug()<<width<<"\n";
        else return;
        this->height=QInputDialog::getInt(this,tr("input pic'height"),tr("input pic'height"),0,0,10000,1,&ok);
        if(ok)qDebug()<<height<<"\n";
        else return;
        uchar* pixBuffer = new uchar[width*height];
        ifstream in(filename,std::ios::in|std::ios::binary);
        in >> noskipws;
        uchar data;
        for (int i = 0; i < height * width; i++) {
            if (in.eof()) {
                break;
            }
            in >> data;
            if (!in.fail()) {
                pixBuffer[i] = data;
            }
        }
        in.close();
        Mat channel(height,width,CV_8UC1,pixBuffer);
        this->img = Mat::zeros(channel.rows,channel.cols,CV_8UC3);
        vector<Mat> channels;
        for (int i=0;i<3;i++)
        {
            channels.push_back(channel);
        }
        merge(channels,img);
    }
    //将三通道Mat复制到orimg中作为副本留存
    this->img.copyTo(this->orimg);
    SHOW(this->img);
}
//灰度线性变换
void MainWindow::on_action_2_triggered()
{
    if(this->img.data==nullptr){return;}
    //获取线性变换的参数
    bool ok;
    double a = QInputDialog::getDouble(this,tr("input a"),tr("input a"),0,-10,10,1,&ok);
    if(ok)qDebug()<<a<<"\n";
    else return;
    double b = QInputDialog::getDouble(this,tr("input b"),tr("input b"),0,-255,255,1,&ok);
    if(ok)qDebug()<<b<<"\n";
    else return;
    //线性变换
    img = linear_transformation(img,a,b);
    //显示图片
    SHOW(this->img);
}
//均值滤波
void MainWindow::on_action_6_triggered()
{
    if(this->img.data==nullptr){return;}
    img = LPF1(img);
    SHOW(img);
}
//加权滤波
void MainWindow::on_action_7_triggered()
{
    if(this->img.data==nullptr){return;}
    img = LPF2(img);
    SHOW(img);
}
//自定义卷积
void MainWindow::on_action_10_triggered()
{
    //判断读入图像是否为空
    if(this->img.data==nullptr){return;}
    //获取卷积核的大小和数值
    bool ok=false;
    int kernelwidth=QInputDialog::getInt(this,tr("input kernel width"),tr("input kernel width"),0,0,20,1,&ok);
    if(ok)qDebug()<<kernelwidth<<"\n";
    else return;
    int kernelheight=QInputDialog::getInt(this,tr("input kernel height"),tr("input kernel height"),0,0,20,1,&ok);
    if(ok)qDebug()<<kernelheight<<"\n";
    else return;
    Mat_<float> kernel(kernelheight,kernelwidth,3);
    for(int i=0;i<kernelheight;i++){
        for(int j=0;j<kernelwidth;j++){
            kernel.at<float>(i,j)=QInputDialog::getDouble(this,QString("input kernel at %1col %2row").arg(i+1).arg(j+1),QString("input kernel at %1col %2row").arg(i+1).arg(j+1),0,-2147483647,2147483647,2,&ok);
            if(ok)qDebug()<<kernel.at<float>(i,j)<<"\n";
            else return;
        }
    }
    Mat result(img.rows + 2, img.cols + 2, CV_8UC3);
    Convolution(img,result,kernel);
    img = result;
    SHOW(img);
}
//拉普拉斯算子
void MainWindow::on_action_5_triggered()
{
    if(this->img.data==nullptr){return;}
    img = HPF1(img);
    SHOW(img);
}
//拉普拉斯增强
void MainWindow::on_action_11_triggered()
{
    if(this->img.data==nullptr){return;}
    img = HPF2(img);
    SHOW(img);
}
//Sobel算子
void MainWindow::on_actionsobel_triggered()
{
    if(this->img.data==nullptr){return;}
    img = HPF3(img);
    SHOW(img);
}
//恢复原图
void MainWindow::on_action_14_triggered()
{
    this->orimg.copyTo(this->img);
    SHOW(img);
}
//保存
void MainWindow::on_action_15_triggered()
{
    //获取文件名
    QString str = QFileDialog::getSaveFileName(this,tr("open picture"),"C:",tr("pic(*jpg *png *bmp *raw)"));
    if(str.isEmpty()){
        qDebug()<<"save pic error\n";
        return;
    }
    QFileInfo info(str);
    string filename = str.toUtf8().data();
    //jpg&png图像保存
    if(info.suffix()=="jpg"||info.suffix()=="png"||info.suffix()=="bmp"){
        imwrite(filename,this->img);
    }
    //raw图像分三个波段保存
    else if(info.suffix()=="raw"){
        ofstream out1(filename.substr(0,filename.length()-4)+"_Band1.raw",std::ios::out|std::ios::binary);
        ofstream out2(filename.substr(0,filename.length()-4)+"_Band2.raw",std::ios::out|std::ios::binary);
        ofstream out3(filename.substr(0,filename.length()-4)+"_Band3.raw",std::ios::out|std::ios::binary);
        for (int row = 0; row < height; row++)
        {
            Vec3b* ptr = img.ptr<Vec3b>(row);
            for (int col = 0; col < width; col++)
            {
                out1<<ptr[col][0];
                out2<<ptr[col][1];
                out3<<ptr[col][2];
            }
        }
        out1.close();
        out2.close();
        out3.close();
    }
}
//中值滤波
void MainWindow::on_action_9_triggered()
{
    if(this->img.data==nullptr){return;}
    img = MF(img);
    SHOW(img);
}
//显示图像直方图
void MainWindow::on_action_3_triggered()
{
    if(this->img.data==nullptr){return;}
    vector<Mat>hists = hist(img);
    //归一化
    int hist_h = 400;//直方图的图像的高
    int hist_w = 512;//直方图的图像的宽
    int bin_w = hist_w / 256;//直方图的等级
    Mat histImage(hist_w, hist_h, CV_8UC3, Scalar(255, 255, 255));//绘制直方图显示的图像
    normalize(hists[0], hists[0], 0, hist_h, NORM_MINMAX, -1, Mat());//归一化
    normalize(hists[1], hists[1], 0, hist_h, NORM_MINMAX, -1, Mat());
    normalize(hists[2], hists[2], 0, hist_h, NORM_MINMAX, -1, Mat());

    //绘制直方图
    for (int i = 1; i < 256; i++)
    {
        //绘制蓝色分量直方图
        line(histImage, Point((i - 1)*bin_w, hist_h - cvRound(hists[0].at<float>(i - 1))),
                Point((i)*bin_w, hist_h - cvRound(hists[0].at<float>(i))), Scalar(255, 0, 0), 2, 8);
        //绘制绿色分量直方图
        line(histImage, Point((i - 1)*bin_w, hist_h - cvRound(hists[1].at<float>(i - 1))),
                Point((i)*bin_w, hist_h - cvRound(hists[1].at<float>(i))), Scalar(0, 255, 0), 2, 8);
        //绘制红色分量直方图
        line(histImage, Point((i - 1)*bin_w, hist_h - cvRound(hists[2].at<float>(i - 1))),
                Point((i)*bin_w, hist_h - cvRound(hists[2].at<float>(i))), Scalar(0, 0, 255), 2, 8);
    }
    SHOW(histImage);
}
//关闭直方图的显示
void MainWindow::on_action_4_triggered()
{
    if(this->img.data==nullptr){return;}
    SHOW(img);
}
//状态法(基于谷底最小值的阈值)
void MainWindow::on_action_12_triggered()
{
    if(this->img.data==nullptr){return;}
    vector<Mat> hists = hist(img);
    int threshold = TwoPeakThreshold(hists[0]);
    vector<Mat> imgs;
    split(img,imgs);
    img = imgs[0];
    for(int i = 0;i<img.rows;i++){
        for(int j = 0;j<img.cols;j++){
            if(img.at<uchar>(i,j)>threshold){
                img.at<uchar>(i,j)=255;
            }else{
                img.at<uchar>(i,j)=0;
            }
        }
    }
    imgs.clear();
    imgs.push_back(img);
    imgs.push_back(img);
    imgs.push_back(img);
    merge(imgs,img);
    SHOW(img);
}
//判断分析法(Ostu大津法)
void MainWindow::on_action_13_triggered()
{
    if(this->img.data==nullptr){return;}
    vector<Mat> hists = hist(img);
    int threshold = OstuThreshold(hists[0],img.cols,img.rows);
    vector<Mat> imgs;
    split(img,imgs);
    img = imgs[0];
    for(int i = 0;i<img.rows;i++){
        for(int j = 0;j<img.cols;j++){
            if(img.at<uchar>(i,j)>threshold){
                img.at<uchar>(i,j)=255;
            }else{
                img.at<uchar>(i,j)=0;
            }
        }
    }
    imgs.clear();
    imgs.push_back(img);
    imgs.push_back(img);
    imgs.push_back(img);
    merge(imgs,img);
    SHOW(img);
}
//色彩平衡
void MainWindow::on_action_8_triggered()
{
    if(this->img.data==nullptr){return;}
    img = Color_balance(img);
    SHOW(img);
}
//灰度共生矩阵
void MainWindow::on_action_16_triggered()
{
    if(this->img.data==nullptr){return;}
    vector<Mat> imgs;
    split(img,imgs);
    bool ok;
    int band = QInputDialog::getInt(this,tr("input calculate band"),tr("input calculate band"),1,1,3,1,&ok);
    if(ok)qDebug()<<band<<"\n";
    img = Compression(imgs[band-1]);
    Mat glcm1 = GLCM(img,GLCM_TYPE::VERTICAL);
    Mat glcm2 = GLCM(img,GLCM_TYPE::HORIZATION);
    Mat glcm3 = GLCM(img,GLCM_TYPE::ANGLE45);
    Mat glcm4 = GLCM(img,GLCM_TYPE::ANGLE135);
    //获取文件名
    QString str = QFileDialog::getSaveFileName(this,tr("save glcm"),"C:",tr("text(*txt)"));
    if(str.isEmpty()){
        qDebug()<<"save glcm error\n";
        return;
    }
    QFileInfo info(str);
    string filename = str.toUtf8().data();
    ofstream out(filename);
    out<<setiosflags(ios::right)<<setiosflags(ios::fixed)<<setprecision(7);
    out<<"灰度共生矩阵——垂直"<<endl;
    for(int i=0;i<16;i++){
        for(int j = 0;j<16;j++){
            out<<setw(10)<<glcm1.at<int>(i,j)<<' ';
        }
        out<<'\n';
    }
    glcm1 = Normalize(glcm1);
    out<<"归一化后为:"<<endl;
    for(int i=0;i<16;i++){
        for(int j = 0;j<16;j++){
            out<<glcm1.at<float>(i,j)<<' ';
        }
        out<<'\n';
    }
    out<<"\n\n";
    out<<"灰度共生矩阵——水平"<<endl;
    for(int i=0;i<16;i++){
        for(int j = 0;j<16;j++){
            out<<setw(10)<<glcm2.at<int>(i,j)<<' ';
        }
        out<<'\n';
    }
    glcm2 = Normalize(glcm2);
    out<<"归一化后为:"<<endl;
    for(int i=0;i<16;i++){
        for(int j = 0;j<16;j++){
            out<<glcm2.at<float>(i,j)<<' ';
        }
        out<<'\n';
    }
    out<<"\n\n";
    out<<"灰度共生矩阵——45度"<<endl;
    for(int i=0;i<16;i++){
        for(int j = 0;j<16;j++){
            out<<setw(10)<<glcm3.at<int>(i,j)<<' ';
        }
        out<<'\n';
    }
    glcm3 = Normalize(glcm3);
    out<<"归一化后为:"<<endl;
    for(int i=0;i<16;i++){
        for(int j = 0;j<16;j++){
            out<<glcm3.at<float>(i,j)<<' ';
        }
        out<<'\n';
    }
    out<<"\n\n";
    out<<"灰度共生矩阵——135度"<<endl;
    for(int i=0;i<16;i++){
        for(int j = 0;j<16;j++){
            out<<setw(10)<<glcm4.at<int>(i,j)<<' ';
        }
        out<<'\n';
    }
    glcm4 = Normalize(glcm4);
    out<<"归一化后为:"<<endl;
    for(int i=0;i<16;i++){
        for(int j = 0;j<16;j++){
            out<<glcm4.at<float>(i,j)<<' ';
        }
        out<<'\n';
    }
    out<<"\n\n";

    out<<"水平ASM能量  :"<<ASM(glcm1)<<endl;
    out<<"水平CON对比度:"<<CON(glcm1)<<endl;
    out<<"水平IDM逆差矩:"<<IDM(glcm1)<<endl;
    out<<"水平ENT熵    :"<<ENT(glcm1)<<endl;
    out<<"水平COR自相关:"<<COR(glcm1)<<endl;

    out<<"垂直ASM能量  :"<<ASM(glcm2)<<endl;
    out<<"垂直CON对比度:"<<CON(glcm2)<<endl;
    out<<"垂直IDM逆差矩:"<<IDM(glcm2)<<endl;
    out<<"垂直ENT熵    :"<<ENT(glcm2)<<endl;
    out<<"垂直COR自相关:"<<COR(glcm2)<<endl;

    out<<"45度ASM能量  :"<<ASM(glcm3)<<endl;
    out<<"45度CON对比度:"<<CON(glcm3)<<endl;
    out<<"45度IDM逆差矩:"<<IDM(glcm3)<<endl;
    out<<"45度ENT熵    :"<<ENT(glcm3)<<endl;
    out<<"45度COR自相关:"<<COR(glcm3)<<endl;

    out<<"135度ASM能量  :"<<ASM(glcm4)<<endl;
    out<<"135度CON对比度:"<<CON(glcm4)<<endl;
    out<<"135度IDM逆差矩:"<<IDM(glcm4)<<endl;
    out<<"135度ENT熵    :"<<ENT(glcm4)<<endl;
    out<<"135度COR自相关:"<<COR(glcm4)<<endl;

    out<<"平均ASM能量  :"<<(ASM(glcm1)+ASM(glcm2)+ASM(glcm3)+ASM(glcm4))/4<<endl;
    out<<"平均CON对比度:"<<(CON(glcm1)+CON(glcm2)+CON(glcm3)+CON(glcm4))/4<<endl;
    out<<"平均IDM逆差矩:"<<(IDM(glcm1)+IDM(glcm2)+IDM(glcm3)+IDM(glcm4))/4<<endl;
    out<<"平均ENT熵    :"<<(ENT(glcm1)+ENT(glcm2)+ENT(glcm3)+ENT(glcm4))/4<<endl;
    out<<"平均COR自相关:"<<(COR(glcm1)+COR(glcm2)+COR(glcm3)+COR(glcm4))/4<<endl;
    out.close();
}
