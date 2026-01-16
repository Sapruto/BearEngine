#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <cmath>

#include "include/core/GameObject.h"
#include "include/core/Component.h"
#include "include/core/math/Vector2.h"
#include "include/core/graphics_engine/DrawingWindow.h"
#include "include/core/math/Transform2D.h"
#include "include/core/graphics_engine/Render.h"
#include "include/core/phisic_engine/Rigidbody2D.h"
#include "include/core/phisic_engine/BoxCollider2D.h"

// === КОНСТАНТЫ ===
const float ARENA_WIDTH = 2.4f;
const float ARENA_HEIGHT = 2.4f;
const Vector2 GRAVITY(0.0f, 0.0f);
const float FIXED_TIMESTEP = 1.0f / 60.0f;

// === ГЛОБАЛЬНЫЕ ПЕРЕМЕННЫЕ ===
std::vector<GameObject*> gameObjects;
std::vector<GameObject*> objectsToDestroy;

// Переменные для игры
int playerHealth = 100;
int score = 0;
float playerDamageCooldown = 0.0f;

// Позиция мыши
Vector2 mousePosition(0.0f, 0.0f);

// === ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ ===
bool CheckCollision(const Vector2& pos1, const Vector2& size1, 
                    const Vector2& pos2, const Vector2& size2) {
    Vector2 min1 = pos1 - size1 * 0.5f;
    Vector2 max1 = pos1 + size1 * 0.5f;
    Vector2 min2 = pos2 - size2 * 0.5f;
    Vector2 max2 = pos2 + size2 * 0.5f;
    
    return (max1.x > min2.x && min1.x < max2.x &&
            max1.y > min2.y && min1.y < max2.y);
}

// Функция для преобразования координат мыши в мировые координаты
Vector2 ScreenToWorld(GLFWwindow* window, const Vector2& screenPos) {
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    
    // Преобразуем координаты экрана в нормализованные [-1, 1]
    float x = (screenPos.x / width) * 2.0f - 1.0f;
    float y = 1.0f - (screenPos.y / height) * 2.0f; // Инвертируем Y
    
    // Преобразуем в мировые координаты (учитываем размер арены)
    return Vector2(x * (ARENA_WIDTH / 2.0f), y * (ARENA_HEIGHT / 2.0f));
}

// === КОМПОНЕНТ ГРАНИЦ ===
class ArenaBounds : public Component {
public:
    void Update(float deltaTime) override {
        Transform2D* transform = GetGameObject()->GetComponentOfType<Transform2D>();
        RigidBody2D* rb = GetGameObject()->GetComponentOfType<RigidBody2D>();
        
        if (!transform || !rb) return;
        
        BoxCollider2D* collider = GetGameObject()->GetComponentOfType<BoxCollider2D>();
        float halfWidth = collider ? collider->GetSize().x * 0.5f : 0.05f;
        float halfHeight = collider ? collider->GetSize().y * 0.5f : 0.05f;
        
        bool collision = false;
        Vector2 newVel = rb->GetVelocity();
        Vector2 pos = transform->position;
        
        // Левая граница
        if (pos.x - halfWidth < -ARENA_WIDTH/2) {
            transform->position.x = -ARENA_WIDTH/2 + halfWidth;
            newVel.x = std::abs(newVel.x) * 0.3f;
            collision = true;
        }
        
        // Правая граница
        if (pos.x + halfWidth > ARENA_WIDTH/2) {
            transform->position.x = ARENA_WIDTH/2 - halfWidth;
            newVel.x = -std::abs(newVel.x) * 0.3f;
            collision = true;
        }
        
        // Верхняя граница
        if (pos.y + halfHeight > ARENA_HEIGHT/2) {
            transform->position.y = ARENA_HEIGHT/2 - halfHeight;
            newVel.y = -std::abs(newVel.y) * 0.3f;
            collision = true;
        }
        
        // Нижняя граница (пол)
        if (pos.y - halfHeight < -ARENA_HEIGHT/2) {
            transform->position.y = -ARENA_HEIGHT/2 + halfHeight;
            newVel.y = std::abs(newVel.y) * 0.3f;
            collision = true;
        }
        
        if (collision) {
            rb->SetVelocity(newVel);
        }
    }
};

// === ИГРОК ===
class Player : public Component {
private:
    GLFWwindow* window;
    RigidBody2D* rb = nullptr;
    float moveForce = 8.0f;
    float jumpForce = 5.0f;
    bool canJump = false;
    float coyoteTimer = 0.0f;
    const float COYOTE_TIME = 0.15f;
    
public:
    Player(GLFWwindow* win) : window(win) {}
    
    void Start() override {
        rb = GetGameObject()->GetComponentOfType<RigidBody2D>();
        if (rb) {
            rb->SetMass(1.0f);
            rb->SetDrag(0.5f);
        }
    }
    
