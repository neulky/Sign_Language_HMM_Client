#include <windows.h>
#include <iostream> 
#include <NuiApi.h>
#include <opencv2/opencv.hpp>
#include "DataDefine.h"

using namespace std;
using namespace cv;

#define RIGHTHAND "righthand"

Mat getDepthImage(HANDLE &depthEvent, HANDLE &depthStreamHandle, Mat &depthImage, Mat &fullDepthImage, point * HandPoint);
int extrudeRightHandPosition(HANDLE &skeletonEvent, point * HandPoint);
point HandPosition1[HAND_COUNT] = {};           //用于显示手部深度位置信息

Mat getColorImage(HANDLE &colorEvent, HANDLE &colorStreamHandle, Mat &colorImage, point* HandPoint);
/********************************************************************
*函数功能：通过Kinect采集左右手的位置
*handCount 手的数量，目前为2，左手和右手
*kinectDataPoint[][] 二维数组，记录左右手的位置
*********************************************************************/
int AcquireKinectData(point kinectDataPoint[][MAXFRAME], int* kinectFrameCount, vector<Mat> &rightMatSeq) 
{
	//Mat colorImage;
	//colorImage.create(480, 640, CV_8UC3);
	Mat depthImage;            //显示手部轨迹图像
	depthImage.create(240, 320, CV_8UC3);
	Mat fullDepthImage;        //显示整个深度图像
	fullDepthImage.create(240, 320, CV_8UC3);

	kinectFrameCount[LEFT_HAND_FLAG] = 0;
	kinectFrameCount[RIGHT_HAND_FLAG] = 0;

	namedWindow("RightHandTrack", CV_WINDOW_AUTOSIZE);
	namedWindow("FullDepthImage", CV_WINDOW_AUTOSIZE);
	namedWindow("handMat", CV_WINDOW_AUTOSIZE);
	//namedWindow("ColorImage", CV_WINDOW_AUTOSIZE);
	//namedWindow("handImage", CV_WINDOW_AUTOSIZE);

	HANDLE colorEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	HANDLE depthEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	HANDLE skeletonEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	HANDLE colorStreamHandle = NULL;
	HANDLE depthStreamHandle = NULL;

	HRESULT hr = NuiInitialize(NUI_INITIALIZE_FLAG_USES_COLOR | NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX | NUI_INITIALIZE_FLAG_USES_SKELETON);
	if (hr != S_OK)
	{
		cout << "NuiInitialize failed" << endl;
		return hr;
	}

	//hr = NuiImageStreamOpen(NUI_IMAGE_TYPE_COLOR, NUI_IMAGE_RESOLUTION_640x480, NULL, 2, colorEvent, &colorStreamHandle);
	//if (hr != S_OK)
	//{
	//	cout << "Open the color Stream failed" << endl;
	//	NuiShutdown();
	//	return hr;
	//}
	hr = NuiImageStreamOpen(NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX, NUI_IMAGE_RESOLUTION_320x240, NULL, 2, depthEvent, &depthStreamHandle);
	if (hr != S_OK)
	{
		cout << "Open the depth Stream failed" << endl;
		NuiShutdown();
		return hr;
	}

	hr = NuiSkeletonTrackingEnable(skeletonEvent, 0);//打开骨骼跟踪事件     
	if (hr != S_OK)
	{
		cout << "NuiSkeletonTrackingEnable failed" << endl;
		NuiShutdown();
		return hr;
	}

	int leftStartFlag = 0;   //开始标志  当开始记录帧时，startFlag置为1；当再次出现手部坐标为（0，0）且startFlag=1时，结束记录
	int rightStartFlag = 0;
	point  HandPosition[HAND_COUNT] = {};
	Mat handMat1;

	while (1)
	{
		if (WaitForSingleObject(skeletonEvent, INFINITE) == 0)
			extrudeRightHandPosition(skeletonEvent, HandPosition);      //修改数组HandPosition,及修改左右手坐标

		if (WaitForSingleObject(depthEvent, 0) == 0)
		{
			Mat &handMat = getDepthImage(depthEvent, depthStreamHandle, depthImage, fullDepthImage, HandPosition1);   //显示左右手坐标
			handMat1 = handMat.clone();
			imshow("handMat", handMat1);

		}
		//if (WaitForSingleObject(colorEvent, 0) == 0)
		//{
		//	Mat &handImage = getColorImage(colorEvent, colorStreamHandle, colorImage, HandPosition1);
		//	imshow("handImage", handImage);
		//}

		imshow("RightHandTrack", depthImage);
		imshow("FullDepthImage", fullDepthImage);
		//imshow("ColorImage", colorImage);


		if (cvWaitKey(1) == 27)      //此函数为等待函数，待考查
			break;

		if (HandPosition[RIGHT_HAND_FLAG].x == 0 && HandPosition[RIGHT_HAND_FLAG].y == 0 &&
			HandPosition[LEFT_HAND_FLAG].x == 0 && HandPosition[LEFT_HAND_FLAG].y == 0 && (rightStartFlag == 1 || leftStartFlag ==1))    //两只手同时都在下面，且曾经有一只手上去过
			break;

		if (HandPosition[RIGHT_HAND_FLAG].x == 0 && HandPosition[RIGHT_HAND_FLAG].y == 0 && rightStartFlag == 0)    //以右手为基准 当右手部位置第一次超过髋关节时开始提取帧，做完动作，当手部低于髋关节时结束帧提取
		{

		}
		else{
			rightStartFlag = 1;
			if (HandPosition[RIGHT_HAND_FLAG].x != 0 && HandPosition[RIGHT_HAND_FLAG].y != 0)
			{
				kinectDataPoint[RIGHT_HAND_FLAG][kinectFrameCount[RIGHT_HAND_FLAG]].x = HandPosition[RIGHT_HAND_FLAG].x;
				kinectDataPoint[RIGHT_HAND_FLAG][kinectFrameCount[RIGHT_HAND_FLAG]].y = HandPosition[RIGHT_HAND_FLAG].y;
				kinectDataPoint[RIGHT_HAND_FLAG][kinectFrameCount[RIGHT_HAND_FLAG]].z = HandPosition[RIGHT_HAND_FLAG].z;
				cout <<"Right: "<< "(" << kinectDataPoint[RIGHT_HAND_FLAG][kinectFrameCount[RIGHT_HAND_FLAG]].x << "," << kinectDataPoint[RIGHT_HAND_FLAG][kinectFrameCount[RIGHT_HAND_FLAG]].y << ","
					<< kinectDataPoint[RIGHT_HAND_FLAG][kinectFrameCount[RIGHT_HAND_FLAG]].z << ")" << endl;
				kinectFrameCount[RIGHT_HAND_FLAG]++;

				FileStorage handMat_w("handMat.xml", FileStorage::WRITE);
				handMat_w << RIGHTHAND << handMat1;
				handMat_w.release();

				rightMatSeq.push_back(handMat1);      //将该帧handMat存入右手Mat序列

			}
		}
		if (HandPosition[LEFT_HAND_FLAG].x == 0 && HandPosition[LEFT_HAND_FLAG].y == 0 && leftStartFlag == 0)    //以右手为基准 当右手部位置第一次超过髋关节时开始提取帧，做完动作，当手部低于髋关节时结束帧提取
		{

		}
		else{
			leftStartFlag = 1;
			if (HandPosition[LEFT_HAND_FLAG].x != 0 && HandPosition[LEFT_HAND_FLAG].y != 0)
			{
				kinectDataPoint[LEFT_HAND_FLAG][kinectFrameCount[LEFT_HAND_FLAG]].x = HandPosition[LEFT_HAND_FLAG].x;
				kinectDataPoint[LEFT_HAND_FLAG][kinectFrameCount[LEFT_HAND_FLAG]].y = HandPosition[LEFT_HAND_FLAG].y;
				kinectDataPoint[LEFT_HAND_FLAG][kinectFrameCount[LEFT_HAND_FLAG]].z = HandPosition[LEFT_HAND_FLAG].z;
				cout << "Left: " << "(" << kinectDataPoint[LEFT_HAND_FLAG][kinectFrameCount[LEFT_HAND_FLAG]].x << "," << kinectDataPoint[LEFT_HAND_FLAG][kinectFrameCount[LEFT_HAND_FLAG]].y << ","
					 << kinectDataPoint[LEFT_HAND_FLAG][kinectFrameCount[LEFT_HAND_FLAG]].z << ")" << endl;

				kinectFrameCount[LEFT_HAND_FLAG]++;
			}	
		}
	}

	NuiShutdown();
	return 1;

}
//Mat getColorImage(HANDLE &colorEvent, HANDLE &colorStreamHandle, Mat &colorImage, point* handPosition)
//{
//	Mat handImage(40, 40, CV_8UC3);
//	const NUI_IMAGE_FRAME *colorFrame = NULL;
//
//	NuiImageStreamGetNextFrame(colorStreamHandle, 0, &colorFrame);
//	INuiFrameTexture *pTexture = colorFrame->pFrameTexture;
//
//	NUI_LOCKED_RECT LockedRect;
//	pTexture->LockRect(0, &LockedRect, NULL, 0);
//
//	if (LockedRect.Pitch != 0)
//	{
//		for (int i = 0; i<colorImage.rows; i++)
//		{
//			uchar *ptr = colorImage.ptr<uchar>(i);  //第i行的指针                    
//			//每个字节代表一个颜色信息，直接使用uchar  
//			uchar *pBuffer = (uchar*)(LockedRect.pBits) + i * LockedRect.Pitch;
//			for (int j = 0; j<colorImage.cols; j++)
//			{
//				ptr[3 * j] = pBuffer[4 * j];  //内部数据是4个字节，0-1-2是BGR，第4个现在未使用   
//				ptr[3 * j + 1] = pBuffer[4 * j + 1];
//				ptr[3 * j + 2] = pBuffer[4 * j + 2];
//			}
//		}
//		int hand_i = 0;
//		for (int i = ((0 > int(handPosition[RIGHT_HAND_FLAG].y - 20)) ? 0 : int(handPosition[RIGHT_HAND_FLAG].y - 20)); i < ((colorImage.rows < int(handPosition[RIGHT_HAND_FLAG].y + 20)) ? colorImage.rows : int(handPosition[RIGHT_HAND_FLAG].y + 20)); i++)
//		{
//			uchar *ptr = handImage.ptr<uchar>(hand_i);
//			uchar *pBuffer = (uchar*)(LockedRect.pBits) + i * LockedRect.Pitch;
//			int hand_j = 0;
//			for (int j = ((0 > int(handPosition[RIGHT_HAND_FLAG].x - 20)) ? 0 : int(handPosition[RIGHT_HAND_FLAG].x - 20)); j < ((colorImage.cols < int(handPosition[RIGHT_HAND_FLAG].x + 20)) ? colorImage.cols : int(handPosition[RIGHT_HAND_FLAG].x + 20)); j++)
//			{
//				ptr[3 * hand_j] = pBuffer[4 * j];
//				ptr[3 * hand_j + 1] = pBuffer[4 * j + 1];
//				ptr[3 * hand_j + 2] = pBuffer[4 * j + 2];
//				hand_j++;
//			}
//			hand_i++;
//		}
//	}
//	else
//	{
//		cout << "捕捉色彩图像出现错误" << endl;
//	}
//
//	pTexture->UnlockRect(0);
//	NuiImageStreamReleaseFrame(colorStreamHandle, colorFrame);
//	return handImage;
//}

