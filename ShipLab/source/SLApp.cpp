//
//  SLApp.cpp
//  Ship Lab
//
//  This is the root class for your game.  The file main.cpp accesses this class
//  to run the application.  While you could put most of your game logic in
//  this class, we prefer to break the game up into player modes and have a
//  class for each mode.
//
//  Author: Walker White
//  Version: 1/20/22
//
#include "SLApp.h"
#include "cugl/netcode/CUAnalyticsConnection.h"

using namespace cugl;
using namespace graphics;
using namespace audio;

#pragma mark -
#pragma mark Gameplay Control

/**
 * The method called after OpenGL is initialized, but before running the application.
 *
 * This is the method in which all user-defined program intialization should
 * take place.  You should not create a new init() method.
 *
 * When overriding this method, you should call the parent method as the
 * very last line.  This ensures that the state will transition to FOREGROUND,
 * causing the application to run.
 */
void ShipApp::onStartup() {
    _assets = AssetManager::alloc();
    _batch  = SpriteBatch::alloc();
    auto cam = OrthographicCamera::alloc(getDisplaySize());
    
    // Start-up basic input (DESKTOP ONLY)
    Input::activate<Mouse>();
    Input::activate<Keyboard>();

    _assets->attach<Texture>(TextureLoader::alloc()->getHook());
    _assets->attach<Sound>(SoundLoader::alloc()->getHook());
    _assets->attach<Font>(FontLoader::alloc()->getHook());
    _assets->attach<JsonValue>(JsonLoader::alloc()->getHook());
    _assets->attach<scene2::SceneNode>(Scene2Loader::alloc()->getHook()); // Needed for loading screen

    // Create a "loading" screen
    _loaded = false;   
    _assets->loadDirectory("json/loading.json");
    _loading.init(_assets,"json/assets.json");
    _loading.setSpriteBatch(_batch);
    _loading.start();
    
    // Queue up the other assets
    _assets->loadDirectoryAsync("json/assets.json",nullptr);
    
    AudioEngine::start();
    Application::onStartup(); // YOU MUST END with call to parent
}

/**
 * The method called when the application is ready to quit.
 *
 * This is the method to dispose of all resources allocated by this
 * application.  As a rule of thumb, everything created in onStartup()
 * should be deleted here.
 *
 * When overriding this method, you should call the parent method as the
 * very last line.  This ensures that the state will transition to NONE,
 * causing the application to be deleted.
 */
void ShipApp::onShutdown() {
    _loading.dispose();
    _gameplay.dispose();
    _assets = nullptr;
    _batch = nullptr;
    _analyticsConn = nullptr;

    // Shutdown input
    Input::deactivate<Keyboard>();
    Input::deactivate<Mouse>();

    AudioEngine::stop();
    Application::onShutdown();  // YOU MUST END with call to parent
}

/**
 * The method called when the application is suspended and put in the background.
 *
 * When this method is called, you should store any state that you do not
 * want to be lost.  There is no guarantee that an application will return
 * from the background; it may be terminated instead.
 *
 * If you are using audio, it is critical that you pause it on suspension.
 * Otherwise, the audio thread may persist while the application is in
 * the background.
 */
void ShipApp::onSuspend() {
    AudioEngine::get()->pause();
}

/**
 * The method called when the application resumes and put in the foreground.
 *
 * If you saved any state before going into the background, now is the time
 * to restore it. This guarantees that the application looks the same as
 * when it was suspended.
 *
 * If you are using audio, you should use this method to resume any audio
 * paused before app suspension.
 */
void ShipApp::onResume() {
    AudioEngine::get()->resume();
}

/**
 * The method called to update the application data.
 *
 * This is your core loop and should be replaced with your custom implementation.
 * This method should contain any code that is not an OpenGL call.
 *
 * When overriding this method, you do not need to call the parent method
 * at all. The default implmentation does nothing.
 *
 * @param timestep  The amount of time (in seconds) since the last frame
 */
void ShipApp::update(float timestep) {
    if (!_loaded && _loading.isActive()) {
        _loading.update(0.01f);
    } else if (!_loaded) {
        _loading.dispose(); // Disables the input listeners in this mode

        // Initializes Analytics Connection
        netcode::NetworkLayer::start(netcode::NetworkLayer::Log::INFO);
        std::shared_ptr<JsonValue> json = _assets->get<JsonValue>("server");

        _config.set(json->get("analytics server"));
        _analyticsConn = cugl::netcode::analytics::AnalyticsConnection::alloc(
                    _config,
         this->getOrganization(),
                 this->getName(),
            "1.0.0",
                      false
        );
        _analyticsConn->addTasks({
         analytics::Task::alloc("Destroy 5 asteroids"),
         analytics::Task::alloc("Destroy 10 asteroids"),
         analytics::Task::alloc("Win game")});
        _gameplay.init(_assets, _analyticsConn);

        _loaded = true;
    } else {
        _gameplay.update(timestep);
    }
}

/**
 * The method called to draw the application to the screen.
 *
 * This is your core loop and should be replaced with your custom implementation.
 * This method should OpenGL and related drawing calls.
 *
 * When overriding this method, you do not need to call the parent method
 * at all. The default implmentation does nothing.
 */
void ShipApp::draw() {
    if (!_loaded) {
        _loading.render();
    } else {
        _gameplay.render(_batch);
    }
}


