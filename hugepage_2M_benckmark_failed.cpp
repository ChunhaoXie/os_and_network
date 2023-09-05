#include <iostream>
#include <memory>
#include <cstdlib>
#include <sys/mman.h>
#include <chrono>
using namespace std;

/**
* result is 
      4k page time: 403642, checksum is 2449700
      2M page time: 404999, checksum is 2449700
* seems that 2MB huge page does not improve performance in this benchmark
*/

#define assure(exp) do { if (!(exp)) { std::cerr << "not assure: " # exp << std::endl;} } while (0)

inline auto GetMicros(){
    return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
}
int main() {
    size_t PAGE_SIZE = 1024ULL * 1024 * 2;
    size_t ALLOC_SIZE = 8 * 1024ULL * 1024 * 1024;
    auto huge = aligned_alloc(PAGE_SIZE, ALLOC_SIZE);
    assure(huge != nullptr);
    assure(madvise(huge, ALLOC_SIZE, MADV_HUGEPAGE) == 0);

    auto small = malloc(ALLOC_SIZE);
    auto size = ALLOC_SIZE / sizeof(int);

    auto p1 = (int*)small;
    auto p2 = (int*)huge;

    int start = 0x256123;
    for (size_t i=0; i<size; i++) {
        p1[i] = start;
        start *= start;
    }
    start = 0x256123;
    for (size_t i=0; i<size; i++){
        p2[i] = start;
        start *= start;
    }
    
    auto t1 = GetMicros();
    int checksum1 = 0;
    for (size_t i=0; i<size; i++)
        checksum1 += p1[i];
    t1 = GetMicros() - t1;
    cout << "4k page time: " << t1 << ", checksum is " << checksum1 << endl;

    auto t2 = GetMicros();
    int checksum2 = 0;
    for (size_t i=0; i<size; i++)
        checksum2 += p2[i];
    t2 = GetMicros() - t2;
    cout << "2M page time: " << t2 << ", checksum is " << checksum2 << endl;

}
