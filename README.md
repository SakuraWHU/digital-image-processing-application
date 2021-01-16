# digital-image-processing-application
SakuraWHU created this digital-image-processing-application at his sophomore.


## 大二数字图像处理实习
这是SakuraWHU在大二的数字图像处理实习做的小成果
### 功能
- 打开图片并显示在qt界面的graphicsview中，支持jpg\png\bmp\raw等多种格式
- 保存图片，支持保存为jpg\png\bmp\raw,其中保存为raw格式会将图片分三波段分别保存
- 对图像进行线性变换 $Y\eq aX+b$  ，$X$为原图，$Y$为处理后结果
- 对图像进行空间域低通滤波，包含均值滤波和加权均值滤波两种
- 对图像进行空间域高通滤波，包含拉普拉斯算子，sobel算子，拉普拉斯增强算子三种
- 对图像进行二值化，包含峰谷法，Ostu大津法两种
- 对图像进行色彩平衡，采用95%明度(Y)作为阈值
- 对图像进行灰度共生矩阵的计算与基于共生矩阵的特征量提取

### 技术路线
使用了Qt作为基本界面显示，图像处理算法大多数采用造轮子方式编写，没有采用OpenCV的库函数，仅使用OpenCV库函数做图像读写
