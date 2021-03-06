//
//  LCMPHostScene.cpp
//  Low Control Mall Patrol
//
//  Author: Kevin Games
//  Version: 3/31/2022
//

#include <cugl/cugl.h>
#include <iostream>
#include <sstream>

#include "LCMPCustomizeScene.h"

using namespace cugl;
using namespace std;

//  MARK: - Constants

/** Regardless of logo, lock the height to this */
#define SCENE_HEIGHT  720
#define COP_RUN_RIGHT           "ss_cop_right"
#define COP_RUN_RIGHT_F         "ss_cop_right_f"

#define THIEF_RUN_RIGHT         "ss_thief_right"
#define THIEF_RUN_RIGHT_F       "ss_thief_right_f"

#define DURATION 3.0f
#define DISTANCE 200
#define WALKPACE 50
#define CHOICE_COOLDOWN         0.5f


//  MARK: - Constructors

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
bool CustomizeScene::init(const std::shared_ptr<cugl::AssetManager>& assets,
                     std::shared_ptr<NetworkController>& network,
                     std::shared_ptr<AudioController>& audio,
                    std::shared_ptr<scene2::ActionManager>& actions) {
    // Initialize the scene to a locked width
    Size dimen = Application::get()->getDisplaySize();
    dimen *= SCENE_HEIGHT/dimen.height;
    _dimen = dimen;
    _offset = Vec2((_dimen.width-SCENE_WIDTH)/2.0f,(_dimen.height-SCENE_HEIGHT)/2.0f);
    
    // Give up if initialization fails early
    if (assets == nullptr) return false;
    else if (!Scene2::init(dimen)) return false;
    
    // Save the references to managers and controllers
    _assets = assets;
    _network = network;
    _audio = audio;
    _actions = actions;
    
    // Acquire the scene built by the asset loader and resize it the scene
    std::shared_ptr<scene2::SceneNode> scene = _assets->get<scene2::SceneNode>("customize");
    scene->setContentSize(dimen);
    scene->doLayout(); // Repositions the HUD
    
    _skinKeys = {"cat_ears", "propeller_hat", "police_hat", "halo", "plant"};
    
    for (string key : _skinKeys) {
        _accessoriesM[key] = std::dynamic_pointer_cast<scene2::PolygonNode>(assets->get<scene2::SceneNode>("customize_" + key));
        _accessoriesM[key]->setAnchor(Vec2::ANCHOR_CENTER);
        _accessoriesM[key]->setScale(0.5);
        _accessoriesM[key]->setVisible(false);
        _accessoriesF[key] = std::dynamic_pointer_cast<scene2::PolygonNode>(assets->get<scene2::SceneNode>("customize_" + key + "_f"));
        _accessoriesF[key]->setAnchor(Vec2::ANCHOR_CENTER);
        _accessoriesF[key]->setScale(0.5);
        _accessoriesF[key]->setVisible(false);
        _savedPurchases[key] = false;
    }
    
    // Get the interactive UI elements that we need to access later
    _startgame = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("customize_backdrop_start"));
    //_startgame->setPosition(Vec2(SCENE_WIDTH/2 + _offset.x, SCENE_HEIGHT_ADJUST + _offset.y));
    //_startgame->setAnchor(Vec2(0.5,0));
    _backout = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("customize_backdrop_back"));
    _title = std::dynamic_pointer_cast<scene2::Label>(_assets->get<scene2::SceneNode>("customize_backdrop_title"));
    _title->setPositionX(SCENE_WIDTH/2 + _offset.x);
    _title->setAnchor(Vec2(0.5,0.5));
    
