#pragma once

#include "Vec2.h"
#include <SFML/Graphics.hpp>
#include "Animation.h"
#include "Assets.h"

// INCLUDES:    Component (base class)
//              Animation
//              BoundingBox
//              Damage
//              FollowPlayer
//              Health
//              Input
//              Invulnerable
//              Gravity (not used in this game)
//              Lifespan
//              Patrol
//              State
//              Transform

class Component
{
public:
    bool has = false;
};

class CAnimation : public Component
{
public:
    Animation animation;
    bool repeating;

    CAnimation() {}
    CAnimation(const Animation& anim, bool repeat)
        : animation(anim), repeating(repeat)
        {}
};

class CBoundingBox : public Component
{
public:
    Vec2 size;
    Vec2 halfSize;

    bool blockMove = true;
    bool blockVision = true;

    CBoundingBox() {}
    CBoundingBox(const Vec2& s)
        : size(s), halfSize(s.x/2.0f, s.y/2.0f)
        {}
    CBoundingBox(const Vec2& s, const bool bm, const bool bv)
        : size(s), halfSize(s.x/2.0f, s.y/2.0f), blockMove(bm), blockVision(blockVision)
        {}
};

class CDamage : public Component
{
public:
    size_t damage = 0;

    CDamage() {}
    CDamage(const int d)
        : damage(d)
        {}
};

class CFollowPlayer : public Component
{
public:
    Vec2 home   = {0,0};
    float speed = 0;

    CFollowPlayer() {}
    CFollowPlayer(Vec2 p, float s)
        : home(p), speed(s)
        {}
};

class CHealth : public Component
{
public:
    int health       = 0;
    size_t maxHealth = 0;

    CHealth() {}
    CHealth(const int h)
        : health(h), maxHealth(h)
        {}
};

class CInput : public Component
{
public:
    bool up        = false;
    bool down      = false;
    bool left      = false;
    bool right     = false;
    bool weapon    = false;
    bool canAttack = true;

    CInput() {}
};

class CInvulnerable : public Component
{
public:
    size_t duration = 0;

    CInvulnerable() {}
    CInvulnerable(const size_t duration)
        : duration(duration)
        {}
};

class CGravity : public Component
{
public:
    float gravity = 0;

    CGravity() {}
    CGravity(float g) 
        : gravity(g)
        {}
};

class CLifespan : public Component
{
public:
    int lifespan        = 0;
    int frameCreated    = 0;
    
    CLifespan() {}
    CLifespan(int duration, int frame)
        : lifespan(duration), frameCreated(frame)
        {}
};

class CPatrol : public Component
{
public:
    std::vector<Vec2> positions;
    size_t currentPosition = 0;
    float speed = 0;

    CPatrol() {}
    CPatrol(std::vector<Vec2>& p, float s)
        : positions(p), speed(s)
        {}
};

class CState : public Component
{
public:
    std::string state     = "down";
    bool isMoving         = false;     
    bool isAttacking      = false;
    
    CState() {}
    CState(const std::string& s)
        : state(s)
        {}
};

class CTransform : public Component
{
public:
    Vec2    pos         = { 0.0, 0.0 };
    Vec2    prevPos     = { 0.0, 0.0 };
    Vec2    scale       = { 1.0, 1.0 };
    Vec2    velocity    = { 0.0, 0.0 };
    float   angle       = 0.0;

    CTransform() {}
    CTransform(const Vec2& p)
        : pos(p), prevPos(p)
        {}
    CTransform(const Vec2& p, const Vec2& v, float a)
        : pos(p), prevPos(p), velocity(v), angle(a)
        {}
};