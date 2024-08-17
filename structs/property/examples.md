Here are over 10 examples demonstrating various ways to use the `Property` class template from the provided code. Each example highlights different features of the `Property` class, including callbacks, validation, coercion, and bindings.

### 1. Basic Usage
```cpp
#include "Property.h"

int main() {
    Property<int> prop1(10); // Initialize property with value 10
    cout << "Initial value: " << static_cast<int>(prop1) << endl;
    
    prop1 = 20; // Change value to 20
    cout << "New value: " << static_cast<int>(prop1) << endl;

    return 0;
}
```

### 2. Using Change Callbacks
```cpp
#include "Property.h"

void OnChange(int& oldValue, int& newValue) {
    cout << "Value changed from " << oldValue << " to " << newValue << endl;
}

int main() {
    Property<int> prop1(10);
    prop1.AddChangeCallback(OnChange);
    
    prop1 = 20; // Triggers the callback

    return 0;
}
```

### 3. Adding Multiple Callbacks
```cpp
#include "Property.h"

void Callback1(int& oldValue, int& newValue) {
    cout << "Callback1: " << oldValue << " -> " << newValue << endl;
}

void Callback2(int& oldValue, int& newValue) {
    cout << "Callback2: " << oldValue << " -> " << newValue << endl;
}

int main() {
    Property<int> prop1(10);
    prop1.AddChangeCallback(Callback1);
    prop1.AddChangeCallback(Callback2);
    
    prop1 = 20; // Triggers both callbacks

    return 0;
}
```

### 4. Removing a Callback
```cpp
#include "Property.h"

void OnChange(int& oldValue, int& newValue) {
    cout << "OnChange: " << oldValue << " -> " << newValue << endl;
}

int main() {
    Property<int> prop1(10);
    auto callbackID = prop1.AddChangeCallback(OnChange);
    
    prop1 = 20; // Triggers the callback
    prop1.RemoveChangeCallback(callbackID); // Remove callback
    
    prop1 = 30; // No callback triggered

    return 0;
}
```

### 5. Using Validators
```cpp
#include "Property.h"

bool Validator(int& newValue) {
    return newValue >= 0; // Only allow non-negative values
}

int main() {
    Property<int> prop1(10, Validator);
    
    prop1 = -5; // Invalid, so value won't change
    cout << "Current value: " << static_cast<int>(prop1) << endl;

    prop1 = 15; // Valid
    cout << "Current value: " << static_cast<int>(prop1) << endl;

    return 0;
}
```

### 6. Using Coerce Callbacks
```cpp
#include "Property.h"

void CoerceCallback(int& newValue) {
    if (newValue < 0) newValue = 0; // Coerce negative values to zero
}

int main() {
    Property<int> prop1(10, CoerceCallback);
    
    prop1 = -5; // Coerced to 0
    cout << "Current value: " << static_cast<int>(prop1) << endl;

    return 0;
}
```

### 7. Validator and Coerce Callback Together
```cpp
#include "Property.h"

bool Validator(int& newValue) {
    return newValue % 2 == 0; // Only allow even values
}

void CoerceCallback(int& newValue) {
    newValue += 2 - (newValue % 2); // Coerce to the next even number
}

int main() {
    Property<int> prop1(10, Validator, CoerceCallback);
    
    prop1 = 11; // Coerced to 12 because 11 is odd
    cout << "Current value: " << static_cast<int>(prop1) << endl;

    return 0;
}
```

### 8. Binding Properties
```cpp
#include "Property.h"

int main() {
    Property<int> prop1(10);
    Property<int> prop2(20);
    
    prop1.AddOneWayBind(prop2); // Bind prop2 to prop1
    
    prop1 = 30; // Updates prop2 to 30
    cout << "prop2 value: " << static_cast<int>(prop2) << endl;

    return 0;
}
```

### 9. Two-Way Binding
```cpp
#include "Property.h"

int main() {
    Property<int> prop1(10);
    Property<int> prop2(20);
    
    prop1.AddBind(prop2); // Two-way binding
    
    prop1 = 30; // Updates prop2 to 30
    cout << "prop2 value: " << static_cast<int>(prop2) << endl;

    prop2 = 40; // Updates prop1 to 40
    cout << "prop1 value: " << static_cast<int>(prop1) << endl;

    return 0;
}
```

### 10. Removing Bindings
```cpp
#include "Property.h"

int main() {
    Property<int> prop1(10);
    Property<int> prop2(20);
    
    prop1.AddBind(prop2); // Two-way binding
    
    prop1 = 30; // Updates prop2 to 30
    prop2 = 40; // Updates prop1 to 40
    
    prop1.RemoveBind(prop2); // Remove binding
    
    prop2 = 50; // No longer updates prop1
    cout << "prop1 value: " << static_cast<int>(prop1) << endl;
    cout << "prop2 value: " << static_cast<int>(prop2) << endl;

    return 0;
}
```

### 11. Handling Thread-Safety with Callbacks
```cpp
#include "Property.h"
#include <thread>

void ThreadFunction(Property<int>& prop) {
    prop = 50; // Change property value in a different thread
}

int main() {
    Property<int> prop(10);

    prop.AddChangeCallback([](int& oldValue, int& newValue) {
        cout << "Value changed in thread: " << oldValue << " -> " << newValue << endl;
    });

    std::thread t(ThreadFunction, std::ref(prop));
    t.join(); // Wait for thread to finish

    return 0;
}
```

### 12. Advanced Usage: Combining All Features
```cpp
#include "Property.h"

void OnChange(int& oldValue, int& newValue) {
    cout << "OnChange: " << oldValue << " -> " << newValue << endl;
}

bool Validator(int& newValue) {
    return newValue > 0; // Only positive values
}

void CoerceCallback(int& newValue) {
    if (newValue <= 0) newValue = 1; // Coerce non-positive values to 1
}

int main() {
    Property<int> prop1(10, Validator, CoerceCallback);
    Property<int> prop2(20);
    
    prop1.AddChangeCallback(OnChange);
    prop1.AddBind(prop2); // Two-way binding

    prop1 = -5; // Coerced to 1
    cout << "prop1 value: " << static_cast<int>(prop1) << endl;
    cout << "prop2 value: " << static_cast<int>(prop2) << endl;

    prop2 = 30; // Updates prop1 to 30
    cout << "prop1 value: " << static_cast<int>(prop1) << endl;

    return 0;
}
```

These examples cover various aspects of the `Property` class, including basic operations, callbacks, validation, coercion, and property binding. Adjust and expand them based on your specific needs and project requirements!
