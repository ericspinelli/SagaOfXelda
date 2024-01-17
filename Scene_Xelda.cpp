#include "Scene_Xelda.h"

Scene_Xelda::Scene_Xelda(GameEngine* gameEngine, const std::string& levelPath)
    : Scene(gameEngine)
    , m_levelPath(levelPath)
{
    init(m_levelPath);
}

void Scene_Xelda::init(const std::string& levelPath)
{
    // Bind keys for game play and debugging options
    registerAction(sf::Keyboard::P,     "PAUSE");
    registerAction(sf::Keyboard::Escape,"QUIT");
    registerAction(sf::Keyboard::T,     "TOGGLE_TEXTURE");      // toggle drawing (T)extures
    registerAction(sf::Keyboard::C,     "TOGGLE_COLLISION");    // toggle drawing (C)ollision Boxes
    registerAction(sf::Keyboard::F,     "TOGGLE_CAMERA");       // toggle camera mode (F)ollow <-> Room

    // Bind keys for player movement and actions
    registerAction(sf::Keyboard::Left,  "LEFT");                // player moves LEFT
    registerAction(sf::Keyboard::Right, "RIGHT");               // player moves RIGHT
    registerAction(sf::Keyboard::Up,    "UP");                  // player moves UP
    registerAction(sf::Keyboard::Down,  "DOWN");                // player moves DOWN
    registerAction(sf::Keyboard::Space, "WEAPON");              // player uses WEAPON

    loadLevel(levelPath);
}

void Scene_Xelda::registerAction(sf::Keyboard::Key input, std::string actionName) 
{
    m_actionMap[input] = actionName;
}

void Scene_Xelda::loadLevel(const std::string& filename)
{
    // reset the entity manager whenever level is loaded
    m_entityManager = EntityManager();

    std::ifstream fin(filename);
    std::string temp;

    while (fin >> temp)
    {
        if (temp == "Player")
        {
            fin >> m_playerConfig.X >> m_playerConfig.Y     // Player starting position (x, y)
                >> m_playerConfig.CX >> m_playerConfig.CY   // Player collision box height, width (x, y)
                >> m_playerConfig.SPEED                     // Player move speed (all directions)
                >> m_playerConfig.HEALTH                    // Player starting health / starting max health
                >> m_playerConfig.IFRAMES;                  // # of frames of invulnerability after being hit
            
            spawnPlayer();
        }
        if (temp == "Tile")
        {
            std::string animName;
            int roomX, roomY, gridX, gridY;
            bool blockMove, blockVision;

            fin >> animName
                >> roomX >> roomY               // Room coordinates (room units = windowWidth x windowHeight)
                >> gridX >> gridY               // Grid number from room top left (grid unit = 64x64px)
                >> blockMove >> blockVision;    // Blocks movement, blocks vision
            
            auto tile = m_entityManager.addEntity("tile");

            tile->addComponent<CAnimation>(m_game->assets().getAnimation(animName), true);
            tile->addComponent<CBoundingBox>(tile->getComponent<CAnimation>().animation.getSize(), blockMove, blockVision);
            tile->addComponent<CTransform>(getPosition(roomX, roomY, gridX, gridY, tile));
            Vec2 pos = getPosition(roomX, roomY, gridX, gridY, tile);
            std::cout << animName << ": " << pos.x << "," << pos.y << std::endl;
            
        }
        if (temp == "NPC")
        {
            std::cout << "NPC" << std::endl;

            std::string animName, typeAI;
            int roomX, roomY, gridX, gridY, health, damage;
            bool blockMove, blockVision;

            fin >> animName
                >> roomX >> roomY               // Room coordinates (room units = windowWidth x windowHeight)
                >> gridX >> gridY               // Grid number from room top left (grid unit = 64x64px)
                >> blockMove >> blockVision     // Blocks movement, blocks vision
                >> health >> damage;            // Max/starting health, damage to player
            
            auto enemy = m_entityManager.addEntity("enemy");

            enemy->addComponent<CAnimation>(m_game->assets().getAnimation(animName), true);
            enemy->addComponent<CBoundingBox>(enemy->getComponent<CAnimation>().animation.getSize(), blockMove, blockVision);
            enemy->addComponent<CTransform>(getPosition(roomX, roomY, gridX, gridY, enemy));
            enemy->addComponent<CHealth>(health);
            enemy->addComponent<CDamage>(damage);

            Vec2 pos = getPosition(roomX, roomY, gridX, gridY, enemy);
            std::cout << animName << " @ " << pos.x << "," << pos.y << std::endl;
        }
    }
}

