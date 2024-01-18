#include "Physics.h"

Vec2 Physics::getOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
    Vec2 aPos = a->getComponent<CTransform>().pos;
    Vec2 bPos = b->getComponent<CTransform>().pos;

    float dx = fabsf(aPos.x - bPos.x);
    float dy = fabsf(aPos.y - bPos.y); 
    
    Vec2 aHalfSize = a->getComponent<CBoundingBox>().halfSize;
    Vec2 bHalfSize = b->getComponent<CBoundingBox>().halfSize;

    return Vec2((aHalfSize.x + bHalfSize.x) - dx, (aHalfSize.y + bHalfSize.y) - dy);
}

Vec2 Physics::getPreviousOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
    Vec2 aPos = a->getComponent<CTransform>().prevPos;
    Vec2 bPos = b->getComponent<CTransform>().prevPos;

    float dx = fabsf(aPos.x - bPos.x);
    float dy = fabsf(aPos.y - bPos.y); 
    
    Vec2 aHalfSize = a->getComponent<CBoundingBox>().halfSize;
    Vec2 bHalfSize = b->getComponent<CBoundingBox>().halfSize;

    return Vec2((aHalfSize.x + bHalfSize.x) - dx, (aHalfSize.y + bHalfSize.y) - dy);
}

bool Physics::isCollision(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
    Vec2 overlap = getOverlap(a, b);
    
    return ((overlap.x > 0) && (overlap.y > 0));
}

bool Physics::isCollision(Vec2 overlap)
{
    return ((overlap.x > 0) && (overlap.y > 0));
}

float Physics::crossProduct(Vec2 a, Vec2 b)
{
    return ((a.x * b.y) - (a.y * b.x));
}

Intersect Physics::lineIntersect(const Vec2 a, const Vec2 b, const Vec2 c, const Vec2 d)
{
    Vec2 r    = (b - a);
    Vec2 s    = (d - c);
    Vec2 cma  = (c - a);
    float rxs = crossProduct(r, s);    
    float t   = (crossProduct(cma, s) / rxs);
    float u   = (crossProduct(cma, r) / rxs);
    
    Intersect output;
    if (t >= 0 && t <= 1 && u >= 0 && u <=1)
    {
        output.result = true;
        output.pos = Vec2(a.x + (t * r.x), a.y + (t * r.y));
    }
    else
    {
        output.result = false;
        output.pos = Vec2(0,0);
    }
    return output;

    //return { true, Vec2(0,0) };
}