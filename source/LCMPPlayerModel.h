//
//  LCMPPlayerModel.h
//  Low Control Mall Patrol
//
//  Author: Kevin Games
//  Version: 2/20/22
//

#ifndef __LCMP_PLAYER_MODEL_H__
#define __LCMP_PLAYER_MODEL_H__
#include <cugl/cugl.h>
#include <cugl/scene2/actions/CUActionManager.h>
#include <cugl/scene2/actions/CUMoveAction.h>
#include <cugl/scene2/actions/CUScaleAction.h>
#include <cugl/scene2/actions/CUAnimateAction.h>
#include <cugl/math/CUEasingBezier.h>

/** Define the time settings for animation */
#define DURATION 0.4f
#define DISTANCE 200
#define WALKPACE 50
#define ACT_KEY  "current"
#define RIGHT_ANIM_KEY 0
#define BACK_ANIM_KEY 1
#define LEFT_ANIM_KEY 2
#define FRONT_ANIM_KEY 3
#define STILL_ANIM_KEY 4
#define CHAR_SCALE 0.2f

class PlayerModel : public cugl::physics2::CapsuleObstacle {
protected:
//  MARK: - Properties
    /** A reference to the Action Manager */
    std::shared_ptr<cugl::scene2::ActionManager> _actions;

    // Views
    /** The top-level node for displaying the player */
    std::shared_ptr<cugl::scene2::SceneNode> _node;
    /** The child node for displaying the player's dropshadow */
    std::shared_ptr<cugl::scene2::PolygonNode> _dropshadow;
    /** The child nodes for displaying the player */
    std::vector<std::shared_ptr<cugl::scene2::SpriteNode>> _spriteNodes;
    /** The player animations */
    std::vector<std::shared_ptr<cugl::scene2::Animate>> _animations;
    /** The player sprite sheets */
    std::vector<std::shared_ptr<cugl::Texture>> _spriteSheets;
    /** The number of animation frames per player animation */
    std::vector<int> _animFrames;
    
    // Physics
    /** A multipler for the damping constant */
    float _dampingMultiplier       = 1.0f;
    /** A multipler for the max speed constant */
    float _maxSpeedMultiplier      = 1.0f;
    /** A multipler for the acceleration constant */
    float _accelerationMultiplier  = 1.0f;
    /** The last force applied to this player */
    cugl::Vec2 _movement;
    
    /** The ratio to scale the textures. (SCENE UNITS / WORLD UNITS) */
    float _scale;
    /** The key for the collision sound */
    std::string _collisionSound;
    /** The key for the obstacle sound */
    std::string _obstacleSound;
    /** Whether we are mid animation */
    bool _occupied;
    /** The unique identifier for this player */
    int _playerNumber;

    
public:
    /** A reference to the sprite showing the player running to the back */
    std::shared_ptr<cugl::scene2::SpriteNode> runBack;
    /** A reference to the sprite showing the player running to the front */
    std::shared_ptr<cugl::scene2::SpriteNode> runFront;
    /** A reference to the sprite showing the player running to the left */
    std::shared_ptr<cugl::scene2::SpriteNode> runLeft;
    /** A reference to the sprite showing the player running to the right */
    std::shared_ptr<cugl::scene2::SpriteNode> runRight;

//  MARK: - Constructors
    
    /**
     * Constructs a Player Model
     */
    PlayerModel() {};
    
    /**
     * Destructs a Player Model
     */
    ~PlayerModel() { dispose(); }
    
    /**
     * Disposes of all resources in this instance of Player Model
     */
    void dispose();
    
    /**
     * Initializes a Player Model
     */
    bool init(int playerNumber, const cugl::Vec2 pos, const cugl::Size size, float scale,
              const std::shared_ptr<cugl::scene2::SceneNode>& node,
              std::shared_ptr<cugl::scene2::ActionManager>& actions);
    
//  MARK: - Methods
    
    /**
     * Gets the appropriate key for the sound for collision
     */
    const std::string& getCollisionSound() const { return _collisionSound; }

    /**
     * Sets the key for a collision sound
     */
    void setCollisionSound(const std::string& key) { _collisionSound = key; }
    
    /**
     * Gets the appropriate key for the sound for collision
     */
    const std::string& getObstacleSound() const { return _obstacleSound; }

    /**
     * Sets the key for a collision sound
     */
    void setObstacleSound(const std::string& key) { _obstacleSound = key; }
    
    /**
     * Returns the node of this player
     */
    std::shared_ptr<cugl::scene2::SceneNode> getNode() { return _node; }
    
    /**
     * Returns the velocity of the player's body
     */
    cugl::Vec2 getVelocity() { return getLinearVelocity(); }
    
    /**
     * Returns the damping constant of this player
     */
    virtual float getDamping() { return 10.0f; }
    
    /**
     * Returns the max speed of this player
     */
    virtual float getMaxSpeed() { return 10.0f; }

    /**
     * Returns the acceleration of this player
     */
    virtual float getAcceleration() { return 10.0f; }
    
    /**
     * Sets the damping multiplier of this player
     */
    void setDampingMultiplier(float value) {  _dampingMultiplier = value; }
    
    /**
     * Sets the max speed of this player.
     */
    void setMaxSpeedMultiplier(float value) { _maxSpeedMultiplier = value; }
    
    /**
     * Sets the acceleration of this player.
     */
    void setAccelerationMultiplier(float value) { _accelerationMultiplier = value; }
    
    /**
     * Applies a force to the player (most likely for local updates)
     */
    void applyForce(cugl::Vec2 force);
    
    /**
     * Updates the position and velocity of the player (most likely for network updates)
     */
    void applyNetwork(cugl::Vec2 position, cugl::Vec2 velocity, cugl::Vec2 force);
    
    /**
     * Updates the player node based on this player's body
     */
    void update(float timestep) override;
    
    /**
     * Performs a film strip action
     */
    void playAnimation();
    
    /**
     * Returns player animation key
     */
    int findDirection(cugl::Vec2 movement);
    
    /**
     * Sets sprite nodes for animation
     */
    void setSpriteNodes(float width);
    
};

#endif /* __LCMP_PLAYER_MODEL_H__ */
