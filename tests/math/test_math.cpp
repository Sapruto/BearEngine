#include "../TestCore.h"

#include "Vector3.h"
#include "Vector2.h"
#include "Quaternion.h"
#include "Matrix/Matrix4x4.h"
#include <cmath>

using namespace Tests;

bool TestVector3Basic() {
    Vector3f v1(1.0f, 2.0f, 3.0f);
    Vector3f v2(4.0f, 5.0f, 6.0f);
    
    TEST_ASSERT_EQUAL(v1.x, 1.0f);
    TEST_ASSERT_EQUAL(v1.y, 2.0f);
    TEST_ASSERT_EQUAL(v1.z, 3.0f);
    
    Vector3f sum = v1 + v2;
    TEST_ASSERT_EQUAL(sum.x, 5.0f);
    TEST_ASSERT_EQUAL(sum.y, 7.0f);
    TEST_ASSERT_EQUAL(sum.z, 9.0f);
    
    return true;
}

bool TestVector3Magnitude() {
    Vector3f v(3.0f, 4.0f, 0.0f);
    TEST_ASSERT_EQUAL(v.magnitude(), 5.0f);
    TEST_ASSERT_EQUAL(v.sqrMagnitude(), 25.0f);
    return true;
}

bool TestVector3Dot() {
    Vector3f v1(1.0f, 2.0f, 3.0f);
    Vector3f v2(4.0f, 5.0f, 6.0f);
    TEST_ASSERT_EQUAL(v1.dot(v2), 32.0f);
    return true;
}

bool TestVector3Cross() {
    Vector3f v1(1.0f, 0.0f, 0.0f);
    Vector3f v2(0.0f, 1.0f, 0.0f);
    Vector3f cross = v1.cross(v2);
    TEST_ASSERT_EQUAL(cross.x, 0.0f);
    TEST_ASSERT_EQUAL(cross.y, 0.0f);
    TEST_ASSERT_EQUAL(cross.z, 1.0f);
    return true;
}

bool TestVector3Normalize() {
    Vector3f v(3.0f, 4.0f, 0.0f);
    Vector3f n = v.normalized();
    TEST_ASSERT_FLOAT_EQUAL(n.magnitude(), 1.0f, 0.0001f);
    TEST_ASSERT_FLOAT_EQUAL(n.x, 0.6f, 0.0001f);
    TEST_ASSERT_FLOAT_EQUAL(n.y, 0.8f, 0.0001f);
    return true;
}

bool TestQuaternionIdentity() {
    Quaternionf q = Quaternionf::Identity();
    TEST_ASSERT_EQUAL(q.scalar, 1.0f);
    TEST_ASSERT_EQUAL(q.ratioVec.x, 0.0f);
    TEST_ASSERT_EQUAL(q.ratioVec.y, 0.0f);
    TEST_ASSERT_EQUAL(q.ratioVec.z, 0.0f);
    return true;
}

bool TestQuaternionCreation() {
    float angle = 3.14159f / 2.0f;
    Vector3f axis(0.0f, 0.0f, 1.0f);
    Quaternionf q = Quaternionf::createRotation(angle, axis);
    
    TEST_ASSERT_FLOAT_EQUAL(q.scalar, 0.7071f, 0.001f);
    TEST_ASSERT_FLOAT_EQUAL(q.ratioVec.z, 0.7071f, 0.001f);
    return true;
}

bool TestQuaternionRotateVector() {
    Quaternionf q = Quaternionf::createRotation(3.14159f / 2.0f, Vector3f(0, 0, 1));
    Vector3f v(1.0f, 0.0f, 0.0f);
    Vector3f result = q.rotateVector(v);
    
    TEST_ASSERT_FLOAT_EQUAL(result.x, 0.0f, 0.001f);
    TEST_ASSERT_FLOAT_EQUAL(result.y, 1.0f, 0.001f);
    TEST_ASSERT_FLOAT_EQUAL(result.z, 0.0f, 0.001f);
    return true;
}

bool TestQuaternionSlerp() {
    Quaternionf q1 = Quaternionf::Identity();
    Quaternionf q2 = Quaternionf::createRotation(3.14159f / 2.0f, Vector3f(0, 0, 1));
    
    Quaternionf mid = Quaternionf::slerp(q1, q2, 0.5f);
    TEST_ASSERT_FLOAT_EQUAL(mid.scalar, 0.9238f, 0.001f);
    TEST_ASSERT_FLOAT_EQUAL(mid.ratioVec.z, 0.3826f, 0.001f);
    return true;
}

bool TestQuaternionConjugate() {
    Quaternionf q = Quaternionf::createRotation(1.0f, Vector3f(1, 0, 0));
    Quaternionf conj = q.conjugate();
    
    TEST_ASSERT_FLOAT_EQUAL(conj.scalar, q.scalar, 0.001f);
    TEST_ASSERT_FLOAT_EQUAL(conj.ratioVec.x, -q.ratioVec.x, 0.001f);
    TEST_ASSERT_FLOAT_EQUAL(conj.ratioVec.y, -q.ratioVec.y, 0.001f);
    TEST_ASSERT_FLOAT_EQUAL(conj.ratioVec.z, -q.ratioVec.z, 0.001f);
    return true;
}

bool TestMatrixIdentity() {
    Matrix4x4f m = Matrix4x4f::Identity();
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            float expected = (i == j) ? 1.0f : 0.0f;
            TEST_ASSERT_EQUAL(m(i, j), expected);
        }
    }
    return true;
}

bool TestMatrixMultiplication() {
    Matrix4x4f m1 = Matrix4x4f::Identity();
    Matrix4x4f m2 = Matrix4x4f::Identity();
    m2(0, 1) = 2.0f;
    m2(1, 0) = 3.0f;
    
    Matrix4x4f result = m1 * m2;
    TEST_ASSERT_EQUAL(result(0, 1), 2.0f);
    TEST_ASSERT_EQUAL(result(1, 0), 3.0f);
    return true;
}

