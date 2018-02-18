#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "BaseTest.h"

DEFINE_TEST_SUITE(HelloWorldPage);

class HelloWorldScene : public TestCase
{
public:
    static HelloWorldScene* create();
};

//------------------------------------------------------------------

class HelloWorld : public cocos2d::Layer
{
public:
    HelloWorld(){};
    ~HelloWorld(){};
    
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);
    
    static cocos2d::Scene* createScene();

    virtual bool init() override;
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
    
    virtual void onEnter() override;
    virtual void onEnterTransitionDidFinish() override;
};

#endif // __HELLOWORLD_SCENE_H__
