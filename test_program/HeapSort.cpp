#include<iostream>
#include<stdlib.h>
#include<time.h>
using namespace std;

#define MAX_SIZE 1000000              //����Ҫ����������С

void Swap(int &a, int &b)
{
	int temp = a;
	a = b;
	b = temp;
}
void Heap_build(int *arr, int root, int length)
{
	int lchild = root * 2 + 1;//���ڵ�����ӽ���±�
	if (lchild < length)//���ӽ���±겻�ܳ�������ĳ���
	{
		int flag = lchild;//flag�������ҽڵ������ֵ���±�
		int rchild = lchild + 1;//���ڵ�����ӽ���±�
		if (rchild < length)//���ӽ���±겻�ܳ�������ĳ���(����еĻ�)
		{
			if (arr[rchild] > arr[flag])//�ҳ������ӽ���е����ֵ
			{
				flag = rchild;
			}
		}
		if (arr[root] < arr[flag])
		{
			//���������ͱȸ����������ӽڵ�
			Swap(arr[root], arr[flag]);
			//�Ӵ˴�����ӽڵ���Ǹ�λ�ÿ�ʼ�ݹ齨��
			Heap_build(arr, flag, length);
		}
	}
}
void Heap_sort(int *arr, int len)
{
	for (int i = len / 2; i >= 0; --i)//�����һ����Ҷ�ӽڵ�ĸ���㿪ʼ����
	{
		Heap_build(arr, i, len);
	}

	for (int j = len - 1; j > 0; --j)//j��ʾ�����ʱ�ĳ��ȣ���Ϊlen�����Ѿ������ˣ���len-1��ʼ
	{
		Swap(arr[0], arr[j]);//������βԪ��,�����ֵ��������������λ�ñ���
		Heap_build(arr, 0, j);//ȥ�����λ�õ�Ԫ�����½��ѣ��˴�j��ʾ����ĳ��ȣ����һ��λ���±��Ϊlen-2
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
	Heap_sort(a, MAX_SIZE);
	clock_t t2 = clock();
	//for (int i = 0; i < MAX_SIZE; i++)        //�鿴����������Ԫ��
	//{
	//	cout << a[i];
	//	cout << " ";
	//}
	//cout << endl;
	cout << "������" << (double)(t2 - t1) / CLOCKS_PER_SEC << endl;        //CLOCKS_PER_SEC�Ǳ�׼c��time.hͷ�����к궨���һ����������ʾһ������CPU���е�ʱ�������������ڽ�clock()�����Ľ��ת��Ϊ����Ϊ��λ����
	return 0;
}