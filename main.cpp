#define SDL_MAIN_HANDLED
#define GLAD_GL_IMPLEMENTATION

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>
#include <cmath>
#include <memory>
#include <vector>
#include <string>
#include <random>
#include <algorithm>

#include "src/dependencies/imgui-master/backends/imgui_impl_glfw.h"
#include "src/dependencies/imgui-master/backends/imgui_impl_opengl3.h"

// НОВАЯ АРХИТЕКТУРА
#include "include/core/graphics_engine/GraphicsManager.h"
#include "include/core/graphics_engine/Sprite/SpriteRenderer.h"
#include "include/core/graphics_engine/Sprite/Sprite.h"
#include "include/core/graphics_engine/Camera.h"
#include "include/core/GameObject.h"
#include "include/core/math/Transform2D.h"
#include "include/core/phisic_engine/Rigidbody2D.h"
#include "include/core/system_engine/tags_system/TagManager.h"
#include "include/core/system_engine/tags_system/TagLibrary.h"
#include "include/core/system_engine/input_system/InputSystem.h"
#include "include/core/system_engine/time_system/Time.h"
#include "include/core/scene_engine/SceneManager.h"
#include "include/core/scene_engine/Scene.h"
#include "include/core/math/geometry/colliders_manager/ColliderManager.h"
#include "include/core/math/geometry/colliders/Polygon2D.h"
#include "include/core/system_engine/resource_system/ResourceManager.h"
#include "include/core/graphics_engine/RenderSettings.h" 

// ============================================
// КОНСТАНТЫ
// ============================================
const float ARENA_WIDTH = 2.4f;
const float ARENA_HEIGHT = 2.4f;
const float PIXELS_PER_UNIT = 200.0f;
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

// ============================================
// ТЕГИ
// ============================================
namespace GameTags {
    inline const TagLibrary Player(1, "Player");
    inline const TagLibrary Enemy(2, "Enemy");
    inline const TagLibrary EnemyFast(3, "EnemyFast");
    inline const TagLibrary EnemyTank(4, "EnemyTank");
    inline const TagLibrary EnemyShooter(5, "EnemyShooter");
    inline const TagLibrary EnemyBomber(6, "EnemyBomber");
    inline const TagLibrary EnemyHealer(7, "EnemyHealer");
    inline const TagLibrary Bullet(8, "Bullet");
    inline const TagLibrary EnemyBullet(9, "EnemyBullet");
}

// ============================================
// ГЛОБАЛЬНОЕ СОСТОЯНИЕ
// ============================================
struct GameState {
    static int playerHealth;
    static int score;
    static int waveNumber;
    static int enemiesRemaining;
    static int totalKills;
    static bool gameRunning;
    static Vector2 mousePosition;
    static ColliderManager* colliderManager;
    static Scene* currentScene;
};

int GameState::playerHealth = 100;
int GameState::score = 0;
int GameState::waveNumber = 0;
int GameState::enemiesRemaining = 0;
int GameState::totalKills = 0;
bool GameState::gameRunning = true;
Vector2 GameState::mousePosition(0.0f, 0.0f);
ColliderManager* GameState::colliderManager = nullptr;
Scene* GameState::currentScene = nullptr;

// ============================================
// ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ
// ============================================
Vector2 ScreenToWorld(const Vector2& screenPos, int width, int height) {
    float worldX = (screenPos.x - width / 2.0f) / PIXELS_PER_UNIT;
    float worldY = (height / 2.0f - screenPos.y) / PIXELS_PER_UNIT;
    return Vector2(worldX, worldY);
}

Polygon2D* AddBoxCollider(GameObject* obj, float size) {
    if (!obj) return nullptr;
    
    std::vector<Vector2> vertices;
    float halfSize = size * 0.5f;
    
    vertices.push_back(Vector2(-halfSize, -halfSize));
    vertices.push_back(Vector2(halfSize, -halfSize));
    vertices.push_back(Vector2(halfSize, halfSize));
    vertices.push_back(Vector2(-halfSize, halfSize));
    
    auto polygon = std::make_unique<Polygon2D>(std::move(vertices));
    Polygon2D* ptr = polygon.get();
    obj->AddComponent(polygon.release());
    
    if (ptr && GameState::colliderManager) {
        GameState::colliderManager->AddCollider(ptr);
    }
    
    return ptr;
}

// ============================================
// ПРЕДВАРИТЕЛЬНЫЕ ОБЪЯВЛЕНИЯ
// ============================================
class Player;
class Enemy;
class Bullet;
class EnemyBullet;
class WaveSystem;

// ============================================
// ИГРОК
// ============================================
class Player : public Component {
private:
    RigidBody2D* rb = nullptr;
    Polygon2D* collider = nullptr;
    float moveForce = 15.0f;
    float maxSpeed = 6.0f;
    float invincibilityTime = 0.0f;

public:
    void Start() override {
        TagManager::GetInstance().RegisterObject(GetGameObject(), GameTags::Player, true);
        rb = GetGameObject()->GetComponentOfType<RigidBody2D>();
        collider = AddBoxCollider(GetGameObject(), 0.3f);
    }
    