Vec2 Scene_Xelda::getPosition(int rx, int ry, int tx, int ty, std::shared_ptr<Entity> entity) const
{
    sf::Vector2 window = m_game->window().getSize();
    Vec2 entitySize = entity->getComponent<CAnimation>().animation.getSize();
    Vec2 halfSize = entitySize / 2;

    Vec2 room = Vec2((float)window.x * (float)rx, (float)window.y * (float)ry);
    Vec2 tileSize = Vec2((tx * entitySize.x) + halfSize.x, (ty * entitySize.y) + halfSize.y);

    return room + tileSize;
}

void Scene_Xelda::spawnPlayer()
{
    auto p = m_entityManager.addEntity("player");

    p->addComponent<CAnimation>(m_game->assets().getAnimation("StandDown"), true);
    p->addComponent<CTransform>(Vec2(m_playerConfig.X, m_playerConfig.Y));
    p->addComponent<CBoundingBox>(Vec2(m_playerConfig.CX, m_playerConfig.CY));
    p->addComponent<CHealth>(m_playerConfig.HEALTH);
    p->addComponent<CInput>();
    p->addComponent<CState>();

    m_player = p;
}

void Scene_Xelda::spawnWeapon()
{
    auto w = m_entityManager.addEntity("weapon");
    auto playerPos = m_player->getComponent<CTransform>().pos;
    auto playerState = m_player->getComponent<CState>().state;
    Vec2 weaponPos;
    Vec2 weaponScale = Vec2(1,1);

    if (playerState == "left")
    {
        w->addComponent<CAnimation>(m_game->assets().getAnimation("SwordRight"), true);
        weaponPos.x = playerPos.x - m_game->assets().getAnimation("SwordRight").getSize().x;
        weaponPos.y = playerPos.y;
        weaponScale.x = -1;
    }
    else if (playerState == "right")
    {
        w->addComponent<CAnimation>(m_game->assets().getAnimation("SwordRight"), true);
        weaponPos.x = playerPos.x + m_game->assets().getAnimation("SwordRight").getSize().x;
        weaponPos.y = playerPos.y;
        weaponScale.x = 1;
    }
    else if (playerState == "up")
    {
        w->addComponent<CAnimation>(m_game->assets().getAnimation("SwordUp"), true);
        weaponPos.x = playerPos.x; 
        weaponPos.y = playerPos.y - m_game->assets().getAnimation("SwordUp").getSize().y;
        weaponScale.y = 1;
    }
    else if (playerState == "down")
    {
        w->addComponent<CAnimation>(m_game->assets().getAnimation("SwordUp"), true);
        weaponPos.x = playerPos.x; 
        weaponPos.y = playerPos.y + m_game->assets().getAnimation("SwordUp").getSize().y;
        weaponScale.y = -1;
    }
    w->addComponent<CTransform>(weaponPos);
    w->getComponent<CTransform>().scale = weaponScale;

    w->addComponent<CBoundingBox>(w->getComponent<CAnimation>().animation.getSize());
    w->addComponent<CDamage>(1);
    w->addComponent<CLifespan>(10, m_currentFrame);
}

void Scene_Xelda::update()
{
    m_entityManager.update();

    if (!m_paused)
    {
        sMovement();
        sWeapon();
        sCollision();
        sLifespan();
        sCamera();
        sAnimation();    
    }
    sRender();
    
    m_currentFrame++;
}