    void Update(float deltaTime) override {
        if (!rb) return;
        
        Transform2D* transform = GetGameObject()->GetComponentOfType<Transform2D>();
        if (!transform) return;
        
        // Проверка земли с койот-таймом
        if (transform->position.y <= -ARENA_HEIGHT/2 + 0.11f && 
            std::abs(rb->GetVelocity().y) < 0.2f) {
            canJump = true;
            coyoteTimer = COYOTE_TIME;
        } else {
            coyoteTimer -= deltaTime;
            if (coyoteTimer <= 0.0f) {
                canJump = false;
            }
        }
        
        // Движение влево/вправо
        Vector2 force(0, 0);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            force.x -= moveForce;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            force.x += moveForce;
        }
        
        // Ограничиваем горизонтальную скорость
        Vector2 vel = rb->GetVelocity();
        if (std::abs(vel.x) > 4.0f) {
            vel.x = (vel.x > 0) ? 4.0f : -4.0f;
            rb->SetVelocity(vel);
        }
        
        rb->AddForce(force);
        
        // Прыжок
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && canJump) {
            vel = rb->GetVelocity();
            vel.y = jumpForce;
            rb->SetVelocity(vel);
            canJump = false;
            coyoteTimer = 0.0f;
        }
        
        // Быстрое падение
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            if (vel.y > -10.0f) {
                rb->AddForce(Vector2(0, -15.0f));
            }
        }
    }
};

// === БАЗОВЫЙ ВРАГ ===
class BaseEnemy : public Component {
protected:
    GameObject* targetPlayer;
    RigidBody2D* rb = nullptr;
    float chaseForce;
    float detectionRange;
    float colorPulse = 0.0f;
    int health;
    int scoreValue;
    Vector2 color;
    
public:
    BaseEnemy(GameObject* player, float chaseForce, float detectionRange, 
              int health, int scoreValue, const Vector2& color)
        : targetPlayer(player), chaseForce(chaseForce), detectionRange(detectionRange),
          health(health), scoreValue(scoreValue), color(color) {}
    
    void Start() override {
        rb = GetGameObject()->GetComponentOfType<RigidBody2D>();
        if (rb) {
            rb->SetMass(1.0f);
            rb->SetDrag(0.8f);
        }
        
        TriangleRenderer* renderer = GetGameObject()->GetComponentOfType<TriangleRenderer>();
        if (renderer) {
            renderer->SetColor(color.x, color.y, 0.2f);
        }
    }
    
    void Update(float deltaTime) override {
        if (!rb || !targetPlayer) return;
        
        // Пульсация цвета
        colorPulse += deltaTime * 2.0f;
        TriangleRenderer* renderer = GetGameObject()->GetComponentOfType<TriangleRenderer>();
        if (renderer) {
            float pulse = (sin(colorPulse) + 1.0f) * 0.15f;
            renderer->SetColor(color.x + pulse, color.y + pulse, 0.2f);
        }
        
        // Преследование игрока
        Transform2D* transform = GetGameObject()->GetComponentOfType<Transform2D>();
        Transform2D* playerTransform = targetPlayer->GetComponentOfType<Transform2D>();
        
        if (transform && playerTransform) {
            Vector2 toPlayer = playerTransform->position - transform->position;
            float distance = toPlayer.magnitude();
            
            if (distance < detectionRange && distance > 0.1f) {
                Vector2 force = toPlayer.normalized() * chaseForce;
                
                // Избегание других врагов
                for (auto obj : gameObjects) {
                    if (obj == GetGameObject()) continue;
                    
                    BaseEnemy* other = obj->GetComponentOfType<BaseEnemy>();
                    if (other) {
                        Transform2D* otherTransform = obj->GetComponentOfType<Transform2D>();
                        if (otherTransform) {
                            Vector2 toOther = transform->position - otherTransform->position;
                            float otherDist = toOther.magnitude();
                            if (otherDist < 0.3f && otherDist > 0.01f) {
                                force += toOther.normalized() * 4.0f;
                            }
                        }
                    }
                }
                
                rb->AddForce(force);
            }
        }
    }
    
    void TakeDamage(int damage) {
        health -= damage;
        if (health <= 0) {
            objectsToDestroy.push_back(GetGameObject());
            score += scoreValue;
            std::cout << "Enemy destroyed! +" << scoreValue << " Score: " << score << std::endl;
        }
    }
    
    int GetHealth() const { return health; }
    GameObject* GetPlayer() const { return targetPlayer; }
};

// === ОБЫЧНЫЙ ВРАГ ===
class NormalEnemy : public BaseEnemy {
public:
    NormalEnemy(GameObject* player) 
        : BaseEnemy(player, 3.0f, 1.5f, 1, 100, Vector2(0.9f, 0.2f)) {}
};

