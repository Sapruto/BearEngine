#include <cmath>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>

#include "Scene.h"
#include "GameObject.h"
#include "Component.h"
#include "ComponentLibrary.h"
#include "Systems/Serialization/SerializeField.h"

#include "Systems/Serialization/SceneParser/SceneCreator.h"

using namespace SerializeField;
using namespace Serialization;

BEGIN_COMPONENT(TestNamedComponent, Component)
public:
    TestNamedComponent() = default;
    ~TestNamedComponent() override = default;

    void SetLabel(const std::string& v) { label.GetValue() = v; }
    const std::string& GetLabel() const { return label.GetValue(); }

    void SetValue(int v) { value.GetValue() = v; }
    int GetValue() const { return value.GetValue(); }

private:
    FIELD(std::string, label)
    FIELD(int, value)

    SERIALIZED_FIELDS_BASE(&label, &value)
END_COMPONENT(TestNamedComponent)

BEGIN_COMPONENT(TestFloatComponent, Component)
public:
    TestFloatComponent() = default;
    ~TestFloatComponent() override = default;

    void SetX(float v) { x.GetValue() = v; }
    float GetX() const { return x.GetValue(); }

private:
    FIELD(float, x)

    SERIALIZED_FIELDS_BASE(&x)
END_COMPONENT(TestFloatComponent)

static int g_passed = 0;
static int g_failed = 0;

#define CHECK(cond, msg)                                                    \
    do {                                                                    \
        if (cond) { ++g_passed;  std::cout << "  [ OK ] " << msg << "\n"; } \
        else      { ++g_failed; std::cout << "  [FAIL] " << msg << "\n"; }  \
    } while (0)

#define CHECK_EQ(a, b, msg)                                                 \
    do {                                                                    \
        auto va = (a); auto vb = (b);                                       \
        if (va == vb) { ++g_passed;  std::cout << "  [ OK ] " << msg << "\n"; } \
        else          { ++g_failed; std::cout << "  [FAIL] " << msg         \
                                              << "  (" << va << " != " << vb << ")\n"; } \
    } while (0)

#define CHECK_NEAR(a, b, eps, msg)                                          \
    do {                                                                    \
        auto va = (a); auto vb = (b);                                       \
        if (std::fabs(va - vb) < (eps)) { ++g_passed;                       \
            std::cout << "  [ OK ] " << msg << "\n"; }                      \
        else { ++g_failed; std::cout << "  [FAIL] " << msg                  \
                                     << "  (" << va << " != " << vb << ")\n"; } \
    } while (0)

static const std::filesystem::path SAVE_FILE = "scene_save.txt";

static std::unique_ptr<Scene> BuildTestScene(const std::string& name = "TestScene") {
    auto scene = std::make_unique<Scene>();
    scene->SetName(name);

    {
        auto obj = std::make_unique<GameObject>("Alpha");
        auto* c = obj->AddComponent<TestNamedComponent>();
        c->SetLabel("first");
        c->SetValue(42);
        scene->AddGameObject(std::move(obj));
    }

    {
        auto obj = std::make_unique<GameObject>("Beta");
        auto* flt = obj->AddComponent<TestFloatComponent>();
        flt->SetX(3.14f);
        auto* named = obj->AddComponent<TestNamedComponent>();
        named->SetLabel("second");
        named->SetValue(-7);
        scene->AddGameObject(std::move(obj));
    }

    {
        auto obj = std::make_unique<GameObject>("Gamma");
        scene->AddGameObject(std::move(obj));
    }

    return scene;
}

static void PrintRegistered() {
    std::cout << "Registered components:\n";
    for (const auto& name : ComponentRegistry::GetAllNames()) {
        std::cout << "  " << name << "\n";
    }
    std::cout << "---\n\n";
}

static TestNamedComponent* FindNamed(GameObject* go) {
    if (!go) return nullptr;
    for (auto* c : go->GetComponents()) {
        if (auto* n = dynamic_cast<TestNamedComponent*>(c)) return n;
    }
    return nullptr;
}

static TestFloatComponent* FindFloat(GameObject* go) {
    if (!go) return nullptr;
    for (auto* c : go->GetComponents()) {
        if (auto* f = dynamic_cast<TestFloatComponent*>(c)) return f;
    }
    return nullptr;
}

static GameObject* FindObject(Scene* scene, const std::string& name) {
    if (!scene) return nullptr;
    for (auto* go : scene->GetGameObjects()) {
        if (go && go->GetName() == name) return go;
    }
    return nullptr;
}

