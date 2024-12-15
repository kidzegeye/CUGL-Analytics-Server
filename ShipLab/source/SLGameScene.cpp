//
//  SLGameScene.cpp
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
#include <cugl/cugl.h>
#include <memory>
#include "SLGameScene.h"
#include "cugl/core/assets/CUJsonValue.h"
#include "cugl/netcode/CUAnalyticsConnection.h"

using namespace cugl;
using namespace std;
using namespace graphics;
using namespace netcode;
using namespace analytics;
using namespace audio;
using namespace scene2;

#pragma mark -
#pragma mark Level Layout

// Lock the screen size to fixed height regardless of aspect ratio
#define SCENE_HEIGHT 720

#pragma mark -
#pragma mark Constructors
/**
 * Initializes the controller contents, and starts the game
 *
 * The constructor does not allocate any objects or memory.  This allows
 * us to have a non-pointer reference to this controller, reducing our
 * memory allocation.  Instead, allocation happens in this method.
 *
 * @param assets    The (loaded) assets for this game mode
 * @param analyticsConnection    The connection to the analytics server
 *
 * @return true if the controller is initialized properly, false otherwise.
 */
bool GameScene::init(const std::shared_ptr<AssetManager>& assets, const std::shared_ptr<AnalyticsConnection>& analyticsConnection) {
    // Initialize the scene to a locked width
    std::shared_ptr<AnalyticsConnection> _analyticsConn = analyticsConnection;
    Size dimen = Application::get()->getDisplaySize();
    dimen *= SCENE_HEIGHT/dimen.height;
    if (assets == nullptr) {
        return false;
    } else if (!Scene2::initWithHint(dimen)) {
        return false;
    }
    
    // Start up the input handler
    _assets = assets;
    
    // Get the background image and constant values
    _background = assets->get<Texture>("background");
    _constants = assets->get<JsonValue>("constants");

    // Make a ship and set its texture
    _ship = std::make_shared<Ship>(getSize()/2, _constants->get("ship"));
    _ship->setTexture(assets->get<Texture>("ship"));

    // Initialize the asteroid set
    _asteroids.init(_constants->get("asteroids"));
    _asteroids.setTexture(assets->get<Texture>("asteroid1"));
    
    // Initialize the photon set
    _photons.init(_constants->get("photons"));
    _photons.setTexture(assets->get<Texture>("photon"));

    // Get the bang sound
    _bang = assets->get<Sound>("bang");
    
    // Get the laser sound
    _laser = assets->get<Sound>("laser");
    
    // Get the blast sound
    _blast = assets->get<Sound>("blast");

    // Create and layout the health meter
    std::string msg = strtool::format("Health %d", _ship->getHealth());
    _text = TextLayout::allocWithText(msg, assets->get<Font>("pixel32"));
    _text->layout();
    
    _game_end_text = TextLayout::allocWithText("", assets->get<Font>("pixel32"));
    
    _collisions.init(getSize());

    // Prepare task attempts
    auto tasks = _analyticsConn->getTasks();

    std::shared_ptr<JsonValue> tattempt1_stats = JsonValue::allocWithJson("{\"destroyed\": 0}");
    std::shared_ptr<TaskAttempt> tattempt1 = TaskAttempt::alloc(tasks["Destroy 5 asteroids"], tattempt1_stats);

    std::shared_ptr<JsonValue> tattempt2_stats = JsonValue::allocWithJson("{\"destroyed\": 0}");
    std::shared_ptr<TaskAttempt> tattempt2 = TaskAttempt::alloc(tasks["Destroy 10 asteroids"], tattempt2_stats);

    std::shared_ptr<JsonValue> tattempt3_stats = JsonValue::allocObject();
    std::shared_ptr<TaskAttempt> tattempt3 = TaskAttempt::alloc(tasks["Win game"], tattempt3_stats);

    tattempt1->setStatus(TaskAttempt::Status::PENDING);
    tattempt2->setStatus(TaskAttempt::Status::PENDING);
    tattempt3->setStatus(TaskAttempt::Status::PENDING);

    _taskAttempts = {
        {"Destroy 5 asteroids", tattempt1},
        {"Destroy 10 asteroids", tattempt2},
        {"Win game", tattempt3}
    };

    _analyticsConn->addTaskAttempts({tattempt1,tattempt2,tattempt3});
    
    reset();
    return true;
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void GameScene::dispose() {
    if (_active) {
        removeAllChildren();
        _active = false;
    }
}


#pragma mark -
#pragma mark Gameplay Handling
/**
 * Resets the status of the game so that we can play again.
 */
void GameScene::reset() {
    _ship->setPosition(getSize()/2);
    _ship->setAngle(0);
    _ship->setVelocity(Vec2::ZERO);
    _ship->setHealth(_constants->get("ship")->getInt("health",0));
    _photons.init(_constants->get("photons"));
    _asteroids.init(_constants->get("asteroids"));
    game_status=-1;
    displayed_win_loss_text=false;
}

/**
 * The method called to update the game mode.
 *
 * This method contains any gameplay code that is not an OpenGL call.
 *
 * @param timestep  The amount of time (in seconds) since the last frame
 */
void GameScene::update(float timestep) {
    // Read the keyboard for each controller.
    _input.readInput();
    if (_input.didPressReset()) {
        reset();
    }
    if(game_status == -1){
        // Move the ships and photons forward (ignoring collisions)
        _ship->move( _input.getForward(),  _input.getTurn(), getSize());
        
        // Fire photons if pressed
        if (_input.didPressFire() && _ship->canFireWeapon()){
            _photons.spawnPhoton(_ship->getPosition(), _ship->getVelocity(), _ship->getAngle());
            AudioEngine::get()->play("laser", _laser, false, _laser->getVolume(), true);
            _ship->reloadWeapon();
        }
        else{
            _ship->setRefire(_ship->getRefire()+1);
        }
        
        // Move the asteroids
        _asteroids.update(getSize());
        
        // Move photons
        _photons.update(getSize());
        
        // get taskAttempt objs
        std::shared_ptr<TaskAttempt> taskAttempt1 = _taskAttempts["Destroy 5 asteroids"];
        std::shared_ptr<TaskAttempt> taskAttempt2 = _taskAttempts["Destroy 10 asteroids"];
        std::shared_ptr<TaskAttempt> taskAttempt3 = _taskAttempts["Win game"];
        // Check for collisions and play sound
        if (_collisions.resolveCollision(_ship, _asteroids)) {
            AudioEngine::get()->play("bang", _bang, false, _bang->getVolume(), true);
        }
        if (_collisions.resolveCollision(_photons, _asteroids)) {
            AudioEngine::get()->play("blast", _blast, false, _blast->getVolume(), true);
            // sync the task Attempt here
            std::shared_ptr<JsonValue> stats1 = taskAttempt1->getTaskStatistics();
            std::shared_ptr<JsonValue> stats2 = taskAttempt2->getTaskStatistics();
            CULog("statistics: %s", stats2->toString().c_str());
            long val = stats2->getLong("destroyed") + 1;
            if (!taskAttempt1->hasEnded()) {
                if (val == 5) {
                    taskAttempt1->setStatus(TaskAttempt::Status::SUCCEEDED);
                }
                stats1->get("destroyed")->set(val);
                stats2->get("destroyed")->set(val);
                CULog("statistics: %s", stats2->toString().c_str());             
                taskAttempt1->setTaskStatistics(stats1);
                taskAttempt2->setTaskStatistics(stats2);
                _analyticsConn->syncTaskAttempt(taskAttempt1);
                _analyticsConn->syncTaskAttempt(taskAttempt2);
            }
            else if (!taskAttempt2->hasEnded()) {
                if (val == 10) {
                    taskAttempt2->setStatus(TaskAttempt::Status::SUCCEEDED);
                }
                stats2->get("destroyed")->set(val);
                taskAttempt2->setTaskStatistics(stats2);
                _analyticsConn->syncTaskAttempt(taskAttempt2);
            }

            if (_asteroids.isEmpty()){
                game_status=1;
                taskAttempt3->setStatus(TaskAttempt::Status::SUCCEEDED);
                _analyticsConn->syncTaskAttempt(taskAttempt3);
                return;
            }
        }
        
        // Update the health meter
        _text->setText(strtool::format("Health %d", _ship->getHealth()));
        _text->layout();
        if (_ship->getHealth()==0){
            taskAttempt1->setStatus(TaskAttempt::Status::FAILED);
            taskAttempt2->setStatus(TaskAttempt::Status::FAILED);
            taskAttempt3->setStatus(TaskAttempt::Status::FAILED);
            _analyticsConn->syncTaskAttempt(taskAttempt1);
            _analyticsConn->syncTaskAttempt(taskAttempt2);
            _analyticsConn->syncTaskAttempt(taskAttempt3);
            game_status=0;
        }
    }
}

/**
 * Draws all this scene to the given SpriteBatch.
 *
 * The default implementation of this method simply draws the scene graph
 * to the sprite batch.  By overriding it, you can do custom drawing
 * in its place.
 *
 * @param batch     The SpriteBatch to draw with.
 */
void GameScene::render(const std::shared_ptr<SpriteBatch>& batch) {
    // For now we render 3152-style
    // DO NOT DO THIS IN YOUR FINAL GAME
    batch->begin(getCamera()->getCombined());
    
    batch->draw(_background,Rect(Vec2::ZERO,getSize()));
    _asteroids.draw(batch,getSize());
    _photons.draw(batch,getSize());
    _ship->draw(batch,getSize());
    
    batch->setColor(Color4::BLACK);
    batch->drawText(_text,Vec2(10,getSize().height-_text->getBounds().size.height));
    batch->setColor(Color4::WHITE);
    if(game_status!=-1){
        Affine2 trans;
        trans.scale(3);
        if(game_status==1){
            if(!displayed_win_loss_text){
                _game_end_text = TextLayout::allocWithText("You Win!", _assets->get<Font>("pixel32"));
                _game_end_text->layout();
                displayed_win_loss_text=true;
            }
            trans.translate(Vec2((getSize().width-_game_end_text->getBounds().size.width)/3,(getSize().height-_game_end_text->getBounds().size.height)/2));
            batch->setColor(Color4::GREEN);
            batch->drawText(_game_end_text,trans);
            batch->setColor(Color4::WHITE);
        }
        else if(game_status==0){
            if(!displayed_win_loss_text){
                _game_end_text = TextLayout::allocWithText("You Lost...", _assets->get<Font>("pixel32"));
                _game_end_text->layout();
                displayed_win_loss_text=true;
            }
            trans.translate(Vec2((getSize().width-_game_end_text->getBounds().size.width)/3,(getSize().height-_game_end_text->getBounds().size.height)/2));
            batch->setColor(Color4::RED);
            batch->drawText(_game_end_text,trans);
            batch->setColor(Color4::WHITE);
        }
    }
    batch->end();
}

