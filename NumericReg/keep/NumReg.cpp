#include <iostream>   
#include <vector>
#include <opencv2/opencv.hpp> 
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
using namespace cv;
using std::cout;
using std::endl;
using std::vector;

int getColSum(Mat& src, int col)
{
	int ColSum = 0;
	for (int i = 0; i < src.rows; ++i) {
		ColSum += (src.at<uchar>(i, col));
	}
	return ColSum;
}

int getRowSum(Mat& src, int row)
{
	int RowSum = 0;
	for (int i = 0; i < src.cols; ++i) {
		RowSum += (src.at<uchar>(row, i));
	}
	return RowSum;
}

void cutTop(Mat& src, Mat& dstImg)//上下切割  
{
	int top, bottom;
	top = 0;
	bottom = src.rows;

	int i;
	for (i = 0; i < src.rows; i++)
	{
		int colValue = getRowSum(src, i);
		if (colValue > 0)
		{
			top = i;
			break;
		}
	}
	for (; i < src.rows; i++)
	{
		int colValue = getRowSum(src, i);
		if (colValue == 0)
		{
			bottom = i;
			break;
		}
	}

	int height = bottom - top;
	Rect rect(0, top, src.cols, height);
	dstImg = src(rect).clone();
}

int cutLeft(Mat& src, Mat& leftImg, Mat& rightImg)//左右切割
{
	//cout << "cutLeft" << endl;
	int left, right;
	left = -1;
	right = src.cols;
	int i = 0;
	for (i = 0; i < src.cols; ++i) {
		int colValue = getColSum(src, i);
		//cout << "col " << i << " : " << colValue << endl;
		if (colValue > 0) {
			left = i;
			break;
		}
	}
	if (left == -1) {
		return 1;
	}

	for (; i < src.cols; i++) {
		int colValue = getColSum(src, i);
		//cout << "col " << i << " : " << colValue << endl;
		if (colValue == 0) {
			right = i;
			break;
		}
	}
	int width = right - left;
	Rect rect(left, 0, width, src.rows);
	leftImg = src(rect).clone();
	Rect rectRight(right, 0, src.cols - right, src.rows);
	rightImg = src(rectRight).clone();
	cutTop(leftImg, leftImg);
	return 0;
}

void getPXSum(Mat &src, int &diff)//获取所有像素点和  
{
	threshold(src, src, 100, 255, CV_THRESH_BINARY);
	diff = 0;
	for (int i = 0; i < src.rows;i++)
	{
		for (int j = 0; j < src.cols; j++)
		{
			diff += src.at <uchar>(i, j);
		}
	}
}

int  getSubtract(Mat &src, int TemplateNum) //两张图片相减  
{
	Mat img_result;
	int min = 100000000;
	int serieNum = 0;
	for (int i = 0; i < TemplateNum; i++) {
		char name[30];
		sprintf_s(name, "source\\pattern\\%d.jpg", i);
		//cout << name << endl;
		Mat Template = imread(name, CV_LOAD_IMAGE_GRAYSCALE);
		threshold(Template, Template, 100, 255, CV_THRESH_BINARY);
		threshold(src, src, 100, 255, CV_THRESH_BINARY);
		resize(src, src, Size(60, 60), 0, 0, CV_INTER_LINEAR);
		resize(Template, Template, Size(60, 60), 0, 0, CV_INTER_LINEAR);//调整尺寸      
																		//imshow("src", src);
																		//imshow(name, Template);  
																		//两张图片相减
		absdiff(Template, src, img_result);
		int diff = 0;
		getPXSum(img_result, diff);
		if (diff < min)
		{
			min = diff;
			serieNum = i;
		}
	}
	//printf("最小距离是%d ", min);
	//printf("匹配到第%d个模板匹配的数字是%d\n", serieNum, serieNum);
	return serieNum;
}

void preTrans(Mat& src, Mat& dst, int day)
{
	int pos[10] = { 70, 215, 365, 510, 655, 800, 950 };
	resize(src, src, Size(1080, 1920), 0, 0, CV_INTER_LINEAR);
	Rect rect(pos[day], 1470, 60, 60);
	dst = src(rect).clone();
	int Sum = 0;
	for (int i = 0; i < 5; ++i) {
		for (int j = 0; j < 5; ++j) {
			Sum += dst.at<uchar>(i, j);
		}
	}
	Sum /= 25;
	if (Sum > 200)
		threshold(dst, dst, 200, 255, CV_THRESH_BINARY_INV);//二值化  
	else threshold(dst, dst, 200, 255, CV_THRESH_BINARY);//二值化  
	//imshow("origin", dst);//显示图片
}

void getPattern(char* patternPath)
{
	//对图片二值化
	Mat source = imread(patternPath, CV_LOAD_IMAGE_GRAYSCALE);//读取图片并进行灰度化处理  
	Mat src;
	preTrans(source, src, 2);

	Mat leftImg, rightImg;
	int res = cutLeft(src, leftImg, rightImg);
	vector<int> compression_params;
	compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);
	compression_params.push_back(100);
	char path[25] = "source\\pattern\\3.jpg";
	while (res == 0)
	{
		try {
			imwrite(path, leftImg, compression_params);
		}
		catch (cv::Exception& ex) {
			printf("Exception converting image to JPG format: %s\n", ex.what());
			return;
		}
		path[15] = '5';
		cout << path << endl;
		Mat srcTmp = rightImg;
		res = cutLeft(srcTmp, leftImg, rightImg);
	}
}

void Match(char* matchImgPath, int day)
{
	//对图片二值化
	Mat source = imread(matchImgPath, CV_LOAD_IMAGE_GRAYSCALE);//读取图片并进行灰度化处理  
	Mat src;
	preTrans(source, src, day);
	//imshow("origin", src);//显示图片  

	int ans = 0;
	Mat leftImg, rightImg;
	int res = cutLeft(src, leftImg, rightImg);
	while (res == 0)
	{
		ans = ans * 10 + getSubtract(leftImg, 10);
		Mat srcTmp = rightImg;
		res = cutLeft(srcTmp, leftImg, rightImg);
	}
	printf("识别结果： %d\n", ans);
}

int main()
{
	//getPattern("source\\1.jpg");
	Match("source\\4.jpg", 5);
	//waitKey(100000);
	return 0;
}
