#include "AudioManager.h"
//#include "SimpleAudioEngine.h"
#include "KawazCatchMain.h"
#include "KawazCatchTitle.h"

// namespaceとしてcocos2dを使うことを宣言するマクロ
USING_NS_CC;

//------------------------------

/// フルーツの画面上端からのマージン（px）
const int FRUIT_TOP_MARGIN = 40;
/// フルーツの出現率
/// 約20フレームに一度の頻度で出現する
const int FRUIT_SPAWN_RATE = 20;
/// 制限時間
const float TIME_LIMIT_SECOND = 60.0f;
/// 黄金のフルーツを取得した時のポイント
const int GOLDEN_FRUIT_SCORE = 5;
/// 爆弾を取得した時のマイナスポイント
const int BOMB_PENALTY_SCORE = 4;

//------------------------------

KawazCatchMain::KawazCatchMain()
: _player(NULL)
, _score(0)
, _scoreLabel(NULL)
, _second(TIME_LIMIT_SECOND)
, _secondLabel(NULL)
, _state(GameState::READY)
, _isCrash(false)
, _fruitsBatchNode(NULL)
{
    
}

// デストラクタ
KawazCatchMain::~KawazCatchMain()
{
    // releaseしてメモリリークを防ぐ
    CC_SAFE_RELEASE_NULL(_player);
    CC_SAFE_RELEASE_NULL(_scoreLabel);
    CC_SAFE_RELEASE_NULL(_secondLabel);
    CC_SAFE_RELEASE_NULL(_fruitsBatchNode);
}

Scene* KawazCatchMain::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = KawazCatchMain::create();
    
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool KawazCatchMain::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    // directorを取り出す
    auto director = Director::getInstance();
    
    // 画面サイズを取得する
    auto size = director->getWinSize();
    
    // サーチパスを設定する
    FileUtils::getInstance()->addSearchPath("TestProjects/KawazCatch");
    
    //----------
    // 背景関連
    
    // 背景のスプライトを表示する
    auto background = Sprite::create("image/background.png");
    // スプライトの表示位置を設定する
    background->setPosition(Vec2(size.width / 2.0f, size.height / 2.0f));
    // 親ノードにスプライトを追加する
    this->addChild(background);
    
    //----------
    
    //----------
    // プレイヤー関連
    
    // Spriteを生成して_playerに格納
    this->setPlayer(Sprite::create("image/player.png"));
    // _playerの位置配置
    _player->setPosition(Vec2(size.width / 2.0f, size.height - 445));
    // シーンに_playerを配置する
    this->addChild(_player);
    
    // タッチイベント
    auto listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = [](Touch* touch, Event* event){
        // タッチされた時の処理
        return true;    // イベントを実行する
    };
    listener->onTouchMoved = [this](Touch* touch, Event* event){
        // タッチ中に動いた時の処理
        if(!this->getIsCrash()){    // クラッシュしていない時
            // 前回とのタッチ位置との差をベクトルで取得する
            Vec2 delta = touch->getDelta();
            // 現在のプレイヤーの位置を取得する
            Vec2 position = _player->getPosition();
            // 現在の座標 ＋ 移動量を新たな座標にする
            Vec2 newPosition = position + delta;
            // 画面サイズを取得する
            auto winSize = Director::getInstance()->getWinSize();
            newPosition = newPosition.getClampPoint(Vec2(0, position.y), Vec2(winSize.width, position.y));
            _player->setPosition(newPosition);
        }
    };
    director->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
    
    //----------
    
    //----------
    // スコア関連
    auto scoreLabel = Label::createWithSystemFont(StringUtils::toString(_score), "Marker Felt", 16);
    scoreLabel->setPosition(Vec2(size.width / 2.0f * 1.5f, size.height - FRUIT_TOP_MARGIN));
    // 影を有効にする
    scoreLabel->enableShadow();
    // 赤色、太字5のアウトラインを表示する
    scoreLabel->enableOutline(Color4B::RED, 5);
    this->setScoreLabel(scoreLabel);
    this->addChild(_scoreLabel);
    
    // スコアヘッダーの追加
    auto scoreLabelHeader = Label::createWithSystemFont("SCORE", "Marker Felt", 16);
    scoreLabelHeader->setPosition(Vec2(size.width / 2.0f * 1.5f, size.height - FRUIT_TOP_MARGIN + 20));
    scoreLabelHeader->enableShadow();
    scoreLabelHeader->enableOutline(Color4B::RED, 5);
    this->addChild(scoreLabelHeader);
    
    //----------
    
    //----------
    // タイマー関連
    int second = static_cast<int>(_second);
    auto secondLabel = Label::createWithSystemFont(StringUtils::toString(second), "Marker Felt", 16);
    secondLabel->setPosition(Vec2(size.width / 2.0f, size.height - FRUIT_TOP_MARGIN));
    // 影を有効にする
    secondLabel->enableShadow();
    // 赤色、太字5のアウトラインを表示する
    secondLabel->enableOutline(Color4B::RED, 5);
    this->setSecondLabel(secondLabel);
    this->addChild(secondLabel);
    
    // タイマーヘッダーの追加
    auto secondLabelHeader = Label::createWithSystemFont("TIME", "Marker Felt", 16);
    secondLabelHeader->setPosition(Vec2(size.width / 2.0f, size.height - FRUIT_TOP_MARGIN + 20));
    secondLabelHeader->enableShadow();
    secondLabelHeader->enableOutline(Color4B::RED, 5);
    this->addChild(secondLabelHeader);
    
    //----------
    
    //----------
    // BatchNodeの初期化
    auto fruitsBatchNode = SpriteBatchNode::create("image/fruits.png");
    this->addChild(fruitsBatchNode);
    this->setFruitsBatchNode(fruitsBatchNode);
    
    //----------
    
    // updateを毎フレーム実行するように登録する
    this->scheduleUpdate();
    
    return true;
}

