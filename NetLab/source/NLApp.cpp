//
//  SGApp.cpp
//  Network Lab
//
//  This is the root class for your game.  The file main.cpp accesses this class
//  to run the application.  While you could put most of your game logic in
//  this class, we prefer to break the game up into player modes and have a
//  class for each mode.
//
//  Author: Walker White, Aidan Hobler
//  Version: 2/8/22
//
#include "NLApp.h"

using namespace cugl;
using namespace cugl::graphics;

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
void NetApp::onStartup() {
    _assets = AssetManager::alloc();
    _batch  = SpriteBatch::alloc();
    auto cam = OrthographicCamera::alloc(getDisplaySize());
        CULog("Startup");

    // Start-up basic input
#ifdef CU_MOBILE
    Input::activate<Touchscreen>();
#else
    Input::activate<Mouse>();
    Input::get<Mouse>()->setPointerAwareness(Mouse::PointerAwareness::DRAG);
#endif
    Input::activate<Keyboard>();
    Input::activate<TextInput>();

    _assets->attach<Font>(FontLoader::alloc()->getHook());
    _assets->attach<Texture>(TextureLoader::alloc()->getHook());
    _assets->attach<JsonValue>(JsonLoader::alloc()->getHook());
    _assets->attach<WidgetValue>(WidgetLoader::alloc()->getHook());
    _assets->attach<scene2::SceneNode>(scene2::Scene2Loader::alloc()->getHook());

    // Create a "loading" screen
    _scene = State::LOAD;
    _assets->loadDirectory("json/loading.json");
    _loading.init(_assets,"json/assets.json");
    _loading.setSpriteBatch(_batch);
    
    // Queue up the other assets
    _loading.start();
    Application::get()->setClearColor(Color4(192,192,192,255));
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
void NetApp::onShutdown() {
    _loading.dispose();
    _mainmenu.dispose();
    _gameplay.dispose();
    _hostgame.dispose();
    _joingame.dispose();
    _analyticsConn=nullptr;
    _assets = nullptr;
    _batch = nullptr;

    // Shutdown input
#ifdef CU_MOBILE
    Input::deactivate<Touchscreen>();
#else
    Input::deactivate<Mouse>();
#endif
    Input::deactivate<TextInput>();
    Input::deactivate<Keyboard>();
    netcode::NetworkLayer::stop();
    Application::onShutdown();  // YOU MUST END with call to parent
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
void NetApp::update(float timestep) {
    // Normally we would make things cleaner than all these if statements.
    // But the logic is fine
    switch (_scene) {
        case LOAD:
            updateLoadingScene(timestep);
            break;
        case MENU:
            updateMenuScene(timestep);
            break;
        case HOST:
            updateHostScene(timestep);
            break;
        case CLIENT:
            updateClientScene(timestep);
            break;
        case GAME:
            updateGameScene(timestep);
            break;
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
void NetApp::draw() {
    switch (_scene) {
        case LOAD:
            _loading.render();
            break;
        case MENU:
            _mainmenu.render();
            break;
        case HOST:
            _hostgame.render();
            break;
        case CLIENT:
            _joingame.render();
            break;
        case GAME:
            _gameplay.render();
            break;
    }
}

/**
 * Inidividualized update method for the loading scene.
 *
 * This method keeps the primary {@link #update} from being a mess of switch
 * statements. It also handles the transition logic from the loading scene.
 *
 * @param timestep  The amount of time (in seconds) since the last frame
 */
void NetApp::updateLoadingScene(float timestep) {
    if (_loading.isActive()) {
        _loading.update(timestep);
    } else {
        _loading.dispose(); // Permanently disables the input listeners in this mode
        netcode::NetworkLayer::start(netcode::NetworkLayer::Log::INFO);
        // Create the analytics server configuration
        auto json = _assets->get<JsonValue>("server");
        _config.set(json->get("analytics server"));
        _analyticsConn = cugl::netcode::analytics::AnalyticsConnection::alloc(_config, this->getOrganization(),  this->getName(), "1.0.0", false);
        _analyticsConn->open();
        _mainmenu.init(_assets, _analyticsConn);
        _mainmenu.setSpriteBatch(_batch);
        _hostgame.init(_assets, _analyticsConn);
        _hostgame.setSpriteBatch(_batch);
        _joingame.init(_assets, _analyticsConn);
        _joingame.setSpriteBatch(_batch);
        _gameplay.init(_assets, _analyticsConn);
        _gameplay.setSpriteBatch(_batch);
        _mainmenu.setActive(true);
        _scene = State::MENU;
    }
}


/**
 * Inidividualized update method for the menu scene.
 *
 * This method keeps the primary {@link #update} from being a mess of switch
 * statements. It also handles the transition logic from the menu scene.
 *
 * @param timestep  The amount of time (in seconds) since the last frame
 */
void NetApp::updateMenuScene(float timestep) {
    _mainmenu.update(timestep);
    switch (_mainmenu.getChoice()) {
        case MenuScene::Choice::HOST:
            _mainmenu.setActive(false);
            _hostgame.setActive(true);
            _scene = State::HOST;
            break;
        case MenuScene::Choice::JOIN:
            _mainmenu.setActive(false);
            _joingame.setActive(true);
            _scene = State::CLIENT;
            break;
        case MenuScene::Choice::NONE:
            // DO NOTHING
            break;
    }
}

/**
 * Inidividualized update method for the host scene.
 *
 * This method keeps the primary {@link #update} from being a mess of switch
 * statements. It also handles the transition logic from the host scene.
 *
 * @param timestep  The amount of time (in seconds) since the last frame
 */
void NetApp::updateHostScene(float timestep) {
    _hostgame.update(timestep);
    switch (_hostgame.getStatus()) {
        case HostScene::Status::ABORT:
            _hostgame.setActive(false);
            _mainmenu.setActive(true);
            _scene = State::MENU;
            break;
        case HostScene::Status::START:
            _hostgame.setActive(false);
            _gameplay.setActive(true);
            _scene = State::GAME;
            // Transfer connection ownership
            _gameplay.setConnection(_hostgame.getConnection());
            _hostgame.disconnect();
            _gameplay.setHost(true);
            break;
        case HostScene::Status::WAIT:
        case HostScene::Status::IDLE:
            // DO NOTHING
            break;
    }
}

/**
 * Inidividualized update method for the client scene.
 *
 * This method keeps the primary {@link #update} from being a mess of switch
 * statements. It also handles the transition logic from the client scene.
 *
 * @param timestep  The amount of time (in seconds) since the last frame
 */
void NetApp::updateClientScene(float timestep) {
    _joingame.update(timestep);
    switch (_joingame.getStatus()) {
        case ClientScene::Status::ABORT:
            _joingame.setActive(false);
            _mainmenu.setActive(true);
            _scene = State::MENU;
            break;
        case ClientScene::Status::START:
            _joingame.setActive(false);
            _gameplay.setActive(true);
            _scene = State::GAME;
            // Transfer connection ownership
            _gameplay.setConnection(_joingame.getConnection());
            _joingame.disconnect();
            _gameplay.setHost(false);
            break;
        case ClientScene::Status::WAIT:
        case ClientScene::Status::IDLE:
        case ClientScene::Status::JOIN:
            // DO NOTHING
            break;
    }
}

/**
 * Inidividualized update method for the game scene.
 *
 * This method keeps the primary {@link #update} from being a mess of switch
 * statements. It also handles the transition logic from the game scene.
 *
 * @param timestep  The amount of time (in seconds) since the last frame
 */
void NetApp::updateGameScene(float timestep) {
    _gameplay.update(timestep);
    if (_gameplay.didQuit()) {
        _gameplay.setActive(false);
        _mainmenu.setActive(true);
        _gameplay.disconnect();
        _scene = State::MENU;
    }
}


