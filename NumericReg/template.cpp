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

void cutTop(Mat& src, Mat& dstImg)//�����и�  
{
	int top, bottom;
	top = 0;
	bottom = src.rows;

	int i;
	for (i = 0; i < src.rows; i++)
	{
		int colValue = getRowSum(src, i);
		if (colValue>0)
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

int cutLeft(Mat& src, Mat& leftImg, Mat& rightImg)//�����и�
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

void getPXSum(Mat &src, int &diff)//��ȡ�������ص��  
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

int  getSubtract(Mat &src, int TemplateNum) //����ͼƬ���  
{
	Mat img_result;
	int min = 100000000;
	int serieNum = 0;
	for (int i = 0; i < TemplateNum; i++) {
		char name[20];
		sprintf_s(name, "pattern\\%d.png", i);
		//cout << name << endl;
		Mat Template = imread(name, CV_LOAD_IMAGE_GRAYSCALE);
		threshold(Template, Template, 100, 255, CV_THRESH_BINARY);
		threshold(src, src, 100, 255, CV_THRESH_BINARY);
		resize(src, src, Size(32, 48), 0, 0, CV_INTER_LINEAR);
		resize(Template, Template, Size(32, 48), 0, 0, CV_INTER_LINEAR);//�����ߴ�      
		//imshow("src", src);
		//imshow(name, Template);  
		//����ͼƬ���
		absdiff(Template, src, img_result);
		int diff = 0;
		getPXSum(img_result, diff);
		if (diff < min)
		{
			min = diff;
			serieNum = i;
		}
	}
	printf("��С������%d ", min);
	printf("ƥ�䵽��%d��ģ��ƥ���������%d\n", serieNum, serieNum);
	return serieNum;
}

void getPattern(char* patternPath)
{
	//��ͼƬ��ֵ��
	Mat src = imread(patternPath, CV_LOAD_IMAGE_GRAYSCALE);//��ȡͼƬ�����лҶȻ�����  
	threshold(src, src, 100, 255, CV_THRESH_BINARY_INV);//��ֵ��  
	//imshow("origin", src);//��ʾͼƬ  

	Mat leftImg, rightImg;
	int res = cutLeft(src, leftImg, rightImg);
	vector<int> compression_params;
	compression_params.push_back(IMWRITE_PNG_COMPRESSION);
	compression_params.push_back(9);
	char tag = '0';
	char path[15] = "pattern\\0.png";
	while (res == 0)
	{
		path[8] = tag;
		//cout << path << endl;
		tag += 1;
		imwrite(path, leftImg, compression_params);
		Mat srcTmp = rightImg;
		res = cutLeft(srcTmp, leftImg, rightImg);
	}
}

void Match(char* matchImgPath)
{
	//��ͼƬ��ֵ��
	Mat src = imread(matchImgPath, CV_LOAD_IMAGE_GRAYSCALE);//��ȡͼƬ�����лҶȻ�����  
	threshold(src, src, 100, 255, CV_THRESH_BINARY_INV);//��ֵ�� 
	//imshow("origin", src);//��ʾͼƬ  

	Mat leftImg, rightImg;
	int res = cutLeft(src, leftImg, rightImg);
	while (res == 0)
	{
		getSubtract(leftImg, 10);
		Mat srcTmp = rightImg;
		res = cutLeft(srcTmp, leftImg, rightImg);
	}
}

/*int main()
{
	//getPattern("sample.png");
	Match("test.png");
	//waitKey(100000);
	return 0;
}*/