void Scene_Xelda::sDoAction(const Action& action)
{
    if (action.type() == "START")
    {
        // Gameplay and debugging actions
             if (action.name() == "TOGGLE_TEXTURE")     { m_drawTextures = !m_drawTextures; }
        else if (action.name() == "TOGGLE_COLLISION")   { m_drawCollision = !m_drawCollision; }
        else if (action.name() == "TOGGLE_CAMERA")      { m_follow = !m_follow; }
        else if (action.name() == "QUIT")               { onEnd(); }
        else if (action.name() == "PAUSE")              { setPaused(!m_paused); }

        // Player movement / actions
        else if (action.name() == "LEFT")               { m_player->getComponent<CInput>().left     = true; }
        else if (action.name() == "RIGHT")              { m_player->getComponent<CInput>().right    = true; }
        else if (action.name() == "UP")                 { m_player->getComponent<CInput>().up       = true; }
        else if (action.name() == "DOWN")               { m_player->getComponent<CInput>().down     = true; }
        else if (action.name() == "WEAPON")             { if (m_player->getComponent<CInput>().canAttack == true)
                                                          { 
                                                              m_player->getComponent<CInput>().weapon   = true;
                                                              m_player->getComponent<CInput>().canAttack = false;
                                                          }
                                                        }
    }
    else if (action.type() == "END")
    {
             if (action.name() == "LEFT")               { m_player->getComponent<CInput>().left      = false; }
        else if (action.name() == "RIGHT")              { m_player->getComponent<CInput>().right     = false; }
        else if (action.name() == "UP")                 { m_player->getComponent<CInput>().up        = false; }
        else if (action.name() == "DOWN")               { m_player->getComponent<CInput>().down      = false; }
        else if (action.name() == "WEAPON")             { m_player->getComponent<CInput>().canAttack = true; }
    }
}

void Scene_Xelda::sMovement()
{
    // Record current position as previous position
    auto& playerTransform = m_player->getComponent<CTransform>();
    playerTransform.prevPos = playerTransform.pos;

    // Set player velocity based on input
    Vec2 playerVelocity = {0, 0};

         if (m_player->getComponent<CInput>().left)      { playerVelocity.x = -m_playerConfig.SPEED; }
    else if (m_player->getComponent<CInput>().right)     { playerVelocity.x =  m_playerConfig.SPEED; }
    else if (m_player->getComponent<CInput>().up)        { playerVelocity.y = -m_playerConfig.SPEED; }
    else if (m_player->getComponent<CInput>().down)      { playerVelocity.y =  m_playerConfig.SPEED; }
    
    if (m_player->getComponent<CInput>().left && m_player->getComponent<CInput>().right) { playerVelocity.x =  0; }
    if (m_player->getComponent<CInput>().up && m_player->getComponent<CInput>().down)    { playerVelocity.y =  0; }                                        
    
    // Set player state based on velocity
         if (playerVelocity.x < 0) { m_player->getComponent<CState>().isMoving = true;
                                     m_player->getComponent<CState>().state = "left";
                                     m_player->getComponent<CTransform>().scale.x = -1; }
    else if (playerVelocity.x > 0) { m_player->getComponent<CState>().isMoving = true;
                                     m_player->getComponent<CState>().state = "right";
                                     m_player->getComponent<CTransform>().scale.x = 1;}
    else if (playerVelocity.y < 0) { m_player->getComponent<CState>().isMoving = true;
                                     m_player->getComponent<CState>().state = "up"; }
    else if (playerVelocity.y > 0) { m_player->getComponent<CState>().isMoving = true;
                                     m_player->getComponent<CState>().state = "down"; }
    else                           { m_player->getComponent<CState>().isMoving = false; }
                                     //m_player->getComponent<CState>().state = "stand"; }

    m_player->getComponent<CTransform>().velocity = playerVelocity;

    // Update player position using velocity if player is NOT attacking
    if (!m_player->getComponent<CState>().isAttacking)
    {
        m_player->getComponent<CTransform>().pos += playerVelocity;
    }
}

void Scene_Xelda::sWeapon()
{
    if (m_player->getComponent<CInput>().weapon && m_entityManager.getEntities("weapon").size() == 0)
    {
        spawnWeapon();
        m_player->getComponent<CInput>().weapon = false;
        m_player->getComponent<CState>().isAttacking = true;
    }
}