    void Destroy() override {

    }
    
    void Update() override {
        if (!rb || !GameState::gameRunning) return;
        
        Transform2D* transform = GetGameObject()->GetComponentOfType<Transform2D>();
        if (!transform) return;
        
        auto& input = InputSystem::GetInstance();
        
        Vector2 force(0, 0);
        if (input.GetKey(Keys::A)) force.x -= moveForce;
        if (input.GetKey(Keys::D)) force.x += moveForce;
        if (input.GetKey(Keys::W)) force.y += moveForce;
        if (input.GetKey(Keys::S)) force.y -= moveForce;
        
        if (force.magnitude() > 0) {
            rb->AddForce(force);
        }
        
        Vector2 vel = rb->GetVelocity();
        if (vel.magnitude() > maxSpeed) {
            vel = vel.normalized() * maxSpeed;
            rb->SetVelocity(vel);
        }
        
        float halfW = ARENA_WIDTH / 2.0f - 0.2f;
        float halfH = ARENA_HEIGHT / 2.0f - 0.2f;
        
        if (transform->position.x < -halfW) transform->position.x = -halfW;
        if (transform->position.x > halfW) transform->position.x = halfW;
        if (transform->position.y < -halfH) transform->position.y = -halfH;
        if (transform->position.y > halfH) transform->position.y = halfH;
        
        if (invincibilityTime > 0) {
            invincibilityTime -= Time::DeltaTime();
        }
        
        if (collider && GameState::colliderManager && invincibilityTime <= 0) {
            auto collisions = GameState::colliderManager->GetCollisionsFor(collider);
            auto enemies = TagManager::GetInstance().FindObjectsWithTag(GameTags::Enemy);
            
            for (auto otherCollider : collisions) {
                if (!otherCollider) continue;
                
                GameObject* otherObj = otherCollider->GetGameObject();
                if (!otherObj) continue;
                
                for (auto enemyObj : enemies) {
                    if (enemyObj == otherObj) {
                        TakeDamage(10);
                        break;
                    }
                }
            }
        }
    }
    
    void TakeDamage(int damage) {
        if (invincibilityTime <= 0 && GameState::gameRunning) {
            GameState::playerHealth -= damage;
            invincibilityTime = 1.0f;
            
            if (GameState::playerHealth <= 0) {
                GameState::playerHealth = 0;
                GameState::gameRunning = false;
            }
        }
    }
};

// ============================================
// БАЗОВЫЙ ВРАГ
// ============================================
class Enemy : public Component {
protected:
    RigidBody2D* rb = nullptr;
    Polygon2D* collider = nullptr;
    GameObject* player = nullptr;
    int health = 1;
    int scoreValue = 100;
    float chaseForce = 3.0f;
    float attackDamage = 10.0f;
    float attackCooldown = 0.5f;
    float attackTimer = 0.0f;
    float colliderSize = 0.2f;
    Scene* scene = nullptr;

public:
    void SetScene(Scene* s) { scene = s; }
    
    void Start() override {
        TagManager::GetInstance().RegisterObject(GetGameObject(), GameTags::Enemy, true);
        rb = GetGameObject()->GetComponentOfType<RigidBody2D>();
        collider = AddBoxCollider(GetGameObject(), colliderSize);
        
        if (scene) {
            auto players = scene->FindGameObjectsWithComponent<Player>();
            if (!players.empty()) {
                player = players[0];
            }
        }
    }
    
    void Destroy() override {
    }
    
    void Update() override {
        if (!rb || !player || !GameState::gameRunning) return;
        
        attackTimer += Time::DeltaTime();
        
        Transform2D* enemyTrans = GetGameObject()->GetComponentOfType<Transform2D>();
        Transform2D* playerTrans = player->GetComponentOfType<Transform2D>();
        
        if (enemyTrans && playerTrans) {
            Vector2 toPlayer = playerTrans->position - enemyTrans->position;
            float distance = toPlayer.magnitude();
            
            if (distance < 3.0f) {
                Vector2 force = toPlayer.normalized() * chaseForce;
                rb->AddForce(force);
                
                Vector2 vel = rb->GetVelocity();
                float maxSpeed = 2.0f;
                if (vel.magnitude() > maxSpeed) {
                    vel = vel.normalized() * maxSpeed;
                    rb->SetVelocity(vel);
                }
            }
        }
        
        if (collider && GameState::colliderManager && attackTimer >= attackCooldown) {
            auto collisions = GameState::colliderManager->GetCollisionsFor(collider);
            auto players = TagManager::GetInstance().FindObjectsWithTag(GameTags::Player);
            
            for (auto otherCollider : collisions) {
                if (!otherCollider) continue;
                
                GameObject* otherObj = otherCollider->GetGameObject();
                if (!otherObj) continue;
                
                for (auto playerObj : players) {
                    if (playerObj == otherObj) {
                        Player* playerComp = playerObj->GetComponentOfType<Player>();
                        if (playerComp) {
                            playerComp->TakeDamage(attackDamage);
                            attackTimer = 0.0f;
                            break;
                        }
                    }
                }
            }
        }
        
        Transform2D* enemyTransform = GetGameObject()->GetComponentOfType<Transform2D>();
        if (enemyTransform) {
            float halfW = ARENA_WIDTH / 2.0f - 0.2f;
            float halfH = ARENA_HEIGHT / 2.0f - 0.2f;
            if (enemyTransform->position.x < -halfW) enemyTransform->position.x = -halfW;
            if (enemyTransform->position.x > halfW) enemyTransform->position.x = halfW;
            if (enemyTransform->position.y < -halfH) enemyTransform->position.y = -halfH;
            if (enemyTransform->position.y > halfH) enemyTransform->position.y = halfH;
        }
    }
    
