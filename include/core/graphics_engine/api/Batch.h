#pragma once

#include <vector>
#include <string>
#include "api/base/Element.h"

template<typename Matrix, typename Vector, typename Color>
class Batch {
private:
    std::vector<Element<Matrix, Vector, Color>> elements;
    
public:
    Batch() = default;

    void AddElement(const Element<Matrix, Vector, Color>& element) {
        elements.push_back(element);
    }
    
    void AddElements(const std::vector<Element<Matrix, Vector, Color>>& newElements) {
        elements.insert(elements.end(), newElements.begin(), newElements.end());
    }
    
    void Clear() { elements.clear(); }
    
    std::vector<Element<Matrix, Vector, Color>>& GetElements() {
        return elements;
    }
    const std::vector<Element<Matrix, Vector, Color>>& GetElements() const {
        return elements;
    }
    
    size_t GetElementCount() const { return elements.size(); }
    bool IsEmpty() const { return elements.empty(); }
};