#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class Player : public sf::Drawable {
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
    sf::Vector2f currentMovement;
    int playerFrame;
    int totalFrames;
    int textureRow;
    float animationSpeed;
    bool isMoving;
    bool isAttacking;
    bool isHurt;
    bool isDead;
    bool justAttacked;

    // --- Atributos de Mira ---
    sf::Vector2f aimDirection{};
    float aimAngle{};
    sf::Vector2f targetPos{};

    float currentMovementScore;

    int currentMana;
    int maxMana;
    int manaPotions;
    bool magicUnlocked;
    bool attackIsMagic;

    // --- Atributos de RPG ---
    int level;
    int currentExp;
    int maxExp;
    int maxHp{};
    int currentHp;
    int potions;
    int keys;
    int armor;
    float speed{};
    float baseDamage{};
    float attackRange{};
    bool attackDamageReady;

    int unspentAttributePoints;

    // --- Dialogo Inicial ---
    sf::Font dialogFont;
    sf::Text dialogText;
    sf::RectangleShape dialogBg;
    float dialogTimer;
    bool hasDialog;

    void updateAnimation();

    void setupClassStats();

    // O SFML exige essa função para o window.draw(player) funcionar
    auto draw(sf::RenderTarget &target, sf::RenderStates states) const -> void override;

public:
    void drawDialog(sf::RenderTarget& target, sf::RenderStates states) const;
    Player(float startX, float startY);

    bool loadTextures(const std::string& idlePath,
                      const std::string& walkPath,
                      const std::string& attackPath,
                      const std::string& hurtPath,
                      const std::string& deathPath);

    void processInput(const sf::RenderWindow& window); // Lê o teclado
    sf::FloatRect getNextHitbox() const; // Calcula onde a hitbox vai estar se ele andar
    bool updateAndMove(bool canMove, float deltaTime); // Aplica o movimento e processa os frames da animação

    bool attack();
    bool popAttackFlag();
    bool hasPendingAttack() const;
    bool isPendingAttackMagic() const;
    bool consumeAttackDamageReady();
    sf::FloatRect getAttackHitbox() const;
    int getDamage() const;
    int takeDamage(int amount);
    void heal(int amount);
    bool usePotion();
    void gainExp(int amount);
    void addPotion(int amount = 1);
    void addManaPotion(int amount = 1);
    void addKey(int amount = 1);
    void increaseDamage(int amount);
    void increaseArmor(int amount);
    void increaseMaxHp(int amount);
    void resetForNewGame(float startX, float startY);
    bool checkDead() const;
    
    int getCurrentMana() const;
    int getMaxMana() const;
    int getManaPotions() const;
    int getUnspentPoints() const;

    void upgradeHealth();
    void upgradeDamage();
    void upgradeSpeed();
    bool hasMagicUnlocked() const;
    void unlockMagic();
    bool useManaPotion();
    void recoverMana(int amount);
    float getAttackRange() const;
    float getBaseDamage() const;
    sf::Vector2f getAimDirection() const;
    sf::Vector2f getTargetPos() const;

    void setPosition(float x, float y);
    sf::Vector2f getPosition() const;
    sf::Vector2f getCenterPosition() const;
    sf::FloatRect getHitbox() const;
    int getCurrentHp() const;
    int getMaxHp() const;
    int getLevel() const;
    int getCurrentExp() const;
    int getMaxExp() const;
    int getPotions() const;
    int getKeys() const;
    int getArmor() const;
};
