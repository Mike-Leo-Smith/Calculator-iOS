//
//  ViewController.swift
//  Calculator-Swift
//
//  Created by Mike Smith on 19/08/2017.
//  Copyright © 2017 Mike Smith. All rights reserved.
//

import UIKit

class ViewController: UIViewController {
    override func viewDidLoad() {
        super.viewDidLoad()
    }
    
    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
    }
    
    @IBOutlet weak var display: UILabel!
    
    var displayText: String {
        get {
            return display.text ?? "0"
        }
        set {
            display.text = newValue
        }
    }
    
    @IBAction func buttonTouched(_ sender: UIButton) {
        if let input = sender.currentTitle {
            if displayText != "0" || input == "." {
                displayText += input
            } else {
                displayText = input
            }
        }
    }
    
    let engine = CalculatorModel()
    
    @IBAction func doOperation(_ sender: UIButton) {
        if let operation = sender.currentTitle {
            displayText = engine.operate(displayText, with: operation)
        }
    }
    
}
