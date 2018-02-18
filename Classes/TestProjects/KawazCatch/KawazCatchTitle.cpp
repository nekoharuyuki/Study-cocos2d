#include "AudioManager.h"
//#include "SimpleAudioEngine.h"
#include "KawazCatchMain.h"
#include "KawazCatchTitle.h"

// namespaceとしてcocos2dを使うことを宣言するマクロ
USING_NS_CC;

KawazCatchPage::KawazCatchPage()
{
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    // 全ての端末で画面サイズを320*480にする
    glview->setFrameSize(320, 480);
    glview->setDesignResolutionSize(glview->getFrameSize().width, glview->getFrameSize().height, ResolutionPolicy::EXACT_FIT);
    
    //-- KawazCatch --
    // サーチパスを設定する
    FileUtils::getInstance()->addSearchPath("TestProjects/KawazCatch");
    // オーディオ定義ファイルの読み込み
    AudioManager::getInstance()->readAudioListFile("data/audio.json");
    // Device Orientation / Portrait	設定の説明（ホームボタンの位置）/ 縦画面（下）
    auto scene = KawazCatchTitle::createScene();
    // run
    director->replaceScene(scene);
};

//------------------------------------------------------------------

KawazCatchTitle::KawazCatchTitle()
{
}

// デストラクタ
KawazCatchTitle::~KawazCatchTitle()
{
}

Scene* KawazCatchTitle::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    // 'layer' is an autorelease object
    auto layer = KawazCatchTitle::create();
    // add layer as a child to scene
    scene->addChild(layer);
    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool KawazCatchTitle::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() ){
        return false;
    }
    // directorを取り出す
    auto director = Director::getInstance();
    // 画面サイズを取得する
    auto size = director->getWinSize();
    
    //----------
    // 背景関連
    // 背景のスプライトを表示する
    auto background = Sprite::create("image/title_background.png");
    // スプライトの表示位置を設定する
    background->setPosition(Vec2(size.width / 2.0f, size.height / 2.0f));
    // 親ノードにスプライトを追加する
    this->addChild(background);
    
    //----------
    
    //----------
    // ロゴ関連
    // ロゴのスプライトを表示する
    auto logo = Sprite::create("image/title_logo.png");
    // ロゴの位置配置
    logo->setPosition(Vec2(size.width / 2.0f, size.height - 150));
    // シーンにlogoを配置する
    this->addChild(logo);
    //----------
    
    //----------
    // タッチイベント
    // Touch to Startの追加
    auto touchToStart = Sprite::create("image/title_start.png");
    touchToStart->setPosition(Vec2(size.width / 2.0f, 90));
    // 点滅させるアクションの定義
    auto blink = Sequence::create(FadeTo::create(0.5f, 127),
                                  FadeTo::create(0.5f, 255),
                                  NULL);
    touchToStart->runAction(RepeatForever::create(blink));
    this->addChild(touchToStart);
    
    // 画面にタッチした時にメイン画面へ遷移
    auto listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = [this](Touch* touch, Event* event){
        // タッチされた時の処理
        AudioManager::getInstance()->playSe("decide");
        //CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("se/decide.mp3");
        // 何度も押されないように一度押されたらアクションを無効にする
        this->getEventDispatcher()->removeAllEventListeners();
        // 0.5秒待ってからCallFuncを呼ぶ
        auto delay = DelayTime::create(0.5f);
        // ゲームを始めるアクション
        auto startGame = CallFunc::create([]{
            AudioManager::getInstance()->stopBgm(1.5f);
            auto scene = KawazCatchMain::createScene();
            auto transition = TransitionPageTurn::create(0.5f, scene, true);
            Director::getInstance()->replaceScene(transition);
        });
        this->runAction(Sequence::create(delay, startGame, NULL));
        return true;    // イベントを実行する
    };
    //----------
    
    director->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
    return true;
}

void KawazCatchTitle::onEnterTransitionDidFinish()
{
    Layer::onEnterTransitionDidFinish();
    // BGMを再生する
    AudioManager::getInstance()->playBgm("title", 1.5f);
    //CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("bgm/title.mp3", true);
}
