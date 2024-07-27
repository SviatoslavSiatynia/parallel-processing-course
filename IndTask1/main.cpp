#include <iostream>
#include <vector>
#include <thread>

using namespace std;

double function(double x) {
    return 4.0 / (1.0 + x * x);
}

double integrate(int start, int end, double step) {
    double local_result = 0.0;
    for (int i = start; i < end; ++i) {
        double x = (i + 0.5) * step;
        local_result += function(x);
    }
    return local_result * step;
}

int main() {

    int num_threads = thread::hardware_concurrency();
    int n = 1000;

    double local_results[num_threads];
    double step = 1.0 / n;

    vector<thread> threads;

    for (int i = 0; i < num_threads; ++i) {
        int start = i * n / num_threads;
        int end = (i + 1) * n / num_threads;
        threads.emplace_back([&local_results, start, end, step, i](){
            local_results[i] = integrate(start, end, step);
        });
    }

    for (auto &thread: threads) {
        thread.join();
    }

    double result = 0.0;
    for (int i = 0; i < num_threads; ++i) {
        result += local_results[i];
    }

    cout << "Result: " << result << endl;

    return 0;
}
