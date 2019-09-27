#include<iostream>
#include<stdlib.h>
#include<time.h>
using namespace std;

#define MAX_SIZE 1000000              //定义要排序的数组大小

void Swap(int &a, int &b)
{
	int temp = a;
	a = b;
	b = temp;
}
void Heap_build(int *arr, int root, int length)
{
	int lchild = root * 2 + 1;//根节点的左子结点下标
	if (lchild < length)//左子结点下标不能超出数组的长度
	{
		int flag = lchild;//flag保存左右节点中最大值的下标
		int rchild = lchild + 1;//根节点的右子结点下标
		if (rchild < length)//右子结点下标不能超出数组的长度(如果有的话)
		{
			if (arr[rchild] > arr[flag])//找出左右子结点中的最大值
			{
				flag = rchild;
			}
		}
		if (arr[root] < arr[flag])
		{
			//交换父结点和比父结点大的最大子节点
			Swap(arr[root], arr[flag]);
			//从此次最大子节点的那个位置开始递归建堆
			Heap_build(arr, flag, length);
		}
	}
}
void Heap_sort(int *arr, int len)
{
	for (int i = len / 2; i >= 0; --i)//从最后一个非叶子节点的父结点开始建堆
	{
		Heap_build(arr, i, len);
	}

	for (int j = len - 1; j > 0; --j)//j表示数组此时的长度，因为len长度已经建过了，从len-1开始
	{
		Swap(arr[0], arr[j]);//交换首尾元素,将最大值交换到数组的最后位置保存
		Heap_build(arr, 0, j);//去除最后位置的元素重新建堆，此处j表示数组的长度，最后一个位置下标变为len-2
	}
}

int main()
{
	int *a = (int*)malloc(sizeof(int)*MAX_SIZE);       //申请内存为数组分配
	for (int i = 0; i < MAX_SIZE; ++i)
	{
		a[i] = rand();
	}
	clock_t t1 = clock();                   //clock_t返回的时间单位是毫秒
	Heap_sort(a, MAX_SIZE);
	clock_t t2 = clock();
	//for (int i = 0; i < MAX_SIZE; i++)        //查看排序后的数组元素
	//{
	//	cout << a[i];
	//	cout << " ";
	//}
	//cout << endl;
	cout << "堆排序：" << (double)(t2 - t1) / CLOCKS_PER_SEC << endl;        //CLOCKS_PER_SEC是标准c的time.h头函数中宏定义的一个常数，表示一秒钟内CPU运行的时钟周期数，用于将clock()函数的结果转化为以秒为单位的量
	return 0;
}