// === БЫСТРЫЙ ВРАГ ===
class FastEnemy : public BaseEnemy {
public:
    FastEnemy(GameObject* player) 
        : BaseEnemy(player, 5.0f, 2.0f, 1, 150, Vector2(0.2f, 0.9f)) {}
    
    void Start() override {
        BaseEnemy::Start();
        // Быстрый враг легче и быстрее
        if (rb) {
            rb->SetMass(0.7f);
            rb->SetDrag(0.6f);
        }
        
        TriangleRenderer* renderer = GetGameObject()->GetComponentOfType<TriangleRenderer>();
        if (renderer) {
            renderer->SetColor(0.2f, 0.9f, 0.2f); // Зеленый для быстрого
        }
    }
};

// === ПУЛЯ ВРАГА ===
class EnemyBullet : public Component {
private:
    float lifetime = 2.0f;
    float timeAlive = 0.0f;
    Vector2 direction;
    int damage = 1;
    
public:
    EnemyBullet(Vector2 dir, int dmg = 1) : direction(dir), damage(dmg) {}
    
    void Start() override {
        RigidBody2D* rb = GetGameObject()->GetComponentOfType<RigidBody2D>();
        if (rb) {
            rb->SetMass(0.05f);
            rb->SetDrag(0.1f);
            rb->SetVelocity(direction * 8.0f); // Вражеские пули медленнее
        }
        
        TriangleRenderer* renderer = GetGameObject()->GetComponentOfType<TriangleRenderer>();
        if (renderer) {
            renderer->SetColor(1.0f, 0.2f, 0.2f); // Красный цвет
        }
    }
    
    void Update(float deltaTime) override {
        timeAlive += deltaTime;
        
        if (timeAlive >= lifetime) {
            objectsToDestroy.push_back(GetGameObject());
            return;
        }
        
        // Мерцание перед исчезновением
        if (timeAlive > lifetime * 0.7f) {
            TriangleRenderer* renderer = GetGameObject()->GetComponentOfType<TriangleRenderer>();
            if (renderer) {
                float alpha = 0.5f + 0.5f * sin(timeAlive * 10.0f);
                renderer->SetColor(1.0f * alpha, 0.2f * alpha, 0.2f * alpha);
            }
        }
    }
    
    int GetDamage() const { return damage; }
};

// === СТРЕЛЯЮЩИЙ ВРАГ ===
class ShootingEnemy : public BaseEnemy {
private:
    float shootCooldown = 2.0f;
    float shootTimer = 0.0f;
    GLFWwindow* window;
    
public:
    ShootingEnemy(GameObject* player, GLFWwindow* win) 
        : BaseEnemy(player, 2.0f, 2.5f, 2, 200, Vector2(0.9f, 0.9f)), window(win) {}
    
    void Start() override {
        BaseEnemy::Start();
        // Стреляющий враг медленнее
        if (rb) {
            rb->SetMass(1.2f);
            rb->SetDrag(0.9f);
        }
        
        TriangleRenderer* renderer = GetGameObject()->GetComponentOfType<TriangleRenderer>();
        if (renderer) {
            renderer->SetColor(0.9f, 0.9f, 0.2f); // Желтый для стреляющего
        }
    }
    
    void Update(float deltaTime) override {
        BaseEnemy::Update(deltaTime);
        
        shootTimer -= deltaTime;
        
        if (shootTimer <= 0.0f) {
            TryShoot();
            shootTimer = shootCooldown;
        }
    }
    
    void TryShoot() {
        Transform2D* transform = GetGameObject()->GetComponentOfType<Transform2D>();
        if (!transform) return;
        
        GameObject* player = GetPlayer();
        if (!player) return;
        
        Transform2D* playerTransform = player->GetComponentOfType<Transform2D>();
        if (!playerTransform) return;
        
        // Направление к игроку
        Vector2 direction = playerTransform->position - transform->position;
        if (direction.magnitude() < 0.1f) return;
        
        direction = direction.normalized();
        
        // Создаем вражескую пулю
        GameObject* bullet = new GameObject();
        
        Transform2D* bulletTransform = new Transform2D();
        bulletTransform->position = transform->position + direction * 0.15f;
        bulletTransform->scale = Vector2(0.025f, 0.04f);
        bullet->AddComponent(bulletTransform);
        
        TriangleRenderer* renderer = new TriangleRenderer(window);
        renderer->SetColor(1.0f, 0.2f, 0.2f); // Красные пули у врагов
        bullet->AddComponent(renderer);
        
        bullet->AddComponent(new EnemyBullet(direction, 1));
        
        RigidBody2D* rb = new RigidBody2D(0.05f, 0.1f);
        bullet->AddComponent(rb);
        
        BoxCollider2D* collider = new BoxCollider2D();
        collider->SetSize(0.025f, 0.04f);
        bullet->AddComponent(collider);
        
        bullet->AddComponent(new ArenaBounds());
        
        bullet->Start();
        gameObjects.push_back(bullet);
    }
};

