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
                >> m_playerConfig.HEALTH;                   // Player starting health / starting max health
            
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
            
            auto tile = m_entityManager.addEntity("Tile");

            tile->addComponent<CAnimation>(m_game->assets().getAnimation(animName), true);
            tile->addComponent<CBoundingBox>(tile->getComponent<CAnimation>().animation.getSize(), blockMove, blockVision);
            tile->addComponent<CTransform>(getPosition(roomX, roomY, gridX, gridY, tile));
            Vec2 pos = getPosition(roomX, roomY, gridX, gridY, tile);
            std::cout << animName << ": " << pos.x << "," << pos.y << std::endl;
            
        }
    }
}

Vec2 Scene_Xelda::getPosition(int rx, int ry, int tx, int ty, std::shared_ptr<Entity> entity) const
{
    sf::Vector2 window = m_game->window().getSize();
    Vec2 entitySize = entity->getComponent<CAnimation>().animation.getSize();
    Vec2 halfSize = entitySize / 2;

    Vec2 room = Vec2(window.x * rx, window.y * ry);
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

}

void Scene_Xelda::update()
{
    m_entityManager.update();

    if (!m_paused)
    {
        sMovement();
        sCollision();
        sLifespan();
        sAnimation();    
    }
    sRender();
}

void Scene_Xelda::sDoAction(const Action& action)
{
    if (action.type() == "START")
    {
        // Gameplay and debugging actions
             if (action.name() == "TOGGLE_TEXTURE")     { m_drawTextures = !m_drawTextures; }
        else if (action.name() == "TOGGLE_COLLISION")   { m_drawCollision = !m_drawCollision; }
        else if (action.name() == "TOGGLE_CAMERA")      { m_drawGrid = !m_drawGrid; }
        else if (action.name() == "QUIT")               { onEnd(); }
        else if (action.name() == "PAUSE")              { setPaused(!m_paused); }

        // Player movement
        else if (action.name() == "LEFT")               { m_player->getComponent<CInput>().left     = true; }
        else if (action.name() == "RIGHT")              { m_player->getComponent<CInput>().right    = true; }
        else if (action.name() == "UP")                 { m_player->getComponent<CInput>().up       = true; }
        else if (action.name() == "DOWN")               { m_player->getComponent<CInput>().down     = true; }
        else if (action.name() == "WEAPON")             { m_player->getComponent<CInput>().weapon   = true; }
    }
    else if (action.type() == "END")
    {
             if (action.name() == "LEFT")               { m_player->getComponent<CInput>().left     = false; }
        else if (action.name() == "RIGHT")              { m_player->getComponent<CInput>().right    = false; }
        else if (action.name() == "UP")                 { m_player->getComponent<CInput>().up       = false; }
        else if (action.name() == "DOWN")               { m_player->getComponent<CInput>().down     = false; }
        else if (action.name() == "WEAPON")             { m_player->getComponent<CInput>().weapon   = false; }
    }
}

void Scene_Xelda::sMovement()
{
    // Set player velocity based on input
    Vec2 playerVelocity = {0, 0};

         if (m_player->getComponent<CInput>().left)      { playerVelocity.x = -m_playerConfig.SPEED; }
    else if (m_player->getComponent<CInput>().right)     { playerVelocity.x =  m_playerConfig.SPEED; }
    else if (m_player->getComponent<CInput>().up)        { playerVelocity.y = -m_playerConfig.SPEED; }
    else if (m_player->getComponent<CInput>().down)      { playerVelocity.y =  m_playerConfig.SPEED; }
    
    if (m_player->getComponent<CInput>().left && m_player->getComponent<CInput>().right) { playerVelocity.x =  0; }
    if (m_player->getComponent<CInput>().up && m_player->getComponent<CInput>().down)    { playerVelocity.y =  0; }                                        
    
    // Set player state based on velocity
         if (playerVelocity.x < 0) { m_player->getComponent<CState>().state = "left"; m_player->getComponent<CTransform>().scale.x = -1;}
    else if (playerVelocity.x > 0) { m_player->getComponent<CState>().state = "right"; m_player->getComponent<CTransform>().scale.x = 1;}
    else if (playerVelocity.y < 0) { m_player->getComponent<CState>().state = "up"; }
    else if (playerVelocity.y > 0) { m_player->getComponent<CState>().state = "down"; }
    else                           { m_player->getComponent<CState>().state = "stand"; }

    m_player->getComponent<CTransform>().velocity = playerVelocity;

    // Update player position using velocity
    m_player->getComponent<CTransform>().pos += playerVelocity;
    std::cout << m_player->getComponent<CTransform>().pos.x << "," << m_player->getComponent<CTransform>().pos.y << std::endl;
}

void Scene_Xelda::sCollision()
{

}

void Scene_Xelda::sLifespan()
{

}

void Scene_Xelda::sAnimation()
{
    // Set player animation based on state. Do NOT overwrite animation if it matches current state.
    std::string animation = m_player->getComponent<CAnimation>().animation.getName();

    if ((m_player->getComponent<CState>().state == "left") && (animation != "RunRight"))
    {
        m_player->addComponent<CAnimation>(m_game->assets().getAnimation("RunRight"), true);
    }
    else if ((m_player->getComponent<CState>().state == "right") && (animation != "RunRight"))
    {
        m_player->addComponent<CAnimation>(m_game->assets().getAnimation("RunRight"), true);
    }
    else if ((m_player->getComponent<CState>().state == "up") && (animation != "RunUp"))
    {
        m_player->addComponent<CAnimation>(m_game->assets().getAnimation("RunUp"), true);
    }
    else if ((m_player->getComponent<CState>().state == "down") && (animation != "RunDown"))
    {
        m_player->addComponent<CAnimation>(m_game->assets().getAnimation("RunDown"), true);
    }
    else if (m_player->getComponent<CState>().state == "stand")
    {
             if (animation == "RunRight") { m_player->addComponent<CAnimation>(m_game->assets().getAnimation("StandRight"), true); }
        else if (animation == "RunUp")    { m_player->addComponent<CAnimation>(m_game->assets().getAnimation("StandUp"), true); }
        else if (animation == "RunDown")  { m_player->addComponent<CAnimation>(m_game->assets().getAnimation("StandDown"), true); }
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

}

void Scene_Xelda::sRender()
{
    // Clear the window to a blue
    m_game->window().setView(m_game->window().getDefaultView());
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
        }
    }
    
    m_game->window().display();
}

void Scene_Xelda::onEnd()
{
    m_game->changeScene("MENU", std::make_shared<Scene_Menu>(m_game));
}