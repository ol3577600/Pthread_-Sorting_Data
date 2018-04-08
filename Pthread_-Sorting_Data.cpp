#include <string>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <algorithm>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>
#include<cmath>
#include<Windows.h> 
using namespace std;

struct Merge_argument{
	int *array , *workArray  ;
	int length, leftStart, leftCount, rightStart, rightCount ;
};

void* Bubble_Sort_Thread(void *ptr2);
int* Bubble_Sort (int* array, int length) ;
int* Bubble_Sort_2 (int* array, int leftStart, int leftCount) ;
int* Merge(int* array, int* workArray, int length, int leftStart, int leftCount, int rightStart, int rightCount);
void* runMerge(void *ptr) ;
int main() {
	cout << "Welcome To OS Project One !" << endl ;
	int c = -1, total = 0 ;
	string input = "",output = "" ;
	cout << "Please input file name (do not add .txt):";
	cin >> input ;
	output = input+"_output.txt" ;
	input += ".txt" ;
	fstream fin(input.c_str()), fin_num(input.c_str()), fin_out(output.c_str(), ios::out) ;
	if(!fin){    cout << "File not found." << endl;   }
	else {
	  /*讀檔和計算輸入總個數*/
	  int junk = -1, piece = -1, count_in = -1, count_in_save = -1 ;	
	  fin_num >> c ;	
	  fin >> c ;
	  while(fin_num >> junk) total++ ;
	  fin_num.close();
	  int *array = new int[total], *out = new int[total], *test = new int[total], *use_test = new int[9];
	  //end 
	  /*計算CPU時間宣告*/
	  float costTime = 0 ;
	  clock_t start, end ;
	  //end
	  if ( c == 2 || c == 4 ) {
	    cout << "請問要切成幾份?";
		cin >> piece ;
		count_in = ceil((float)total / (float)piece);
	  }
	  pthread_t thread_merge[piece-1];
	  pthread_t thread_bubble[piece];
	  struct Merge_argument merge_one ;
	  int total_thread = 0, total_thread_bubble = 0 ;
	  switch ( c ) {
	  	 
         case 1:/*直接作Bubble Sort*/
         	for (int i = 0 ; i < total; i++) fin >> array[i] ;
         	start = clock();
         	out = Bubble_Sort(array, total) ;
         	end = clock();
         	for (int i = 0 ; i < total; i++) fin_out << out[i] << " " ;
         	costTime = (float)(end - start)/CLK_TCK;
         	fin_out << endl << "CPU Time:" << costTime << endl ;
         	cout << endl << "CPU Time:" << costTime << endl ;
            break;
         case 2:/*將N個數目字切成k份,並由K個threads分別進行Bubble_Sort之後,再由k-1個threads作Merge Sort*/
            start = clock();
            merge_one.array = new int[total] ;            
            for (int i = 0 ; i < total ; i++) fin >> merge_one.array[i] ;
			merge_one.workArray = new int[total] ;
			merge_one.length = total ;
			for (int lStart = 0; lStart < total; lStart += count_in) {
              if (count_in > total - lStart) merge_one.leftCount = total - lStart ;
              else merge_one.leftCount = count_in;
              merge_one.leftStart = lStart ;
			  pthread_create(&thread_bubble[total_thread_bubble], NULL , Bubble_Sort_Thread ,(void *)& merge_one ) ;
			  pthread_join(thread_bubble[total_thread_bubble],NULL);
			  total_thread_bubble++ ;
            }
            
			for (; count_in < total; count_in *= 2)
		      for (int leftStart = 0; leftStart < total; leftStart += 2 * count_in) {
                 if (count_in > total - leftStart) break;
                 merge_one.leftStart = leftStart, merge_one.leftCount = count_in, merge_one.rightStart =leftStart + count_in,
			     merge_one.rightCount = min(count_in, total - leftStart - count_in) ;
			     pthread_create(&thread_merge[total_thread], NULL , runMerge ,(void *)& merge_one ) ;
			     pthread_join(thread_merge[total_thread],NULL);
			     total_thread++ ;
              }
            end = clock();
            for (int i = 0 ; i < total; i++) fin_out << merge_one.array[i] << " " ;
            costTime = (float)(end - start)/CLK_TCK;
            fin_out << endl << "CPU Time:" << costTime << endl ;
         	cout << endl << "CPU Time:" << costTime << endl ;
            //for (int i = 0; i < total ; i++) cout << merge_one.array[i] << " " ; 
            break;
         case 3:/*將N個數目字切成k份,並由K個Processes分別進行Bubble_Sort之後,再由k-1個Processes作Merge Sort*/
            break;
         case 4:/*將N個數目字切成k份,在一個Process內對K份資料進行Bubble_Sort之後,再用同一個Process作Merge Sort*/
            for (int i = 0 ; i < total; i++) fin >> array[i] ;
            start = clock();
            for (int lStart = 0; lStart < total; lStart += count_in) {
              if (count_in > total - lStart) count_in_save = total - lStart ;
              else count_in_save = count_in;
			  Bubble_Sort_2 (array, lStart, count_in_save ) ;
            }
            
            for (; count_in < total; count_in *= 2)
              for (int leftStart = 0; leftStart < total; leftStart += 2 * count_in) {
                if (count_in > total - leftStart) break;
                Merge(array, test, total, leftStart, count_in, leftStart + count_in, min(count_in, total - leftStart - count_in));
              }
            end = clock();  
            for (int i = 0 ; i < total; i++) fin_out << array[i] << " " ;
			costTime = (float)(end - start)/CLK_TCK;
			fin_out << endl << "CPU Time:" << costTime << endl ;
			cout << endl << "CPU Time:" << costTime << endl ; 
            break;
         default:
            cout << "command error! pleease try again.";
      }
      cout << "Total Data Number:" << total << endl ;
    }
    
    fin_out.close() ;
    fin.close();
	return 0 ;
}