    virtual void TakeDamage(int damage) {
        health -= damage;
        if (health <= 0) {
            GameState::score += scoreValue;
            GameState::totalKills++;
            GameState::enemiesRemaining--;
            GetGameObject()->Destroy();
        }
    }
};

// ============================================
// FAST ENEMY
// ============================================
class FastEnemy : public Enemy {
public:
    FastEnemy() {
        health = 1;
        scoreValue = 150;
        chaseForce = 6.0f;
        attackDamage = 5.0f;
        attackCooldown = 0.3f;
        colliderSize = 0.16f;
    }
    
    void Start() override {
        TagManager::GetInstance().RegisterObject(GetGameObject(), GameTags::EnemyFast, true);
        Enemy::Start();
    }
};

// ============================================
// TANK ENEMY
// ============================================
class TankEnemy : public Enemy {
public:
    TankEnemy() {
        health = 4;
        scoreValue = 300;
        chaseForce = 1.5f;
        attackDamage = 20.0f;
        attackCooldown = 1.2f;
        colliderSize = 0.3f;
    }
    
    void Start() override {
        TagManager::GetInstance().RegisterObject(GetGameObject(), GameTags::EnemyTank, true);
        Enemy::Start();
    }
};

// ============================================
// ПУЛЯ ВРАГА
// ============================================
class EnemyBullet : public Component {
private:
    Vector2 direction;
    float lifetime = 2.0f;
    float timeAlive = 0.0f;
    int damage = 5;
    Polygon2D* collider = nullptr;
    Scene* scene = nullptr;

public:
    void SetScene(Scene* s) { scene = s; }
    void SetDirection(const Vector2& dir) { direction = dir.normalized(); }
    
    void Start() override {
        TagManager::GetInstance().RegisterObject(GetGameObject(), GameTags::EnemyBullet, true);
        
        RigidBody2D* rb = GetGameObject()->GetComponentOfType<RigidBody2D>();
        if (rb) {
            rb->SetVelocity(direction * 8.0f);
        }
        
        collider = AddBoxCollider(GetGameObject(), 0.06f);
    }
    
    void Destroy() override {
    }
    
    void Update() override {
        timeAlive += Time::DeltaTime();
        if (timeAlive >= lifetime) {
            GetGameObject()->Destroy();
            return;
        }
        
        if (collider && GameState::colliderManager) {
            auto collisions = GameState::colliderManager->GetCollisionsFor(collider);
            auto players = TagManager::GetInstance().FindObjectsWithTag(GameTags::Player);
            
            for (auto otherCollider : collisions) {
                if (!otherCollider) continue;
                
                GameObject* otherObj = otherCollider->GetGameObject();
                if (!otherObj) continue;
                
                for (auto playerObj : players) {
                    if (playerObj == otherObj) {
                        Player* player = playerObj->GetComponentOfType<Player>();
                        if (player) {
                            player->TakeDamage(damage);
                            GetGameObject()->Destroy();
                            return;
                        }
                    }
                }
            }
        }
    }
};

// ============================================
// SHOOTER ENEMY
// ============================================
class ShooterEnemy : public Enemy {
private:
    float shootTimer = 0.0f;
    float shootCooldown = 1.5f;
    float shootRange = 3.0f;

public:
    ShooterEnemy() {
        health = 2;
        scoreValue = 200;
        chaseForce = 1.0f;
        attackDamage = 10.0f;
        colliderSize = 0.2f;
    }
    
    void Start() override {
        TagManager::GetInstance().RegisterObject(GetGameObject(), GameTags::EnemyShooter, true);
        Enemy::Start();
    }
    
