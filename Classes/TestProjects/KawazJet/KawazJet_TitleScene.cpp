//
//  TitleScene.cpp
//  KawazJet
//
//  Created by giginet on 2014/8/1.
//
//

#include "cocos2d.h"
#include "KawazJet_TitleScene.h"
#include "KawazJet_MainScene.h"

USING_NS_CC;

KawazJetPage::KawazJetPage()
{
    auto fu = FileUtils::getInstance();
    fu->addSearchPath("TestProjects/KawazJet/");
    
    // 画像リソースを振り分ける
    // 画像リソースを検索する順番を定義する配列
    std::vector<std::string> searchResolutionOrder;
    auto director = Director::getInstance();
    if(director == nullptr){ return; }
    auto glview = director->getOpenGLView();
    auto frameSize = glview->getFrameSize(); // 現在の端末の画面サイズ
    Application::Platform platform = Application::getInstance()->getTargetPlatform();
    if (platform == Application::Platform::OS_IPHONE) { // もし、iPhoneなら
        fu->addSearchPath("music/caf");
        fu->addSearchPath("se/caf");
        if (frameSize.height > 320.f) {
            // Retinaディスプレイのとき
            // 高解像度画像を有効にする
            director->setContentScaleFactor(2.0f);
            if (frameSize.width == 1136) {
                // iPhone 4inchのとき
                // 4インチ対応の画面サイズに変更する
                glview->setDesignResolutionSize(568, 320, ResolutionPolicy::NO_BORDER);
                // Resources/4inchフォルダに画像ファイルがあれば、最優先で利用する
                searchResolutionOrder.push_back("images/4inch");
            } else {
                // Retina 3.5インチのとき
                glview->setDesignResolutionSize(480, 320, ResolutionPolicy::NO_BORDER);
            }
            searchResolutionOrder.push_back("images/retina");
        } else { // non-Retina 3.5インチ
            glview->setDesignResolutionSize(480, 320, ResolutionPolicy::NO_BORDER);
            searchResolutionOrder.push_back("images/nonretina");
        }
    } else if (platform == Application::Platform::OS_IPAD) {
        fu->addSearchPath("music/caf");
        fu->addSearchPath("se/caf");
        if (frameSize.width > 768.f) {
            director->setContentScaleFactor(2.0f);
            searchResolutionOrder.push_back("images/retina");
        } else {
            searchResolutionOrder.push_back("images/nonretina");
        }
        glview->setDesignResolutionSize(568, 320, ResolutionPolicy::SHOW_ALL);
    } else if (platform == Application::Platform::OS_ANDROID) {
        // Android端末のとき
        glview->setDesignResolutionSize(480, 320, ResolutionPolicy::SHOW_ALL);
        fu->addSearchPath("music/ogg");
        fu->addSearchPath("se/ogg");
        searchResolutionOrder.push_back("images/nonretina");
    } else if (platform == Application::Platform::OS_MAC) {
        glview->setDesignResolutionSize(480, 320, ResolutionPolicy::SHOW_ALL);
        fu->addSearchPath("music/caf");
        fu->addSearchPath("se/caf");
        // オーディオ定義ファイルの読み込み
        searchResolutionOrder.push_back("images/nonretina");
    }
    
    // 画像の読み込み順を設定する
    FileUtils::getInstance()->setSearchResolutionsOrder(searchResolutionOrder);
    
    auto scene = KawazJetTitleScene::createScene();
    if(scene == nullptr){ return; }
    
    // run
    director->replaceScene(scene);
};

//------------------------------------------------------------------

Scene* KawazJetTitleScene::createScene()
{
    auto scene = Scene::create();
    auto layer = KawazJetTitleScene::create();
    scene->addChild(layer);
    return scene;
}

KawazJetTitleScene::KawazJetTitleScene()
{
}

KawazJetTitleScene::~KawazJetTitleScene()
{
}

bool KawazJetTitleScene::init()
{
    if (!Layer::init()) {
        return false;
    }
    
    auto winSize = Director::getInstance()->getWinSize();
    auto background = Sprite::create("title.png");
    background->setPosition(Vec2(winSize.width / 2.0, winSize.height / 2.0));
    this->addChild(background);
    
    auto start = Sprite::create("start.png");
    this->addChild(start);
    start->setPosition(Vec2(winSize.width / 2, winSize.height / 4));
    
    // 画面をタッチしたときにメイン画面へ遷移
    auto listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = [this, start](Touch* touch, Event* event) {
        start->setTexture(Director::getInstance()->getTextureCache()->addImage("start_pressed.png"));
        //CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(AudioUtils::getFileName("start").c_str());
        this->getEventDispatcher()->removeAllEventListeners();
        
        this->runAction(Sequence::create(DelayTime::create(1.0),
                                         CallFunc::create([] {
            auto main = KawazJetMainScene::createSceneWithStage(0);
            auto transition = TransitionCrossFade::create(0.5, main);
            
            Director::getInstance()->replaceScene(transition);
        }), NULL));
        
        return true;
    };
    this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
    
    return true;
}

void KawazJetTitleScene::onEnterTransitionDidFinish()
{
    // BGMの再生
    //CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic(AudioUtils::getFileName("title").c_str(), true);
}