int* Bubble_Sort (int* array, int length) {
	for (int i = length - 1; i > 0; --i)
        for (int j = 0; j < i; ++j)
            if (array[j] > array[j + 1])
                swap(array[j], array[j + 1]);
    return array ;
}

int* Bubble_Sort_2 (int* array, int leftStart, int leftCount ) {
  for (int i = leftStart + leftCount - 1; i > leftStart; --i)
    for (int j = leftStart; j < i; ++j)
      if (array[j] > array[j + 1])
        swap(array[j], array[j + 1]);
  return array ; 
}
 
int* Merge(int* array, int* workArray, int length, int leftStart, int leftCount, int rightStart, int rightCount) {
    int i = leftStart, j = rightStart, leftBound = leftStart + leftCount, rightBound = rightStart + rightCount, index = leftStart;
    while (i < leftBound || j < rightBound)
    {
        if (i < leftBound && j < rightBound)
        {
            if (array[j] < array[i])
                workArray[index] = array[j++];
            else
                workArray[index] = array[i++];
        }
        else if (i < leftBound)
            workArray[index] = array[i++];
        else
            workArray[index] = array[j++];
        ++index;
    }
    for (i = leftStart; i < index; ++i) array[i] = workArray[i];
    return array ;
}

void* Bubble_Sort_Thread(void *ptr2) {
	struct Merge_argument* ptr = (struct Merge_argument* ) ptr2 ;
	int length= ptr->length ;
	int* array = new int[ptr->length] ;
	for (int i = 0 ; i < ptr->length ; i++) array[i] = ptr->array[i];
	
	for (int i = ptr->leftStart + ptr->leftCount - 1; i > ptr->leftStart; --i)
        for (int j = ptr->leftStart; j < i; ++j)
            if (array[j] > array[j + 1])
                swap(array[j], array[j + 1]);
    for (int i = 0 ; i < length; i++) ptr->array[i] = array[i] ;
}
void* runMerge(void *ptr2) {
	struct Merge_argument* ptr = (struct Merge_argument *) ptr2 ;
	int* array = new int[ptr->length], * workArray =  new int[ptr->length];
	for (int i = 0 ; i < ptr->length ; i++) array[i] = ptr->array[i];
	int length= ptr->length, leftStart= ptr->leftStart, leftCount= ptr->leftCount, rightStart= ptr->rightStart, rightCount= ptr->rightCount ;
	int i = leftStart, j = rightStart, leftBound = leftStart + leftCount, rightBound = rightStart + rightCount, index = leftStart;
    while (i < leftBound || j < rightBound)
    {
        if (i < leftBound && j < rightBound)
        {
            if (array[j] < array[i])
                workArray[index] = array[j++];
            else
                workArray[index] = array[i++];
        }
        else if (i < leftBound)
            workArray[index] = array[i++];
        else
            workArray[index] = array[j++];
        ++index;
    }
    for (i = leftStart; i < index; ++i) ptr->array[i] = workArray[i];       
}

