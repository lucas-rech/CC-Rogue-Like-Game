#pragma once
#include <SFML/Graphics.hpp>
#include <string>

enum class HeroClass { Warrior, Vampire, Archer };

class Player : public sf::Drawable {
private:
    // --- Atributos Gráficos ---
    sf::Sprite sprite;
    sf::Texture idleTexture;
    sf::Texture walkTexture;
    sf::Texture attackTexture;

    // --- Controle de Animação e Movimento ---
    int frameWidth{};
    int frameHeight{};
    int idleFrames{};
    int walkFrames{};
    int attackFrames{};
    int rowDown{};
    int rowUp{};
    int rowLeft{};
    int rowRight{};

    sf::Clock animationClock;
    sf::Vector2f currentMovement;
    int playerFrame;
    int totalFrames;
    int textureRow;
    float animationSpeed;
    bool isMoving;
    bool isAttacking;

    // --- Atributos de Mira ---
    sf::Vector2f aimDirection{};
    float aimAngle{};

    // --- Atributos de RPG ---
    HeroClass heroClass;
    int level;
    int currentExp;
    int maxExp;
    int maxHp{};
    int currentHp;
    int potions;
    float speed{};
    float baseDamage{};

    void setupClassStats();

    // O SFML exige essa função para o window.draw(player) funcionar
    auto draw(sf::RenderTarget &target, sf::RenderStates states) const -> void override;

public:
    Player(HeroClass type, float startX, float startY);

    bool loadTextures(const std::string& idlePath, const std::string& walkPath, const std::string& attackPath);

    void processInput(const sf::RenderWindow& window); // Lê o teclado
    sf::FloatRect getNextHitbox() const; // Calcula onde a hitbox vai estar se ele andar
    void updateAndMove(bool canMove); // Aplica o movimento e processa os frames da animação

    void attack();
    void takeDamage(int amount);
    void heal(int amount);
    void usePotion();
    void gainExp(int amount);
    HeroClass getClass() const;

    void setPosition(float x, float y);
    sf::Vector2f getPosition() const;
    sf::Vector2f getCenterPosition() const;
};