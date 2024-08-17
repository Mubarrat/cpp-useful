# Mubarrat/cpp-useful: Property Template

## Overview

The `Property` class template provides a robust mechanism for managing property values with support for change notifications, validation, coercion, and property bindings. This class is designed to be thread-safe and is suitable for a wide range of applications where property management is needed.

## Features

- **Change Callbacks**: Notify listeners when the property value changes.
- **Validation**: Ensure that new values meet specified criteria before updating.
- **Coercion**: Automatically adjust values to conform to certain rules.
- **One-Way and Two-Way Bindings**: Link properties so that changes propagate between them.

## Usage

See [examples](examples.md) markdown to see usage.

## API Documentation

### Constructors

- **`Property(T value = T())`**
  - Initializes a property with a default or specified value.

- **`Property(Validator validator)`**
  - Initializes a property with a validator function.

- **`Property(CoerceCallback coerceCallback)`**
  - Initializes a property with a coercion callback function.

- **`Property(T value, Validator validator)`**
  - Initializes a property with a default or specified value and a validator function.

- **`Property(T value, CoerceCallback coerceCallback)`**
  - Initializes a property with a default or specified value and a coercion callback function.

- **`Property(T value, Validator validator, CoerceCallback coerceCallback)`**
  - Initializes a property with a default or specified value, a validator function, and a coercion callback function.

### Assignment Operator

- **`Property<T>& operator=(T& newValue)`**
  - Sets a new value for the property. Applies the coercion callback if specified, validates the value if a validator is provided, and updates bound properties.

### Change Callbacks

- **`CallbackID AddChangeCallback(ChangeCallback callback)`**
  - Registers a callback to be invoked when the property value changes.
  - **Parameters**: `ChangeCallback callback` - The callback function to add.
  - **Returns**: `CallbackID` - The ID of the added callback.

- **`void RemoveChangeCallback(ChangeCallback callback)`**
  - Removes a change callback using the callback function.
  - **Parameters**: `ChangeCallback callback` - The callback function to remove.

- **`void RemoveChangeCallback(CallbackID id)`**
  - Removes a change callback using its ID.
  - **Parameters**: `CallbackID id` - The ID of the callback to remove.

### Property Bindings

- **`void AddOneWayBind(Property<T>& other)`**
  - Creates a one-way binding from this property to another property. Changes in this property will update the bound property.
  - **Parameters**: `Property<T>& other` - The property to bind to.

- **`void RemoveOneWayBind(Property<T>& other)`**
  - Removes a one-way binding to another property.
  - **Parameters**: `Property<T>& other` - The property to unbind.

- **`void AddOneWayToSourceBind(Property<T>& other)`**
  - Creates a one-way-to-source binding from another property to this property. Changes in the other property will update this property.
  - **Parameters**: `Property<T>& other` - The property to bind with this property.

- **`void RemoveOneWayToSourceBind(Property<T>& other)`**
  - Removes a one-way-to-source binding from another property.
  - **Parameters**: `Property<T>& other` - The property to unbind from this property.

- **`void AddBind(Property<T>& other)`**
  - Creates a two-way binding between this property and another property. Changes in either property will update the other.
  - **Parameters**: `Property<T>& other` - The property to bind with.

- **`void RemoveBind(Property<T>& other)`**
  - Removes a two-way binding with another property.
  - **Parameters**: `Property<T>& other` - The property to unbind.

### Validators and Coerce Callbacks

- **`void SetValidator(Validator validator)`**
  - Sets or changes the validator function for new values.
  - **Parameters**: `Validator validator` - The validator function.

- **`void SetCoerceCallback(CoerceCallback coerceCallback)`**
  - Sets or changes the coercion callback function for new values.
  - **Parameters**: `CoerceCallback coerceCallback` - The coercion callback function.

## License

This repository is licensed under the [MIT License](LICENSE.md).