static void Test_Registry_HasTestComponents() {
    std::cout << "\n=== Test_Registry_HasTestComponents ===\n";

    CHECK(ComponentRegistry::Has("TestNamedComponent"), "TestNamedComponent registered");
    CHECK(ComponentRegistry::Has("TestFloatComponent"), "TestFloatComponent registered");
    CHECK(ComponentRegistry::Create("TestNamedComponent") != nullptr, "Create TestNamedComponent");
    CHECK(ComponentRegistry::Create("TestFloatComponent") != nullptr, "Create TestFloatComponent");
}

static void Test_Serialize_ContainsEverything() {
    std::cout << "\n=== Test_Serialize_ContainsEverything ===\n";

    SceneCreator creator;
    auto scene = BuildTestScene();
    std::string text = creator.SerializeToString(*scene);

    CHECK(!text.empty(), "text is not empty");
    CHECK(text.find("TestScene") != std::string::npos, "has scene name");
    CHECK(text.find("Alpha") != std::string::npos, "has Alpha");
    CHECK(text.find("Beta")  != std::string::npos, "has Beta");
    CHECK(text.find("Gamma") != std::string::npos, "has Gamma");
    CHECK(text.find("TestNamedComponent") != std::string::npos, "has TestNamedComponent");
    CHECK(text.find("TestFloatComponent") != std::string::npos, "has TestFloatComponent");
    CHECK(text.find("first") != std::string::npos, "has 'first' label");
    CHECK(text.find("second") != std::string::npos, "has 'second' label");
}

static void Test_Deserialize_ObjectStructure() {
    std::cout << "\n=== Test_Deserialize_ObjectStructure ===\n";

    SceneCreator creator;
    auto original = BuildTestScene();
    std::string text = creator.SerializeToString(*original);
    auto restored = creator.DeserializeFromString(text);

    CHECK(restored != nullptr, "restored not null");
    CHECK_EQ(restored->GetName(), std::string("TestScene"), "scene name");
    CHECK_EQ(restored->GetGameObjects().size(), size_t(3), "object count");

    auto* alpha = FindObject(restored.get(), "Alpha");
    auto* beta  = FindObject(restored.get(), "Beta");
    auto* gamma = FindObject(restored.get(), "Gamma");

    if (alpha) {
        CHECK_EQ(alpha->GetComponents().size(), size_t(1), "Alpha has 1 component");
    } else {
        CHECK(false, "skipped Alpha components check (alpha == nullptr)");
    }
    if (beta) {
        CHECK_EQ(beta->GetComponents().size(), size_t(2), "Beta has 2 components");
    } else {
        CHECK(false, "skipped Beta components check (beta == nullptr)");
    }
    if (gamma) {
        CHECK_EQ(gamma->GetComponents().size(), size_t(0), "Gamma has 0 components");
    } else {
        CHECK(false, "skipped Gamma components check (gamma == nullptr)");
    }
    CHECK_EQ(alpha->GetComponents().size(), size_t(1), "Alpha has 1 component");
    CHECK_EQ(beta->GetComponents().size(),  size_t(2), "Beta has 2 components");
    CHECK_EQ(gamma->GetComponents().size(), size_t(0), "Gamma has 0 components");
}

static void Test_Deserialize_ComponentFields() {
    std::cout << "\n=== Test_Deserialize_ComponentFields ===\n";

    SceneCreator creator;
    auto original = BuildTestScene();
    std::string text = creator.SerializeToString(*original);
    auto restored = creator.DeserializeFromString(text);

    auto* alpha = FindObject(restored.get(), "Alpha");
    auto* beta  = FindObject(restored.get(), "Beta");

    if (auto* named = FindNamed(alpha)) {
        CHECK_EQ(named->GetLabel(), std::string("first"), "Alpha.label");
        CHECK_EQ(named->GetValue(), 42, "Alpha.value");
    } else {
        CHECK(false, "Alpha has TestNamedComponent");
    }

    if (auto* named = FindNamed(beta)) {
        CHECK_EQ(named->GetLabel(), std::string("second"), "Beta.named.label");
        CHECK_EQ(named->GetValue(), -7, "Beta.named.value");
    } else {
        CHECK(false, "Beta has TestNamedComponent");
    }

    if (auto* flt = FindFloat(beta)) {
        CHECK_NEAR(flt->GetX(), 3.14f, 1e-5f, "Beta.float.x");
    } else {
        CHECK(false, "Beta has TestFloatComponent");
    }
}

static void Test_RoundTrip_Stable() {
    std::cout << "\n=== Test_RoundTrip_Stable ===\n";

    SceneCreator creator;
    auto original = BuildTestScene();

    std::string text1 = creator.SerializeToString(*original);
    auto restored = creator.DeserializeFromString(text1);
    std::string text2 = creator.SerializeToString(*restored);

    CHECK_EQ(text1, text2, "serialize -> deserialize -> serialize == same");
}

