#include "Player.hpp"

#include <cmath>
#include <iostream>
#include <filesystem>

Player::Player(const float startX, const float startY) {
    level = 1;
    currentExp = 0;
    maxExp = 100;
    potions = 3;
    keys = 0;
    armor = 0;
    
    currentMana = 100;
    maxMana = 100;
    manaPotions = 3;
    magicUnlocked = false;
    attackIsMagic = false;

    playerFrame = 0;
    totalFrames = 12;
    textureRow = 0;
    animationSpeed = 0.08f;
    isMoving = false;
    isAttacking = false;
    isHurt = false;
    isDead = false;
    justAttacked = false;
    attackDamageReady = false;
    hasDialog = true;
    dialogTimer = 10.0f; // Exibir por 10 segundos
    sprite.setPosition(startX, startY);

    // Carregar fonte para o dialogo
    std::vector<std::string> fontPaths = {
        "assets/fonts/arial.ttf",
        "assets/fonts/DejaVuSans.ttf",
        "assets/fonts/OpenSans-Regular.ttf",
        "assets/fonts/Roboto-Regular.ttf"
    };

    if (const char* windowsDir = std::getenv("WINDIR")) {
        fontPaths.emplace_back(std::string(windowsDir) + "/Fonts/arial.ttf");
    }

    for (const auto& path : fontPaths) {
        if (std::filesystem::exists(path) && dialogFont.loadFromFile(path)) {
            break;
        }
    }

    dialogText.setFont(dialogFont);
    dialogText.setString("Minha cabeca... o que aconteceu? \nOnde eu estou? Preciso encontrar respostas.");
    dialogText.setCharacterSize(9);
    dialogText.setFillColor(sf::Color::White);
    
    dialogBg.setFillColor(sf::Color(0, 0, 0, 180));
    dialogBg.setOutlineThickness(1.f);
    dialogBg.setOutlineColor(sf::Color::White);
    
    sf::FloatRect textBounds = dialogText.getLocalBounds();
    dialogBg.setSize(sf::Vector2f(textBounds.width + 10.f, textBounds.height + 10.f));

    setupClassStats();
}

bool Player::loadTextures(const std::string& idlePath,
                          const std::string& walkPath,
                          const std::string& attackPath,
                          const std::string& hurtPath,
                          const std::string& deathPath) {
    if (!idleTexture.loadFromFile(idlePath) ||
        !walkTexture.loadFromFile(walkPath) ||
        !attackTexture.loadFromFile(attackPath) ||
        !hurtTexture.loadFromFile(hurtPath) ||
        !deathTexture.loadFromFile(deathPath)) {
        std::cout << "Erro ao carregar as texturas do jogador!" << std::endl;
        return false;
    }

    sprite.setTexture(idleTexture);
    sprite.setTextureRect(sf::IntRect(0, 0, frameWidth, frameHeight));
    return true;
}

void Player::setupClassStats() {
    maxExp = 100;
    unspentAttributePoints = 0;
    level = 1;
    maxHp = 100;
    speed = 1.5f;
    baseDamage = 30.0f;
    attackRange = 42.0f;

    frameWidth = 64;
    frameHeight = 64;
    attackFrames = 8;
    walkFrames = 6;
    idleFrames = 12;
    hurtFrames = 5;
    deathFrames = 7;

    rowDown  = 0;
    rowLeft  = 1;
    rowRight = 2;
    rowUp    = 3;

    currentHp = maxHp;
}

