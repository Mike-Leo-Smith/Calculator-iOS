//
//  CalculationEngine.swift
//  Calculator-Swift
//
//  Created by Mike Smith on 20/08/2017.
//  Copyright © 2017 Mike Smith. All rights reserved.
//

import Foundation

private func calculate(_ expression: String) -> String {
    let replacements = [
        "×": "*",
        "÷": "/",
        "−": "-"
    ]
    var translatedExpression = expression
    for replacement in replacements {
        translatedExpression = translatedExpression.replacingOccurrences(of: replacement.key, with: replacement.value)
    }
    
    let engine = CalculationEngine()
    return engine.calculate(translatedExpression)
}

private func trimStringLastCharacter(_ originalString: String) -> String {
    var result = originalString
    result.remove(at: result.index(before: result.endIndex))
    return result
}

struct CalculatorModel {
    private let operations: Dictionary<String, (String) -> String> = [
        "=": calculate,
        "AC": { _ in
            "0"
        },
        "←": trimStringLastCharacter
    ]
    
    func operate(_ expression: String, with operation: String) -> String {
        if let op = operations[operation] {
            let result = op(expression)
            if result != "" {
                return result
            }
        }
        return "0"
    }
}
