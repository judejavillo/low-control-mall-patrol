//
//  LCMPThiefModel.h
//  Low Control Mall Patrol
//
//  Author: Kevin Games
//  Version: 2/20/22
//

#ifndef __LCMP_THIEF_MODEL_H__  
#define __LCMP_THIEF_MODEL_H__
#include "LCMPPlayerModel.h"
#include "LCMPConstants.h"

/** Defining the filter bits for the thief model*/
#define THIEF_FILTER_BITS 0b10101

class ThiefModel : public PlayerModel {
public:
//  MARK: - Properties

    /** Flag for whether thief is in range to activate a trap. -1 indicates out of range, otherwise it indicates the id of the trap the thief will activate */
    int trapActivationFlag = -1;


//  MARK: - Constructors
    
    /**
     * Constructs a Thief Model
     */
    ThiefModel() {};
    
    /**
     * Destructs a Thief Model
     */
    ~ThiefModel() { dispose(); }
    
    /**
     * Initializes a Thief Model
     */
    bool init(float scale,
              const std::shared_ptr<cugl::scene2::SceneNode>& node,
              const std::shared_ptr<cugl::AssetManager>& assets,
              std::shared_ptr<cugl::scene2::ActionManager>& actions,
              string skinKey);
    
//  MARK: - Methods
    
    /**
     * Returns the damping constant
     */
    cugl::Vec2 getDamping() override { return cugl::Vec2( THIEF_DAMPING_DEFAULT * _dampingMultiplier.x, THIEF_DAMPING_DEFAULT * _dampingMultiplier.y); }
    
    /**
     * Returns the max speed of the thief
     */
    float getMaxSpeed() override { return THIEF_MAX_SPEED_DEFAULT * _maxSpeedMultiplier; }
    
    /**
     * Returns the acceleration of the thief
     */
    cugl::Vec2 getAcceleration() override { return cugl::Vec2( THIEF_ACCELERATION_DEFAULT * _accelerationMultiplier.x, THIEF_ACCELERATION_DEFAULT * _accelerationMultiplier.y); }
    
};

#endif /* __LCMP_THIEF_MODEL_H__ */