// === ПУЛЯ ИГРОКА ===
class Bullet : public Component {
private:
    float lifetime = 1.5f;
    float timeAlive = 0.0f;
    Vector2 direction;
    int damage = 1;
    
public:
    Bullet(Vector2 dir, int dmg = 1) : direction(dir), damage(dmg) {}
    
    void Start() override {
        RigidBody2D* rb = GetGameObject()->GetComponentOfType<RigidBody2D>();
        if (rb) {
            rb->SetMass(0.05f);
            rb->SetDrag(0.1f);
            rb->SetVelocity(direction * 12.0f);
        }
        
        TriangleRenderer* renderer = GetGameObject()->GetComponentOfType<TriangleRenderer>();
        if (renderer) {
            renderer->SetColor(1.0f, 0.9f, 0.2f);
        }
    }
    
    void Update(float deltaTime) override {
        timeAlive += deltaTime;
        
        if (timeAlive >= lifetime) {
            objectsToDestroy.push_back(GetGameObject());
            return;
        }
        
        // Мерцание перед исчезновением
        if (timeAlive > lifetime * 0.7f) {
            TriangleRenderer* renderer = GetGameObject()->GetComponentOfType<TriangleRenderer>();
            if (renderer) {
                float alpha = 0.5f + 0.5f * sin(timeAlive * 10.0f);
                renderer->SetColor(1.0f, 0.9f * alpha, 0.2f * alpha);
            }
        }
    }
    
    int GetDamage() const { return damage; }
};

// === СИСТЕМА СТРЕЛЬБЫ С ПРИЦЕЛИВАНИЕМ ПО МЫШИ ===
class ShootingSystem : public Component {
private:
    GLFWwindow* window;
    float shootCooldown = 0.2f;
    float timeSinceLastShot = 0.0f;
    int bulletsInMagazine = 20;
    float reloadTime = 1.5f;
    bool isReloading = false;
    bool spaceWasPressed = false;
    bool mouseWasPressed = false;
    
public:
    ShootingSystem(GLFWwindow* win) : window(win) {}
    
    void Update(float deltaTime) override {
        timeSinceLastShot += deltaTime;
        
        // Перезарядка
        if (isReloading) {
            if (timeSinceLastShot >= reloadTime) {
                bulletsInMagazine = 20;
                isReloading = false;
                std::cout << "Reloaded! Bullets: " << bulletsInMagazine << std::endl;
            }
            return;
        }
        
        // Стрельба по SPACE
        bool spacePressed = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
        if (spacePressed && !spaceWasPressed && 
            timeSinceLastShot >= shootCooldown && 
            bulletsInMagazine > 0) {
            
            ShootAtMouse();
            bulletsInMagazine--;
            timeSinceLastShot = 0.0f;
            
            if (bulletsInMagazine <= 0) {
                isReloading = true;
                std::cout << "Reloading..." << std::endl;
            }
        }
        spaceWasPressed = spacePressed;
        
        // Стрельба по ЛКМ
        bool mousePressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
        if (mousePressed && !mouseWasPressed && 
            timeSinceLastShot >= shootCooldown && 
            bulletsInMagazine > 0) {
            
            ShootAtMouse();
            bulletsInMagazine--;
            timeSinceLastShot = 0.0f;
            
            if (bulletsInMagazine <= 0) {
                isReloading = true;
                std::cout << "Reloading..." << std::endl;
            }
        }
        mouseWasPressed = mousePressed;
        
        // Принудительная перезарядка по R
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && !isReloading && bulletsInMagazine < 20) {
            isReloading = true;
            timeSinceLastShot = 0.0f;
            std::cout << "Manual reload..." << std::endl;
        }
    }
    
    void ShootAtMouse() {
        Transform2D* transform = GetGameObject()->GetComponentOfType<Transform2D>();
        if (!transform) return;
        
        // Получаем позицию игрока
        Vector2 playerPos = transform->position;
        
        // Направление от игрока к курсору мыши
        Vector2 direction = mousePosition - playerPos;
        
        // Если мышка близко к игроку, стреляем вверх
        if (direction.magnitude() < 0.1f) {
            direction = Vector2(0.0f, 1.0f);
        } else {
            direction = direction.normalized();
        }
        
        // Небольшой разброс
        float spread = (rand() % 100 - 50) / 1000.0f;
        direction = Vector2(direction.x + spread * 0.3f, direction.y + spread * 0.3f).normalized();
        
        CreateBullet(playerPos, direction);
    }
    
    void CreateBullet(const Vector2& position, const Vector2& direction) {
        GameObject* bullet = new GameObject();
        
        // Трансформ
        Transform2D* bulletTransform = new Transform2D();
        bulletTransform->position = position + direction * 0.15f;
        bulletTransform->scale = Vector2(0.03f, 0.05f);
        bullet->AddComponent(bulletTransform);
        
        // Рендерер
        TriangleRenderer* renderer = new TriangleRenderer(window);
        renderer->SetColor(1.0f, 0.9f, 0.2f);
        bullet->AddComponent(renderer);
        
        // Компонент пули
        bullet->AddComponent(new Bullet(direction));
        
        // Физика
        RigidBody2D* rb = new RigidBody2D(0.05f, 0.1f);
        bullet->AddComponent(rb);
        
        // Коллайдер
        BoxCollider2D* collider = new BoxCollider2D();
        collider->SetSize(0.03f, 0.05f);
        bullet->AddComponent(collider);
        
        // Границы арены
        bullet->AddComponent(new ArenaBounds());
        
        bullet->Start();
        gameObjects.push_back(bullet);
    }
};

