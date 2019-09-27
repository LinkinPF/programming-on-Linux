#include<iostream>
#include<stdlib.h>
#include<time.h>
using namespace std;

#define MAX_SIZE 10000000              //定义要排序的数组大小


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
	int *a = (int*)malloc(sizeof(int)*MAX_SIZE);       //申请内存为数组分配
	for (int i = 0; i < MAX_SIZE; ++i)
	{
		a[i] = rand();
	}
	clock_t t1 = clock();                   //clock_t返回的时间单位是毫秒
	QuickSort(a, 0, MAX_SIZE - 1);
	clock_t t2 = clock();
	//for (int i = 0; i < MAX_SIZE; i++)        //查看排序后的数组元素
	//{
	//	cout << a[i];
	//	cout << " ";
	//}
	//cout << endl;
	cout << "快速排序：" << (double)(t2 - t1) / CLOCKS_PER_SEC << endl;        //CLOCKS_PER_SEC是标准c的time.h头函数中宏定义的一个常数，表示一秒钟内CPU运行的时钟周期数，用于将clock()函数的结果转化为以秒为单位的量
	return 0;
}