    void Update() override {
        Enemy::Update();
        
        shootTimer += Time::DeltaTime();
        
        if (!player || !GameState::gameRunning || !scene) return;
        
        Transform2D* enemyTrans = GetGameObject()->GetComponentOfType<Transform2D>();
        Transform2D* playerTrans = player->GetComponentOfType<Transform2D>();
        
        if (enemyTrans && playerTrans && shootTimer >= shootCooldown) {
            float distance = (playerTrans->position - enemyTrans->position).magnitude();
            
            if (distance < shootRange) {
                shootTimer = 0.0f;
                
                Vector2 dirToPlayer = (playerTrans->position - enemyTrans->position).normalized();
                
                GameObject* bullet = scene->CreateGameObject();
                
                auto bulletTrans = std::make_unique<Transform2D>();
                bulletTrans->position = enemyTrans->position + dirToPlayer * 0.3f;
                bulletTrans->scale = Vector2(0.05f, 0.05f);
                bullet->AddComponent(bulletTrans.release());
                
                auto bulletComp = std::make_unique<EnemyBullet>();
                bulletComp->SetDirection(dirToPlayer);
                bulletComp->SetScene(scene);
                bullet->AddComponent(bulletComp.release());
                
                auto bulletRb = std::make_unique<RigidBody2D>(0.1f, 0.1f);
                bullet->AddComponent(bulletRb.release());
                
                bullet->Start();
            }
        }
    }
};

// ============================================
// BOMBER ENEMY
// ============================================
class BomberEnemy : public Enemy {
private:
    float explosionRange = 0.5f;
    int explosionDamage = 30;

public:
    BomberEnemy() {
        health = 2;
        scoreValue = 250;
        chaseForce = 2.5f;
        attackDamage = 15.0f;
        attackCooldown = 0.8f;
        colliderSize = 0.22f;
    }
    
    void Start() override {
        TagManager::GetInstance().RegisterObject(GetGameObject(), GameTags::EnemyBomber, true);
        Enemy::Start();
    }
    
    void TakeDamage(int damage) override {
        health -= damage;
        if (health <= 0) {
            if (player) {
                Transform2D* enemyTrans = GetGameObject()->GetComponentOfType<Transform2D>();
                Transform2D* playerTrans = player->GetComponentOfType<Transform2D>();
                
                if (enemyTrans && playerTrans) {
                    float dist = (playerTrans->position - enemyTrans->position).magnitude();
                    if (dist < explosionRange) {
                        Player* playerComp = player->GetComponentOfType<Player>();
                        if (playerComp) {
                            playerComp->TakeDamage(explosionDamage);
                        }
                    }
                }
            }
            
            GameState::score += scoreValue;
            GameState::totalKills++;
            GameState::enemiesRemaining--;
            GetGameObject()->Destroy();
        }
    }
};

// ============================================
// HEALER ENEMY
// ============================================
class HealerEnemy : public Enemy {
public:
    HealerEnemy() {
        health = 3;
        scoreValue = 350;
        chaseForce = 1.2f;
        attackDamage = 8.0f;
        attackCooldown = 1.0f;
        colliderSize = 0.22f;
    }
    
    void Start() override {
        TagManager::GetInstance().RegisterObject(GetGameObject(), GameTags::EnemyHealer, true);
        Enemy::Start();
    }
};

// ============================================
// ПУЛЯ ИГРОКА
// ============================================
class Bullet : public Component {
private:
    Vector2 direction;
    float lifetime = 2.0f;
    float timeAlive = 0.0f;
    int damage = 1;
    Polygon2D* collider = nullptr;
    Scene* scene = nullptr;

public:
    ~Bullet() {
    }

    void SetScene(Scene* s) { scene = s; }
    void SetDirection(const Vector2& dir) { direction = dir.normalized(); }
    
    void Start() override {
        TagManager::GetInstance().RegisterObject(GetGameObject(), GameTags::Bullet, true);
        
        RigidBody2D* rb = GetGameObject()->GetComponentOfType<RigidBody2D>();
        if (rb) {
            rb->SetVelocity(direction * 12.0f);
        }
        
        collider = AddBoxCollider(GetGameObject(), 0.06f);
    }
    
    void Destroy() override {
    }
    
    void Update() override {
        timeAlive += Time::DeltaTime();
        if (timeAlive >= lifetime) {
            GetGameObject()->Destroy();
            return;
        }
        
        if (collider && GameState::colliderManager) {
            auto collisions = GameState::colliderManager->GetCollisionsFor(collider);
            auto enemies = TagManager::GetInstance().FindObjectsWithTag(GameTags::Enemy);
            
            for (auto otherCollider : collisions) {
                if (!otherCollider) continue;
                
                GameObject* otherObj = otherCollider->GetGameObject();
                if (!otherObj) continue;
                
                for (auto enemyObj : enemies) {
                    if (enemyObj == otherObj) {
                        Enemy* enemy = enemyObj->GetComponentOfType<Enemy>();
                        if (enemy) {
                            enemy->TakeDamage(damage);
                            GetGameObject()->Destroy();
                            return;
                        }
                    }
                }
            }
        }
    }
};

