#include "HelloWorldScene.h"

USING_NS_CC;

HelloWorldPage::HelloWorldPage() {
    ADD_TEST_CASE(HelloWorldScene);
}

HelloWorldScene* HelloWorldScene::create()
{
    auto scene = new (std::nothrow) HelloWorldScene;
    if(scene == nullptr){return nullptr;}
    scene->init();
    scene->addChild(HelloWorld::createScene());
    return scene;
}

//------------------------------------------------------------------
//
// HelloWorld
//
//------------------------------------------------------------------

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    if(scene == nullptr){ return nullptr; }
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();
    if(layer == nullptr){ return nullptr; }
    
    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() ) {
        return false;
    }
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    
    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.
    
    FileUtils::getInstance()->addSearchPath("TestProjects/HelloWorld");
    
    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem = MenuItemImage::create("CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));
    if(closeItem == nullptr){ return false; }
	closeItem->setPosition(VisibleRect::right().x - 30, VisibleRect::top().y - 30);
    
    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, NULL);
    if(menu == nullptr){ return false; }
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu);
    
    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label
    
    auto label = Label::createWithTTF("Hello World", "fonts/Marker Felt.ttf", 24);
    if(label == nullptr){ return false; }
    
    // position the label on the center of the screen
    label->setPosition(Vec2(origin.x + visibleSize.width/2,
                            origin.y + visibleSize.height - label->getContentSize().height));
    
    // add the label as a child to this layer
    this->addChild(label);

    // add "HelloWorld" splash screen"
    auto sprite = Sprite::create("HelloWorld.png");
    if(sprite == nullptr){ return false; }
    
    // position the sprite on the center of the screen
    sprite->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));

    // add the sprite as a child to this layer
    this->addChild(sprite);
    
    // VR Button
    auto button = MenuItemFont::create("Enable / Disable VR", [](Ref* ref){
        auto glview = Director::getInstance()->getOpenGLView();
        auto vrimpl = glview->getVR();
        if (vrimpl){
            glview->setVR(nullptr);
        }
        else{
            auto genericvr = new VRGenericRenderer;
            glview->setVR(genericvr);
        }
    });
    button->setFontSizeObj(16);
    auto menu_vr = Menu::create(button, nullptr);
    menu_vr->setPosition(Vec2(origin.x + button->getContentSize().width/2,
                              origin.y + visibleSize.height - button->getContentSize().height/2));
    this->addChild(menu_vr);
    return true;
}

void HelloWorld::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}

void HelloWorld::onEnter()
{
    Layer::onEnter();
}

void HelloWorld::onEnterTransitionDidFinish()
{
    Layer::onEnterTransitionDidFinish();
}