// 毎フレーム実行される
void KawazCatchMain::update(float dt)
{
    // ゲームプレイ中
    if(_state == GameState::PLAYING){
        int random = rand() % FRUIT_SPAWN_RATE;
        if(random == 0){    // 適当な乱数が0の時
            this->addFruit();
        }
        
        /// 当たり判定チェック
        // フルーツをキャラクターがキャッチする
        for (auto& fruit : _fruits){
            Vec2 busketPosition = _player->getPosition() - Vec2(0, 10);
            // フルーツの矩形を取り出す
            Rect boundingBox = fruit->getBoundingBox();
            bool isHit = boundingBox.containsPoint(busketPosition);
            if(isHit){
                this->catchFruit(fruit);
            }
        }
        
        // 残りの秒数を減らす
        _second -= dt;
        // 残り秒数を減らす
        int second = static_cast<int>(_second);
        _secondLabel->setString(StringUtils::toString(second));
        
        // 制限時間になった場合
        if(_second < 0){
            // 終了状態に遷移を移行
            _state = GameState::ENDING;
            // 終了文字を表示
            auto finish = Sprite::create("image/finish.png");
            // 画面サイズを取得する
            auto winSize = Director::getInstance()->getWinSize();
            finish->setPosition(Vec2(winSize.width / 2.0f, winSize.height / 2.0f));
            finish->setScale(0);
            AudioManager::getInstance()->playSe("finish");
            //CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("se/finish.mp3");
            
            // アクションの作成
            auto appear = EaseExponentialIn::create(ScaleTo::create(0.25f, 1.0f));
            auto disappear = EaseExponentialIn::create(ScaleTo::create(0.25f, 0));
            finish->runAction(Sequence::create(appear,
                              DelayTime::create(2.0f),
                              disappear,
                              DelayTime::create(1.0f),
                              CallFunc::create([this]{
                                               _state = GameState::ENDING;     // リザルト状態に遷移を移行
                                               this->onResult();               // メニュー画面を出現させる
                                               }), NULL));
            this->addChild(finish);
        }
    }
}

void KawazCatchMain::onEnterTransitionDidFinish()
{
    // シーンの遷移が完了した時
    Layer::onEnterTransitionDidFinish();
    // BGMを再生する
    AudioManager::getInstance()->playBgm("main");
    //CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("bgm/main.mp3", true);
    // 「READY」演出を行う
    this->addReadyLabel();
}

/** 画面にフルーツを新たに配置して、それを返す
 * @return 新たに作成されたフルーツ
 */
