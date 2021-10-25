#include <iostream>
#include <random>
#include <chrono>
#include <future>
#include <vector>
#include <algorithm>

template <class T>
void quicksortParallel(T* array, int n, T* lesser, T* greater, uint32_t *temp1, uint32_t *temp2);
template <class T>
void quicksortSequential(T* array, int n, T* lesser, T* greater);

const uint32_t ARRAY_SIZE = 1e8;
const uint32_t THRESHOLD=100000;


int main() {
    auto array1 = new double_t[ARRAY_SIZE];
    auto array2 = new double_t[ARRAY_SIZE];
    auto lesser = new double_t[ARRAY_SIZE];
    auto greater = new double_t[ARRAY_SIZE];
    auto temp1 = new uint32_t[ARRAY_SIZE];
    auto temp2 = new uint32_t[ARRAY_SIZE];

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 100.0);
    for (uint32_t i = 0; i < ARRAY_SIZE; ++i) {
        array2[i] = array1[i] = dis(gen);
    }

    std::cout << "Parallel version:\n";

    auto startTime = std::chrono::system_clock::now();
    quicksortParallel(array1, ARRAY_SIZE, lesser, greater, temp1, temp2);
    auto finishTime = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(finishTime - startTime).count();

    std::cout << "Sorting took " << duration << " milliseconds\n";
    for (uint32_t i = 1; i < ARRAY_SIZE; ++i) {
        if (array1[i] < array1[i - 1]) { std::cout << " Error! "; break; }
    }

    std::cout << "Sequential version:\n";

    startTime = std::chrono::system_clock::now();
    quicksortSequential(array2, ARRAY_SIZE, lesser, greater);
    finishTime = std::chrono::system_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(finishTime - startTime).count();

    std::cout << "Sorting took " << duration << " milliseconds\n";
    for (uint32_t i = 1; i < ARRAY_SIZE; ++i) {
        if (array2[i] < array2[i - 1]) std::cout << "Error!";
    }

    delete[] array1;
    delete[] array2;
    delete[] lesser;
    delete[] greater;
    delete[] temp1;
    delete[] temp2;
    return 0;
}


template <class T>
void quicksubsortParallel(T* array, int p, int r, T* lesserGlobal, T* greaterGlobal, uint32_t *temp1, uint32_t *temp2)
{
    if (p + 1 < r)
    {
        T q = array[0];
        auto lesser = lesserGlobal + p;
        auto greater = greaterGlobal + p;
        uint32_t lesserSize, greaterSize;

        if (r - p > THRESHOLD) {
            lesserSize = 0;
            for (uint32_t i = 0; i < r - p; ++i) {
                if (array[i] < q) {lesser[lesserSize] = array[i]; ++lesserSize;}
            }
            greaterSize = 0;
            for (uint32_t i = 1; i < r - p; ++i) {
                if (array[i] >= q) {greater[greaterSize] = array[i]; ++greaterSize;}
            }

            for (uint32_t i = 0; i < lesserSize; ++i) {
                array[i] = lesser[i];
            }
            array[lesserSize] = q;
            for (uint32_t i = lesserSize + 1; i < r - p; ++i) {
                array[i] = greater[i - lesserSize - 1];
            }

            auto fut1 = std::async([&]() {
                quicksubsortParallel(array, p, p + lesserSize, lesserGlobal, greaterGlobal, temp1, temp2);
            });
            quicksubsortParallel(array + lesserSize + 1, p + lesserSize + 1, r, lesserGlobal, greaterGlobal, temp1, temp2);
            fut1.wait();
        }
        else {
            lesserSize = 0;
            for (uint32_t i = 0; i < r - p; ++i) {
                if (array[i] < q) {lesser[lesserSize] = array[i]; ++lesserSize;}
            }
            greaterSize = 0;
            for (uint32_t i = 1; i < r - p; ++i) {
                if (array[i] >= q) {greater[greaterSize] = array[i]; ++greaterSize;}
            }

            for (uint32_t i = 0; i < lesserSize; ++i) {
                array[i] = lesser[i];
            }
            array[lesserSize] = q;
            for (uint32_t i = lesserSize + 1; i < r - p; ++i) {
                array[i] = greater[i - lesserSize - 1];
            }

            quicksubsortParallel(array, p, p + lesserSize, lesserGlobal, greaterGlobal, temp1, temp2);
            quicksubsortParallel(array + lesserSize + 1, p + lesserSize + 1, r, lesserGlobal, greaterGlobal, temp1, temp2);
        }
    }
}


template <class T>
void quicksortParallel(T* array, int n, T* lesser, T* greater, uint32_t *temp1, uint32_t *temp2)
{
    quicksubsortParallel(array, 0, n, lesser, greater, temp1, temp2);
}


template <class T>
void quicksubsortSequential(T* array, int p, int r, T* lesserGlobal, T* greaterGlobal)
{
    if (p + 1 < r)
    {
        T q = array[0];
        auto lesser = lesserGlobal + p;
        auto greater = greaterGlobal + p;
        uint32_t lesserSize, greaterSize;

        lesserSize = 0;
        for (uint32_t i = 0; i < r - p; ++i) {
            if (array[i] < q) {lesser[lesserSize] = array[i]; ++lesserSize;}
        }
        greaterSize = 0;
        for (uint32_t i = 1; i < r - p; ++i) {
            if (array[i] >= q) {greater[greaterSize] = array[i]; ++greaterSize;}
        }

        for (uint32_t i = 0; i < lesserSize; ++i) {
            array[i] = lesser[i];
        }
        array[lesserSize] = q;
        for (uint32_t i = lesserSize + 1; i < r - p; ++i) {
            array[i] = greater[i - lesserSize - 1];
        }

        quicksubsortSequential(array, p, p + lesserSize, lesserGlobal, greaterGlobal);
        quicksubsortSequential(array + lesserSize + 1, p + lesserSize + 1, r, lesserGlobal, greaterGlobal);
    }
}


template <class T>
void quicksortSequential(T* array, int n, T* lesser, T* greater)
{
    quicksubsortSequential(array, 0, n, lesser, greater);
}