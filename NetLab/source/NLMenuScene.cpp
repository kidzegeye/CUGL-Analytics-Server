//
//  NLMenuScene.h
//  Network Lab
//
//  This class presents the initial menu scene.  It allows the player to
//  choose to be a host or a client.
//
//  Author: Walker White, Aidan Hobler
//  Version: 2/8/22
//
#include <cugl/cugl.h>
#include <iostream>
#include <sstream>

#include "NLMenuScene.h"

using namespace cugl;
using namespace cugl::netcode::analytics;
using namespace std;

#pragma mark -
#pragma mark Level Layout

/** Regardless of logo, lock the height to this */
#define SCENE_HEIGHT  720


#pragma mark -
#pragma mark Constructors
/**
 * Initializes the controller contents, and starts the game
 *
 * In previous labs, this method "started" the scene.  But in this
 * case, we only use to initialize the scene user interface.  We
 * do not activate the user interface yet, as an active user
 * interface will still receive input EVEN WHEN IT IS HIDDEN.
 *
 * That is why we have the method {@link #setActive}.
 *
 * @param assets    The (loaded) assets for this game mode
 *
 * @return true if the controller is initialized properly, false otherwise.
 */
bool MenuScene::init(const std::shared_ptr<cugl::AssetManager>& assets, const std::shared_ptr<AnalyticsConnection>& analyticsConn) {
    // Initialize the scene to a locked height
    if (assets == nullptr) {
        return false;
    } else if (!Scene2::initWithHint(Size(0,SCENE_HEIGHT))) {
        return false;
    }
    
    // Start up the input handler
    _assets = assets;
    
    // Store the analytics server pointer
    _analyticsConn = analyticsConn;
    // Define tasks for the scene
    // they will be called only once
    std::shared_ptr<Task> task0 = Task::alloc("Host Lobby once.");
    bool status = _analyticsConn->addTask(task0);
    // create a reusable taskStatistic obj
    std::shared_ptr<JsonValue> taskStats = JsonValue::allocObject();
    // JsonValue::alloc(JsonValue::Type::NumberType)) set it to Number Type's default value
    taskStats->appendChild("num_trial", JsonValue::alloc(JsonValue::Type::NumberType));

    std::shared_ptr<TaskAttempt> attempt0 = TaskAttempt::alloc(task0, taskStats);
    attempt0->setStatus(TaskAttempt::Status::PENDING);
    _analyticsConn->addTaskAttempt(attempt0);
    _taskAttempts.push_back(attempt0);

    // Can I reuse the shared_ptr?
    std::shared_ptr<Task> task1 = Task::alloc("Join Lobby 5 times");
    status = _analyticsConn->addTask(task1);
    std::shared_ptr<TaskAttempt> attempt1 = TaskAttempt::alloc(task1, taskStats);
    attempt1->setStatus(TaskAttempt::Status::PENDING);
    _analyticsConn->addTaskAttempt(attempt1);
    _taskAttempts.push_back(attempt1);

    // Create placeholder key possible actions
    _userAction = JsonValue::allocObject();
    _userAction->appendChild("", JsonValue::allocNull());
    // Play around
    _userAction->appendChild("Cody", JsonValue::alloc((std::string)"Cody"));
    
    // Acquire the scene built by the asset loader and resize it the scene
    std::shared_ptr<scene2::SceneNode> scene = _assets->get<scene2::SceneNode>("menu");
    scene->setContentSize(_size);
    scene->doLayout(); // Repositions the HUD
    _choice = Choice::NONE;
    _hostbutton = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("menu.host"));
    _joinbutton = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("menu.join"));
    
    // Program the buttons
    _hostbutton->addListener([this](const std::string& name, bool down) {
        if (down) {
            _choice = Choice::HOST;
            _userAction->get("")->set((std::string)"Hosted a lobby!");
            _analyticsConn->recordAction(_userAction);
            // sync the task Attempt here
            std::shared_ptr<TaskAttempt> taskAtt0 = _taskAttempts[0];
            std::shared_ptr<JsonValue> stats0 = taskAtt0->getTaskStatistics();
            long val = stats0->getLong("num_trial") + 1;
            if (!taskAtt0->hasEnded()) {
                if (val == 1) {
                    taskAtt0->setStatus(TaskAttempt::Status::SUCCEEDED);
                }
                stats0->get("num_trial")->set(val);
                taskAtt0->setTaskStatistics(stats0);
                _analyticsConn->syncTaskAttempt(taskAtt0);
            }
        }
    });
    _joinbutton->addListener([this](const std::string& name, bool down) {
        if (down) {
            _choice = Choice::JOIN;
            _userAction->get("")->set((std::string)"Joined a lobby!");
            _analyticsConn->recordAction(_userAction);
        }
    });

    addChild(scene);
    setActive(false);
    return true;
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void MenuScene::dispose() {
    if (_active) {
        removeAllChildren();
        _active = false;
        for (auto task : _taskAttempts) {
            task = nullptr;
        }
        _taskAttempts.clear();
    }
    
}


/**
 * Sets whether the scene is currently active
 *
 * This method should be used to toggle all the UI elements.  Buttons
 * should be activated when it is made active and deactivated when
 * it is not.
 *
 * @param value whether the scene is currently active
 */
void MenuScene::setActive(bool value) {
    if (isActive() != value) {
        Scene2::setActive(value);
        if (value) {
            _choice = NONE;
            _hostbutton->activate();
            _joinbutton->activate();
        } else {
            _hostbutton->deactivate();
            _joinbutton->deactivate();
            // If any were pressed, reset them
            _hostbutton->setDown(false);
            _joinbutton->setDown(false);
        }
    }
}
