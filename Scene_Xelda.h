#pragma once

#include <map>
#include <memory>
#include <SFML/Graphics.hpp>

#include "GameEngine.h"
#include "EntityManager.h"
#include "Scene.h"
#include "Action.h"
#include "Physics.h"

class Scene_Xelda : public Scene
{
    struct PlayerConfig
    {
        float X, Y, CX, CY, SPEED, HEALTH;
        size_t IFRAMES;
        std::string WEAPON;
    };

    struct WeaponConfig
    {
        float SPEED, LIFESPAN;
        std::string WEAPON;
    };

protected:
    std::shared_ptr<Entity> m_player;
    std::string             m_levelPath;
    PlayerConfig            m_playerConfig;
    WeaponConfig            m_weaponConfig;
    std::vector<std::string> m_dropVec;
    bool                    m_drawTextures = true;
    bool                    m_drawCollision = false;
    bool                    m_drawGrid = false;
    bool                    m_follow = false;
    const Vec2              m_gridSize = {64, 64};
    sf::Text                m_gridText;


    void init(const std::string& levelPath);
    void registerAction(sf::Keyboard::Key input, std::string actionName);
    
    void loadLevel(const std::string& filename);
    Vec2 getPosition(int rx, int ry, int tx, int ty, std::shared_ptr<Entity> entity) const;

    void spawnPlayer();
    void spawnWeapon();
    void spawnDrop(std::shared_ptr<Entity> source);

    void update();
    void sDoAction(const Action& action);
    void sMovement();
    void sWeapon();
    void sAI();
    void sCollision();
    void sLifespan();
    void sAnimation();
    void sCamera();
    void sRender();

    void onEnd();

public:
    Scene_Xelda(GameEngine* gameEngine, const std::string& levelPath);
};