void Scene_Xelda::sCollision()
{
    for (auto tile : m_entityManager.getEntities("tile"))
    {
        if (!(tile->hasComponent<CBoundingBox>())) {continue;}

        // PLAYER & TILES
        Vec2 overlap          = Physics::getOverlap(m_player, tile);
        Vec2 previousOverlap  = Physics::getPreviousOverlap(m_player, tile);

        // typedef for readability
        auto& playerTransform = m_player->getComponent<CTransform>();
        auto& tileTransform   = tile->getComponent<CTransform>();
        auto& tileType = tile->getComponent<CAnimation>().animation.getName();

        if (Physics::isCollision(overlap))
        {
            if (tile->getComponent<CBoundingBox>().blockMove)
            {
                // Collision from RIGHT
                if (previousOverlap.y > 0 && playerTransform.prevPos.x > tileTransform.pos.x)
                {
                    std::cout << "from R" << std::endl;
                    std::cout << "pPrev: " << playerTransform.prevPos.x << " tPos: " << tileTransform.pos.x << std::endl;
                    playerTransform.pos.x += overlap.x;
                    std::cout << "new pPos: " << playerTransform.pos.x << std::endl;
                }
                // Collision from LEFT
                else if (previousOverlap.y > 0 && playerTransform.prevPos.x < tileTransform.pos.x)
                {
                    std::cout << "from L" << std::endl;
                    playerTransform.pos.x -= overlap.x;
                }

                if (previousOverlap.x > 0 && playerTransform.prevPos.y < tileTransform.prevPos.y)
                {
                    std::cout << "from U" << std::endl;
                    playerTransform.pos.y -= overlap.y;
                }
                else if (previousOverlap.x > 0 && playerTransform.prevPos.y > tileTransform.prevPos.y)
                {
                    std::cout << "from D" << std::endl;
                    playerTransform.pos.y += overlap.y;
                }
                
                playerTransform.velocity = Vec2(0,0);
            }
        }
    }

    // ENEMY collisions
    for (auto enemy : m_entityManager.getEntities("enemy"))
    {
        if (!(enemy->hasComponent<CBoundingBox>())) {continue;}

        // PLAYER & ENEMIES
        Vec2 overlap          = Physics::getOverlap(m_player, enemy);
        Vec2 previousOverlap  = Physics::getPreviousOverlap(m_player, enemy);

        if (Physics::isCollision(overlap))
        {
            if (enemy->hasComponent<CDamage>() && !m_player->hasComponent<CInvulnerable>())
            {
                std::cout << m_player->getComponent<CHealth>().health << std::endl;
                m_player->getComponent<CHealth>().health -= enemy->getComponent<CDamage>().damage;
                std::cout << "Damage: " << enemy->getComponent<CDamage>().damage << " HEALTH: " << m_player->getComponent<CHealth>().health << std::endl;
                m_player->addComponent<CInvulnerable>(m_playerConfig.IFRAMES);
            }
        }

        // WEAPON & ENEMIES
        for (auto weapon : m_entityManager.getEntities("weapon"))
        {
            if (!weapon->hasComponent<CDamage>()) { continue; }
            
            overlap = Physics::getOverlap(weapon, enemy);
            if (Physics::isCollision(overlap))
            {
                std::cout << enemy->getComponent<CAnimation>().animation.getName() << ": " << enemy->getComponent<CHealth>().health << "/" << enemy->getComponent<CHealth>().maxHealth << std::endl;
                enemy->getComponent<CHealth>().health -= weapon->getComponent<CDamage>().damage;
                weapon->removeComponent<CDamage>();
                std::cout << enemy->getComponent<CAnimation>().animation.getName() << ": " << enemy->getComponent<CHealth>().health << "/" << enemy->getComponent<CHealth>().maxHealth << std::endl;
            }
        }
        
    }
}

