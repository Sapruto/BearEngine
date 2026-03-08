#pragma once

#include "Vector3.h"

struct Force {
    Vector3 direction;
    float magnitude;
    
    Force() : direction(0, 0, 0), magnitude(0) {}
    Force(const Vector3& dir, float mag) : direction(dir.normalized()), magnitude(mag) {}
    
    Vector3 GetForceVector() const {
        return direction * magnitude;
    }

    Force operator-() const { 
        return Force(-direction, magnitude); 
    }
    
    Force& operator+=(const Force& other) {
        Vector3 total = GetForceVector() + other.GetForceVector();
        float mag = total.magnitude();
        if (mag > 0) {
            direction = total / mag;  
            magnitude = mag;
        } else {
            direction = Vector3(0, 0, 0);
            magnitude = 0;
        }
        return *this;
    }
    
    Force& operator-=(const Force& other) {
        Vector3 total = GetForceVector() - other.GetForceVector();
        float mag = total.magnitude();
        if (mag > 0) {
            direction = total / mag;
            magnitude = mag;
        } else {
            direction = Vector3(0, 0, 0);
            magnitude = 0;
        }
        return *this;
    }
    
    Force& operator*=(float scalar) {
        magnitude *= scalar;
        return *this;
    }
    
    Force& operator/=(float scalar) {
        magnitude /= scalar;
        return *this;
    }
};

inline Force operator+(Force a, const Force& b) {
    return a += b;
}

inline Force operator-(Force a, const Force& b) {
    return a -= b;
}

inline Force operator*(Force f, float scalar) {
    return f *= scalar;
}

inline Force operator*(float scalar, Force f) {
    return f *= scalar;
}

inline Force operator/(Force f, float scalar) {
    return f /= scalar;
}