Sprite* KawazCatchMain::addFruit()
{
    // 画面サイズを取得する
    auto winSize = Director::getInstance()->getWinSize();
    // フルーツの種類を選択する
    int fruitType = rand() % static_cast<int>(FruitType::COUNT);
    // フルーツのサイズを取り出す
    auto textureSize = _fruitsBatchNode->getTextureAtlas()->getTexture()->getContentSize();
    // テクスチャの横幅を個数で割った物がフルーツ1個の幅になる
    auto fruitWidth = textureSize.width / static_cast<int>(FruitType::COUNT);
    auto fruit = Sprite::create("image/fruits.png",
                                Rect(fruitWidth * fruitType,
                                     0,
                                     fruitWidth,
                                     textureSize.height));
    
    // フルーツの種類をタグとして指定する
    fruit->setTag(fruitType);
    // フルーツのサイズを取得
    auto fruitSize = fruit->getContentSize();
    // X軸のランダムな位置を選択する
    float fruitXPos = rand() % static_cast<int>(winSize.width);
    fruit->setPosition(Vec2(fruitXPos, winSize.height - FRUIT_TOP_MARGIN));
    // BatchNodeにフルーツを追加する
    _fruitsBatchNode->addChild(fruit);
    _fruits.pushBack(fruit);
    
    // フルーツに動きを付ける
    
    // 地面の座標を取得
    auto ground = Vec2(fruitXPos, 0);
    // 3秒かけてgroundの位置まで落下させるアクション
    auto fall = MoveTo::create(3, ground);
    // removeFruitを即座に呼び出すアクション
    auto remove = CallFuncN::create([this](Node *node){
        // NodeをSpriteにダウンキャストする
        auto sprite = dynamic_cast<Sprite *>(node);
        // removeFruitを呼び出す
        this->removeFruit(sprite);
    });
    // 左右に揺れるアクション
    auto swing = Repeat::create(Sequence::create(RotateTo::create(0.25f, -30), RotateTo::create(0.25f, 30), NULL), 2);
    fruit->setScale(0);
    // fallとremoveを連続して実行させるアクション
    fruit->runAction(Sequence::create(ScaleTo::create(0.25f, 1),
                                      swing,
                                      RotateTo::create(0, 0.125f),
                                      fall,
                                      remove,
                                      NULL));
    
    return fruit;
}

/** マップからフルーツを取り除く
 * @param fruit 削除するフルーツ
 * @return 正しく削除されたか
 */
bool KawazCatchMain::removeFruit(cocos2d::Sprite *fruit)
{
    // _fruitsにfruitが含まれているかを確認する
    if(_fruits.contains(fruit)){
        // 親ノードから削除する
        fruit->removeFromParent();
        // _fruits配列から削除する
        _fruits.eraseObject(fruit);
        return true;
    }
    return false;
}

/** フルーツを取得する
 * @param fruit 取得するフルーツ
 */
void KawazCatchMain::catchFruit(cocos2d::Sprite *fruit)
{
    // もしクラッシュしていたら、フルーツを取得できない
    if(this->getIsCrash()){
        return;
    }
    
    //auto audioEngine = CocosDenshion::SimpleAudioEngine::getInstance();
    
    // フルーツタイプ取得
    FruitType fruitType = static_cast<FruitType>(fruit->getTag());
    switch (fruitType) {
        case KawazCatchMain::FruitType::GOLDEN:
            // 黄金のフルーツの時
            _score += GOLDEN_FRUIT_SCORE;
            AudioManager::getInstance()->playSe("catch_golden");
            //audioEngine->playEffect("se/catch_golden.mp3");
            break;
        case KawazCatchMain::FruitType::BOMB:
            // 爆弾の時
            this->onCatchBomb();
            AudioManager::getInstance()->playSe("bomb");
            //audioEngine->playEffect("se/bomb.mp3");
            break;
        default:
            // その他のフルーツの時
            // スコアを1点追加する
            _score += 1;
            AudioManager::getInstance()->playSe("catch_fruit");
            //audioEngine->playEffect("se/catch_fruit.mp3");
            break;
    }
    
    // フルーツを削除する
    this->removeFruit(fruit);
    
    // スコア用のラベルの表示を更新する
    _scoreLabel->setString(StringUtils::toString(_score));
    
}

/** 爆弾を取得した時の処理
 */
