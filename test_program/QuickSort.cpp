#include<iostream>
#include<stdlib.h>
#include<time.h>
using namespace std;

#define MAX_SIZE 10000000              //����Ҫ����������С


int quickPartion(int *arr, int low, int high)
{
	int pivot = arr[low];
	while (low < high)
	{
		while (low < high && arr[high] >= pivot)
			--high;
		arr[low] = arr[high];
		while (low < high && arr[low] <= pivot)
			++low;
		arr[high] = arr[low];
	}
	arr[low] = pivot;
	return low;
}
void QuickSort(int *arr, int low, int high)
{
	if (low < high)
	{
		int mid = quickPartion(arr, low, high);
		QuickSort(arr, low, mid - 1);
		QuickSort(arr, mid + 1, high);
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
	QuickSort(a, 0, MAX_SIZE - 1);
	clock_t t2 = clock();
	//for (int i = 0; i < MAX_SIZE; i++)        //�鿴����������Ԫ��
	//{
	//	cout << a[i];
	//	cout << " ";
	//}
	//cout << endl;
	cout << "��������" << (double)(t2 - t1) / CLOCKS_PER_SEC << endl;        //CLOCKS_PER_SEC�Ǳ�׼c��time.hͷ�����к궨���һ����������ʾһ������CPU���е�ʱ�������������ڽ�clock()�����Ľ��ת��Ϊ����Ϊ��λ����
	return 0;
}