// === СИСТЕМА ВОЛН ===
class WaveSystem {
private:
    GameObject* playerObj;
    GLFWwindow* window;
    int baseEnemiesPerWave = 2;
    float waveCooldown = 3.0f;
    float waveTimer = 0.0f;
    bool waveActive = false;
    int waveNumber = 0;
    
public:
    WaveSystem(GameObject* player, GLFWwindow* win) : playerObj(player), window(win) {
        StartNextWave();
    }
    
    void Update(float deltaTime) {
        // Подсчет живых врагов
        int aliveEnemies = 0;
        for (auto obj : gameObjects) {
            if (obj->GetComponentOfType<BaseEnemy>()) {
                aliveEnemies++;
            }
        }
        
        if (waveActive && aliveEnemies == 0) {
            waveActive = false;
            waveTimer = waveCooldown;
            waveNumber++;
            
            std::cout << "=== WAVE " << waveNumber << " CLEARED! ===" << std::endl;
            std::cout << "Next wave in " << waveCooldown << " seconds..." << std::endl;
        }
        
        if (!waveActive) {
            waveTimer -= deltaTime;
            if (waveTimer <= 0.0f) {
                StartNextWave();
            }
        }
    }
    
    void StartNextWave() {
        if (waveActive) return;
        
        waveNumber = (waveNumber == 0) ? 1 : waveNumber + 1;
        int totalEnemies = baseEnemiesPerWave + waveNumber;
        
        std::cout << "\n=== WAVE " << waveNumber << " ===" << std::endl;
        std::cout << "Enemies: " << totalEnemies << std::endl;
        
        Transform2D* playerTransform = playerObj->GetComponentOfType<Transform2D>();
        Vector2 playerPos = playerTransform ? playerTransform->position : Vector2(0, 0);
        
        for (int i = 0; i < totalEnemies; i++) {
            float angle = (float)i / totalEnemies * 3.14159f * 2.0f;
            float distance = 1.5f + (waveNumber * 0.1f);
            Vector2 spawnPos(
                sin(angle) * distance,
                cos(angle) * distance
            );
            
            // Не спавнить слишком близко к игроку
            if ((spawnPos - playerPos).magnitude() < 0.8f) {
                spawnPos = spawnPos.normalized() * 1.2f;
            }
            
            // Выбор типа врага в зависимости от волны
            CreateEnemyByWave(spawnPos, waveNumber, i);
        }
        
        waveActive = true;
    }
    