//    if (filetool::file_exists(Application::get()->getSaveDirectory() + "purchases.json")) {
//        std::shared_ptr<JsonReader> reader = JsonReader::alloc(Application::get()->getSaveDirectory() + "purchases.json");
//        std::shared_ptr<JsonValue> readValue = reader->readJson();
//        for (string key : _skinKeys) {
//            _savedPurchases[key] = readValue->get(key)->asBool();
//        }
//    }

    _status = Status::IDLE;
    
    // Initialize animation assets
    _thiefKeys = {THIEF_RUN_RIGHT, THIEF_RUN_RIGHT_F};
    for (string key : _skinKeys) {
        _thiefKeys.push_back("ss_thief_" + key);
        _thiefKeys.push_back("ss_thief_" + key + "_f");
        _savedPurchases[key] = false;
    }
    _copKeys = {COP_RUN_RIGHT, COP_RUN_RIGHT_F};
    
    skin = 0;
    skinKey = "";
    _didLeft = false;
    _customTime = 0.0;
    _lastChoice = -CHOICE_COOLDOWN;
    
    _leftButton = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("customize_backdrop_left_button"));
    //_leftButton->setPosition(Vec2(0, SCENE_HEIGHT/2) + _offset);
    _leftButton->setAnchor(Vec2(0.5,0.5));
    _rightButton = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("customize_backdrop_right_button"));
    //_rightButton->setPosition(Vec2(SCENE_WIDTH, SCENE_HEIGHT/2) + _offset);
    _rightButton->setAnchor(Vec2(0.5,0.5));
    _aniFrame = 0;
    _prevTime = 0;

    _lockLeft = std::dynamic_pointer_cast<scene2::PolygonNode>(assets->get<scene2::SceneNode>("customize_left_lock"));
    _lockLeft->setPosition(Vec2(SCENE_WIDTH/4, SCENE_HEIGHT/2) + _offset - Vec2(256 * 0.3, 256 * 0.3));
    _lockLeft->setVisible(true);
    _lockCenter = std::dynamic_pointer_cast<scene2::PolygonNode>(assets->get<scene2::SceneNode>("customize_center_lock"));
    _lockCenter->setPosition(Vec2(SCENE_WIDTH/2, SCENE_HEIGHT/2 + 20) + _offset - Vec2(256 * 0.3, 256 * 0.3));
    _lockCenter->setVisible(true);
    _lockRight = std::dynamic_pointer_cast<scene2::PolygonNode>(assets->get<scene2::SceneNode>("customize_right_lock"));
    _lockRight->setPosition(Vec2(3*SCENE_WIDTH/4, SCENE_HEIGHT/2) + _offset - Vec2(256 * 0.3, 256 * 0.3));
    _lockRight->setVisible(true);
    
    for (int i = 0; i < _thiefKeys.size(); i++) {
        std::string key = "customize_" + _thiefKeys[i];
        _thiefSpriteNodes.push_back(std::dynamic_pointer_cast<scene2::SpriteNode>(assets->get<scene2::SceneNode>(key)));
        _thiefSpriteNodes[i]->setAnchor(Vec2::ANCHOR_CENTER);
        _thiefSpriteNodes[i]->setScale(0.5);
        _thiefSpriteNodes[i]->setVisible(false);
    }
    for (int i = 0; i < _copKeys.size(); i++) {
        std::string key = "customize_" + _copKeys[i];
        _copSpriteNodes.push_back(std::dynamic_pointer_cast<scene2::SpriteNode>(assets->get<scene2::SceneNode>(key)));
        _copSpriteNodes[i]->setAnchor(Vec2::ANCHOR_CENTER);
        _copSpriteNodes[i]->setScale(0.5);
        _copSpriteNodes[i]->setVisible(false);
    }
    
    displaySkins(0);
    
    // Program the buttons
    _backout->addListener([this](const std::string& name, bool down) {
        if (down) {
            _status = Status::ABORT;
            _audio->stopSfx(CLICK_SFX);
            _audio->playSound(_assets, CLICK_SFX, true, 0);
        }
    });

    _startgame->addListener([this](const std::string& name, bool down) {
        if (down) {
            startGame();
            _audio->stopSfx(CLICK_SFX);
            _audio->playSound(_assets, CLICK_SFX, true, 0);
        }
    });
    
    _leftButton->addListener([this](const std::string& name, bool down) {
        if (down) {
            skin -= 1;
        }
    });
    
    _rightButton->addListener([this](const std::string& name, bool down) {
        if (down) {
            skin += 1;
        }
    });
    
    addChild(scene);
    setActive(false, true);
    return true;
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void CustomizeScene::dispose() {
    if (_active) {
        removeAllChildren();
        _active = false;
    }
}

//  MARK: - Methods

/**
 * The method called to update the scene.
 *
 * We need to update this method to constantly talk to the server
 *
 * @param timestep  The amount of time (in seconds) since the last frame
 */