// ============================================
// СИСТЕМА СТРЕЛЬБЫ
// ============================================
class ShootingSystem : public Component {
private:
    float shootCooldown = 0.15f;
    float timeSinceLastShot = 0.0f;
    Scene* scene = nullptr;
    SpriteRenderer* renderer = nullptr;

public:
    void SetScene(Scene* s) { scene = s; }
    void SetRenderer(SpriteRenderer* r) { renderer = r; }
    
    void Update() override {
        if (!GameState::gameRunning || !scene || !renderer) return;
        
        timeSinceLastShot += Time::DeltaTime();
        
        auto& input = InputSystem::GetInstance();
        bool shoot = input.GetKey(Keys::Space) || input.GetMouseButton(GLFW_MOUSE_BUTTON_LEFT);
        
        if (shoot && timeSinceLastShot >= shootCooldown) {
            timeSinceLastShot = 0.0f;
            
            Transform2D* playerTrans = GetGameObject()->GetComponentOfType<Transform2D>();
            if (!playerTrans) return;
            
            Vector2 toMouse = GameState::mousePosition - playerTrans->position;
            if (toMouse.magnitude() < 0.1f) {
                toMouse = Vector2(0.0f, 1.0f);
            } else {
                toMouse = toMouse.normalized();
            }
            
            GameObject* bullet = scene->CreateGameObject();
            
            auto bulletTrans = std::make_unique<Transform2D>();
            bulletTrans->position = playerTrans->position + toMouse * 0.3f;
            bulletTrans->scale = Vector2(0.05f, 0.05f);
            bullet->AddComponent(bulletTrans.release());
            
            auto bulletSprite = std::make_unique<Sprite>(*renderer, "Assets/textures/bullet_player.png");
            bulletSprite->SetSize(10.0f, 10.0f);
            bullet->AddComponent(bulletSprite.release());
            
            auto bulletComp = std::make_unique<Bullet>();
            bulletComp->SetDirection(toMouse);
            bulletComp->SetScene(scene);
            bullet->AddComponent(bulletComp.release());
            
            auto bulletRb = std::make_unique<RigidBody2D>(0.1f, 0.1f);
            bullet->AddComponent(bulletRb.release());
            
            bullet->Start();
        }
    }
};

// ============================================
// СИСТЕМА ВОЛН
// ============================================
class WaveSystem : public Component {
private:
    Scene& scene;
    SpriteRenderer& renderer;
    int baseEnemies = 3;
    float timeBetweenWaves = 3.0f;
    float waveTimer = 0.0f;
    bool waveActive = true;
    bool gameStarted = false;
    
    template<typename T>
    void SpawnEnemy(const char* texturePath, Vector2 scale, float mass = 1.0f, float drag = 0.5f) {
        GameObject* enemy = scene.CreateGameObject();
        
        float angle = (rand() % 360) * 3.14159f / 180.0f;
        float distance = 1.8f;
        Vector2 pos = Vector2(sin(angle) * distance, cos(angle) * distance);
        
        auto transform = std::make_unique<Transform2D>();
        transform->position = pos;
        transform->scale = scale;
        enemy->AddComponent(transform.release());
        
        auto sprite = std::make_unique<Sprite>(renderer, texturePath);
        sprite->SetSize(40.0f, 40.0f);
        enemy->AddComponent(sprite.release());
        
        T* enemyComp = new T();
        enemyComp->SetScene(&scene);
        enemy->AddComponent(enemyComp);
        
        auto rb = std::make_unique<RigidBody2D>(mass, drag);
        enemy->AddComponent(rb.release());
        
        enemy->Start();
    }
    
public:
    WaveSystem(Scene& s, SpriteRenderer& r) : scene(s), renderer(r) {}
    
