//
//  Calculation.hpp
//  Calculator-Swift
//
//  Created by Mike Smith on 20/08/2017.
//  Copyright © 2017 Mike Smith. All rights reserved.
//

#ifndef Calculation_h
#define Calculation_h

#include <string>

class Calculation {
private:
    class VariableTable *_var_table;
    std::string _result;

public:
    Calculation();
    ~Calculation();
    const std::string &calculate(const std::string &expr);
};

#endif /* Calculation_h */