static void Test_File_Save() {
    std::cout << "\n=== Test_File_Save ===\n";

    SceneCreator creator;
    auto scene = BuildTestScene();

    bool saved = creator.UpdateSceneFile(SAVE_FILE.string(), *scene);
    CHECK(saved, "UpdateSceneFile returned true");
    CHECK(std::filesystem::exists(SAVE_FILE), "file exists on disk");
    CHECK(std::filesystem::file_size(SAVE_FILE) > 0, "file is not empty");

    std::cout << "  saved to: " << std::filesystem::absolute(SAVE_FILE) << "\n";
}

static void Test_File_Load() {
    std::cout << "\n=== Test_File_Load ===\n";

    CHECK(std::filesystem::exists(SAVE_FILE), "save file exists (from previous test)");

    SceneCreator creator;
    auto loaded = creator.GetScene(SAVE_FILE.string());

    CHECK(loaded != nullptr, "GetScene returned non-null");
    CHECK_EQ(loaded->GetName(), std::string("TestScene"), "loaded scene name");
    CHECK_EQ(loaded->GetGameObjects().size(), size_t(3), "loaded object count");

    auto* alpha = FindObject(loaded.get(), "Alpha");
    auto* beta  = FindObject(loaded.get(), "Beta");
    CHECK(alpha != nullptr, "loaded Alpha");
    CHECK(beta  != nullptr, "loaded Beta");

    if (auto* named = FindNamed(alpha)) {
        CHECK_EQ(named->GetLabel(), std::string("first"), "loaded Alpha.label");
        CHECK_EQ(named->GetValue(), 42, "loaded Alpha.value");
    }
    if (auto* named = FindNamed(beta)) {
        CHECK_EQ(named->GetLabel(), std::string("second"), "loaded Beta.named.label");
        CHECK_EQ(named->GetValue(), -7, "loaded Beta.named.value");
    }
    if (auto* flt = FindFloat(beta)) {
        CHECK_NEAR(flt->GetX(), 3.14f, 1e-5f, "loaded Beta.float.x");
    }
}

static void Test_File_RoundTrip() {
    std::cout << "\n=== Test_File_RoundTrip ===\n";

    SceneCreator creator;
    auto scene = BuildTestScene();

    std::string originalText = creator.SerializeToString(*scene);

    creator.UpdateSceneFile(SAVE_FILE.string(), *scene);
    auto loaded = creator.GetScene(SAVE_FILE.string());
    std::string loadedText = creator.SerializeToString(*loaded);

    CHECK_EQ(originalText, loadedText, "serialize(load(file)) == serialize(original)");
}

static void Test_File_Delete() {
    std::cout << "\n=== Test_File_Delete ===\n";

    SceneCreator creator;
    bool removed = creator.DeleteSceneFile(SAVE_FILE.string());
    CHECK(removed, "DeleteSceneFile returned true");
    CHECK(!std::filesystem::exists(SAVE_FILE), "file removed from disk");
}

static void Test_GetScene_NonExistent() {
    std::cout << "\n=== Test_GetScene_NonExistent ===\n";

    SceneCreator creator;
    auto scene = creator.GetScene("definitely_not_a_real_file.scene");
    CHECK(scene == nullptr, "non-existent file returns nullptr");
}

static void Test_Deserialize_EmptyString() {
    std::cout << "\n=== Test_Deserialize_EmptyString ===\n";

    SceneCreator creator;
    auto restored = creator.DeserializeFromString("");
    CHECK(restored != nullptr, "empty input doesn't crash");
}

static void Test_Deserialize_MalformedLine() {
    std::cout << "\n=== Test_Deserialize_MalformedLine ===\n";

    SceneCreator creator;
    auto restored = creator.DeserializeFromString("just_a_garbage_line_without_space\n");
    CHECK(restored != nullptr, "malformed input doesn't crash");
}

int main() {
    ComponentRegistry::Register<TestNamedComponent>("TestNamedComponent");
    ComponentRegistry::Register<TestFloatComponent>("TestFloatComponent");

    PrintRegistered();

    Test_Registry_HasTestComponents();
    Test_Serialize_ContainsEverything();
    Test_Deserialize_ObjectStructure();
    Test_Deserialize_ComponentFields();
    Test_RoundTrip_Stable();

    Test_File_Save();
    Test_File_Load();
    Test_File_RoundTrip();
    //Test_File_Delete();

    Test_GetScene_NonExistent();
    Test_Deserialize_EmptyString();
    Test_Deserialize_MalformedLine();

    std::cout << "\n============================\n";
    std::cout << "Passed: " << g_passed << "\n";
    std::cout << "Failed: " << g_failed << "\n";
    std::cout << "============================\n";

    return g_failed == 0 ? 0 : 1;
}