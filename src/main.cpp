#include "core/GameObject.h"
#include "core/math/Vector2.h"
#include "core/math/Transform2D.h"
#include "core/graphics_engine/DrawingWindow.h"
#include "core/graphics_engine/Render.h"
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>

class Player : public Component {
private:
    GLFWwindow* window;
    float speed = 0.8f;
    
public:
    Player(GLFWwindow* win) : window(win) {}
    
    void Update(float deltaTime) override {
        Transform2D* transform = gameObject->GetComponentOfType<Transform2D>();
        if (!transform) return;
        
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) transform->position.x -= speed * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) transform->position.x += speed * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) transform->position.y += speed * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) transform->position.y -= speed * deltaTime;
        
        if (transform->position.x < -1.0f) transform->position.x = -1.0f;
        if (transform->position.x > 1.0f) transform->position.x = 1.0f;
        if (transform->position.y < -1.0f) transform->position.y = -1.0f;
        if (transform->position.y > 1.0f) transform->position.y = 1.0f;
    }
};

class Enemy : public Component {
private:
    float time = 0;
    Vector2 startPos;
    float amplitude = 0.3f;
    float speed = 1.5f;
    
public:
    void Start() override {
        Transform2D* transform = gameObject->GetComponentOfType<Transform2D>();
        if (transform) startPos = transform->position;
    }
    
    void Update(float deltaTime) override {
        Transform2D* transform = gameObject->GetComponentOfType<Transform2D>();
        if (!transform) return;
        
        time += deltaTime;
        transform->position.x = startPos.x + sinf(time * speed) * amplitude;
        transform->rotation = time * 2.0f;
    }
};

class Bullet : public Component {
private:
    Vector2 direction;
    float speed = 2.0f;
    float lifetime = 3.0f;
    float timeAlive = 0;
    
public:
    Bullet(Vector2 dir) : direction(dir.normalized()) {}
    
    void Update(float deltaTime) override {
        Transform2D* transform = gameObject->GetComponentOfType<Transform2D>();
        if (!transform) return;
        
        transform->position += direction * speed * deltaTime;
        timeAlive += deltaTime;
        
        if (timeAlive > lifetime) {
            return;  // Просто возвращаемся, не удаляем
        }
        
        if (transform->position.x < -1.2f || transform->position.x > 1.2f ||
            transform->position.y < -1.2f || transform->position.y > 1.2f) {
            return;  // Просто возвращаемся, не удаляем
        }
    }
};

class Shooting : public Component {
private:
    GLFWwindow* window;
    float shootCooldown = 0.3f;
    float timeSinceLastShot = 0;
    std::vector<GameObject*> bullets;
    
public:
    Shooting(GLFWwindow* win) : window(win) {}
    
    void Update(float deltaTime) override {
        timeSinceLastShot += deltaTime;
        
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && timeSinceLastShot >= shootCooldown) {
            timeSinceLastShot = 0;
            
            Transform2D* playerTransform = gameObject->GetComponentOfType<Transform2D>();
            if (!playerTransform) return;
            
            Vector2 mousePos = GetMouseWorldPos();
            Vector2 direction = (mousePos - playerTransform->position).normalized();
            
            ShootBullet(playerTransform->position, direction);
        }
    }
    
    Vector2 GetMouseWorldPos() {
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        
        return Vector2(
            (x / width) * 2.0f - 1.0f,
            -((y / height) * 2.0f - 1.0f)
        );
    }
    
    void ShootBullet(Vector2 position, Vector2 direction) {
        GameObject* bullet = new GameObject();
        Transform2D* bulletTransform = new Transform2D();
        bulletTransform->position = position;
        bulletTransform->scale = Vector2(0.03f, 0.03f);
        bullet->AddComponent(bulletTransform);
        
        TriangleRenderer* bulletRenderer = new TriangleRenderer(window);
        bulletRenderer->SetColor(1.0f, 1.0f, 0.0f);
        bullet->AddComponent(bulletRenderer);
        
        bullet->AddComponent(new Bullet(direction));
        bullet->Start();
        
        bullets.push_back(bullet);
    }
    
    void UpdateBullets(float deltaTime) {
        for (auto& bullet : bullets) {
            if (bullet) bullet->Update(deltaTime);
        }
    }
    
    std::vector<GameObject*>& GetBullets() { return bullets; }
    
    // Новый метод для безопасного удаления пули
    void MarkBulletForDeletion(GameObject* bullet) {
        for (auto& b : bullets) {
            if (b == bullet) {
                delete b;
                b = nullptr;
                break;
            }
        }
    }
    
    // Метод для очистки nullptr из вектора
    void CleanupBullets() {
        bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
            [](GameObject* obj) { return !obj; }), bullets.end());
    }
};