Mat getDepthImage(HANDLE &depthEvent, HANDLE &depthStreamHandle, Mat &depthImage, Mat &fullDepthImage, point * HandPoint)  //在深度图中显示手的位置，通过多个帧最终连成一个运动轨迹
{
	Mat handMat(40, 40, CV_8UC1, Scalar(0));

	const NUI_IMAGE_FRAME *depthFrame = NULL;

	NuiImageStreamGetNextFrame(depthStreamHandle, 0, &depthFrame);
	INuiFrameTexture *pTexture = depthFrame->pFrameTexture;

	NUI_LOCKED_RECT LockedRect;
	pTexture->LockRect(0, &LockedRect, NULL, 0);

	RGBQUAD q;

	if (LockedRect.Pitch != 0)
	{
		int handMat_x = 0;
		//设置范围
		for (int i = ((0 > int(HandPoint[RIGHT_HAND_FLAG].y - 20)) ? 0 : int(HandPoint[RIGHT_HAND_FLAG].y - 20)); i < ((depthImage.rows < int(HandPoint[RIGHT_HAND_FLAG].y + 20)) ? depthImage.rows : int(HandPoint[RIGHT_HAND_FLAG].y + 20)); i++)
		{
			uchar *ptr = depthImage.ptr<uchar>(i);
			uchar *pBuffer = (uchar*)(LockedRect.pBits) + i * LockedRect.Pitch;
			USHORT *pBufferRun = (USHORT*)pBuffer;

			uchar *ptr_hand = handMat.ptr<uchar>(handMat_x);      //找到第handMat_x行的头指针
			handMat_x++;
			
			int handMat_y = 0;
			for (int j = ((0 > int(HandPoint[RIGHT_HAND_FLAG].x - 20)) ? 0 : int(HandPoint[RIGHT_HAND_FLAG].x - 20)); j < ((depthImage.cols < int(HandPoint[RIGHT_HAND_FLAG].x + 20)) ? depthImage.cols : int(HandPoint[RIGHT_HAND_FLAG].x + 20)); j++)
			{
				int player = pBufferRun[j] & 7;
				int data = (pBufferRun[j] & 0xfff8) >> 3;
												
				uchar imageData = 255 - (uchar)(256 * data / 0x0fff);

				ptr_hand[handMat_y] = imageData;     //对handMat进行像素赋值
				handMat_y++;

				q.rgbBlue = q.rgbGreen = q.rgbRed = 0;


				switch (player)

				{
				case 0:
					q.rgbRed = 100;
					q.rgbBlue = 100;
					q.rgbGreen = 100;
					break;
				case 1:
					q.rgbRed = imageData;
					break;
				case 2:
					q.rgbGreen = imageData;
					break;
				case 3:
					q.rgbRed = imageData / 4;
					q.rgbGreen = q.rgbRed * 4;  //这里利用乘的方法，而不用原来的方法可以避免不整除的情况   
					q.rgbBlue = q.rgbRed * 4;  //可以在后面的getTheContour()中配合使用，避免遗漏一些情况   
					break;
				case 4:
					q.rgbBlue = imageData / 4;
					q.rgbRed = q.rgbBlue * 4;
					q.rgbGreen = q.rgbBlue * 4;
					break;
				case 5:
					q.rgbGreen = imageData / 4;
					q.rgbRed = q.rgbGreen * 4;
					q.rgbBlue = q.rgbGreen * 4;
					break;
				case 6:
					q.rgbRed = imageData / 2;
					q.rgbGreen = imageData / 2;
					q.rgbBlue = q.rgbGreen * 2;
					break;
				case 7:
					q.rgbRed = 100;
					q.rgbGreen = 100;
					q.rgbBlue = 100;
				}
				ptr[3 * j] = q.rgbBlue;
				ptr[3 * j + 1] = q.rgbGreen;
				ptr[3 * j + 2] = q.rgbRed;
			}
		}

		for (int i = ((0 > int(HandPoint[LEFT_HAND_FLAG].y - 10)) ? 0 : int(HandPoint[LEFT_HAND_FLAG].y - 10)); i < ((depthImage.rows < int(HandPoint[LEFT_HAND_FLAG].y + 10)) ? depthImage.rows : int(HandPoint[LEFT_HAND_FLAG].y + 10)); i++)
		{
			uchar *ptr = depthImage.ptr<uchar>(i);
			uchar *pBuffer = (uchar*)(LockedRect.pBits) + i * LockedRect.Pitch;
			USHORT *pBufferRun = (USHORT*)pBuffer;

			for (int j = ((0 > int(HandPoint[LEFT_HAND_FLAG].x - 10)) ? 0 : int(HandPoint[LEFT_HAND_FLAG].x - 10)); j < ((depthImage.cols < int(HandPoint[LEFT_HAND_FLAG].x + 10)) ? depthImage.cols : int(HandPoint[LEFT_HAND_FLAG].x + 10)); j++)
			{
				int player = pBufferRun[j] & 7;
				int data = (pBufferRun[j] & 0xfff8) >> 3;

				uchar imageData = 255 - (uchar)(256 * data / 0x0fff);
				q.rgbBlue = q.rgbGreen = q.rgbRed = 0;


				switch (player)

				{
				case 0:
					q.rgbRed = 100;
					q.rgbBlue = 100;
					q.rgbGreen = 100;
					break;
				case 1:
					q.rgbRed = imageData;
					break;
				case 2:
					q.rgbGreen = imageData;
					break;
				case 3:
					q.rgbRed = imageData / 4;
					q.rgbGreen = q.rgbRed * 4;  //这里利用乘的方法，而不用原来的方法可以避免不整除的情况   
					q.rgbBlue = q.rgbRed * 4;  //可以在后面的getTheContour()中配合使用，避免遗漏一些情况   
					break;
				case 4:
					q.rgbBlue = imageData / 4;
					q.rgbRed = q.rgbBlue * 4;
					q.rgbGreen = q.rgbBlue * 4;
					break;
				case 5:
					q.rgbGreen = imageData / 4;
					q.rgbRed = q.rgbGreen * 4;
					q.rgbBlue = q.rgbGreen * 4;
					break;
				case 6:
					q.rgbRed = imageData / 2;
					q.rgbGreen = imageData / 2;
					q.rgbBlue = q.rgbGreen * 2;
					break;
				case 7:
					q.rgbRed = 100;
					q.rgbGreen = 100;
					q.rgbBlue = 100;
				}
				ptr[3 * j] = q.rgbBlue;
				ptr[3 * j + 1] = q.rgbGreen;
				ptr[3 * j + 2] = q.rgbRed;
			}
		}

		for (int i = 0; i<fullDepthImage.rows; i++)         //每次都对fullDepthImage画布进行更新
		{
			uchar *ptr = fullDepthImage.ptr<uchar>(i);

			for (int j = 0; j<fullDepthImage.cols; j++)
			{
				ptr[3 * j] = 100;
				ptr[3 * j + 1] = 100;
				ptr[3 * j + 2] = 100;
			}
		}

		for (int i = 0; i < fullDepthImage.rows; i++)       //显示整个深度图像
		{
			uchar *ptr1 = fullDepthImage.ptr<uchar>(i);
			uchar *pBuffer1 = (uchar*)(LockedRect.pBits) + i*LockedRect.Pitch;
			USHORT *pBufferRun1 = (USHORT *)pBuffer1;

			for (int j = 0; j < fullDepthImage.cols; j++)
			{
				int player = pBufferRun1[j] & 7;
				int data = (pBufferRun1[j] & 0xfff8) >> 3;

				uchar imageData = 255 - (uchar)(256 * data / 0x0fff);
				q.rgbBlue = q.rgbGreen = q.rgbRed = 0;

				switch (player)
				{
				case 0:
					q.rgbRed = 100;
					q.rgbBlue = 100;
					q.rgbGreen = 100;
					break;
				case 1:
					q.rgbRed = imageData;
					break;
				case 2:
					q.rgbGreen = imageData;
					break;
				case 3:
					q.rgbRed = imageData / 4;
					q.rgbGreen = q.rgbRed * 4;  //这里利用乘的方法，而不用原来的方法可以避免不整除的情况   
					q.rgbBlue = q.rgbRed * 4;  //可以在后面的getTheContour()中配合使用，避免遗漏一些情况   
					break;
				case 4:
					q.rgbBlue = imageData / 4;
					q.rgbRed = q.rgbBlue * 4;
					q.rgbGreen = q.rgbBlue * 4;
					break;
				case 5:
					q.rgbGreen = imageData / 4;
					q.rgbRed = q.rgbGreen * 4;
					q.rgbBlue = q.rgbGreen * 4;
					break;
				case 6:
					q.rgbRed = imageData / 2;
					q.rgbGreen = imageData / 2;
					q.rgbBlue = q.rgbGreen * 2;
					break;
				case 7:
					q.rgbRed = 100;
					q.rgbGreen = 100;
					q.rgbBlue = 100;
				}
				ptr1[3 * j] = q.rgbBlue;
				ptr1[3 * j + 1] = q.rgbGreen;
				ptr1[3 * j + 2] = q.rgbRed;
			}
		}


	}
	else
	{
		cout << "捕捉深度图像出现错误" << endl;
	}

	pTexture->UnlockRect(0);
	NuiImageStreamReleaseFrame(depthStreamHandle, depthFrame);

	return handMat;
}