void Player::processInput(const sf::RenderWindow& window) {
    if (isDead) {
        return;
    }

    currentMovement = sf::Vector2f(0.f, 0.f);
    isMoving = false;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) { currentMovement.x += speed; isMoving = true; }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) { currentMovement.x -= speed; isMoving = true; }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) { currentMovement.y += speed; isMoving = true; }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) { currentMovement.y -= speed; isMoving = true; }

    const sf::Vector2i mousePixelPosition = sf::Mouse::getPosition(window);
    const sf::Vector2f mouseWorldPos = window.mapPixelToCoords(mousePixelPosition);
    const sf::Vector2f playerCenter = getCenterPosition();

    float dx = mouseWorldPos.x - playerCenter.x;
    float dy = mouseWorldPos.y - playerCenter.y;
    float length = std::sqrt(dx * dx + dy * dy);

    if (length != 0.f) {
        aimDirection = sf::Vector2f(dx / length, dy / length);
    }

    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        targetPos = mouseWorldPos;
        attackIsMagic = false;
        attack();
    } else if (sf::Mouse::isButtonPressed(sf::Mouse::Right) && magicUnlocked && currentMana >= 20) {
        targetPos = mouseWorldPos;
        attackIsMagic = true;
        attack();
    }

    if (isAttacking) {
        aimAngle = std::atan2(dy, dx) * 180.f / 3.14159265f;
        if (aimAngle > -45.f && aimAngle <= 45.f) textureRow = rowRight * frameHeight;
        else if (aimAngle > 45.f && aimAngle <= 135.f) textureRow = rowDown * frameHeight;
        else if (aimAngle > 135.f || aimAngle <= -135.f) textureRow = rowLeft * frameHeight;
        else if (aimAngle > -135.f && aimAngle <= -45.f) textureRow = rowUp * frameHeight;
    } else if (isMoving) {
        float moveAngle = std::atan2(currentMovement.y, currentMovement.x) * 180.f / 3.14159265f;
        if (moveAngle > -45.f && moveAngle <= 45.f) textureRow = rowRight * frameHeight;
        else if (moveAngle > 45.f && moveAngle <= 135.f) textureRow = rowDown * frameHeight;
        else if (moveAngle > 135.f || moveAngle <= -135.f) textureRow = rowLeft * frameHeight;
        else if (moveAngle > -135.f && moveAngle <= -45.f) textureRow = rowUp * frameHeight;
    }
}

sf::FloatRect Player::getNextHitbox() const {
    sf::FloatRect nextHitbox = sprite.getGlobalBounds();
    nextHitbox.width = 12.f;
    nextHitbox.height = 20.f;
    nextHitbox.left += currentMovement.x + 26.f;
    nextHitbox.top += currentMovement.y + 26.f;
    return nextHitbox;
}

bool Player::updateAndMove(bool canMove, float deltaTime) {
    bool moved = false;

    if (isDead) {
        sprite.setTexture(deathTexture);
        totalFrames = deathFrames;
        animationSpeed = 0.1f;

        if (animationClock.getElapsedTime().asSeconds() > animationSpeed) {
            if (playerFrame < totalFrames - 1) {
                playerFrame++;
            }
            animationClock.restart();
        }

        sprite.setTextureRect(sf::IntRect(playerFrame * frameWidth, textureRow, frameWidth, frameHeight));
        return false;
    }

    if (isHurt) {
        sprite.setTexture(hurtTexture);
        totalFrames = hurtFrames;
        animationSpeed = 0.1f;

        if (animationClock.getElapsedTime().asSeconds() > animationSpeed) {
            playerFrame++;
            if (playerFrame >= totalFrames) {
                isHurt = false;
                playerFrame = 0;
            }
            animationClock.restart();
        }

        sprite.setTextureRect(sf::IntRect(playerFrame * frameWidth, textureRow, frameWidth, frameHeight));
        return false;
    }

    if (isMoving && canMove) {
        sf::Vector2f actualMovement = currentMovement;
        if (isAttacking) {
            actualMovement.x *= 0.1f;
            actualMovement.y *= 0.1f;
        }

        sprite.move(actualMovement);
        moved = actualMovement.x != 0.f || actualMovement.y != 0.f;
    }

    if (isAttacking) {
        sprite.setTexture(attackTexture);
        totalFrames = attackFrames;
        animationSpeed = 0.05f;
    } else if (isMoving) {
        sprite.setTexture(walkTexture);
        totalFrames = walkFrames;
        animationSpeed = 0.10f;
    } else {
        sprite.setTexture(idleTexture);
        totalFrames = (textureRow == rowUp * frameHeight) ? 4 : idleFrames;
        animationSpeed = 0.15f;
    }

    if (animationClock.getElapsedTime().asSeconds() > animationSpeed) {
        playerFrame++;

        if (playerFrame >= totalFrames) {
            playerFrame = 0;
            if (isAttacking) {
                isAttacking = false;
            }
        }

        animationClock.restart();
    }

    sprite.setTextureRect(sf::IntRect(playerFrame * frameWidth, textureRow, frameWidth, frameHeight));

    if (hasDialog) {
        dialogTimer -= deltaTime;
        if (dialogTimer <= 0.f) {
            hasDialog = false;
        } else {
            dialogBg.setPosition(sprite.getPosition().x - dialogBg.getSize().x / 2.f + 32.f, sprite.getPosition().y - 40.f);
            dialogText.setPosition(dialogBg.getPosition().x + 5.f, dialogBg.getPosition().y + 5.f);
        }
    }

    return moved;
}

