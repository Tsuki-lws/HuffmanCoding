#include <iostream>

void prefixSum(long long* filesize, int n) {
    for (int i = 1; i < n; ++i) {
        filesize[i] += filesize[i - 1];
    }
}
void prefixSumToArray(long long* input, long long* output, int n) {
    output[0] = input[0];
    for (int i = 1; i < n; ++i) {
        output[i] = output[i - 1] + input[i];
    }
}
int main() {
    int n;
    std::cout << "Enter the number of elements: ";
    std::cin >> n;

    long long* filesize = new long long[n];
    std::cout << "Enter the elements: ";
    for (int i = 0; i < n; ++i) {
        std::cin >> filesize[i];
    }

    prefixSum(filesize, n);

    std::cout << "Prefix sum array: ";
    for (int i = 0; i < n; ++i) {
        std::cout << filesize[i] << " ";
    }
    std::cout << std::endl;

    delete[] filesize;
    return 0;
}