void KawazCatchMain::onCatchBomb()
{
    // クラッシュ状態にする
    _isCrash = true;
    
    // アニメーションの作成
    Vector<SpriteFrame *> frames;
    auto playerSize = _player->getContentSize();
    // アニメーションのフレーム数
    const int animationFrameCount = 3;
    // アニメ用のフレームを読み込む
    for (int ii = 0; ii < animationFrameCount; ii++) {
        auto rect = Rect(playerSize.width * ii, 0, playerSize.width, playerSize.height );
        auto frame = SpriteFrame::create("image/player_crash.png", rect);
        frames.pushBack(frame);
    }
    // アニメーションを作成する
    auto animation = Animation::createWithSpriteFrames(frames, 10.0f / 60.0f);
    // 3回繰り返して再生する
    animation->setLoops(3);
    animation->setRestoreOriginalFrame(true);
    _player->runAction(Sequence::create(Animate::create(animation),
                                        CallFunc::create([this]{_isCrash = false;}),
                                        NULL));
    // 4点引いて0未満になったら0点にする
    _score = MAX(0, _score - BOMB_PENALTY_SCORE);
    AudioManager::getInstance()->playSe("crash");
    //CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("se/crash.mp3");
    
}

/** ゲームが終了した時に呼び出す
 */
void KawazCatchMain::onResult()
{
    _state = GameState::RESULT;
    auto winSize = Director::getInstance()->getWinSize();
    
    // 「もう一度遊ぶ」ボタン
    auto replayButton = MenuItemImage::create("image/replay_button.png",
                                              "image/replay_button_pressed.png",
                                              [](Ref* ref){
                                                  // 「もう一度遊ぶ」ボタンを押した時の処理
                                                  // 新しくKawazCatchMainを作成して置き換える
                                                  auto scene = KawazCatchMain::createScene();
                                                  auto transition = TransitionFade::create(0.5f, scene);
                                                  Director::getInstance()->replaceScene(transition);
                                              });
    // 「タイトルへ戻る」ボタン
    auto titleButton = MenuItemImage::create("image/title_button.png",
                                             "image/title_button_pressed.png",
                                             [](Ref* ref){
                                                  // 「タイトルへ戻る」ボタンを押した時の処理
                                                 AudioManager::getInstance()->stopBgm(0.5f);
                                                 auto scene = KawazCatchTitle::createScene();
                                                 auto transition = TransitionCrossFade::create(1.0f, scene);
                                                 Director::getInstance()->replaceScene(transition);
                                              });
    
    // 2つのボタンからメニューを作成する
    auto menu = Menu::create(replayButton, titleButton, NULL);
    // ボタンを縦に並べる
    menu->alignItemsVerticallyWithPadding(15);
    menu->setPosition(Vec2(winSize.width / 2.0f, winSize.height / 2.0f));
    this->addChild(menu);
    
}

/** ゲーム開始の文字
 */
void KawazCatchMain::addReadyLabel()
{
    auto winSize = Director::getInstance()->getWinSize();
    auto center = Vec2(winSize.width / 2.0f, winSize.height / 2.0f);
    
    // Readyの文字を定義する
    auto ready = Sprite::create("image/ready.png");
    ready->setScale(0); // 最初に大きさを0%にしておく
    ready->setPosition(center);
    this->addChild(ready);
    
    // STARTの文字を定義する
    auto start = Sprite::create("image/start.png");
    start->setScale(0); // 最初に大きさを0%にしておく
    start->setPosition(center);
    
    // STARTにアニメーションを追加する
    // 0.5秒かけて拡大とフェードアウトを同時に行う
    start->runAction(Sequence::create(Spawn::create(EaseIn::create(ScaleTo::create(0.5, 5.0), 0.5),
                                                    FadeOut::create(0.5),
                                                    NULL), // 0.5秒かけて拡大とフェードアウトを同時に行う
                                      RemoveSelf::create(), // 自分を削除する
                                      NULL));
    
    // READYにアニメーションを追加する
    ready->runAction(Sequence::create(ScaleTo::create(0.25f, 1),        // 0.25秒かけて等倍に拡大させる
                                      DelayTime::create(1.0f),          // 1秒待つ
                                      CallFunc::create([this, start]{   // ラムダの中でthisとstart変数を使っているのでキャプチャに加える
        //this->addChild(start);          // 「スタート」のラベルを追加する（この時点でスタートのアニメーションが始まる）
        _state = GameState::PLAYING;    // ゲームの状態をPLAYINGに切り替える
        AudioManager::getInstance()->playSe("start");
        //CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("se/start.mp3");
    }),
                                      RemoveSelf::create(),     // 自分を削除する
                                      NULL));
}
