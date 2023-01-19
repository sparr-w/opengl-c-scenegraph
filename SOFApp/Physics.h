/*
* #pragma once
#include "stdafx.h"
#include "TransformNode.h"

struct ContactInfo {
    glm::vec3 normal;
    float depth;
};

class StaticCollider {
    // abstract interface for something that static that collides with a sphere
public:
    virtual bool Collide(const glm::vec3& r, float rad, glm::vec3& normal, float& depth) const = 0;
    virtual ~StaticCollider() {}
};

class QuadCollider : public StaticCollider {
    glm::vec3 r, a, b;
    glm::vec3 aHat, bHat; // unit vectors along sides a and b
    float lena, lenb; // lengths of sides a and b
public:
    QuadCollider(const glm::vec3& r, const glm::vec3& a, const glm::vec3& b) : r(r), a(a), b(b) {
        lena = glm::length(a);
        lenb = glm::length(b);
        aHat = a / lena;
        bHat = b / lenb;
    }
    QuadCollider(glm::mat4 trans) {
        // this constructor assumes a unit quad centered on the origin with sides x, z
        // transform transforms this into place
        glm::vec3 rt = trans * glm::vec4(-0.5f, 0.0f, -0.5f, 1.0f);
        glm::vec3 at = trans * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
        glm::vec3 bt = trans * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
        r = glm::vec3(rt.x, rt.y, rt.z);
        a = glm::vec3(at.x, at.y, at.z);
        b = glm::vec3(bt.x, bt.y, bt.z);
        lena = glm::length(a);
        lenb = glm::length(b);
        aHat = a / lena;
        bHat = b / lenb;
    }
    ~QuadCollider() {}

    bool Collide(const glm::vec3& p, float rad, glm::vec3& normal, float& depth) const override {
        float la, lb;
        la = dot(p - r, aHat);
        lb = dot(p - r, bHat);

        if (la < 0.0f) la = 0.0f; if (la > lena) la = lena;
        if (lb < 0.0f) lb = 0.0f; if (la > lenb) lb = lenb;

        glm::vec3 rNear = r + aHat * la + bHat * lb;

        if (glm::dot(p - rNear, p - rNear) < rad * rad) {
            normal = glm::normalize(p - rNear);
            depth = rad - glm::length(p - rNear);

            return true;
        }
        else
            return false;
    }
};

class CuboidCollider : StaticCollider {
public:
    std::vector<QuadCollider*> faces;
    CuboidCollider(glm::mat4 trans) {
        
        // top
        glm::vec3 R = trans * glm::vec4(-0.5f, 0.0f, -0.5f, 1.0f);
        glm::vec3 A = trans * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
        glm::vec3 B = trans * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
        QuadCollider* newface = new QuadCollider(R, A, B);
        faces.push_back(newface);
        // bottom
        R = trans * glm::vec4(-0.5f, 0.0f, -0.5f, 1.0f);
        A = trans * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
        B = trans * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
        newface = new QuadCollider(R, A, B);
        faces.push_back(newface);
        

        // top
        QuadCollider* faceing = new QuadCollider(trans);
        faces.push_back(faceing);
        // top
        faceing = new QuadCollider(trans);
        faces.push_back(faceing);
    }
    ~CuboidCollider() {}

    bool Collide(const glm::vec3& p, float rad, glm::vec3& normal, float& depth) const override {
        bool hasColl = false;
        for (int i = 0; i < 2; i++) {
            faces[i]->Collide(p, rad, normal, depth);
        }

        return hasColl;
    }
};

struct Projectile {
    glm::vec3 pos;
    glm::vec3 vel;
    int collCount = 0;
    TransformNode* sceneGraphNode;
    Projectile(const glm::vec3& pos, const glm::vec3& vel, TransformNode* sgNode) : pos(pos), vel(vel), sceneGraphNode(sgNode) {}
    ~Projectile() {
        // disconnect and delete the scene graph node
        delete sceneGraphNode;
    }
};

class PhysicsCallback : public UpdateCallback {
    TransformNode* sgNode;
    Projectile* physics;
public:
    PhysicsCallback(TransformNode* sgNode, Projectile* physics) : sgNode(sgNode), physics(physics) {}
    virtual void operator()(float dt) {
        // copy the position from one to the other
        sgNode->SetTransform(glm::translate(glm::mat4(), physics->pos));
    }
};

class Physics {
    std::vector<StaticCollider*> staticColliders;
    std::vector<Projectile*> projectiles;
    glm::vec3 gravity = glm::vec3(0.0f, -10.0f, 0.0f);

public:
    void Update(float deltaTime);
    void AddCollider(StaticCollider* coll) { staticColliders.push_back(coll); }
    //void AddCuboidCollider(CuboidCollider* coll) {
    //    for (int i = 0; i < 2; i++) {
    //        QuadCollider* face = coll->faces[i];
    //        staticColliders.push_back(face);
    //    }
    //}

    void AddProjectile(Projectile* proj) {
        projectiles.push_back(proj);
        proj->sceneGraphNode->SetUpdateCallback(new PhysicsCallback(proj->sceneGraphNode, proj));
    }

    std::vector<ContactInfo> CollideWithWorld(const glm::vec3& spherePos, float sphereRad) const;
};
*/

#pragma once
#include "stdafx.h"
#include "TransformNode.h"

struct ContactInfo {
    glm::vec3 normal;
    float depth;
};

