//Headers
#include "mergesort.hpp"

/*  Funcion merge
*/ 
void merge(vector<int>& arr, int left, int mid, int right) {
    int i, j, k;
    int n1 = mid - left + 1;
    int n2 = right - mid;

    vector<int> left_arr(n1), right_arr(n2);

    for (i = 0; i < n1; i++)
        left_arr[i] = arr[left + i];
    for (j = 0; j < n2; j++)
        right_arr[j] = arr[mid + 1 + j];

    i = 0;
    j = 0;
    k = left;
    while (i < n1 && j < n2) {
        if (left_arr[i] <= right_arr[j]) {
            arr[k] = left_arr[i];
            i++;
        } else {
            arr[k] = right_arr[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        arr[k] = left_arr[i];
        i++;
        k++;
    }

    while (j < n2) {
        arr[k] = right_arr[j];
        j++;
        k++;
    }
}

/*  Funcion merge_sort
*/
void merge_sort_recursive(vector<int>& arr, int left, int right, int sub_length) {
    int n = arr.size();
    if ( n <= sub_length) return;

    if (left < right) {
      
        // Calculate the midpoint
        int mid = left + (right - left) / 2;

        // Sort first and second halves
        merge_sort_recursive(arr, left, mid, sub_length);
        merge_sort_recursive(arr, mid + 1, right, sub_length);

        // Merge the sorted halves
        merge(arr, left, mid, right);
    }


}

void merge_sort(vector<int>& arr, int a){
    int n = arr.size();
    int m = n/a;
    merge_sort_recursive(arr, 0, n, m);
}
