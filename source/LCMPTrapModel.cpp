//
//  LCMPTrapModel.cpp
//  Low Control Mall Patrol
//
//  Author: Kevin Games
//  Version: 2/20/22
//

#include "LCMPTrapModel.h"
#include <cugl/cugl.h>

using namespace std;
using namespace cugl;

//  MARK: - Constructors

/**
 * Initializes a Trap Model
 */
bool TrapModel::init(int trapID,
                     const std::shared_ptr<cugl::physics2::SimpleObstacle> thiefArea, const std::shared_ptr<cugl::physics2::SimpleObstacle> copArea,
                     const std::shared_ptr<cugl::physics2::SimpleObstacle> triggerArea_,
                     const std::shared_ptr<cugl::Vec2> triggerPosition,
                     bool copSolid, bool thiefSolid,
                     int numUses,
                     float lingerDur,
                     const std::shared_ptr<cugl::Affine2> thiefVelMod,
                     const std::shared_ptr<cugl::Affine2> copVelMod,
                     bool startsTriggered) {
    _trapID = trapID;
    thiefEffectArea = thiefArea;
    copEffectArea = copArea;
    triggerArea = triggerArea_;
    triggerPos = triggerPosition;
    copCollide = copSolid;
    thiefCollide = thiefSolid;
    usesRemaining = numUses;
    lingerDuration = lingerDur;
    thiefVelocityModifier = thiefVelMod;
    copVelocityModifier = copVelMod;

    activated = false;

    thiefEffectFilter = b2Filter();
    thiefEffectFilter.maskBits = 0b10000;
    thiefEffectFilter.categoryBits = 0b10000;
    thiefEffectArea->setFilterData(thiefEffectFilter);

    copEffectFilter = b2Filter();
    copEffectFilter.maskBits = 0b01000;
    copEffectFilter.categoryBits = 0b01000;
    copEffectArea->setFilterData(copEffectFilter);
    
    triggerFilter = b2Filter();
    triggerFilter.maskBits = 0b00100;
    triggerFilter.categoryBits = 0b00100;
    triggerArea->setFilterData(triggerFilter);

    // TODO: This can cause some problems: if you init before setAssets, activate will be working with nodes that don't exist
    if (startsTriggered) activate();

    return true;
}

//  MARK: - Methods

/**
 * Sets all of the assets for this trap
 */
void TrapModel::setAssets(float scale,
                          const std::shared_ptr<cugl::scene2::SceneNode>& node,
                          const std::shared_ptr<cugl::AssetManager>& assets,
                          TrapType type) {
    // Get the textures
    _triggerTexture = assets->get<Texture>(getTriggerKey(type));
    _triggerActivatedTexture = assets->get<Texture>(getTriggerKey(type));
    _effectAreaTexture = assets->get<Texture>("tree");
    
    // Create nodes
    _triggerNode = scene2::PolygonNode::allocWithTexture(_triggerTexture);
    _triggerActivatedNode = scene2::PolygonNode::allocWithTexture(_triggerActivatedTexture);
    _effectAreaNode = scene2::PolygonNode::allocWithTexture(_effectAreaTexture);

    // Set the nodes' positions
    _triggerNode->setPosition(triggerPos->x * scale, triggerPos->y * scale);
    _triggerActivatedNode->setPosition(triggerPos->x * scale, triggerPos->y * scale);
    _effectAreaNode->setPosition(thiefEffectArea->getPosition() * scale);
    
    // TODO: these should really be children of a parent node that isn't the world node
    // Add the unactivated trigger node as the child
    _node = node;
    _node->addChild(_triggerNode);
}

/**
 * Sets the debug scene to all of the children nodes
 */
void TrapModel::setDebugScene(const shared_ptr<scene2::SceneNode>& node){
    thiefEffectArea->setDebugScene(node);
    copEffectArea->setDebugScene(node);
    triggerArea->setDebugScene(node);
}

/**
 * Returns true and decrements remaining uses if trap can be used. Returns false and has no effect otherwise.
 */
bool TrapModel::use() {
    if (usesRemaining < 0) return true;
    if (usesRemaining == 0) return false;
    usesRemaining--;
    return true;
}

/**
 * Activates this trap.
 */
void TrapModel::activate(){
    if (activated) return;
    activated = true;
    if (thiefCollide) {
        thiefEffectArea->setSensor(false);
    }
    if (copCollide) {
        copEffectArea->setSensor(false);
    }

    
    // Change which nodes are being shown
    _node->removeChild(_triggerNode);
    _node->addChild(_triggerActivatedNode);
    _node->addChild(_effectAreaNode);
}