    void CreateEnemyByWave(const Vector2& position, int waveNum, int enemyIndex) {
        GameObject* enemy = new GameObject();
        
        // Трансформ
        Transform2D* transform = new Transform2D();
        transform->position = position;
        
        BaseEnemy* enemyComponent = nullptr;
        
        // Первые 2 волны - только обычные враги
        if (waveNum <= 2) {
            transform->scale = Vector2(0.1f, 0.1f);
            enemyComponent = new NormalEnemy(playerObj);
        } 
        // Волны 3-5: добавляем быстрых врагов
        else if (waveNum <= 5) {
            if (enemyIndex % 3 == 0) { // Каждый третий - быстрый
                transform->scale = Vector2(0.09f, 0.09f);
                enemyComponent = new FastEnemy(playerObj);
            } else {
                transform->scale = Vector2(0.1f, 0.1f);
                enemyComponent = new NormalEnemy(playerObj);
            }
        } 
        // Волны 6+: добавляем стреляющих врагов
        else {
            if (enemyIndex % 4 == 0) { // Каждый четвертый - стреляющий
                transform->scale = Vector2(0.11f, 0.11f);
                enemyComponent = new ShootingEnemy(playerObj, window);
            } else if (enemyIndex % 3 == 0) { // Каждый третий - быстрый
                transform->scale = Vector2(0.09f, 0.09f);
                enemyComponent = new FastEnemy(playerObj);
            } else {
                transform->scale = Vector2(0.1f, 0.1f);
                enemyComponent = new NormalEnemy(playerObj);
            }
        }
        
        enemy->AddComponent(transform);
        
        // Рендерер
        TriangleRenderer* renderer = new TriangleRenderer(window);
        enemy->AddComponent(renderer);
        
        // Компонент врага
        enemy->AddComponent(enemyComponent);
        
        // Физика
        RigidBody2D* rb = new RigidBody2D(1.0f, 0.8f);
        enemy->AddComponent(rb);
        
        // Коллайдер
        BoxCollider2D* collider = new BoxCollider2D();
        collider->SetSize(transform->scale.x, transform->scale.y);
        enemy->AddComponent(collider);
        
        enemy->AddComponent(new ArenaBounds());
        
        enemy->Start();
        gameObjects.push_back(enemy);
    }
    
    int GetCurrentWave() const { return waveNumber; }
};

// === ФУНКЦИИ ДЛЯ ПРОВЕРКИ СТОЛКНОВЕНИЙ ===
void CheckBulletEnemyCollisions() {
    for (size_t i = 0; i < gameObjects.size(); i++) {
        GameObject* bulletObj = gameObjects[i];
        
        // Проверяем, является ли объект пулей игрока
        Bullet* playerBullet = nullptr;
        for (auto comp : bulletObj->GetComponents()) {
            playerBullet = dynamic_cast<Bullet*>(comp);
            if (playerBullet) break;
        }
        
        if (!playerBullet) continue;
        
        Transform2D* bulletTransform = bulletObj->GetComponentOfType<Transform2D>();
        BoxCollider2D* bulletCollider = bulletObj->GetComponentOfType<BoxCollider2D>();
        
        if (!bulletTransform || !bulletCollider) continue;
        
        // Проверяем столкновение с каждым врагом
        for (size_t j = 0; j < gameObjects.size(); j++) {
            if (i == j) continue;
            
            GameObject* enemyObj = gameObjects[j];
            BaseEnemy* enemy = enemyObj->GetComponentOfType<BaseEnemy>();
            if (!enemy) continue;
            
            Transform2D* enemyTransform = enemyObj->GetComponentOfType<Transform2D>();
            BoxCollider2D* enemyCollider = enemyObj->GetComponentOfType<BoxCollider2D>();
            
            if (!enemyTransform || !enemyCollider) continue;
            
            if (CheckCollision(bulletTransform->position, bulletCollider->GetSize(),
                              enemyTransform->position, enemyCollider->GetSize())) {
                // Наносим урон врагу
                enemy->TakeDamage(playerBullet->GetDamage());
                
                // Уничтожаем пулю
                objectsToDestroy.push_back(bulletObj);
                break;
            }
        }
    }
}

void CheckEnemyBulletPlayerCollisions(GameObject* playerObj) {
    if (playerDamageCooldown > 0.0f) {
        return;
    }
    
    Transform2D* playerTransform = playerObj->GetComponentOfType<Transform2D>();
    BoxCollider2D* playerCollider = playerObj->GetComponentOfType<BoxCollider2D>();
    
    if (!playerTransform || !playerCollider) return;
    
    for (size_t i = 0; i < gameObjects.size(); i++) {
        GameObject* bulletObj = gameObjects[i];
        
        // Проверяем, является ли объект вражеской пулей
        EnemyBullet* enemyBullet = nullptr;
        for (auto comp : bulletObj->GetComponents()) {
            enemyBullet = dynamic_cast<EnemyBullet*>(comp);
            if (enemyBullet) break;
        }
        
        if (!enemyBullet) continue;
        
        Transform2D* bulletTransform = bulletObj->GetComponentOfType<Transform2D>();
        BoxCollider2D* bulletCollider = bulletObj->GetComponentOfType<BoxCollider2D>();
        
        if (!bulletTransform || !bulletCollider) continue;
        
        if (CheckCollision(playerTransform->position, playerCollider->GetSize(),
                          bulletTransform->position, bulletCollider->GetSize())) {
            playerHealth -= enemyBullet->GetDamage();
            playerDamageCooldown = 0.5f;
            
            std::cout << "Hit by enemy bullet! Health: " << playerHealth << std::endl;
            
            // Отталкивание игрока
            RigidBody2D* playerRb = playerObj->GetComponentOfType<RigidBody2D>();
            if (playerRb && bulletTransform) {
                Vector2 dir = (playerTransform->position - bulletTransform->position).normalized();
                playerRb->AddImpulse(dir * 1.5f);
            }
            
            // Уничтожаем пулю
            objectsToDestroy.push_back(bulletObj);
            
            if (playerHealth <= 0) {
                std::cout << "GAME OVER!" << std::endl;
            }
            break;
        }
    }
}

