//
//  PhotonSet.h
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
#ifndef __SL_PHOTON_SET_H__
#define __SL_PHOTON_SET_H__
#include <cugl/cugl.h>
#include <unordered_set>

using namespace cugl;

/**
 * Model class representing photons
 */
class PhotonSet {
#pragma mark Photon
public:
    /**
     * Inner class that represents individual Photons
     */
    class Photon {
    public:
        /** Photon position */
        Vec2 position;
        /** Photon velocity */
        Vec2 velocity;
    private:
        /** Photon drawing scale */
        float scale;
        /** Number of frames that a photon existed for */
        int age;
        /** Max amount of frames a photon can live for before deletion */
        int maxage;
        /** The sprite sheet for the photon */
        std::shared_ptr<graphics::SpriteSheet> _sprite;

        
    public:
        /**
         * Allocates a Photon with position and velocity.
         *
         * @param p    The position
         * @param v    The velocity
         */
        Photon(const Vec2 p, const Vec2 v);
        
        /**
         * Allocates an Photon by setting its position, velocity, age, and scale
         *
         * @param p     The position
         * @param v     The velocity
         * @param a     The age
         * @param ma   The maxage
         * @param s     The scale
         */
        Photon(const Vec2 p, const Vec2 v, int a, int ma, float scale);
        
        /**
         * Returns the position of this photon.
         *
         * @return the position of this photon.
         */
        Vec2 getPosition() const { return position; }
        
        /**
         * Returns the velocity of this photon.
         *
         * @return the velocity of this photon.
         */
        Vec2 getVelocity() const { return velocity; }
        
        /**
         * Returns the scale of this photon.
         *
         * @return the scale of this photon.
         */
        float getScale() const { return scale; }
        
        /**
         * Returns the age of this photon.
         *
         * @return the age of this photon.
         */
        float getAge() const { return age; }
        
        /**
         * Returns the maxage of this photon.
         *
         * @return the maxage of this photon.
         */
        float getMaxAge() const { return maxage; }
        
        /**
         * Moves the photon one animation frame
         */
        void update(Size size);
        
        /** Gets the sprite of a photon to be reused by all photons
         *
         * @return The SpriteSheet of a single photon
         */
        const std::shared_ptr<graphics::SpriteSheet>& getSprite() const {
            return _sprite;
        }
        
        /**
         * Sets the sprite for a single photon; reused by all photons.
         *
         * @param value the sprite for a single photon; reused by all photon.
         */
        void setSprite(const std::shared_ptr<graphics::SpriteSheet>& sprite);
        
    };
    
private:
    /** The texture for a photon. */
    std::shared_ptr<graphics::Texture> _texture;
    
    /** The sprite for a photon. */
    std::shared_ptr<graphics::SpriteSheet> _sprite;
    
    // These values apply for all Photons in a PhotonSet
    /** Initial photon speed */
    float speed;
    /** Mass of a photon */
    float mass;
    /** Radius of a photon */
    float radius;
    /** Max amount of frames a photon can live for before deletion  */
    int maxage;
    
public:
#pragma mark The Set
    /** The collection of all ACTIVE photons */
    std::unordered_set<std::shared_ptr<Photon>> current;
    
    /** Creates set of Photons with default data */
    PhotonSet();
    
    /** Loads Photon data from a given JSON file 
     *
     * @return True if initialization was sucessful
     */
    bool init(std::shared_ptr<JsonValue> data);
    
    /** Gets the image of a photon to be reused by all photons
     *
     * @return The image of a single photon
     */
    const std::shared_ptr<graphics::Texture>& getTexture() const {
        return _texture;
    }
    
    /**
     * Sets the image for a single photon; reused by all photons.
     *
     * @param value the image for a single photon; reused by all photon.
     */
    void setTexture(const std::shared_ptr<graphics::Texture>& value);
    
    /**
     * Returns the speed of the photons.
     *
     * @return the speed of the photons.
     */
    float getSpeed() const { return speed; }
    
    /**
     * Returns the radius of the photons.
     *
     * @return the radius of the photons.
     */
    float getRadius() const { return radius; }
    
    /**
     * Returns the mass of the photons.
     *
     * @return the mass of the photons.
     */
    float getMass() const { return mass; }
    
    /**
     * Returns the max age of the photons.
     *
     * @return the max age of the photons.
     */
    float getMaxAge() const { return maxage; }
    /**
     * Adds an photon to the active queue.
     *
     * @param p     The photon position.
     * @param v     The photon velocity.
     */
    void spawnPhoton(Vec2 p, Vec2 v, float angle);
    
    /**
     * Moves all the photons in the active set.
     */
    void update(Size size);
    
    /**
     * Draws all active photons to the sprite batch within the given bounds.
     *
     * @param batch     The sprite batch to draw to
     * @param size      The size of the window (for wrap around)
     */
    void draw(const std::shared_ptr<graphics::SpriteBatch>& batch, Size size);
};

#endif /* __SL_PHOTON_SET_H__ */