void Scene_Xelda::sLifespan()
{
    // Check lifespan for weapons, bullets, etc
    for (auto e : m_entityManager.getEntities())
    {
        if (!e->hasComponent<CLifespan>()) { continue; }

        auto& lifespan = e->getComponent<CLifespan>().lifespan;
        lifespan--;

        if (lifespan == 0)
        {
            e->destroy();
            
            if (e->tag() == "weapon")
            {
                m_player->getComponent<CState>().isAttacking = false;
            }
        }
    }

    // Count down invulnerability duration until it hits 0
    if (m_player->hasComponent<CInvulnerable>())
    {
        auto& duration = m_player->getComponent<CInvulnerable>().duration;
        duration--;
        
        std::cout << "iFrames: " << duration << std::endl;
        if (duration == 0)
        {
            m_player->removeComponent<CInvulnerable>();
        }
    }
}

void Scene_Xelda::sAnimation()
{
    // Set player animation based on state. Do NOT overwrite animation if it matches current state.
    std::string state = m_player->getComponent<CState>().state;
    std::string animation = m_player->getComponent<CAnimation>().animation.getName();
    
    // Attacking animations
    if (m_player->getComponent<CState>().isAttacking)
    {
        if (state == "left" || state == "right")
        {
            m_player->addComponent<CAnimation>(m_game->assets().getAnimation("AtkRight"), true);
        }
        else if (state == "up")
        {
            m_player->addComponent<CAnimation>(m_game->assets().getAnimation("AtkUp"), true);
        }
        else if (state == "down")
        {
            m_player->addComponent<CAnimation>(m_game->assets().getAnimation("AtkDown"), true);
        }
    }
    // NON-attacking animations (running/standing)
    else
    {
        // Running animations
        if (m_player->getComponent<CState>().isMoving)
        {
            if ((state == "left" || state == "right") && (animation != "RunRight"))
            {
                m_player->addComponent<CAnimation>(m_game->assets().getAnimation("RunRight"), true);
            }
            else if ((state == "up") && (animation != "RunUp"))
            {
                m_player->addComponent<CAnimation>(m_game->assets().getAnimation("RunUp"), true);
            }
            else if ((state == "down") && (animation != "RunDown"))
            {
                m_player->addComponent<CAnimation>(m_game->assets().getAnimation("RunDown"), true);
            }
        }
        else
        {
            if (state == "left" || state == "right")
                                        { m_player->addComponent<CAnimation>(m_game->assets().getAnimation("StandRight"), true); }
            else if (state == "up")     { m_player->addComponent<CAnimation>(m_game->assets().getAnimation("StandUp"), true); }
            else if (state == "down")   { m_player->addComponent<CAnimation>(m_game->assets().getAnimation("StandDown"), true); }
        }
    }

    // All entity animations
    for (auto e : m_entityManager.getEntities())
    {
        // Skip all entities without Animation component
        if (!e->hasComponent<CAnimation>()) {continue;}

        // Update animation for ALL entities
        auto& animation = e->getComponent<CAnimation>().animation;
        animation.update();

        // Animation clean-up
        if ((!e->getComponent<CAnimation>().repeating) && e->getComponent<CAnimation>().animation.hasEnded()) 
        {
            e->destroy();
        }
    }
}

void Scene_Xelda::sCamera()
{
    auto currView = m_game->window().getView();
    auto windowSize = m_game->window().getSize();
    auto playerPos = m_player->getComponent<CTransform>().pos;
    
    if (m_follow)
    {
        // Keep view centered on player
        sf::View view = sf::View(sf::Vector2f(playerPos.x, playerPos.y), sf::Vector2f(windowSize));
        std::cout << "setting view: " << playerPos.x << "," << playerPos.y<< std::endl;
        m_game->window().setView(view);
    }
    else
    {
        // View each room as you enter it
        sf::Vector2f viewSize = currView.getSize();
        sf::Vector2f viewCenter = currView.getCenter();

        // Find current room. Start is [0,0].
        float roomX = floor(playerPos.x/viewSize.x);            // Float floor: -0.5 goes to -1.0
        float roomY = floor(playerPos.y/viewSize.y);
        std::cout << "Pos: (" << playerPos.x << "," << playerPos.y << ") Room: (" << roomX << "," << roomY << ")" <<std::endl;

        // Calculate view
        float newX = (roomX * viewSize.x) + (viewSize.x / 2);   // Scale room by viewSize and shift coordinates to center
        float newY = (roomY * viewSize.y) + (viewSize.y / 2);

        currView.setCenter(newX, newY);
        m_game->window().setView(currView);
    }
}

