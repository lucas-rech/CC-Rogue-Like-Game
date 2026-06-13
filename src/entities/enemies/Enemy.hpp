#pragma once
#include <SFML/Graphics.hpp>
#include <string>

enum class EnemyType { Orc1, Orc2, Orc3 };

class Enemy : public sf::Drawable {
private:
    // --- Atributos Gráficos ---
    sf::Sprite sprite;
    sf::Texture idleTexture;
    sf::Texture walkTexture;
    sf::Texture attackTexture;
    sf::Texture hurtTexture;
    sf::Texture deathTexture;

    // --- Controle de Animação e Movimento ---
    int frameWidth{};
    int frameHeight{};
    int idleFrames{};
    int walkFrames{};
    int attackFrames{};
    int hurtFrames{};
    int deathFrames{};
    
    int rowDown{};
    int rowUp{};
    int rowLeft{};
    int rowRight{};

    sf::Clock animationClock;
    sf::Clock stateClock; // Relógio para controlar o tempo de hurt/attack

    int currentFrame;
    int totalFrames;
    int textureRow;
    float animationSpeed;
    
    bool isMoving;
    bool isAttacking;
    bool isHurt;
    bool isDead;
    bool justAttacked{false};

    // --- Atributos de RPG ---
    EnemyType enemyType;
    int maxHp{};
    int currentHp;
    float speed{};
    float baseDamage{};
    float attackRange{};
    float aggroRange{};

    void setupStats();

    auto draw(sf::RenderTarget &target, sf::RenderStates states) const -> void override;

public:
    Enemy(EnemyType type, float startX, float startY);

    bool loadTextures(const std::string& idlePath, 
                      const std::string& walkPath, 
                      const std::string& attackPath, 
                      const std::string& hurtPath, 
                      const std::string& deathPath);

    bool popAttackFlag() {
        bool flag = justAttacked;
        justAttacked = false;
        return flag;
    }

    sf::FloatRect getNextHitbox(sf::Vector2f playerPos, sf::Vector2f pushVector) const;

    void updateAndMove(sf::Vector2f playerPos, bool canMove, sf::Vector2f pushVector = sf::Vector2f(0.f, 0.f));

    void attack();
    void takeDamage(int amount);
    
    bool checkDead() const;
    bool getIsDead() const { return isDead; }
    
    sf::FloatRect getNextHitbox(sf::Vector2f movement) const;
    sf::FloatRect getGlobalBounds() const;

    sf::Vector2f getPosition() const;
    sf::Vector2f getCenterPosition() const;
    EnemyType getType() const;
    float getAttackRange() const { return attackRange; }
    float getBaseDamage() const { return baseDamage; }
    
    void setPosition(float x, float y);
};
