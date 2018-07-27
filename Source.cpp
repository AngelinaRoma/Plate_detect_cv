
#include <opencv2/opencv.hpp>
#include <iostream>
using namespace std;
using namespace cv;

// поиск контура
void findLabel(IplImage* img, float max_ratio, float min_ratio, int max_area, int min_area, int Min_Lenght_Fo,
	CvBox2D* boxes, CvRect *Rects, double* areas, int* all_boxes)
{
	const float Min_Fill_Area = 0.2f;
	CvMemStorage* storage = cvCreateMemStorage(0);

	CvSeq* contours = 0;
	cvFindContours(img, storage, &contours, sizeof(CvContour),
		CV_RETR_LIST, CV_CHAIN_APPROX_NONE, cvPoint(0, 0));

	int all = 0;
	for (CvSeq* c = contours; c != NULL; c = c->h_next)
	{
		
		CvBox2D b = cvMinAreaRect2(c);
		if (b.size.height < Min_Lenght_Fo || b.size.width < Min_Lenght_Fo ||
			(b.size.width*b.size.height) < min_area || (b.size.width*b.size.height) > max_area) continue;
		if (fabs(cvContourArea(c)) / (b.size.width*b.size.height) < Min_Fill_Area) continue;
		
	
		

		boxes[all] = b;
		Rects[all] = cvBoundingRect(c);
		
		float ratio = (float) Rects[all].width/ Rects[all].height;
		if (ratio < min_ratio || ratio > max_ratio) continue;

		if (areas != NULL) areas[all] = cvContourArea(c);
		all++;
		if (all >= (*all_boxes)) {
			break;
		}
	}

	(*all_boxes) = all;

	cvReleaseMemStorage(&storage);
}

int main()
{
	char filename[] = "pic1.jpg";
	
IplImage* Img = cvLoadImage(filename, 0); 
IplImage* Gray = cvCreateImage(cvSize(218, 58), 8, 1); 
IplImage* Img2 = cvCreateImage(cvSize(218, 58), 8, 1);
IplImage* Bin = cvCreateImage(cvSize(218, 58), 8, 1);
IplImage* tmp= cvCreateImage(cvSize(21, 33), 8, 1);;

// нормализуем изображение
cvResize(Img, Gray, INTER_LINEAR);
cvSmooth(Gray, Img2, CV_BILATERAL, 3, 3);
cvThreshold(Gray, Img2, 128, 255, CV_THRESH_BINARY);
cvCopy(Img2, Bin);

// изображение после преобразований
namedWindow("image1", WINDOW_AUTOSIZE);
cvShowImage("image1", Img2);


CvBox2D b[20];
CvRect r[20];
double a[20];
int all = 20;


//ищем контуры подходящей формы
findLabel(Img2, 0.9, 0.4, 2500, 250, 5, b, r, a, &all); 


IplImage* dst1=cvCreateImage(cvSize(218, 58), 8, 1);;
double step = 0;
int t = 0;
CvRect key;

// сортируем контуры по возрастанию координаты х
for (int i = 1; i < all; i++) {
	key = r[i];
	t = i - 1;
	while (t >= 0 && r[t].x > key.x) {
		r[t + 1] = r[t];
		t = t - 1;
		r[t + 1] = key;
	}
}


for (int j = 0; j < all; j++)
{


	//выбираем в качестве ИОР найденные символы
	cvSetImageROI(Bin, r[j]);

	cvSetImageROI(dst1, cvRect(0 + step, 0, 21, 33));   

	step += 21;		

	// собираем символы в строку
	cvResize(Bin, tmp, INTER_LINEAR);
	cvCopy(tmp, dst1);
	//cvAddWeighted(Bin, 1, tmp[j], 0, 0.5, dst1);

	//сброс ИОР

	cvResetImageROI(Bin);
	cvResetImageROI(dst1);


	cvRectangle(Gray, cvPoint(r[j].x, r[j].y), cvPoint(r[j].x + r[j].width, r[j].y + r[j].height), CV_RGB(255, 0, 0), 3);

}

namedWindow("image", WINDOW_AUTOSIZE);
cvShowImage("image", Gray);
namedWindow("image2", WINDOW_AUTOSIZE);
cvShowImage("image2", dst1);

waitKey(0);
}