void Scene_Xelda::sRender()
{
    // Clear the window to sand color
    m_game->window().clear(sf::Color(255, 192, 122));
    
    // Entity rendering and animation
    if (m_drawTextures)
    {
        for (auto e : m_entityManager.getEntities())
        {
            auto& transform = e->getComponent<CTransform>();

            if (e->hasComponent<CAnimation>())
            {
                auto& animation = e->getComponent<CAnimation>().animation;
                animation.getSprite().setRotation(transform.angle);
                animation.getSprite().setPosition(transform.pos.x, transform.pos.y);
                animation.getSprite().setScale(transform.scale.x, transform.scale.y);
                m_game->window().draw(animation.getSprite());
            }

            // Health bar animation
            if (e->hasComponent<CHealth>())
            {
                // Entity position
                auto ePos = e->getComponent<CTransform>().pos;

                // Outer rectangle (max health points)
                sf::RectangleShape outerRect = sf::RectangleShape(sf::Vector2f(64, 5));
                outerRect.setOutlineColor(sf::Color(0,0,0,255));            // Black
                outerRect.setOutlineThickness(2);
                outerRect.setFillColor(sf::Color(0,0,0,255));               // Black
                outerRect.setOrigin(sf::Vector2f(32, 3));                   // Center origin
                outerRect.setPosition(ePos.x, ePos.y - 40);                 // Above entity
                m_game->window().draw(outerRect);

                // Inner rectangles (actual health points)
                auto health = e->getComponent<CHealth>();
                float width = outerRect.getSize().x / health.maxHealth;
                float innerX = ePos.x - 31;
                float innerY = ePos.y - 43;

                for (int i = 0; i < health.health; i++)
                {
                    sf::RectangleShape innerRect = sf::RectangleShape(sf::Vector2f(width, 5));
                    innerRect.setOutlineColor(sf::Color(0,0,0,255));        // Black
                    innerRect.setOutlineThickness(1);
                    innerRect.setFillColor(sf::Color(255,0,0,255));         // Red
                    innerRect.setPosition(sf::Vector2f(innerX, innerY));
                    m_game->window().draw(innerRect);
                    innerX += width;
                }   
            }
        }
    }
    
    // Draw bounding boxes around object, colored by what they block
    if (m_drawCollision)
    {
        for (auto e : m_entityManager.getEntities())
        {
            if (e->hasComponent<CBoundingBox>())
            {
                auto& box = e->getComponent<CBoundingBox>();
                auto& transform = e->getComponent<CTransform>();

                sf::RectangleShape rect;
                rect.setSize(sf::Vector2f(box.size.x-1, box.size.y-1));
                rect.setOrigin(sf::Vector2f(box.halfSize.x, box.halfSize.y));
                rect.setPosition(transform.pos.x, transform.pos.y);
                rect.setFillColor(sf::Color(0,0,0,0));
                if (box.blockMove && box.blockVision)
                {
                    rect.setOutlineColor(sf::Color(255,255,255,255));   // White
                }
                else if (box.blockMove)
                {
                    rect.setOutlineColor(sf::Color(0,0,255,255));       // Blue
                }
                else if (box.blockVision)
                {
                    rect.setOutlineColor(sf::Color(255,0,0,255));       // Red
                }
                else
                {
                    rect.setOutlineColor(sf::Color(0,0,0,255));         // Black
                }
                rect.setOutlineThickness(1);
                m_game->window().draw(rect);
            }
        }
    }

    m_game->window().display();
}

void Scene_Xelda::onEnd()
{
    m_game->changeScene("MENU", std::make_shared<Scene_Menu>(m_game));
}