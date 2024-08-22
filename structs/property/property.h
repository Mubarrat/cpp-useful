/*
  MIT License
  
  Copyright (c) 2024 Mubarrat
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
  
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

#pragma once
#include <iostream>
#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <mutex>
#include <queue>

using namespace std;

template<typename T>
struct Property
{
public:
    /**
     * @brief Type definition for change callback function.
     *
     * The function signature should be:
     * void callback(T& oldValue, T& newValue);
     *
     * @param oldValue The old value before the change.
     * @param newValue The new value after the change.
     */
    using ChangeCallback = function<void(T& oldValue, T& newValue)>;

    /**
     * @brief Type definition for value validator function.
     *
     * The function signature should be:
     * bool validator(T& newValue);
     *
     * @param newValue The new value to be validated.
     * @return true if the value is valid, false otherwise.
     */
    using Validator = function<bool(T& newValue)>;

    /**
     * @brief Type definition for coercion callback function.
     *
     * The function signature should be:
     * void coerceCallback(T& newValue);
     *
     * @param newValue The new value that will be coerced.
     */
    using CoerceCallback = function<void(T& newValue)>;

    /**
     * @brief Type definition for callback IDs.
     *
     * Used to uniquely identify callbacks.
     */
    using CallbackID = size_t;

private:
    T m_value; /// The current value of the property.
    unordered_map<CallbackID, ChangeCallback> m_callbacks; /// Map of change callbacks with their IDs.
    unordered_set<Property<T>*> m_bindings; /// Set of properties bound to this property.
    queue<CallbackID> m_availableIDs; /// Queue of available IDs for reuse.
    CallbackID nextCallbackID = 0; /// ID generator for new callbacks.
    Validator m_validator; /// Validator function for new values.
    CoerceCallback m_coerceCallback; /// Coerce callback function for new values.
    mutable mutex m_mutex; /// Mutex for thread-safe access.