struct Projectile
{
    glm::vec3 pos;
    glm::vec3 vel;
    TransformNode* sceneGraphNode;
    Projectile(const glm::vec3& pos, const glm::vec3& vel, TransformNode* sgNode) : pos(pos), vel(vel), sceneGraphNode(sgNode) {}
    ~Projectile()
    {
        // disconnect and delete the scene graph node
        delete sceneGraphNode;
    }
};

class PhysicsCallback : public UpdateCallback
{
    TransformNode* sgNode;
    Projectile* physics;
public:
    PhysicsCallback(TransformNode* sgNode, Projectile* physics) : sgNode(sgNode), physics(physics) {}
    virtual void operator()(float dt)
    {
        // copy the position from one to the other
        sgNode->SetTransform(glm::translate(glm::mat4(), physics->pos));
    }
};

class StaticCollider {
public:
    virtual bool Collide(const glm::vec3& r, float rad, glm::vec3& normal, float& depth) const = 0;
    virtual ~StaticCollider() {}
};

class Physics
{
    std::vector<StaticCollider*> staticColliders;
    std::vector<Projectile*> projectiles;
    glm::vec3 gravity = glm::vec3(0, -10, 0);

public:

    void AddCollider(StaticCollider* coll) { staticColliders.push_back(coll); }
    void AddProjectile(Projectile* proj) {
        projectiles.push_back(proj);
        proj->sceneGraphNode->SetUpdateCallback(new PhysicsCallback(proj->sceneGraphNode, proj));
    }
    std::vector<ContactInfo> CollideWithWorld(const glm::vec3& spherePos, float sphereRad) const;

    void Update(float deltaTime);
};

class QuadCollider : public StaticCollider
{
    glm::vec3 r, a, b;
    glm::vec3 aHat, bHat; // unit vectors along sides a and b
    float lena, lenb; // lengths of sides a and b
public:
    QuadCollider(const glm::vec3& r, const glm::vec3& a, const glm::vec3& b) : r(r), a(a), b(b)
    {
        lena = glm::length(a);
        lenb = glm::length(b);
        aHat = a / lena;
        bHat = b / lenb;
    }
    QuadCollider(glm::mat4 trans)
    {
        // this constructor assumes a unit quad centered on the origin with sides x, z
        // transform transforms this into place
        glm::vec3 rt = trans * glm::vec4(-1.0f, 0.0f, -1.0f, 1.0f);
        glm::vec3 at = trans * glm::vec4(2.0f, 0.0f, 0.0f, 0.0f);
        glm::vec3 bt = trans * glm::vec4(0.0f, 0.0f, 2.0f, 0.0f);
        r = glm::vec3(rt.x, rt.y, rt.z);
        a = glm::vec3(at.x, at.y, at.z);
        b = glm::vec3(bt.x, bt.y, bt.z);
        lena = glm::length(a);
        lenb = glm::length(b);
        aHat = a / lena;
        bHat = b / lenb;
    }
    ~QuadCollider() {}

    bool Collide(const glm::vec3& p, float rad, glm::vec3& normal, float& depth) const override
    {
        float la, lb;
        la = dot(p - r, aHat);
        lb = dot(p - r, bHat);
        if (la < 0.0f) la = 0.0f; if (la > lena) la = lena;
        if (lb < 0.0f)lb = 0.0f; if (lb > lenb) lb = lenb;
        glm::vec3 rNear = r + aHat * la + bHat * lb;

        if (glm::dot(p - rNear, p - rNear) < rad * rad) {
            normal = glm::normalize(p - rNear);
            depth = rad - glm::length(p - rNear);
            return true;
        }
        else
            return false;

    }
};

class MemTexture {
    int width = 0;
    int height = 0;
    unsigned char* image = nullptr;
public:
    glm::vec4 Sample(const glm::vec2& uv) {
        glm::vec2 uvMod = uv;
        // clamp to 0, 1
        uvMod.x = fmin(1.0f, fmax(uvMod.x, 0.0f));
        uvMod.y = fmin(1.0f, fmax(uvMod.y, 0.0f));
        int i, j;
        i = (int)(width * uvMod.x);
        j = (int)(height * uvMod.y);
        unsigned char r, g, b, a;
        r = image[(j * width + i) * 4 + 0];
        g = image[(j * width + i) * 4 + 1];
        b = image[(j * width + i) * 4 + 2];
        a = image[(j * width + i) * 4 + 3];
        return glm::vec4((float)r / 255.0f, (float)g / 255.0f, (float)b / 255.0f, (float)a / 255.0f);
    }
    MemTexture(const std::string& fileName) {
        image = SOIL_load_image(fileName.c_str(), &width, &height, NULL, SOIL_LOAD_RGBA);
        if (image == NULL) {
            std::cerr << "failed to load texture image from " << fileName << std::endl;
            return;
        }
    }
    ~MemTexture() { SOIL_free_image_data(image); }
};

class TerrainCollider : public StaticCollider {
    MemTexture* memTexture;
    glm::vec3 scales;
public:
    TerrainCollider(const std::string& heightMapFile, const glm::vec3 scales) : scales(scales) {
        // load texture here
    }
    ~TerrainCollider() { delete memTexture; }
    bool Collide(const glm::vec3& p, float rad, glm::vec3& normal, float& depth) const override {
        // use the heightmap to detect collision with a sphere
    }
};