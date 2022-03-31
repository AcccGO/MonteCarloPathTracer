#include <chrono>
#include <iostream>
#include <vec3.hpp>

#include "path_tracing.h"

using namespace std;
int main()
{
    auto tracer = PathTracing();

    auto start = std::chrono::high_resolution_clock::now();

    tracer.traceScene("scene.json", 0);

    auto                          finish  = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;
    std::cout << "Tracing took: " << elapsed.count() << " seconds." << std::endl;

    return 0;
}