public:
    /**
     * @brief Default constructor.
     * @param value The initial value of the property.
     */
    Property(T value = T()) : m_value(value) {}

    /**
     * @brief Constructor with validator.
     * @param validator The validator function for new values.
     */
    Property(Validator validator) : m_validator(validator) {}

    /**
     * @brief Constructor with coercion callback.
     * @param coerceCallback The coercion callback function.
     */
    Property(CoerceCallback coerceCallback) : m_coerceCallback(coerceCallback) {}

    /**
     * @brief Constructor with initial value and validator.
     * @param value The initial value of the property.
     * @param validator The validator function for new values.
     */
    Property(T value, Validator validator) : m_value(value), m_validator(validator) {}

    /**
     * @brief Constructor with initial value and coercion callback.
     * @param value The initial value of the property.
     * @param coerceCallback The coercion callback function.
     */
    Property(T value, CoerceCallback coerceCallback) : m_value(value), m_coerceCallback(coerceCallback) {}

    /**
     * @brief Constructor with initial value, validator, and coercion callback.
     * @param value The initial value of the property.
     * @param validator The validator function for new values.
     * @param coerceCallback The coercion callback function.
     */
    Property(T value, Validator validator, CoerceCallback coerceCallback)
        : m_value(value), m_validator(validator), m_coerceCallback(coerceCallback) {}

    /**
     * @brief Assignment operator to set a new value.
     * @param newValue The new value to assign.
     * @return A reference to this property.
     */
    Property<T>& operator=(T& newValue)
    {
        lock_guard<mutex> lock(m_mutex); // Ensure thread-safety.
        if (m_value != newValue)
        {
            if (m_coerceCallback)
                m_coerceCallback(newValue); // Apply coercion if specified.
            if (!m_validator || m_validator(newValue)) // Validate if validator is provided.
            {
                T oldValue = m_value; // Store old value.
                m_value = newValue; // Update to new value.
                NotifyCallbacks(oldValue, newValue); // Notify callbacks.
                NotifyBindings(oldValue, newValue); // Notify bound properties.
            }
        }
        return *this;
    }

    /**
     * @brief Conversion operator to get the value.
     * @return The current value of the property.
     */
    operator T() const
    {
        return m_value;
    }

    /**
     * @brief Arrow operator to access members of the underlying value.
     * 
     * This operator allows direct access to the members of the underlying value
     * if the type `T` is a class or struct. It returns a pointer to `m_value`.
     *
     * @return A pointer to the underlying value.
     */
    T* operator->()
    {
        return &m_value;
    }

    /**
     * @brief Add a change callback and return its ID.
     * @param callback The callback function to add.
     * @return The ID of the added callback.
     */
    CallbackID AddChangeCallback(ChangeCallback callback)
    {
        lock_guard<mutex> lock(m_mutex); // Ensure thread-safety.
        CallbackID id;
        if (m_availableIDs.empty())
        {
            id = nextCallbackID++; // Generate a new ID.
        }
        else
        {
            id = m_availableIDs.front(); // Reuse an available ID.
            m_availableIDs.pop(); // Remove it from the available IDs queue.
        }
        m_callbacks[id] = callback; // Store the callback with the assigned ID.
        return id; // Return the ID that was used.
    }

    /**
     * @brief Remove a change callback using the callback function.
     * @param callback The callback function to remove.
     */
    void RemoveChangeCallback(ChangeCallback callback)
    {
        lock_guard<mutex> lock(m_mutex); // Ensure thread-safety.
        for (auto it = m_callbacks.begin(); it != m_callbacks.end(); ++it)
        {
            if (callback.target_type() == it->second.target_type() &&
                callback.target<void(T&, T&)>() == it->second.target<void(T&, T&)>())
            {
                m_availableIDs.push(it->first); // Add the ID to the available IDs queue.
                m_callbacks.erase(it); // Remove the callback and break the loop.
                break;
            }
        }
    }

    /**
     * @brief Remove a change callback using its ID.
     * @param id The ID of the callback to remove.
     */
    void RemoveChangeCallback(CallbackID id)
    {
        lock_guard<mutex> lock(m_mutex); // Ensure thread-safety.
        if (m_callbacks.erase(id)) // Remove the callback if it exists.
        {
            m_availableIDs.push(id); // Add the ID to the available IDs queue.
        }
    }

    /**
     * @brief Add a one-way binding to another property.
     * @param other The other property to bind to.
     */
    void AddOneWayBind(Property<T>& other)
    {
        m_bindings.insert(&other); // Add to bindings.
    }

    /**
     * @brief Remove a one-way binding to another property.
     * @param other The other property to unbind.
     */
    void RemoveOneWayBind(Property<T>& other)
    {
        m_bindings.erase(&other); // Remove from bindings.
    }

    /**
     * @brief Add a one-way-to-source binding to another property.
     * @param other The other property to bind with this property.
     */
    void AddOneWayToSourceBind(Property<T>& other)
    {
        other.AddOneWayBind(*this); // Add this property to the other property's bindings.
    }

    /**
     * @brief Remove a one-way-to-source binding from another property.
     * @param other The other property to unbind from this property.
     */
    void RemoveOneWayToSourceBind(Property<T>& other)
    {
        other.RemoveOneWayBind(*this); // Remove this property from the other property's bindings.
    }

    /**
     * @brief Add a two-way binding with another property.
     * @param other The other property to bind with.
     */
    void AddBind(Property<T>& other)
    {
        AddOneWayBind(other); // Add one-way binding to the other property.
        AddOneWayToSourceBind(other); // Add one-way-to-source binding with the other property.
    }

    /**
     * @brief Remove a two-way binding with another property.
     * @param other The other property to unbind.
     */
    void RemoveBind(Property<T>& other)
    {
        RemoveOneWayBind(other); // Remove one-way binding to the other property.
        RemoveOneWayToSourceBind(other); // Remove one-way-to-source binding with the other property.
    }

    /**
     * @brief Set the validator function for new values.
     * @param validator The validator function.
     */
    void SetValidator(Validator validator)
    {
        lock_guard<mutex> lock(m_mutex); // Ensure thread-safety.
        m_validator = validator; // Set the validator function.
    }

    /**
     * @brief Set the coercion callback function for new values.
     * @param coerceCallback The coercion callback function.
     */
    void SetCoerceCallback(CoerceCallback coerceCallback)
    {
        lock_guard<mutex> lock(m_mutex); // Ensure thread-safety.
        m_coerceCallback = coerceCallback; // Set the coercion callback function.
    }

private:
    /**
     * @brief Notify all registered callbacks of a change.
     * @param oldValue The old value before the change.
     * @param newValue The new value after the change.
     */
    void NotifyCallbacks(T& oldValue, T& newValue)
    {
        for (const auto& callbackPair : m_callbacks)
            if (const auto& callback = callbackPair.second)
                callback(oldValue, newValue); // Call each registered callback.
    }

    /**
     * @brief Notify all bound properties of a change.
     * @param oldValue The old value before the change.
     * @param newValue The new value after the change.
     */
    void NotifyBindings(T& oldValue, T& newValue)
    {
        for (Property<T>* binding : m_bindings)
        {
            lock_guard<mutex> lock(binding->m_mutex); // Ensure thread-safety.
            if (binding->m_value != newValue)
            {
                if (binding->m_coerceCallback)
                    binding->m_coerceCallback(newValue); // Apply coercion if specified.
                if (!binding->m_validator || binding->m_validator(newValue))
                    binding->m_value = newValue; // Update bound property value.
            }
        }
    }
};
