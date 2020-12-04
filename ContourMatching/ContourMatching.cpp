#include "ShapeMatch.h"
#include <opencv2/core/types_c.h>

#include <direct.h>
#include <io.h>

using namespace std;
using namespace cv;


//获取所有的文件名  
void GetAllFiles(string path, vector<string>& files)
{

	//long   hFile = 0;
	intptr_t hFile = 0;
	//文件信息    
	struct _finddata_t fileinfo;
	string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			if ((fileinfo.attrib & _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
				{
					files.push_back(p.assign(path).append("\\").append(fileinfo.name));
					GetAllFiles(p.assign(path).append("\\").append(fileinfo.name), files);
				}
			}
			else
			{
				files.push_back(p.assign(path).append("\\").append(fileinfo.name));
			}

		} while (_findnext(hFile, &fileinfo) == 0);

		_findclose(hFile);
	}

}

int max_value(vector<double> v)
{
	double max_v = v[0];
	int max_index = 0;
	for (int i = 1; i < v.size(); i++)
	{
		if (v[i] > max_v)
			max_index = i;
	}
	return max_index;
}




int main()
{
	string templatePath = ".\\Input\\ClassiTemplate";
	vector<string> template_file;

	//读取所有的文件，包括子文件的文件  
	GetAllFiles(templatePath, template_file);
	for (int i = 0; i < template_file.size(); i++)
	{
		cout << template_file[i] << endl;
	}

	string filePath = ".\\Input\\file";
	vector<string> file;
	GetAllFiles(filePath, file);
	for (int i = 0; i < file.size(); i++)
	{
		cout << file[i] << endl;
	}
	int f = 0;
	while (f < file.size())
	{
		ShapeMatch shapematch;
		//Mat templateArr = imread("1.BMP");
		IplImage* templateArr = cvLoadImage(file[f].c_str(), -1);
		CvSize templateSize = cvSize(templateArr->width, templateArr->height);
		IplImage* grayTemplateImg = cvCreateImage(templateSize, IPL_DEPTH_8U, 1);

		// Convert color image to gray image.
		if (templateArr->nChannels == 3)
		{
			cvCvtColor(templateArr, grayTemplateImg, CV_RGB2GRAY);
		}
		else
		{
			cvCopy(templateArr, grayTemplateImg);
		}
		double minScore = 0.95;
		double greediness = 0.8;
		double maxContrast = 80;
		double minContrast = 20;
		int pyramidnums = 3;
		double anglestart = -4;
		double angleend = 4;
		double anglestep = 0.5;
		double scalestart = 0.98;
		double scaleend = 1.02;
		double scalestep = 0.005;
		shapematch.CreateMatchModel(grayTemplateImg, maxContrast, minContrast, pyramidnums, anglestart, angleend, anglestep, scalestart, scaleend, scalestep);;

		cout << endl << file[f] << "    ";

		vector<double> allscore;
		int t = 0;
		while (t < template_file.size())
		{

			char s[50];
			sprintf(s, template_file[t].c_str(), t);
			//Mat image = imread(s, 1);
			//IplImage *searchImage = &IplImage(image);
			IplImage* searchImage = cvLoadImage(s, 1);
			CvPoint result = cv::Point(0, 0);
			double score = shapematch.FindGeoMatchModel(searchImage, minScore, greediness, &result, pyramidnums, anglestart, angleend, anglestep, scalestart, scaleend, scalestep);
			//cvReleaseImage(&searchImage);
			allscore.push_back(score);
			cout << score << " ";
			//double time1 = clock();
			//double time2 = clock();
			//cout << "cost time:" << (time2 - time1) / 1000 << endl;
			//imwrite("result.bmp", image);
			//destroyWindow(s);
			t++;

		}
		double category = max_value(allscore);
		cout << category << endl;

		f++;

	}


	
	
	return 0;
}