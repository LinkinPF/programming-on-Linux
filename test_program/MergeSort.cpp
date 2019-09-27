#include<iostream>
#include<stdlib.h>
#include<time.h>
using namespace std;

#define MAX_SIZE 1000000              //定义要排序的数组大小
void Merge(int *arr, int low, int mid, int high)
{
	int leftlen = mid - low + 1;                  //arr[low...mid]的长度
	int rightlen = high - mid;                    //arr[mid+1...high]的长度

	int *L = new int[leftlen + 1];                 //每次归并时都在动态申请内存，这里可以优化
	int *R = new int[rightlen + 1];
	L[leftlen] = INT_MAX;                          //末尾均是哨兵元素    
	R[rightlen] = INT_MAX;
	//赋值，准备有序放入arr[low.....high]
	int i = 0;
	for (; i < leftlen; i++)
		L[i] = arr[low + i];

	int j = 0;
	for (; j < rightlen; j++)
		R[j] = arr[mid + j + 1];

	//有序放入arr[low.....high]
	i = 0; j = 0;
	for (int k = low; k <= high; k++)
	{
		if (L[i] <= R[j])//谁更小，谁就放入arr[k]中
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
	int *a = (int*)malloc(sizeof(int)*MAX_SIZE);       //申请内存为数组分配
	for (int i = 0; i < MAX_SIZE; ++i)
	{
		a[i] = rand();
	}
	clock_t t1 = clock();                   //clock_t返回的时间单位是毫秒
	MergeSort(a, 0, MAX_SIZE - 1);
	clock_t t2 = clock();
	//for (int i = 0; i < MAX_SIZE; i++)        //查看排序后的数组元素
	//{
	//	cout << a[i];
	//	cout << " ";
	//}
	//cout << endl;
	cout << "归并排序：" << (double)(t2 - t1) / CLOCKS_PER_SEC << endl;        //CLOCKS_PER_SEC是标准c的time.h头函数中宏定义的一个常数，表示一秒钟内CPU运行的时钟周期数，用于将clock()函数的结果转化为以秒为单位的量
	return 0;
}