int main() {
    std::srand(std::time(nullptr));
    float bgColor[4] = {0.0f, 0.0f, 0.1f, 1.0f};
    DrawingManager drawing(800, 600, bgColor, "SPACE SHOOTER");
    
    if (!drawing.Initialize()) return -1;
    
    GLFWwindow* window = drawing.GetWindow();
    
    GameObject* player = new GameObject();
    Transform2D* playerTransform = new Transform2D();
    playerTransform->position = Vector2(0, -0.7f);
    playerTransform->scale = Vector2(0.08f, 0.08f);
    player->AddComponent(playerTransform);
    
    TriangleRenderer* playerRenderer = new TriangleRenderer(window);
    playerRenderer->SetColor(0.0f, 1.0f, 0.0f);
    player->AddComponent(playerRenderer);
    
    player->AddComponent(new Player(window));
    Shooting* shooting = new Shooting(window);
    player->AddComponent(shooting);
    
    std::vector<GameObject*> enemies;
    for (int i = 0; i < 8; i++) {
        GameObject* enemy = new GameObject();
        
        float x = (rand() % 1400 - 700) / 1000.0f;
        float y = 0.8f;
        Transform2D* enemyTransform = new Transform2D();
        enemyTransform->position = Vector2(x, y);
        enemyTransform->scale = Vector2(0.07f, 0.07f);
        enemy->AddComponent(enemyTransform);
        
        TriangleRenderer* enemyRenderer = new TriangleRenderer(window);
        enemyRenderer->SetColor(1.0f, 0.2f, 0.2f);
        enemy->AddComponent(enemyRenderer);
        
        enemy->AddComponent(new Enemy());
        enemies.push_back(enemy);
    }
    
    player->Start();
    for (auto enemy : enemies) enemy->Start();
    
    int score = 0;
    int health = 100;
    
    std::cout << "SPACE SHOOTER" << std::endl;
    std::cout << "WASD - Move, SPACE - Shoot, ESC - Exit" << std::endl;
    std::cout << "Shoot red triangles, avoid collisions!" << std::endl;
    
    float lastTime = 0;
    while (!drawing.ShouldClose()) {
        float currentTime = glfwGetTime();
        float deltaTime = currentTime - lastTime;
        if (deltaTime > 0.033f) deltaTime = 0.033f;
        lastTime = currentTime;
        
        drawing.Clear();
        glfwMakeContextCurrent(window);
        
        player->Update(deltaTime);
        shooting->UpdateBullets(deltaTime);
        
        // Удаляем старые пули (которые вышли за границы или устарели)
        auto& bullets = shooting->GetBullets();
        for (auto& bullet : bullets) {
            if (!bullet) continue;
            
            Transform2D* bulletTransform = bullet->GetComponentOfType<Transform2D>();
            if (!bulletTransform) continue;
            
            // Проверяем время жизни
            Bullet* bulletComp = bullet->GetComponentOfType<Bullet>();
            if (bulletComp) {
                // Проверяем вылет за границы
                if (bulletTransform->position.x < -1.2f || bulletTransform->position.x > 1.2f ||
                    bulletTransform->position.y < -1.2f || bulletTransform->position.y > 1.2f) {
                    shooting->MarkBulletForDeletion(bullet);
                    continue;
                }
            }
        }
        
        shooting->CleanupBullets();
        
        // Проверяем столкновения врагов
        for (size_t i = 0; i < enemies.size(); i++) {
            if (!enemies[i]) continue;
            
            enemies[i]->Update(deltaTime);
            
            Transform2D* enemyTransform = enemies[i]->GetComponentOfType<Transform2D>();
            if (!enemyTransform) continue;
            
            bool enemyDestroyed = false;
            
            // Проверка столкновения с пулями
            auto& bullets = shooting->GetBullets();
            for (size_t j = 0; j < bullets.size(); j++) {
                if (!bullets[j]) continue;
                
                Transform2D* bulletTransform = bullets[j]->GetComponentOfType<Transform2D>();
                if (!bulletTransform) continue;
                
                float distance = enemyTransform->position.distanceTo(bulletTransform->position);
                if (distance < 0.08f) {
                    score += 10;
                    std::cout << "Score: " << score << std::endl;
                    
                    // Удаляем врага
                    delete enemies[i];
                    enemies[i] = nullptr;
                    
                    // Удаляем пулю
                    shooting->MarkBulletForDeletion(bullets[j]);
                    
                    enemyDestroyed = true;
                    break;
                }
            }
            
            if (enemyDestroyed) continue;
            
            // Проверка столкновения с игроком
            float distanceToPlayer = enemyTransform->position.distanceTo(playerTransform->position);
            if (distanceToPlayer < 0.1f) {
                health -= 10;
                std::cout << "Health: " << health << std::endl;
                
                delete enemies[i];
                enemies[i] = nullptr;
                
                if (health <= 0) {
                    std::cout << "GAME OVER! Final score: " << score << std::endl;
                    glfwSetWindowShouldClose(window, GLFW_TRUE);
                }
            }
        }
        
        // Удаляем уничтоженных врагов
        enemies.erase(std::remove_if(enemies.begin(), enemies.end(),
            [](GameObject* obj) { return !obj; }), enemies.end());
        
        // Создаем новую волну
        if (enemies.empty()) {
            std::cout << "NEW WAVE!" << std::endl;
            for (int i = 0; i < 8; i++) {
                GameObject* enemy = new GameObject();
                
                float x = (rand() % 1400 - 700) / 1000.0f;
                float y = 0.8f;
                Transform2D* enemyTransform = new Transform2D();
                enemyTransform->position = Vector2(x, y);
                enemyTransform->scale = Vector2(0.07f, 0.07f);
                enemy->AddComponent(enemyTransform);
                
                TriangleRenderer* enemyRenderer = new TriangleRenderer(window);
                enemyRenderer->SetColor(1.0f, 0.2f, 0.2f);
                enemy->AddComponent(enemyRenderer);
                
                enemy->AddComponent(new Enemy());
                enemy->Start();
                enemies.push_back(enemy);
            }
        }
        
        drawing.SwapBuffers();
        drawing.PollEvents();
        
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
    }
    
    delete player;
    for (auto enemy : enemies) {
        if (enemy) delete enemy;
    }
    
    return 0;
}