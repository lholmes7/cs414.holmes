#include <iostream>
#include <vector>
#include <functional>
#include <chrono>
#include <algorithm>


//Strategy Base
class SortStrategy {
public:
    virtual ~SortStrategy() = default;
    virtual void sort(std::vector<int>& data) = 0;
};

//quicksort
class QuickSort : public SortStrategy {
private:
    void quicksort(std::vector<int>& arr, int low, int high) {
        if (low >= high) return;

        int pivot = arr[(low + high) / 2];
        int i = low, j = high;

        while (i <= j) {
            while (arr[i] < pivot) i++;
            while (arr[j] > pivot) j--;
            if (i <= j) std::swap(arr[i++], arr[j--]);
        }

        if (low < j) quicksort(arr, low, j);
        if (i < high) quicksort(arr, i, high);
    }

public:
    void sort(std::vector<int>& data) override {
        quicksort(data, 0, (int)data.size() - 1);
    }
};

//Mergesort
class MergeSort : public SortStrategy {
private:
    void merge(std::vector<int>& arr, int l, int m, int r) {
        std::vector<int> L(arr.begin() + l, arr.begin() + m + 1);
        std::vector<int> R(arr.begin() + m + 1, arr.begin() + r + 1);

        int i = 0, j = 0, k = l;
        while (i < L.size() && j < R.size()) {
            if (L[i] <= R[j]) arr[k++] = L[i++];
            else arr[k++] = R[j++];
        }
        while (i < L.size()) arr[k++] = L[i++];
        while (j < R.size()) arr[k++] = R[j++];
    }

    void mergesort(std::vector<int>& arr, int l, int r) {
        if (l >= r) return;
        int m = (l + r) / 2;
        mergesort(arr, l, m);
        mergesort(arr, m + 1, r);
        merge(arr, l, m, r);
    }

public:
    void sort(std::vector<int>& data) override {
        mergesort(data, 0, (int)data.size() - 1);
    }
};

//Bubblesort
class BubbleSort : public SortStrategy {
public:
    void sort(std::vector<int>& data) override {
        bool swapped = true;
        for (size_t i = 0; i < data.size() && swapped; i++) {
            swapped = false;
            for (size_t j = 0; j < data.size() - i - 1; j++) {
                if (data[j] > data[j + 1]) {
                    std::swap(data[j], data[j + 1]);
                    swapped = true;
                }
            }
        }
    }
};

// Sort context
class SortContext {
private:
    SortStrategy* strategy = nullptr;

public:
    void set_strategy(SortStrategy* s) {
        strategy = s;
    }

    void execute_strategy(std::vector<int>& data) {
        if (!strategy) {
            std::cout << "No strategy set!\n";
            return;
        }

        auto start = std::chrono::high_resolution_clock::now();
        strategy->sort(data);
        auto end = std::chrono::high_resolution_clock::now();

        std::chrono::duration<double, std::milli> ms = end - start;

        std::cout << "Sorted in " << ms.count() << " ms\n";
    }
};

// print helper
void print_vec(const std::vector<int>& v) {
    for (int x : v) std::cout << x << " ";
    std::cout << "\n";
}

// main
int main() {
    SortContext context;

    QuickSort quick;
    MergeSort merge;
    BubbleSort bubble;

    std::vector<int> data = { 5, 2, 9, 1, 5, 6 };

    std::cout << "Original: ";
    print_vec(data);

    // Use sorts
    context.set_strategy(&quick);
    std::cout << "\nQuickSort:\n";
    std::vector<int> d1 = data;
    context.execute_strategy(d1);
    print_vec(d1);


    context.set_strategy(&merge);
    std::cout << "\nMergeSort:\n";
    std::vector<int> d2 = data;
    context.execute_strategy(d2);
    print_vec(d2);

    
    context.set_strategy(&bubble);
    std::cout << "\nBubbleSort:\n";
    std::vector<int> d3 = data;
    context.execute_strategy(d3);
    print_vec(d3);


//Lambda
    std::cout << "\nLambda Sort (std::function):\n";

    std::function<void(std::vector<int>&)> lambda_sort = [](std::vector<int>& v) {
        std::sort(v.begin(), v.end());
        };

    class LambdaStrategy : public SortStrategy {
    public:
        std::function<void(std::vector<int>&)> f;
        LambdaStrategy(std::function<void(std::vector<int>&)> f) : f(f) {}
        void sort(std::vector<int>& data) override { f(data); }
    };

    LambdaStrategy lambda(lambda_sort);

    context.set_strategy(&lambda);
    std::vector<int> d4 = data;
    context.execute_strategy(d4);
    print_vec(d4);

    return 0;
}
