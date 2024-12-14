//
//  SLGameScene.h
//  Ship Lab
//
//  This is the primary class file for running the game.  You should study this file
//  for ideas on how to structure your own root class. This class is a reimagining of
//  the first game lab from 3152 in CUGL.
//
//  Author: Walker White
//  Based on original GameX Ship Demo by Rama C. Hoetzlein, 2002
//  Version: 1/20/22
//
#ifndef __SL_GAME_SCENE_H__
#define __SL_GAME_SCENE_H__
#include <cugl/cugl.h>
#include "SLShip.h"
#include "SLPhotonSet.h"
#include "SLAsteroidSet.h"
#include "SLInputController.h"
#include "SLCollisionController.h"
#include "cugl/netcode/CUAnalyticsConnection.h"

using namespace cugl;
using namespace graphics;
using namespace netcode;
using namespace scene2;

/**
 * This class is the primary gameplay constroller for the demo.
 *
 * A world has its own objects, assets, and input controller.  Thus this is
 * really a mini-GameEngine in its own right.  As in 3152, we separate it out
 * so that we can have a separate mode for the loading screen.
 */
class GameScene : public Scene2 {
protected:
    /** The asset manager for this game mode. */
    std::shared_ptr<AssetManager> _assets;
    
    // CONTROLLERS are attached directly to the scene (no pointers)
    /** The controller to manage the ship */
    InputController _input;
    /** The controller for managing collisions */
    CollisionController _collisions;
    
    // MODELS should be shared pointers or a data structure of shared pointers
    /** The JSON value with all of the constants */
    std::shared_ptr<JsonValue> _constants;
    /** Location and animation information for the ship */
    std::shared_ptr<Ship> _ship;
    /** The location of all of the active asteroids */
    AsteroidSet _asteroids;
    /** The location of all of the active photons */
    PhotonSet _photons;
    
    /** Game status: 1 ==> Win, 0 ==> Loss, -1 ==> Not Set */
    int game_status=-1;
    
    /** If the fire button was held down previously */
    bool fired=false;
    
    /** If the win/loss text was already rendered. Done to avoid running layout every render */
    bool displayed_win_loss_text=false;
    
    // VIEW items are going to be individual variables
    // In the future, we will replace this with the scene graph
    /** The backgrounnd image */
    std::shared_ptr<Texture> _background;
    /** The text with the current health */
    std::shared_ptr<TextLayout> _text;
    /** The text with either win or loss text */
    std::shared_ptr<TextLayout> _game_end_text;
    /** The sound of a ship-asteroid collision */
    std::shared_ptr<audio::Sound> _bang;
    /** The sound of a fired photon */
    std::shared_ptr<audio::Sound> _laser;
    /** The sound of a photon-asteroid collision */
    std::shared_ptr<audio::Sound> _blast;
    /** Connection to the analytics server */
    std::shared_ptr<analytics::AnalyticsConnection> _analyticsConn;
    /** Map of tasks to task attempts */
    std::unordered_map<std::string, std::shared_ptr<analytics::TaskAttempt>> _taskAttempts;
    
public:
#pragma mark -
#pragma mark Constructors
    /**
     * Creates a new game mode with the default values.
     *
     * This constructor does not allocate any objects or start the game.
     * This allows us to use the object without a heap pointer.
     */
    GameScene() : Scene2() {}
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     *
     * This method is different from dispose() in that it ALSO shuts off any
     * static resources, like the input controller.
     */
    ~GameScene() { dispose(); }
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     */
    void dispose() override;
    
    /**
     * Initializes the controller contents, and starts the game
     *
     * The constructor does not allocate any objects or memory.  This allows
     * us to have a non-pointer reference to this controller, reducing our
     * memory allocation.  Instead, allocation happens in this method.
     *
     * @param assets                 The (loaded) assets for this game mode
     * @param analyticsConnection    The connection to the analytics server
     *
     *
     * @return true if the controller is initialized properly, false otherwise.
     */
    bool init(const std::shared_ptr<AssetManager>& assets, const std::shared_ptr<analytics::AnalyticsConnection>& analyticsConnection);

    
#pragma mark -
#pragma mark Gameplay Handling
    /**
     * The method called to update the game mode.
     *
     * This method contains any gameplay code that is not an OpenGL call.
     *
     * @param timestep  The amount of time (in seconds) since the last frame
     */
    void update(float timestep) override;

    /**
     * Draws all this scene to the given SpriteBatch.
     *
     * The default implementation of this method simply draws the scene graph
     * to the sprite batch.  By overriding it, you can do custom drawing
     * in its place.
     *
     * @param batch     The SpriteBatch to draw with.
     */
    void render(const std::shared_ptr<SpriteBatch>& batch);

    /**
     * Resets the status of the game so that we can play again.
     */
    void reset() override;
};

#endif /* __SG_GAME_SCENE_H__ */