bool TestMatrixLookAt() {
    Vector3f eye(0, 0, 5);
    Vector3f target(0, 0, 0);
    Vector3f up(0, 1, 0);
    
    Matrix4x4f view = Matrix4x4f::lookAt(eye, target, up);

    TEST_ASSERT_FLOAT_EQUAL(view(0, 2), 0.0f, 0.001f);
    TEST_ASSERT_FLOAT_EQUAL(view(1, 2), 0.0f, 0.001f);
    TEST_ASSERT_FLOAT_EQUAL(view(2, 2), -1.0f, 0.001f);
    return true;
}

bool TestMatrixPerspective() {
    Matrix4x4f proj = Matrix4x4f::perspective(60.0f, 1.777f, 0.1f, 100.0f);
    TEST_ASSERT_FLOAT_EQUAL(proj(3, 3), 0.0f, 0.001f);
    TEST_ASSERT_FLOAT_EQUAL(proj(2, 3), -1.0f, 0.001f);
    return true;
}

bool TestMatrixOrtho() {
    Matrix4x4f ortho = Matrix4x4f::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 10.0f);
    TEST_ASSERT_EQUAL(ortho(0, 0), 1.0f);
    TEST_ASSERT_EQUAL(ortho(1, 1), 1.0f);
    TEST_ASSERT_FLOAT_EQUAL(ortho(2, 2), -0.20202f, 0.001f);
    return true;
}

bool TestVector2Basic() {
    Vector2f v1(1.0f, 2.0f);
    Vector2f v2(3.0f, 4.0f);
    
    Vector2f sum = v1 + v2;
    TEST_ASSERT_EQUAL(sum.x, 4.0f);
    TEST_ASSERT_EQUAL(sum.y, 6.0f);
    
    TEST_ASSERT_FLOAT_EQUAL(v1.magnitude(), 2.2360f, 0.001f);
    return true;
}

bool TestVector2Angle() {
    Vector2f v(1.0f, 0.0f);
    float angle = Vector2f::Angle(v);
    TEST_ASSERT_EQUAL(angle, 0.0f);
    
    Vector2f v2(0.0f, 1.0f);
    float angle2 = Vector2f::Angle(v2);
    TEST_ASSERT_FLOAT_EQUAL(angle2, 1.5708f, 0.001f);
    return true;
}

bool TestVector2RotateTowards() {
    Vector2f current(1.0f, 0.0f);
    Vector2f target(0.0f, 1.0f);
    Vector2f result = Vector2f::RotateTowards(current, target, 0.5f);
    
    TEST_ASSERT_FLOAT_EQUAL(result.x, 0.8775f, 0.001f);
    TEST_ASSERT_FLOAT_EQUAL(result.y, 0.4794f, 0.001f);
    return true;
}

bool TestSceneCreation() {
    Scene scene("TestScene");
    TEST_ASSERT_EQUAL(scene.GetName(), "TestScene");
    TEST_ASSERT(!scene.IsDestroyed());
    return true;
}

bool TestGameObjectCreation() {
    Scene scene("TestScene");
    GameObject* obj = scene.CreateGameObject();
    TEST_ASSERT(obj != nullptr);
    TEST_ASSERT_EQUAL(obj->GetName(), "GameObject");
    return true;
}

bool TestComponentSystem() {
    Scene scene("TestScene");
    GameObject* obj = scene.CreateGameObject();
    
    Transform2D* transform = obj->AddComponent<Transform2D>();
    TEST_ASSERT(transform != nullptr);
    
    Transform2D* retrieved = obj->GetComponentOfType<Transform2D>();
    TEST_ASSERT(retrieved == transform);
    
    TEST_ASSERT(obj->HasComponent<Transform2D>());
    
    return true;
}

int main() {
    TestSuite suite;
    
    suite.AddTest("Vector3 - базовая операция", TestVector3Basic);
    suite.AddTest("Vector3 - магнитуда", TestVector3Magnitude);
    suite.AddTest("Vector3 - скалярное произведение", TestVector3Dot);
    suite.AddTest("Vector3 - векторное произведение", TestVector3Cross);
    suite.AddTest("Vector3 - нормализация", TestVector3Normalize);
    
    suite.AddTest("Quaternion - identity", TestQuaternionIdentity);
    suite.AddTest("Quaternion - создание", TestQuaternionCreation);
    suite.AddTest("Quaternion - вращение вектора", TestQuaternionRotateVector);
    suite.AddTest("Quaternion - slerp", TestQuaternionSlerp);
    suite.AddTest("Quaternion - сопряжение", TestQuaternionConjugate);
    
    suite.AddTest("Matrix4x4 - identity", TestMatrixIdentity);
    suite.AddTest("Matrix4x4 - умножение", TestMatrixMultiplication);
    suite.AddTest("Matrix4x4 - lookAt", TestMatrixLookAt);
    suite.AddTest("Matrix4x4 - perspective", TestMatrixPerspective);
    suite.AddTest("Matrix4x4 - ortho", TestMatrixOrtho);
    
    suite.AddTest("Vector2 - базовая операция", TestVector2Basic);
    suite.AddTest("Vector2 - угол", TestVector2Angle);
    suite.AddTest("Vector2 - поворот к цели", TestVector2RotateTowards);
    
    suite.AddTest("Scene - создание", TestSceneCreation);
    suite.AddTest("GameObject - создание", TestGameObjectCreation);
    suite.AddTest("Component - система", TestComponentSystem);
    
    suite.RunAll();
    
    return suite.AllPassed() ? 0 : 1;
}