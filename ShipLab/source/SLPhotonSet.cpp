//
//  PhotonSet.cpp
//
//  In the intro class we implemented the photons as a "particle system". That
//  is because memory was a tricky resource in Java. While we obviously need
//  to worry about memory in C++, we don't need to use anything as advanced as
//  free lists just yet.  Smart pointers will take care of us!
//
//  With that said, the design of this class is entirely up to you.  Follow
//  the instructions carefully.
//
//  Author:  Kidus Zegeye
//  Version: 1/25/24
//
#include "SLPhotonSet.h"

using namespace cugl;
using namespace graphics;

#define pi           2 * acos(0.0)

#pragma mark Photon

/**
 * Allocates an photon by setting its position and velocity. Contains default age
 *
 * @param p     The position
 * @param v     The velocity
 */
PhotonSet::Photon::Photon(const Vec2 p, const Vec2 v) : Photon(p,v,0,0,1.5) {}

/**
 * Allocates an Photon by setting its position, velocity,  age, and scale
 *
 * @param p     The position
 * @param v     The velocity
 * @param a     The age
 * @param ma   The max age
 * @param s     The scale
 */
PhotonSet::Photon::Photon(const Vec2 p, const Vec2 v, int a, int ma, float s) {
    position = p;
    velocity = v;
    age = a;
    maxage = ma;
    scale = s;
}
/**
 * Moves the photon one animation frame
 */
void PhotonSet::Photon::update(Size size){
    position += velocity;
    while (position.x > size.width) {
        position.x -= size.width;
    }
    while (position.x < 0) {
        position.x += size.width;
    }
    while (position.y > size.height) {
        position.y -= size.height;
    }
    while (position.y < 0) {
        position.y += size.height;
    }
    age++;
    scale = 1.5 - (1.5*age)/maxage;
}


#pragma mark Photon Set

/**
 * Creates a Photon set with default values (for later initialization with the JSON file via init)
 */
PhotonSet::PhotonSet():
speed(0),
mass(0),
radius(0),
maxage(0) {}

/** Loads Photon data from a given JSON file.
 *
 * @return True if initialization was sucessful
 */
bool PhotonSet::init(std::shared_ptr<JsonValue> data) {
    if (data){
        current.clear();
        speed = data->getFloat("speed",0);
        mass = data->getFloat("mass",0);
        maxage = data->getFloat("max age", 0);
        return true;
    }
    return false;
}

/**
 * Sets the image for a single photon; reused by all photons.
 *
 * @param value the image for a single photon; reused by all photon.
 */
void PhotonSet::setTexture(const std::shared_ptr<Texture>& value) {
    if (value){
        Size size = value->getSize();
        radius = std::max(size.width,size.height)/2;
        _texture = value;
        for(auto it = current.begin(); it != current.end(); ++it) {
            std::shared_ptr<Photon> photon = (*it);
            photon->setSprite(SpriteSheet::alloc(value, 1, 1, 1));
            photon->getSprite()->setOrigin(Vec2(radius,radius));
        }
    }
    else{
        radius = 0;
        _texture = nullptr;
    }
}
/**
 * Sets the sprite for a single photon; reused by all photons.
 *
 * @param value the sprite for a single photon; reused by all photon.
 */
void PhotonSet::Photon::setSprite(const std::shared_ptr<SpriteSheet>& sprite){
    _sprite = sprite;
}

/**
 * Adds an photon to the active queue.
 *
 * @param p     The photon position.
 * @param v     The photon velocity.
 */
void PhotonSet::spawnPhoton(Vec2 p, Vec2 v, float angle){
    angle += 90;
    if (angle > 360){
        angle -= 360;
    }
    Vec2 shot_vector = Vec2::forAngle((angle/180) * pi);
    shot_vector.x = shot_vector.x * speed;
    shot_vector.y = shot_vector.y * speed;
    
    std::shared_ptr<Photon> photon = std::make_shared<Photon>(p,v + shot_vector,0,maxage,1.5);
    if(_texture){
        photon->setSprite(SpriteSheet::alloc(_texture,1,1,1));
        photon->getSprite()->setOrigin(Vec2(radius,radius));
    }
    current.emplace(photon);
}

/**
 * Moves all the photons in the active set.
 */
void PhotonSet::update(Size size) {
    auto it = current.begin();
    while (it != current.end()){
        (*it)->update(size);
        if ((*it)->getAge() >= (*it)->getMaxAge()){
            it = current.erase(it);
        }
        else {
            ++it;
        }
    }
}

/**
 * Draws all active photons to the sprite batch within the given bounds.
 *
 * @param batch     The sprite batch to draw to
 * @param size      The size of the window (for wrap around)
 */
void PhotonSet::draw(const std::shared_ptr<SpriteBatch>& batch, Size size){
    if (_texture) {
        for(auto it = current.begin(); it != current.end(); ++it) {
            float scale = (*it)->getScale();
            Vec2 pos = (*it)->position;
            Vec2 origin(radius,radius);

            Affine2 trans;
            trans.scale(scale);
            trans.translate(pos);
            auto sprite = (*it)->getSprite();

            float r = radius*scale;
            sprite->draw(batch,trans);
            if (pos.x+r > size.width) {
                trans.translate(-size.width,0);
                sprite->draw(batch,trans);
                trans.translate(size.width,0);
            } else if (pos.x-r < 0) {
                trans.translate(size.width,0);
                sprite->draw(batch,trans);
                trans.translate(-size.width,0);
            }
            if (pos.y+r > size.height) {
                trans.translate(0,-size.height);
                sprite->draw(batch,trans);
                trans.translate(0,size.height);
            } else if (pos.y-r < 0) {
                trans.translate(0,size.height);
                sprite->draw(batch,trans);
                trans.translate(0,-size.height);
            }
        }
    }
}


