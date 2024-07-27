#include <iostream>
#include <vector>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <algorithm>

using namespace std;

void merge(vector<int>& arr, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    vector<int> L(n1), R(n2);

    for (int i = 0; i < n1; ++i)
        L[i] = arr[left + i];
    for (int j = 0; j < n2; ++j)
        R[j] = arr[mid + 1 + j];

    int i = 0, j = 0, k = left;

    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            arr[k] = L[i];
            ++i;
        } else {
            arr[k] = R[j];
            ++j;
        }
        ++k;
    }

    while (i < n1) {
        arr[k] = L[i];
        ++i;
        ++k;
    }

    while (j < n2) {
        arr[k] = R[j];
        ++j;
        ++k;
    }
}

void mergeSort(vector<int>& arr, int left, int right) {
    if (left >= right) return;

    int mid = left + (right - left) / 2;

    mergeSort(arr, left, mid);
    mergeSort(arr, mid + 1, right);

    merge(arr, left, mid, right);
}

void parallelMergeSort(vector<int>& arr, int left, int right, int availableThreads) {
    if (left >= right) return;

    if (availableThreads <= 1) {
        mergeSort(arr, left, right);
        return;
    }

    int mid = left + (right - left) / 2;

    int leftThreads = availableThreads / 2;
    int rightThreads = availableThreads - leftThreads;
    /*string out = to_string(leftThreads) + " " + to_string(rightThreads) + "\n";
    cout << out;*/

    mutex mtx;
    condition_variable cv;
    bool leftDone = false, rightDone = false;

    thread leftThread([&](){
        parallelMergeSort(arr, left, mid, leftThreads);
        {
            unique_lock<mutex> lock(mtx);
            leftDone = true;
        }
        cv.notify_one();
    });

    thread rightThread([&](){
        parallelMergeSort(arr, mid + 1, right, rightThreads);
        {
            unique_lock<mutex> lock(mtx);
            rightDone = true;
        }
        cv.notify_one();
    });

    {
        unique_lock<mutex> lock(mtx);
        cv.wait(lock, [&](){ return leftDone && rightDone; });
    }

    merge(arr, left, mid, right);

    leftThread.join();
    rightThread.join();
}

int main() {

    srand(time(nullptr));

    vector<int> arr(1000000);
    for (int i = 0; i < 1000000; ++i)
        arr[i] = rand() % 100;

    /*cout << "Array: ";
    for (int i = 0; i < 100; ++i)
        cout << arr[i] << " ";
    cout << endl;*/

    int availableThreads = thread::hardware_concurrency();
    auto start = chrono::steady_clock::now();
    parallelMergeSort(arr, 0, arr.size() - 1, availableThreads);

    auto end = chrono::steady_clock::now();

    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start).count();

    cout << "Time: " << duration << " ms." << endl;

    /*cout << "Sorted array: ";
    for (int i = 0; i < 100; ++i)
        cout << arr[i] << " ";
    cout << endl;*/

    srand(time(nullptr));

    vector<int> arr2(1000000);
    for (int i = 0; i < 1000000; ++i)
        arr2[i] = rand() % 100;

    auto start2 = chrono::steady_clock::now();

    mergeSort(arr2, 0, arr2.size() - 1);

    auto end2 = chrono::steady_clock::now();
    auto duration2 = chrono::duration_cast<chrono::milliseconds>(end2 - start2).count();

    cout << "Time: " << duration2 << " ms." << endl;

    return 0;
}