int extrudeRightHandPosition(HANDLE &skeletonEvent, point * HandPoint)  //通过HandPoint指针 修改传入的数组及修改左右手的位置坐标 
{
	NUI_SKELETON_FRAME skeletonFrame = { 0 };
	bool bFoundSkeleton = false;

	if (NuiSkeletonGetNextFrame(0, &skeletonFrame) == S_OK)
	{
		for (int i = 0; i < NUI_SKELETON_COUNT; i++)
		{
			if (skeletonFrame.SkeletonData[i].eTrackingState == NUI_SKELETON_TRACKED)
			{
				bFoundSkeleton = true;
				break;
			}
		}
	}
	else
	{
		cout << "没有找到合适的骨骼帧" << endl;
		//return;
	}

	if (!bFoundSkeleton)
	{
		//return;
	}

	NuiTransformSmooth(&skeletonFrame, NULL);//平滑骨骼帧,消除抖动     

	//point rightHandPoint[NUI_SKELETON_COUNT] = {};        //使用该参数，将深度图中的手部坐标作为原始数据，即只有（x，y）
	//point leftHandPoint[NUI_SKELETON_COUNT] = {}; 
	
	float Right_Hand_fx, Right_Hand_fy;
	float Left_Hand_fx, Left_Hand_fy;
	float Hip_Center_fx, Hip_Center_fy;
	float Shoulder_Center_fx, Shoulder_Center_fy;
	
	int flag = 0;
	for (int i = 0; i < NUI_SKELETON_COUNT; i++)
	{
		if (skeletonFrame.SkeletonData[i].eTrackingState == NUI_SKELETON_TRACKED &&
			skeletonFrame.SkeletonData[i].eSkeletonPositionTrackingState[NUI_SKELETON_POSITION_SHOULDER_CENTER] != NUI_SKELETON_POSITION_NOT_TRACKED)
		{
			float shoulder_x, shoulder_y, shoulder_z, hip_x, hip_y, hip_z;
			shoulder_x = skeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_SHOULDER_CENTER].x;
			shoulder_y = skeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_SHOULDER_CENTER].y;
			shoulder_z = skeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_SHOULDER_CENTER].z;
			
			hip_x = skeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HIP_CENTER].x;
			hip_y = skeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HIP_CENTER].y;
			hip_z = skeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HIP_CENTER].z;

			float Personality = sqrt((shoulder_x - hip_x)*(shoulder_x - hip_x) + (shoulder_y - hip_y)*(shoulder_y - hip_y) + (shoulder_z - hip_z)*(shoulder_z - hip_z));    //求出做手语者肩膀中心到臀部的距离
			//cout << "Personality" << Personality << endl;
			NuiTransformSkeletonToDepthImage(skeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HAND_RIGHT], &Right_Hand_fx, &Right_Hand_fy,NUI_IMAGE_RESOLUTION_320x240);
			NuiTransformSkeletonToDepthImage(skeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HAND_LEFT], &Left_Hand_fx, &Left_Hand_fy, NUI_IMAGE_RESOLUTION_320x240);
			NuiTransformSkeletonToDepthImage(skeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HIP_CENTER], &Hip_Center_fx, &Hip_Center_fy, NUI_IMAGE_RESOLUTION_320x240);
			NuiTransformSkeletonToDepthImage(skeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_SHOULDER_CENTER], &Shoulder_Center_fx, &Shoulder_Center_fy, NUI_IMAGE_RESOLUTION_320x240);
			
			if (Right_Hand_fy <= Hip_Center_fy)    //在这里进行帧的提取   		                                         
			{                                                //当手部位置超过髋关节时，记录手部位置坐标	                                           
				//rightHandPoint[i].x = (Right_Hand_fx - Hip_Center_fx) / (Hip_Center_fy - Shoulder_Center_fy);    //获取相对于身体中心的相对位置   
				//rightHandPoint[i].y = (Hip_Center_fy - Right_Hand_fy) / (Hip_Center_fy - Shoulder_Center_fy);
			
				HandPosition1[RIGHT_HAND_FLAG].x = Right_Hand_fx;
				HandPosition1[RIGHT_HAND_FLAG].y = Right_Hand_fy;

				HandPoint[RIGHT_HAND_FLAG].x = (skeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HAND_RIGHT].x - skeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HIP_CENTER].x) / Personality;
				HandPoint[RIGHT_HAND_FLAG].y = (skeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HAND_RIGHT].y - skeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HIP_CENTER].y) / Personality;
				HandPoint[RIGHT_HAND_FLAG].z = (skeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HAND_RIGHT].z - skeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HIP_CENTER].z) / Personality;

			}
			else                                               //当手部位置低于髋关节时，将手部位置坐标记为（0，0）          假设当手部位置低于髋关节时手的坐标为（0，0）
			{
				//rightHandPoint[i].x = 0;
				//rightHandPoint[i].y = 0;

				HandPosition1[RIGHT_HAND_FLAG].x = 0;
				HandPosition1[RIGHT_HAND_FLAG].y = 0;
				
				HandPoint[RIGHT_HAND_FLAG].x = 0;
				HandPoint[RIGHT_HAND_FLAG].y = 0;
				HandPoint[RIGHT_HAND_FLAG].z = 0;
			}
			if (Left_Hand_fy <= Hip_Center_fy)
			{
				//leftHandPoint[i].x = (Left_Hand_fx - Hip_Center_fx) / (Hip_Center_fy - Shoulder_Center_fy);     //获取相对位置
				//leftHandPoint[i].y = (Hip_Center_fy - Left_Hand_fy) / (Hip_Center_fy - Shoulder_Center_fy);
			
				HandPosition1[LEFT_HAND_FLAG].x = Left_Hand_fx;
				HandPosition1[LEFT_HAND_FLAG].y = Left_Hand_fy;

				HandPoint[LEFT_HAND_FLAG].x = (skeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HAND_LEFT].x - skeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HIP_CENTER].x) / Personality;
				HandPoint[LEFT_HAND_FLAG].y = (skeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HAND_LEFT].y - skeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HIP_CENTER].y) / Personality;
				HandPoint[LEFT_HAND_FLAG].z = (skeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HAND_LEFT].z - skeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HIP_CENTER].z) / Personality;

			}
			else
			{
				//leftHandPoint[i].x = 0;
				//leftHandPoint[i].y = 0;

				HandPosition1[LEFT_HAND_FLAG].x = 0;
				HandPosition1[LEFT_HAND_FLAG].y = 0;

				HandPoint[LEFT_HAND_FLAG].x = 0;
				HandPoint[LEFT_HAND_FLAG].y = 0;
				HandPoint[LEFT_HAND_FLAG].z = 0;
			}
			//cout << "(" << rightHandPoint[i].x << "," << rightHandPoint[i].y << ")   ||";
			//cout << "(" << (int)Hip_Center_fx << "," << (int)Hip_Center_fy << ")" << endl;

			flag = i;                        //此程序只返回跟踪到最后一个人的手的位置 
		}
	}

	//HandPoint[LEFT_HAND_FLAG].x = leftHandPoint[flag].x;
	//HandPoint[LEFT_HAND_FLAG].y = leftHandPoint[flag].y;
	//HandPoint[RIGHT_HAND_FLAG].x = rightHandPoint[flag].x;
	//HandPoint[RIGHT_HAND_FLAG].y = rightHandPoint[flag].y;

	return 1;
}