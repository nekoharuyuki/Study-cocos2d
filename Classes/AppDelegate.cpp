#include "AppDelegate.h"

#include "CreatorReader.h"
#include "SimpleAudioEngine.h"
#include "Controller.h"
#include "tests.h"

USING_NS_CC;

static cocos2d::Size designResolutionSize = cocos2d::Size(480, 320);
static cocos2d::Size smallResolutionSize = cocos2d::Size(480, 320);
static cocos2d::Size mediumResolutionSize = cocos2d::Size(1024, 768);
static cocos2d::Size largeResolutionSize = cocos2d::Size(2048, 1536);

AppDelegate::AppDelegate()
{
}

AppDelegate::~AppDelegate() 
{
}

//if you want a different context,just modify the value of glContextAttrs
//it will takes effect on all platforms
void AppDelegate::initGLContextAttrs()
{
    //set OpenGL context attributions,now can only set six attributions:
    //red,green,blue,alpha,depth,stencil
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8};

    GLView::setGLContextAttrs(glContextAttrs);
}

// If you want to use packages manager to install more packages, 
// don't modify or remove this function
static int register_all_packages()
{
    return 0; //flag for packages manager
}

bool AppDelegate::applicationDidFinishLaunching() {
    // initialize director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
        glview = GLViewImpl::createWithRect("AppCocos", Rect(0, 0, designResolutionSize.width, designResolutionSize.height));
#else
        glview = GLViewImpl::create("AppCocos");
#endif
        director->setOpenGLView(glview);
    }
    
    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0 / 60);
    
    // Set the design resolution
    glview->setDesignResolutionSize(designResolutionSize.width,
                                    designResolutionSize.height,
                                    ResolutionPolicy::NO_BORDER);
    auto frameSize = glview->getFrameSize();
    // if the frame's height is larger than the height of medium size.
    if (frameSize.height > mediumResolutionSize.height)
    {
        director->setContentScaleFactor(MIN(largeResolutionSize.height/designResolutionSize.height,
                                            largeResolutionSize.width/designResolutionSize.width));
    }
    // if the frame's height is larger than the height of small size.
    else if (frameSize.height > smallResolutionSize.height)
    {
        director->setContentScaleFactor(MIN(mediumResolutionSize.height/designResolutionSize.height,
                                            mediumResolutionSize.width/designResolutionSize.width));
    }
    // if the frame's height is smaller than the height of medium size.
    else
    {
        director->setContentScaleFactor(MIN(smallResolutionSize.height/designResolutionSize.height,
                                            smallResolutionSize.width/designResolutionSize.width));
    }
    
    // turn on display FPS
    // デベッグ表示のON/OFFを設定する
#if COCOS2D_DEBUG
    director->setDisplayStats(true);
#else
    director->setDisplayStats(false);
#endif
    
    register_all_packages();
    
    // Enable Remote Console
#if COCOS2D_DEBUG
    FileUtils::getInstance()->addSearchPath("System");
    auto console = director->getConsole();
    console->listenOnTCP(5678);
    _projectController = TestController::getInstance();
/*
    // CocosCreatorのリソース読み込み
    // TODO : HelloWorldのPageで管理するように作成する
    auto reader = creator::CreatorReader::createWithFilename("creator/scenes/Main.ccreator");
    reader->setup();
    auto scene = reader->getSceneGraph();
    // Canvasを取得
    auto canvas = scene->getChildByName("Canvas");
    // タイトルロゴを取得
    auto title = canvas->getChildByName("splash_title");
    // アニメーションを読み込み
    reader->getAnimationManager()->playAnimationClip(title , "SplashAnim");

    // run
    director->runWithScene(scene);
    */
#else
    //-- HelloWorld --
    auto scene = HelloWorld::createScene();
    // run
    director->replaceScene(scene);
#endif
    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
// バックグラウンド移行後にコールされるメソッド
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();
    
    // if you use SimpleAudioEngine, it must be pause
    //SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
    CocosDenshion::SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
// フォアグラウンド移行直前にコールされるメソッド
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();
    
    // if you use SimpleAudioEngine, it must resume here
    //SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
    CocosDenshion::SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}
