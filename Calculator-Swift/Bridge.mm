//
//  Calc.c
//  Calculator-Swift
//
//  Created by Mike Smith on 20/08/2017.
//  Copyright © 2017 Mike Smith. All rights reserved.
//

#include "Calculation.hpp"
#include "Bridge.h"

@interface CalculationEngine () {
@private
    Calculation *_obj;
}
@end

@implementation CalculationEngine

- (instancetype)init {
    self = [super init];
    if (self) {
        _obj = new Calculation();
    }
    return self;
}

- (void)dealloc {
    delete _obj;
    _obj = nil;
}

- (nonnull NSString *)calculate:(nonnull NSString *)expr {
    return [NSString stringWithUTF8String:_obj->calculate([expr UTF8String]).c_str()];
}

@end