    void Update() override {
        if (!GameState::gameRunning) return;
        
        if (!gameStarted) {
            gameStarted = true;
            GameState::waveNumber = 1;
            
            for (int i = 0; i < baseEnemies; i++) {
                SpawnEnemy<Enemy>("Assets/textures/enemy_normal.png", Vector2(0.12f, 0.12f));
            }
            GameState::enemiesRemaining = baseEnemies;
            return;
        }
        
        auto enemies = TagManager::GetInstance().FindObjectsWithTag(GameTags::Enemy);
        GameState::enemiesRemaining = enemies.size();
        
        if (waveActive && enemies.empty()) {
            waveActive = false;
            waveTimer = timeBetweenWaves;
            GameState::waveNumber++;
        }
        
        if (!waveActive) {
            waveTimer -= Time::DeltaTime();
            
            if (waveTimer <= 0) {
                waveActive = true;
                int wave = GameState::waveNumber;
                
                int normalCount = 2 + wave;
                int fastCount = (wave >= 2) ? 1 + (wave - 1) / 2 : 0;
                int tankCount = (wave >= 3) ? 1 + (wave - 2) / 3 : 0;
                int shooterCount = (wave >= 4) ? 1 + (wave - 3) / 2 : 0;
                int bomberCount = (wave >= 5) ? 1 + (wave - 4) / 3 : 0;
                int healerCount = (wave >= 6) ? 1 + (wave - 5) / 4 : 0;
                
                for (int i = 0; i < normalCount; i++) {
                    SpawnEnemy<Enemy>("Assets/textures/enemy_normal.png", Vector2(0.12f, 0.12f));
                }
                for (int i = 0; i < fastCount; i++) {
                    SpawnEnemy<FastEnemy>("Assets/textures/enemy_fast.png", Vector2(0.1f, 0.1f), 0.8f, 0.3f);
                }
                for (int i = 0; i < tankCount; i++) {
                    SpawnEnemy<TankEnemy>("Assets/textures/enemy_tank.png", Vector2(0.18f, 0.18f), 2.0f, 0.8f);
                }
                for (int i = 0; i < shooterCount; i++) {
                    SpawnEnemy<ShooterEnemy>("Assets/textures/enemy_shooter.png", Vector2(0.14f, 0.14f));
                }
                for (int i = 0; i < bomberCount; i++) {
                    SpawnEnemy<BomberEnemy>("Assets/textures/enemy_bomber.png", Vector2(0.13f, 0.13f), 1.2f, 0.5f);
                }
                for (int i = 0; i < healerCount; i++) {
                    SpawnEnemy<HealerEnemy>("Assets/textures/enemy_healer.png", Vector2(0.13f, 0.13f));
                }
                
                int total = normalCount + fastCount + tankCount + shooterCount + bomberCount + healerCount;
                GameState::enemiesRemaining = total;
            }
        }
    }
};

// ============================================
// СИСТЕМА КОЛЛИЗИЙ
// ============================================
class CollisionSystem : public Component {
private:
    ColliderManager* colliderManager = nullptr;

public:
    void Start() override {
        colliderManager = new ColliderManager();
        GameState::colliderManager = colliderManager;
    }
    
    void Update() override {
        if (colliderManager && GameState::gameRunning) {
            std::cout << "colliderManager STARTED" << std::endl;
            colliderManager->CheckCollisions();
            std::cout << "colliderManager COMPLITED" << std::endl;
        }
    }
    
    void Destroy() override {
        delete colliderManager;
        colliderManager = nullptr;
        GameState::colliderManager = nullptr;
    }
};

