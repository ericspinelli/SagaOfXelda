#pragma once

#include <memory>
#include <cmath>
#include "Entity.h"
#include "Components.h"
#include "Vec2.h"

struct Intersect { bool result; Vec2 pos; };

namespace Physics
{
    Vec2 getOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b);
    Vec2 getPreviousOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b);

    bool isCollision(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b);
    bool isCollision(Vec2 overlap);

    float crossProduct(Vec2 a, Vec2 b);
    //Intersect lineIntersect(Vec2 a, Vec2 b, Vec2 c, Vec2 d);
    Intersect lineIntersect(const Vec2 a, const Vec2 b, const Vec2 c, const Vec2 d);
};