void Player::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(sprite, states);
}

void Player::drawDialog(sf::RenderTarget& target, sf::RenderStates states) const {
    if (hasDialog) {
        target.draw(dialogBg, states);
        target.draw(dialogText, states);
    }
}

int Player::takeDamage(int amount) {
    if (isDead) {
        return 0;
    }

    int finalDamage = amount - armor;
    if (finalDamage < 1) {
        finalDamage = 1;
    }

    currentHp -= finalDamage;
    if (currentHp <= 0) {
        currentHp = 0;
        isDead = true;
        isHurt = false;
        isAttacking = false;
        playerFrame = 0;
    } else {
        isHurt = true;
        isAttacking = false;
        playerFrame = 0;
    }

    return finalDamage;
}

void Player::heal(int amount) {
    currentHp += amount;
    if (currentHp > maxHp) currentHp = maxHp;
}

bool Player::usePotion() {
    if (potions > 0 && currentHp < maxHp && !isDead) {
        heal(30);
        potions--;
        return true;
    }

    return false;
}

void Player::gainExp(int amount) {
    currentExp += amount;
    while (currentExp >= maxExp) {
        level++;
        currentExp -= maxExp;
        maxExp = static_cast<int>(static_cast<float>(maxExp) * 1.5f);
        unspentAttributePoints++;
        std::cout << "Level Up! Nivel " << level << "! +1 Ponto de Atributo.\n";
    }
}

void Player::addPotion(int amount) {
    potions += amount;
}

void Player::addManaPotion(int amount) {
    manaPotions += amount;
}

void Player::addKey(int amount) {
    keys += amount;
}

void Player::increaseDamage(int amount) {
    baseDamage += static_cast<float>(amount);
}

void Player::increaseArmor(int amount) {
    armor += amount;
}

void Player::increaseMaxHp(int amount) {
    maxHp += amount;
    heal(amount);
}

void Player::resetForNewGame(float startX, float startY) {
    level = 1;
    currentExp = 0;
    maxExp = 100;
    potions = 3;
    keys = 0;
    armor = 0;
    attackDamageReady = false;
    justAttacked = false;
    isMoving = false;
    isAttacking = false;
    isHurt = false;
    isDead = false;
    playerFrame = 0;
    textureRow = 0;
    setupClassStats();
    setPosition(startX, startY);
}

bool Player::attack() {
    if (isDead || isHurt || isAttacking) {
        return false;
    }

    static sf::Clock attackCooldown;
    const float cooldown = attackIsMagic ? 2.0f : 0.3f;

    if (attackCooldown.getElapsedTime().asSeconds() > cooldown) {
        if (attackIsMagic) {
            currentMana -= 20;
        }
        isAttacking = true;
        justAttacked = true;
        attackDamageReady = true;
        playerFrame = 0;
        attackCooldown.restart();
        return true;
    }
    return false;
}

