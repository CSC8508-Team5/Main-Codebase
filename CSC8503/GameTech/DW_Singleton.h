#pragma once

template<typename T>
class DW_Singleton {
public:
    static T& get_instance() noexcept(std::is_nothrow_constructible<T>::value) {
        static T instance{ token() };
        return instance;
    }
    virtual ~DW_Singleton() = default;
    DW_Singleton(const DW_Singleton&) = delete;
    DW_Singleton& operator =(const DW_Singleton&) = delete;
protected:
    struct token {}; // helper class
    DW_Singleton() noexcept = default;
};