void CheckPlayerEnemyCollisions(GameObject* playerObj) {
    if (playerDamageCooldown > 0.0f) {
        return;
    }
    
    Transform2D* playerTransform = playerObj->GetComponentOfType<Transform2D>();
    BoxCollider2D* playerCollider = playerObj->GetComponentOfType<BoxCollider2D>();
    
    if (!playerTransform || !playerCollider) return;
    
    for (auto enemyObj : gameObjects) {
        BaseEnemy* enemy = enemyObj->GetComponentOfType<BaseEnemy>();
        if (!enemy) continue;
        
        Transform2D* enemyTransform = enemyObj->GetComponentOfType<Transform2D>();
        BoxCollider2D* enemyCollider = enemyObj->GetComponentOfType<BoxCollider2D>();
        
        if (!enemyTransform || !enemyCollider) continue;
        
        if (CheckCollision(playerTransform->position, playerCollider->GetSize(),
                          enemyTransform->position, enemyCollider->GetSize())) {
            playerHealth -= 10;
            playerDamageCooldown = 1.0f;
            
            std::cout << "Player hit by enemy! Health: " << playerHealth << std::endl;
            
            // Отталкивание игрока
            RigidBody2D* playerRb = playerObj->GetComponentOfType<RigidBody2D>();
            if (playerRb) {
                Vector2 dir = (playerTransform->position - enemyTransform->position).normalized();
                playerRb->AddImpulse(dir * 2.0f);
            }
            
            if (playerHealth <= 0) {
                std::cout << "GAME OVER!" << std::endl;
            }
            break;
        }
    }
}

void CheckEnemyEnemyCollisions() {
    for (size_t i = 0; i < gameObjects.size(); i++) {
        GameObject* enemy1Obj = gameObjects[i];
        BaseEnemy* enemy1 = enemy1Obj->GetComponentOfType<BaseEnemy>();
        if (!enemy1) continue;
        
        Transform2D* transform1 = enemy1Obj->GetComponentOfType<Transform2D>();
        BoxCollider2D* collider1 = enemy1Obj->GetComponentOfType<BoxCollider2D>();
        
        if (!transform1 || !collider1) continue;
        
        for (size_t j = i + 1; j < gameObjects.size(); j++) {
            GameObject* enemy2Obj = gameObjects[j];
            BaseEnemy* enemy2 = enemy2Obj->GetComponentOfType<BaseEnemy>();
            if (!enemy2) continue;
            
            Transform2D* transform2 = enemy2Obj->GetComponentOfType<Transform2D>();
            BoxCollider2D* collider2 = enemy2Obj->GetComponentOfType<BoxCollider2D>();
            
            if (!transform2 || !collider2) continue;
            
            if (CheckCollision(transform1->position, collider1->GetSize(),
                              transform2->position, collider2->GetSize())) {
                // Разделяем врагов
                Vector2 dir = transform1->position - transform2->position;
                if (dir.magnitude() > 0.01f) {
                    dir = dir.normalized();
                    RigidBody2D* rb1 = enemy1Obj->GetComponentOfType<RigidBody2D>();
                    RigidBody2D* rb2 = enemy2Obj->GetComponentOfType<RigidBody2D>();
                    
                    if (rb1) rb1->AddImpulse(dir * 0.5f);
                    if (rb2) rb2->AddImpulse(-dir * 0.5f);
                }
            }
        }
    }
}

// === КОЛБЭК ДЛЯ МЫШИ ===
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    mousePosition = ScreenToWorld(window, Vector2((float)xpos, (float)ypos));
}

