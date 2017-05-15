#include "Hog.h"

vector<vector<float>> Hog(vector<Mat> &handMatSeq)
{
	vector<vector<float>> handDescriptorSeq;
	handDescriptorSeq.clear();
	HOGDescriptor *hog = new HOGDescriptor(cvSize(40, 40), cvSize(20, 20), cvSize(10, 10), cvSize(10, 10), 9);
	for (int i = 0; i < handMatSeq.size(); i++)       //��handMat��ȡhog������
	{
		vector<float> descriptor;
		hog->compute(handMatSeq[i], descriptor, Size(64, 48), Size(0, 0));    
		handDescriptorSeq.push_back(descriptor);        //�������Ӽ�������������
	}
	return handDescriptorSeq;
}