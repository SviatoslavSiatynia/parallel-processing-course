import java.util.Arrays;
import java.util.Random;
import java.util.concurrent.*;

class Main {

    private static void merge(int[] arr, int left, int mid, int right) {
        int n1 = mid - left + 1;
        int n2 = right - mid;

        int[] L = new int[n1];
        int[] R = new int[n2];

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

    private static void mergeSort(int[] arr, int left, int right) {
        if (left >= right) return;

        int mid = left + (right - left) / 2;

        mergeSort(arr, left, mid);
        mergeSort(arr, mid + 1, right);

        merge(arr, left, mid, right);
    }

    private static void parallelMergeSort(int[] arr, int left, int right, int availableThreads) {
        if (left >= right) return;

        if (availableThreads <= 1) {
            mergeSort(arr, left, right);
            return;
        }

        int mid = left + (right - left) / 2;

        int leftThreads = availableThreads / 2;
        int rightThreads = availableThreads - leftThreads;

        CountDownLatch latch = new CountDownLatch(2);

        ExecutorService executor = Executors.newFixedThreadPool(2);

        executor.submit(() -> {
            parallelMergeSort(arr, left, mid, leftThreads);
            latch.countDown();
        });

        executor.submit(() -> {
            parallelMergeSort(arr, mid + 1, right, rightThreads);
            latch.countDown();
        });

        try {
            latch.await();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }

        executor.shutdown();

        merge(arr, left, mid, right);
    }

    public static void main(String[] args) {
        Random rand = new Random();

        int[] arr = new int[10000000];
        for (int i = 0; i < 10000000; ++i)
            arr[i] = rand.nextInt(100);

        int[] arr2 = Arrays.copyOf(arr, arr.length);

        int availableThreads = Runtime.getRuntime().availableProcessors();
        long start = System.currentTimeMillis();
        parallelMergeSort(arr, 0, arr.length - 1, availableThreads);
        long end = System.currentTimeMillis();
        System.out.println("Time: " + (end - start) + " ms.");
        /*System.out.println("Sorted array (parallel): " + Arrays.toString(arr));*/

        long start1 = System.currentTimeMillis();
        mergeSort(arr2, 0, arr2.length - 1);
        long end1 = System.currentTimeMillis();
        System.out.println("Time: " + (end1 - start1) + " ms.");
        /*System.out.println("Sorted array (sequential): " + Arrays.toString(arr2));*/
    }
}