void CustomizeScene::update(float timestep) {
    if (_network->isConnected() && _status != START && _status != ABORT) {
        _network->update();
        switch (_network->getStatus()) {
        case NetworkController::IDLE:
        case NetworkController::CONNECTING:
            displaySkins(timestep);
            _status = IDLE;
            break;
        case NetworkController::WAIT:
            displaySkins(timestep);
            _status = WAIT;
            break;
        case NetworkController::START:
            _status = START;
            break;
        case NetworkController::ABORT:
            _status = ABORT;
            break;
        }
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
void CustomizeScene::setActive(bool value, bool isThief) {
    _isThief = isThief;
    if (isActive() != value) {
        Scene2::setActive(value);
        if (value) {
            _status = IDLE;
            _backout->activate();
            _startgame->activate();
            _leftButton->activate();
            _rightButton->activate();
        } else {
            _startgame->deactivate();
            _backout->deactivate();
            _leftButton->deactivate();
            _rightButton->deactivate();
            // If any were pressed, reset them
            _startgame->setDown(false);
            _backout->setDown(false);
            _leftButton->setDown(false);
            _rightButton->setDown(false);
        }
    }
    Scene2::setActive(value);
}

//  MARK: - Helpers

/**
 * Connects to the game server
 *
 * @return true if the connection was successful
 */
bool CustomizeScene::connect() {
    _network->connect();
    return true;
}

/**
 * Starts the game.
 *
 * This method is called once the requisite number of players have connected.
 * It locks down the room and sends a "start game" message to all other
 * players.
 */
void CustomizeScene::startGame() {
    if (_network->isConnected()) {
        _status = Status::START;
    }
}

/** Displays the skins */
void CustomizeScene::displaySkins(float timestep) {
    // Initialize selected skin
    _keys = _isThief ? _thiefKeys : _copKeys;
    _spriteNodes = _isThief ? _thiefSpriteNodes : _copSpriteNodes;
    
    skin %= _skinKeys.size();
    skinKey = skin < 2 ? _keys[skin] : _skinKeys[(skin - 2)%_skinKeys.size()];
    
    for (int i = 0; i < _thiefKeys.size(); i++) {
        _thiefSpriteNodes[i]->setScale(0.5);
        _thiefSpriteNodes[i]->setVisible(false);
    }
    for (int i = 0; i < _copKeys.size(); i++) {
        _copSpriteNodes[i]->setScale(0.5);
        _copSpriteNodes[i]->setVisible(false);
    }
    for (string key : _skinKeys) {
        _accessoriesF[key]->setVisible(false);
        _accessoriesM[key]->setVisible(false);
    }
    
    skin % 2 == 0 ? _accessories = _accessoriesM : _accessoriesF;
    _spriteNodes[skin]->setVisible(true);
    _spriteNodes[skin]->setPosition(Vec2(SCENE_WIDTH/2, SCENE_HEIGHT/2 + 20) + _offset);
    _spriteNodes[skin]->setScale(0.7);
    bool locked = true;
    locked = skin < 2 ? true : _savedPurchases[_skinKeys[skin]];
    locked ? _lockCenter->setVisible(false) : _lockCenter->setVisible(true);
    _accessories[_skinKeys[skin]]->setVisible(true);
    _accessories[_skinKeys[skin]]->setPosition(_spriteNodes[skin]->getPosition() + Vec2(0,_spriteNodes[skin]->getHeight()/4));

    int idx;
    idx = skin - 1 < 0 ? _keys.size() - 1 : skin - 1;
    idx % 2 == 0 ? _accessories = _accessoriesM : _accessoriesF;
    _spriteNodes[idx]->setVisible(true);
    _spriteNodes[idx]->setPosition(Vec2(SCENE_WIDTH/4, SCENE_HEIGHT/2) + _offset);
    locked = idx < 2 ? true : _savedPurchases[_skinKeys[(idx - 2) % _skinKeys.size()]] ;
    locked ? _lockLeft->setVisible(false) : _lockLeft->setVisible(true);
    _accessories[_skinKeys[(idx - 2) % _skinKeys.size()]]->setVisible(true);
    _accessories[_skinKeys[(idx - 2) % _skinKeys.size()]]->setPosition(_spriteNodes[idx]->getPosition() + Vec2(0,_spriteNodes[idx]->getHeight()/4));
    
    idx = skin + 1 == _keys.size() ? 0 : skin + 1;
    idx % 2 == 0 ? _accessories = _accessoriesM : _accessoriesF;
    _spriteNodes[idx]->setVisible(true);
    _spriteNodes[idx]->setPosition(Vec2(3*SCENE_WIDTH/4, SCENE_HEIGHT/2) + _offset);
    locked = idx < 2 ? true : _savedPurchases[_skinKeys[(idx - 2) % _skinKeys.size()]] ;
    locked ? _lockRight->setVisible(false) : _lockRight->setVisible(true);
    _accessories[_skinKeys[(idx - 2) % _skinKeys.size()]]->setVisible(true);
    _accessories[_skinKeys[(idx - 2) % _skinKeys.size()]]->setPosition(_spriteNodes[idx]->getPosition() + Vec2(0,_spriteNodes[idx]->getHeight()/4));
    
    _prevTime += timestep;
    if (_prevTime >= 0.1) {
        _prevTime = 0;
        int frame = _spriteNodes[skin]->getFrame();
        frame >= 7 ? frame = 0 : frame++;
        _spriteNodes[skin]->setFrame(frame);
        skin - 1 < 0 ? _spriteNodes[_keys.size()-1]->setFrame(frame) : _spriteNodes[skin-1]->setFrame(frame);
        skin + 1 == _keys.size() ? _spriteNodes[0]->setFrame(frame) : _spriteNodes[skin+1]->setFrame(frame);
    }
}