// ============================================
// HUD
// ============================================
class SimpleHUD : public Component {
public:
    void RenderImGui() {
        ImGui::SetNextWindowPos(ImVec2(10, 10));
        ImGui::SetNextWindowSize(ImVec2(200, 100));
        ImGui::Begin("HUD", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
        
        ImGui::Text("Health: %d", GameState::playerHealth);
        ImGui::ProgressBar((float)GameState::playerHealth / 100.0f, ImVec2(180, 20));
        ImGui::Text("Score: %d", GameState::score);
        ImGui::Text("Wave: %d", GameState::waveNumber);
        ImGui::Text("Enemies: %d", GameState::enemiesRemaining);
        ImGui::Text("Kills: %d", GameState::totalKills);
        
        if (!GameState::gameRunning) {
            ImGui::TextColored(ImVec4(1,0,0,1), "GAME OVER");
            ImGui::Text("Final Score: %d", GameState::score);
            ImGui::Text("Waves Survived: %d", GameState::waveNumber);
        }
        
        ImGui::End();
    }
};

// ============================================
// MAIN
// ============================================
int main() {
    std::cout << "=== ARENA SURVIVAL (NEW ARCH) ===" << std::endl;
    std::cout << "STEP 0: Program started" << std::endl;
    
    srand(time(nullptr));
    
    // 1. Инициализация графики
    std::cout << "\nSTEP 1: Creating GraphicsManager..." << std::endl;
    auto graphics = std::make_unique<GraphicsManager>();
    std::cout << "GraphicsManager created: " << graphics.get() << std::endl;
    
    std::cout << "\nSTEP 2: Creating DrawingManager..." << std::endl;
    float bgColor[4] = {0.05f, 0.05f, 0.1f, 1.0f};
    auto* drawing = new DrawingManager(SCREEN_WIDTH, SCREEN_HEIGHT, bgColor, "ARENA SURVIVAL");
    std::cout << "DrawingManager created: " << drawing << std::endl;
    
    std::cout << "\nSTEP 3: Initializing DrawingManager..." << std::endl;
    if (!drawing->Initialize()) {
        std::cerr << "Failed to initialize window!" << std::endl;
        return -1;
    }
    std::cout << "DrawingManager initialized successfully" << std::endl;
    
    std::cout << "\nSTEP 4: Setting window and camera..." << std::endl;
    graphics->SetWindow(drawing);
    std::cout << "Window set" << std::endl;
    
    graphics->SetCamera(new Camera2D(SCREEN_WIDTH, SCREEN_HEIGHT));
    std::cout << "Camera set" << std::endl;
    
    std::cout << "\nSTEP 5: Creating sprite renderer..." << std::endl;
    auto* spriteRenderer = graphics->AddRender<SpriteRenderer>();
    if (!spriteRenderer) {
        std::cerr << "Failed to create sprite renderer!" << std::endl;
        return -1;
    }
    std::cout << "Sprite renderer created: " << spriteRenderer << std::endl;
    
    std::cout << "\nSTEP 6: Getting GLFW window..." << std::endl;
    GLFWwindow* window = drawing->GetWindow();
    if (!window) {
        std::cerr << "Failed to get GLFW window!" << std::endl;
        return -1;
    }
    std::cout << "GLFW window obtained: " << window << std::endl;
    
    std::cout << "\nSTEP 7: Initializing ImGui..." << std::endl;
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::StyleColorsDark();
    
    if (!ImGui_ImplGlfw_InitForOpenGL(window, true)) {
        std::cerr << "Failed to init ImGui GLFW!" << std::endl;
        return -1;
    }
    std::cout << "ImGui GLFW initialized" << std::endl;
    
    if (!ImGui_ImplOpenGL3_Init("#version 130")) {
        std::cerr << "Failed to init ImGui OpenGL3!" << std::endl;
        return -1;
    }
    std::cout << "ImGui OpenGL3 initialized" << std::endl;

    RenderSettings* renderSettings = graphics->GetRenderSettings();
    if (renderSettings) {
        renderSettings->SetGlobalScale(1.0f);
        renderSettings->SetReferenceResolution(1920.0f, 1080.0f);
        renderSettings->SetCurrentResolution(SCREEN_WIDTH, SCREEN_HEIGHT);
        renderSettings->SetMaintainAspectRatio(true);
        renderSettings->SetBrightness(1.0f);
        renderSettings->SetContrast(1.0f);
        renderSettings->SetOffset(glm::vec2(0.0f, 0.0f));
        
        std::cout << "RenderSettings configured" << std::endl;
    }

    graphics->ApplyRenderSettingsToCamera();
    
    std::cout << "\nSTEP 8: Initializing systems..." << std::endl;
    Time::Initialize();
    std::cout << "Time initialized" << std::endl;
    
    InputSystem::GetInstance().Initialize(window);
    std::cout << "InputSystem initialized" << std::endl;
    
    std::cout << "\nSTEP 9: Loading resources..." << std::endl;
    auto& rm = ResourceManager::GetInstance();
    
    rm.LoadResource("Assets/textures/player.png", ResourceType::Texture);
    rm.LoadResource("Assets/textures/enemy_normal.png", ResourceType::Texture);
    rm.LoadResource("Assets/textures/enemy_fast.png", ResourceType::Texture);
    rm.LoadResource("Assets/textures/enemy_tank.png", ResourceType::Texture);
    rm.LoadResource("Assets/textures/enemy_shooter.png", ResourceType::Texture);
    rm.LoadResource("Assets/textures/enemy_bomber.png", ResourceType::Texture);
    rm.LoadResource("Assets/textures/enemy_healer.png", ResourceType::Texture);
    rm.LoadResource("Assets/textures/bullet_player.png", ResourceType::Texture);
    rm.LoadResource("Assets/textures/enemy_bullet.png", ResourceType::Texture);
    std::cout << "All textures loaded" << std::endl;
    
    std::cout << "\nSTEP 10: Creating scene..." << std::endl;
    auto scene = std::make_unique<Scene>("GameScene");
    if (!scene) {
        std::cerr << "Failed to create scene!" << std::endl;
        return -1;
    }
    GameState::currentScene = scene.get();
    std::cout << "Scene created: " << scene.get() << std::endl;
    
    std::cout << "\nSTEP 11: Creating game objects..." << std::endl;
    
    // CollisionSystem
    std::cout << "Creating CollisionSystem..." << std::endl;
    auto collisionObj = std::make_unique<GameObject>();
    collisionObj->AddComponent(new CollisionSystem());
    scene->AddGameObject(std::move(collisionObj));
    std::cout << "CollisionSystem added" << std::endl;
    
    // HUD
    std::cout << "Creating HUD..." << std::endl;
    auto hudObj = std::make_unique<GameObject>();
    hudObj->AddComponent(new SimpleHUD());
    scene->AddGameObject(std::move(hudObj));
    std::cout << "HUD added" << std::endl;
    
    // WaveSystem
    std::cout << "Creating WaveSystem..." << std::endl;
    auto wavesObj = std::make_unique<GameObject>();
    wavesObj->AddComponent(new WaveSystem(*scene, *spriteRenderer));
    scene->AddGameObject(std::move(wavesObj));
    std::cout << "WaveSystem added" << std::endl;
    
    // Player
    std::cout << "Creating Player..." << std::endl;
    GameObject* playerObj = scene->CreateGameObject();
    if (!playerObj) {
        std::cerr << "Failed to create player object!" << std::endl;
        return -1;
    }
    
    auto playerTrans = std::make_unique<Transform2D>();
    playerTrans->position = Vector2(0, 0);
    playerTrans->scale = Vector2(0.15f, 0.15f);
    playerObj->AddComponent(playerTrans.release());
    
    auto playerSprite = std::make_unique<Sprite>(*spriteRenderer, "Assets/textures/player.png");
    playerSprite->SetSize(30.0f, 30.0f);
    playerObj->AddComponent(playerSprite.release());
    
    playerObj->AddComponent(new Player());
    
    auto shootingSystem = new ShootingSystem();
    shootingSystem->SetScene(scene.get());
    shootingSystem->SetRenderer(spriteRenderer);
    playerObj->AddComponent(shootingSystem);
    
    auto playerRb = std::make_unique<RigidBody2D>(1.0f, 0.5f);
    playerObj->AddComponent(playerRb.release());
    
    playerObj->Start();
    std::cout << "Player created and started" << std::endl;
    
    // Создаем несколько тестовых врагов для отладки
    std::cout << "Creating test enemies..." << std::endl;
    for (int i = 0; i < 3; i++) {
        float angle = (i * 120) * 3.14159f / 180.0f;
        float distance = 1.5f;
        Vector2 pos = Vector2(sin(angle) * distance, cos(angle) * distance);
        
        GameObject* enemy = scene->CreateGameObject();
        
        auto enemyTrans = std::make_unique<Transform2D>();
        enemyTrans->position = pos;
        enemyTrans->scale = Vector2(0.12f, 0.12f);
        enemy->AddComponent(enemyTrans.release());
        
        auto enemySprite = std::make_unique<Sprite>(*spriteRenderer, "Assets/textures/enemy_normal.png");
        enemySprite->SetSize(40.0f, 40.0f);
        enemy->AddComponent(enemySprite.release());
        
        Enemy* enemyComp = new Enemy();
        enemyComp->SetScene(scene.get());
        enemy->AddComponent(enemyComp);
        
        auto enemyRb = std::make_unique<RigidBody2D>(1.0f, 0.5f);
        enemy->AddComponent(enemyRb.release());
        
        enemy->Start();
    }
    GameState::enemiesRemaining = 3;
    GameState::waveNumber = 1;
    
    std::cout << "\nSTEP 12: Entering main loop..." << std::endl;
    
    int frameCount = 0;
    auto lastTime = std::chrono::steady_clock::now();
    bool firstFrame = true;
    
    // Главный игровой цикл
    while (!drawing->ShouldClose()) {
        if (firstFrame) {
            std::cout << "First frame starting..." << std::endl;
            firstFrame = false;
        }
        
        frameCount++;
        auto currentTime = std::chrono::steady_clock::now();
        float dt = std::chrono::duration<float>(currentTime - lastTime).count();
        
        if (dt >= 1.0f) {
            std::cout << "FPS: " << frameCount 
                     << " | Health: " << GameState::playerHealth
                     << " | Enemies: " << GameState::enemiesRemaining 
                     << " | Wave: " << GameState::waveNumber 
                     << " | Score: " << GameState::score << std::endl;
            frameCount = 0;
            lastTime = currentTime;
        }
        
        // Обновление времени и ввода
        Time::Tick();
        InputSystem::GetInstance().Update();
        
        // Получаем позицию мыши
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);
        GameState::mousePosition = ScreenToWorld(Vector2((float)mouseX, (float)mouseY), SCREEN_WIDTH, SCREEN_HEIGHT);
        
        // Очистка экрана
        drawing->Clear();
        
        std::cout << "RENDERING STARTED" << std::endl;
        // ВАЖНО: Сначала рендерим (сбор данных для отрисовки)
        graphics->Update();
        std::cout << "RENDERING COMPLITED" << std::endl;
        
        std::cout << "UpdateScene STARTED" << std::endl;
        // ПОТОМ обновляем логику (здесь могут удаляться объекты)
        scene->UpdateScene();
        std::cout << "UpdateScene COMPLITED" << std::endl;
        
        std::cout << "ImGui STARTED" << std::endl;
        // Рендеринг ImGui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        auto hudObjs = scene->FindGameObjectsWithComponent<SimpleHUD>();
        if (!hudObjs.empty()) {
            auto* hudComp = hudObjs[0]->GetComponentOfType<SimpleHUD>();
            if (hudComp) {
                hudComp->RenderImGui();
            }
        }
        
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        // Смена буферов и обработка событий
        drawing->SwapBuffers();
        drawing->PollEvents();

        std::cout << "ImGui COMPLITED" << std::endl;
        
        // Небольшая задержка для снижения нагрузки на CPU
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    
    std::cout << "\nSTEP FINAL: Shutting down..." << std::endl;
    
    // Очистка ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    
    // Очистка ресурсов
    ResourceManager::GetInstance().ClearAll();
    TagManager::DestroyInstance();
    
    std::cout << "Final Score: " << GameState::score << std::endl;
    std::cout << "=== PROGRAM ENDED ===" << std::endl;
    
    return 0;
}