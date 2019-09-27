#include<iostream>
#include<stdlib.h>
#include<time.h>
using namespace std;

#define MAX_SIZE 1000000              //����Ҫ����������С
void Merge(int *arr, int low, int mid, int high)
{
	int leftlen = mid - low + 1;                  //arr[low...mid]�ĳ���
	int rightlen = high - mid;                    //arr[mid+1...high]�ĳ���

	int *L = new int[leftlen + 1];                 //ÿ�ι鲢ʱ���ڶ�̬�����ڴ棬��������Ż�
	int *R = new int[rightlen + 1];
	L[leftlen] = INT_MAX;                          //ĩβ�����ڱ�Ԫ��    
	R[rightlen] = INT_MAX;
	//��ֵ��׼���������arr[low.....high]
	int i = 0;
	for (; i < leftlen; i++)
		L[i] = arr[low + i];

	int j = 0;
	for (; j < rightlen; j++)
		R[j] = arr[mid + j + 1];

	//�������arr[low.....high]
	i = 0; j = 0;
	for (int k = low; k <= high; k++)
	{
		if (L[i] <= R[j])//˭��С��˭�ͷ���arr[k]��
			arr[k] = L[i++];
		else
			arr[k] = R[j++];
	}

	delete[] L; L = NULL;
	delete[] R; R = NULL;
}
void MergeSort(int *arr, int low, int high)
{
	if (low < high)
	{
		int mid = (low + high) / 2;
		MergeSort(arr, low, mid);
		MergeSort(arr, mid + 1, high);
		Merge(arr, low, mid, high);
	}
}

int main()
{
	int *a = (int*)malloc(sizeof(int)*MAX_SIZE);       //�����ڴ�Ϊ�������
	for (int i = 0; i < MAX_SIZE; ++i)
	{
		a[i] = rand();
	}
	clock_t t1 = clock();                   //clock_t���ص�ʱ�䵥λ�Ǻ���
	MergeSort(a, 0, MAX_SIZE - 1);
	clock_t t2 = clock();
	//for (int i = 0; i < MAX_SIZE; i++)        //�鿴����������Ԫ��
	//{
	//	cout << a[i];
	//	cout << " ";
	//}
	//cout << endl;
	cout << "�鲢����" << (double)(t2 - t1) / CLOCKS_PER_SEC << endl;        //CLOCKS_PER_SEC�Ǳ�׼c��time.hͷ�����к궨���һ����������ʾһ������CPU���е�ʱ�������������ڽ�clock()�����Ľ��ת��Ϊ����Ϊ��λ����
	return 0;
}