bool Player::popAttackFlag() {
    bool flag = justAttacked;
    justAttacked = false;
    return flag;
}

bool Player::hasPendingAttack() const {
    return justAttacked;
}

bool Player::isPendingAttackMagic() const {
    return attackIsMagic;
}

bool Player::consumeAttackDamageReady() {
    if (!attackDamageReady) {
        return false;
    }

    attackDamageReady = false;
    justAttacked = false;
    return true;
}

sf::FloatRect Player::getAttackHitbox() const {
    sf::Vector2f center = getCenterPosition();
    float width = 34.f;
    float height = 34.f;
    float distance = 28.f;

    if (std::abs(aimDirection.x) > std::abs(aimDirection.y)) {
        if (aimDirection.x >= 0.f) {
            return {center.x + distance, center.y - height / 2.f, width, height};
        }
        return {center.x - distance - width, center.y - height / 2.f, width, height};
    }

    if (aimDirection.y >= 0.f) {
        return {center.x - width / 2.f, center.y + distance, width, height};
    }
    return {center.x - width / 2.f, center.y - distance - height, width, height};
}

int Player::getDamage() const {
    return static_cast<int>(baseDamage);
}

sf::Vector2f Player::getPosition() const {
    return sprite.getPosition();
}

sf::Vector2f Player::getCenterPosition() const {
    return {sprite.getPosition().x + 32.f, sprite.getPosition().y + 32.f};
}

sf::FloatRect Player::getHitbox() const {
    sf::FloatRect hitbox = sprite.getGlobalBounds();
    hitbox.width = 12.f;
    hitbox.height = 20.f;
    hitbox.left += 26.f;
    hitbox.top += 26.f;
    return hitbox;
}

int Player::getCurrentMana() const {
    return currentMana;
}

int Player::getMaxMana() const {
    return maxMana;
}

int Player::getManaPotions() const {
    return manaPotions;
}

int Player::getUnspentPoints() const {
    return unspentAttributePoints;
}

void Player::upgradeHealth() {
    if (unspentAttributePoints > 0) {
        unspentAttributePoints--;
        maxHp += 20;
        currentHp += 20; // Heals when upgraded
        std::cout << "Max HP increased to " << maxHp << "!\n";
    }
}

void Player::upgradeDamage() {
    if (unspentAttributePoints > 0) {
        unspentAttributePoints--;
        baseDamage += 4.0f;
        std::cout << "Base Damage increased to " << baseDamage << "!\n";
    }
}

void Player::upgradeSpeed() {
    if (unspentAttributePoints > 0) {
        unspentAttributePoints--;
        speed += 0.2f;
        std::cout << "Speed increased to " << speed << "!\n";
    }
}

bool Player::hasMagicUnlocked() const {
    return magicUnlocked;
}

void Player::unlockMagic() {
    magicUnlocked = true;
}

bool Player::useManaPotion() {
    if (manaPotions > 0 && currentMana < maxMana) {
        manaPotions--;
        recoverMana(30);
        return true;
    }
    return false;
}

void Player::recoverMana(int amount) {
    currentMana += amount;
    if (currentMana > maxMana) currentMana = maxMana;
}

bool Player::checkDead() const {
    return isDead;
}

float Player::getAttackRange() const {
    return attackRange;
}

float Player::getBaseDamage() const {
    return baseDamage;
}

sf::Vector2f Player::getAimDirection() const {
    return aimDirection;
}

sf::Vector2f Player::getTargetPos() const {
    return targetPos;
}

void Player::setPosition(float x, float y) {
    sprite.setPosition(x, y);
}

int Player::getCurrentHp() const {
    return currentHp;
}

int Player::getMaxHp() const {
    return maxHp;
}

int Player::getLevel() const {
    return level;
}

int Player::getCurrentExp() const {
    return currentExp;
}

int Player::getMaxExp() const {
    return maxExp;
}

int Player::getPotions() const {
    return potions;
}

int Player::getKeys() const {
    return keys;
}

int Player::getArmor() const {
    return armor;
}
