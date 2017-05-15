#include "Pca.h"
#include <opencv2/opencv.hpp>
using namespace cv;

vector<vector<float>> Pca(vector<vector<float>> &rightHandDescriptorSeq)
{
	Mat rightHandTrainMat(rightHandDescriptorSeq.size(), rightHandDescriptorSeq[0].size(), CV_32FC1);
	for (int i = 0; i < rightHandDescriptorSeq.size(); i++)
	{
		for (int j = 0; j < rightHandDescriptorSeq[i].size(); j++)
		{
			rightHandTrainMat.at<float>(i, j) = rightHandDescriptorSeq[i][j];
		}
	}

	FileStorage rightHandTrainMatStore("rightTrainMat.xml", FileStorage::WRITE);   //�洢rightHandTrainMat
	rightHandTrainMatStore << "rightHand" << rightHandTrainMat;
	rightHandTrainMatStore.release();


	PCA *pca = new PCA(rightHandTrainMat, Mat(), CV_PCA_DATA_AS_ROW);

	///*int index;
	//float sum = 0, sum0 = 0, ratio;
	//for (int d = 0; d<pca->eigenvalues.rows; ++d)
	//{
	//	sum += pca->eigenvalues.at<float>(d, 0);
	//}
	//for (int d = 0; d<pca->eigenvalues.rows; ++d)
	//{
	//	sum0 += pca->eigenvalues.at<float>(d, 0);
	//	ratio = sum0 / sum;
	//	if (ratio > 0.9){
	//		index = d;
	//		break;
	//	}
	//}

	//cout << "��ά֮ǰ��ά����" << rightHandDescriptorSeq[0].size() << endl;
	//cout << "��ά���ά���ǣ�" << index + 1 << endl;
	cout << pca->eigenvalues << endl;
	Mat eigenvetors_d;
	eigenvetors_d.create(55, rightHandDescriptorSeq[0].size(), CV_32FC1);//eigen values of decreased dimension  
	for (int i = 0; i< 55; ++i)     //�趨����ά�ȵ�40ά
	{
		pca->eigenvectors.row(i).copyTo(eigenvetors_d.row(i));
	}

	FileStorage MaMa_Gesture("MaMa_Gesture.xml", FileStorage::WRITE);
	MaMa_Gesture << "Mean" << pca->mean;
	MaMa_Gesture << "Vector" << eigenvetors_d;

	PCA *pca_encoding = new PCA();                 //����һ���µ�PCA ����������������PCA
	pca_encoding->mean = pca->mean.clone();
	pca_encoding->eigenvectors = eigenvetors_d.clone();

	//PCA *pca_encoding = new PCA();               //����һ���µ�PCA ����������������PCA
	//FileStorage MaMa_Gesture("MaMa_Gesture.xml", FileStorage::READ);
	//MaMa_Gesture["Mean"] >> pca_encoding->mean;
	//MaMa_Gesture["Vector"] >> pca_encoding->eigenvectors;
	//MaMa_Gesture.release();

	Mat rightHandPcaMat(rightHandDescriptorSeq.size(), pca_encoding->eigenvectors.rows, CV_32FC1);
	pca_encoding->project(rightHandTrainMat, rightHandPcaMat);
	
	FileStorage PcaSeq("PcaSeq.xml", FileStorage::WRITE);    //�����ã�������ʾrightHandPcaMat
	PcaSeq << "rightHand" << rightHandPcaMat;
	PcaSeq.release();
	
	Mat rightHandDecodeMat(rightHandDescriptorSeq.size(), rightHandDescriptorSeq[0].size(), CV_32FC1);
	pca_encoding->backProject(rightHandPcaMat, rightHandDecodeMat);      //���Իָ�����

	FileStorage DecodeMat("DecodeMat.xml", FileStorage::WRITE);
	DecodeMat << "rightHand" << rightHandDecodeMat;
	DecodeMat.release();

	vector<vector<float>> rightHandDescriptorPcaSeq;
	for (int i = 0; i < rightHandPcaMat.rows; i++)
	{
		vector<float> rightHandDescriptorPca;
		for (int j = 0; j < rightHandPcaMat.cols; j++)
		{
			rightHandDescriptorPca.push_back(rightHandPcaMat.at<float>(i, j));
		}
		rightHandDescriptorPcaSeq.push_back(rightHandDescriptorPca);
	}

	return rightHandDescriptorPcaSeq;
}