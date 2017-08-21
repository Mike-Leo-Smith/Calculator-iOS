//
//  Use this file to import your target's public headers that you would like to expose to Swift.
//

#import <Foundation/Foundation.h>

@interface CalculationEngine : NSObject

- (nonnull instancetype)init;
- (void)dealloc;
- (nonnull NSString *)calculate:(nonnull NSString *)expr;

@end