// === ГЛАВНАЯ ФУНКЦИЯ ===
int main() {
    srand(static_cast<unsigned>(time(nullptr)));
    
    float bgColor[4] = {0.05f, 0.05f, 0.1f, 1.0f};
    DrawingManager drawing(800, 600, bgColor, "ARENA SURVIVAL");
    
    if (!drawing.Initialize()) {
        std::cerr << "Failed to initialize window!" << std::endl;
        return -1;
    }
    
    GLFWwindow* window = drawing.GetWindow();
    
    // Регистрируем колбэк для мыши
    glfwSetCursorPosCallback(window, mouse_callback);
    
    // === СОЗДАНИЕ ИГРОКА ===
    GameObject* player = new GameObject();
    
    Transform2D* playerTransform = new Transform2D();
    playerTransform->position = Vector2(0.0f, -0.8f);
    playerTransform->scale = Vector2(0.1f, 0.1f);
    player->AddComponent(playerTransform);
    
    TriangleRenderer* playerRenderer = new TriangleRenderer(window);
    playerRenderer->SetColor(0.2f, 0.8f, 0.2f);
    player->AddComponent(playerRenderer);
    
    player->AddComponent(new Player(window));
    player->AddComponent(new ShootingSystem(window));
    
    RigidBody2D* playerRb = new RigidBody2D(1.0f, 0.7f);
    player->AddComponent(playerRb);
    
    BoxCollider2D* playerCollider = new BoxCollider2D();
    playerCollider->SetSize(0.1f, 0.1f);
    player->AddComponent(playerCollider);
    
    player->AddComponent(new ArenaBounds());
    
    player->Start();
    gameObjects.push_back(player);
    
    // === СИСТЕМА ВОЛН ===
    WaveSystem waveSystem(player, window);
    
    // === ИНИЦИАЛИЗАЦИЯ ===
    std::cout << "=== ARENA SURVIVAL ===" << std::endl;
    std::cout << "Controls:" << std::endl;
    std::cout << "A/D - Move left/right" << std::endl;
    std::cout << "W - Jump" << std::endl;
    std::cout << "S - Fast fall" << std::endl;
    std::cout << "SPACE or LMB - Shoot at mouse cursor" << std::endl;
    std::cout << "R - Reload" << std::endl;
    std::cout << "ESC - Exit" << std::endl;
    std::cout << "=====================" << std::endl;
    std::cout << "Enemy types:" << std::endl;
    std::cout << "- Red: Normal (100 points)" << std::endl;
    std::cout << "- Green: Fast (150 points)" << std::endl;
    std::cout << "- Yellow: Shooting (200 points)" << std::endl;
    std::cout << "=====================" << std::endl;
    
    // === ГЛАВНЫЙ ЦИКЛ ===
    float lastTime = static_cast<float>(glfwGetTime());
    float accumulator = 0.0f;
    
    while (!drawing.ShouldClose() && playerHealth > 0) {
        // Время
        float currentTime = static_cast<float>(glfwGetTime());
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        
        if (deltaTime > 0.1f) deltaTime = 0.1f;
        accumulator += deltaTime;
        
        // Обновление кулдауна урона
        if (playerDamageCooldown > 0.0f) {
            playerDamageCooldown -= deltaTime;
            if (playerDamageCooldown < 0.0f) playerDamageCooldown = 0.0f;
        }
        
        // Обработка ввода
        drawing.PollEvents();
        
        // Очищаем экран
        drawing.Clear();
        
        // Фиксированный шаг обновления логики
        while (accumulator >= FIXED_TIMESTEP) {
            // 1. Обновление всех объектов
            for (auto obj : gameObjects) {
                if (obj) {
                    obj->Update(FIXED_TIMESTEP);
                }
            }
            
            // 2. Проверка всех столкновений
            CheckBulletEnemyCollisions();
            CheckEnemyBulletPlayerCollisions(player);
            CheckPlayerEnemyCollisions(player);
            CheckEnemyEnemyCollisions();
            
            // 3. Обновление системы волн
            waveSystem.Update(FIXED_TIMESTEP);
            
            accumulator -= FIXED_TIMESTEP;
        }
        
        // Удаление объектов
        if (!objectsToDestroy.empty()) {
            for (auto obj : objectsToDestroy) {
                auto it = std::find(gameObjects.begin(), gameObjects.end(), obj);
                if (it != gameObjects.end()) {
                    delete *it;
                    gameObjects.erase(it);
                }
            }
            objectsToDestroy.clear();
        }
        
        // SwapBuffers
        drawing.SwapBuffers();
        
        // Ограничение FPS
        float frameTime = static_cast<float>(glfwGetTime()) - currentTime;
        if (frameTime < 1.0f / 60.0f) {
            float sleepTime = (1.0f / 60.0f) - frameTime;
            if (sleepTime > 0) {
                float start = static_cast<float>(glfwGetTime());
                while (static_cast<float>(glfwGetTime()) - start < sleepTime) {
                    // Задержка
                }
            }
        }
        
        // Выход по ESC
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            break;
        }
    }
    
    // === ЗАВЕРШЕНИЕ ИГРЫ ===
    std::cout << "\n=== GAME OVER ===" << std::endl;
    std::cout << "Final Score: " << score << std::endl;
    std::cout << "Waves survived: " << waveSystem.GetCurrentWave() - 1 << std::endl;
    
    // Очистка памяти
    for (auto obj : gameObjects) {
        delete obj;
    }
    gameObjects.clear();
    
    return 0;
}