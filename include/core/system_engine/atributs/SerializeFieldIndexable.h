#pragma once

template<typename T>
concept IsIndexable = requires(T t, size_t i) {
    { t[i] } -> std::convertible_to<typename T::value_type>;
    { t.size() } -> std::convertible_to<size_t>;
    t.push_back(std::declval<typename T::value_type>());
};

template<typename T>
requires IsIndexable<T>
class SerializedFieldIndexable : public SerializedFieldBase {
private:
    T value;
    std::string fieldName;
    
    using ElementField = SerializedField<typename T::value_type>;
    
    std::string ElementToString(const typename T::value_type& elem) const {
        ElementField temp("", elem);
        return temp.ToString();
    }
    
    typename T::value_type ElementFromString(const std::string& str) const {
        ElementField temp("");
        temp.FromString(str);
        return temp.GetValue();
    }
    
public:
    SerializedFieldIndexable(const std::string& name, const T& defaultValue = T()) 
        : fieldName(name), value(defaultValue) {}
    
    ~SerializedFieldIndexable() {
        if constexpr (std::is_pointer_v<typename T::value_type>) {
            for (auto ptr : value) if(ptr) delete ptr;
        }
    }
    
    SerializedFieldIndexable& operator=(const T& newValue) { value = newValue; return *this; }
    
    std::string ToString() const override {
        std::stringstream ss;
        ss << "[";
        for (size_t i = 0; i < value.size(); ++i) {
            if (i > 0) ss << ",";
            ss << ElementToString(value[i]);
        }
        ss << "]";
        return ss.str();
    }
    
    void FromString(const std::string& str) override {
        if constexpr (std::is_pointer_v<typename T::value_type>) {
            for (auto ptr : value) delete ptr;
        }
        value.clear();
        
        if (str.empty() || str[0] != '[') return;
        std::string content = str.substr(1, str.length() - 2);
        if (content.empty()) return;
        
        std::stringstream ss(content);
        std::string elemStr;
        while (std::getline(ss, elemStr, ',')) {
            if (!elemStr.empty()) {
                value.push_back(ElementFromString(elemStr));
            }
        }
    }
    
    std::string GetName() const override { return fieldName; }
    const T& GetValue() const { return value; }
    T& GetValue() { return value; }

    auto& operator[](size_t i) { 
        if constexpr (requires { value[i]; }) {
            return value[i];
        }
    }
    
    const auto& operator[](size_t i) const { 
        if constexpr (requires { value[i]; }) {
            return value[i];
        }
    }
    
    auto begin() { 
        if constexpr (requires { value.begin(); }) {
            return value.begin();
        }
    }
    
    auto end() { 
        if constexpr (requires { value.end(); }) {
            return value.end();
        }
    }
    
    auto begin() const { 
        if constexpr (requires { value.begin(); }) {
            return value.begin();
        }
    }
    
    auto end() const { 
        if constexpr (requires { value.end(); }) {
            return value.end();
        }
    }
    
    size_t size() const { 
        if constexpr (requires { value.size(); }) {
            return value.size();
        }
        return 0;
    }
    
    void push_back(const typename T::value_type& elem) {
        if constexpr (requires { value.push_back(elem); }) {
            value.push_back(elem);
        }
    }
    
    operator T&() { return value; }
    operator const T&() const { return value; }
    
    T* operator->() { return &value; }
    const T* operator->() const { return &value; }
};

#define FIELD_INDEXABLE(type, name) \
    SerializedFieldIndexable<type> name {#name};