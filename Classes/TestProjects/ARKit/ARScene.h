#ifndef __AR_SCENE_H__
#define __AR_SCENE_H__

#include "cocos2d.h"

class ARScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();
    CREATE_FUNC(ARScene);

private:
    cocos2d::Sprite3D* human { nullptr };
    
public:
    virtual bool init();
    
    // MARK:- EventListenerTouchOneByOne
    bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);
};

#endif // __AR_SCENE_H__
