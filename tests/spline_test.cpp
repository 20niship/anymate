#include <iostream>
#include <movutl/core/spline.hpp>

using namespace mu::core;
int main(){
    std::vector<double> X = {5, 60, 100, 90};
    std::vector<double> Y = {60, 30, 80, 100};
    mu::core::Spline gen;
    gen.init(X, Y, 10, true, false);
    gen.generate();
    auto rx = gen.getOutput();
    return 0;
}
