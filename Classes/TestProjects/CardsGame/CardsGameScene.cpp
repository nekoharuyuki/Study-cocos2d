#include "CardsGameScene.h"
#include <random>

USING_NS_CC;
USING_NS_CC_EXT;

//------------------------------

CardsGamePage::CardsGamePage()
{
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    // 全ての端末で画面サイズを960*640にする
    glview->setFrameSize(960, 640);
    auto frameSize = glview->getFrameSize();
    glview->setDesignResolutionSize(frameSize.width, frameSize.height, ResolutionPolicy::SHOW_ALL);
    
    //-- CardsGame --
    // Device Orientation / Landscape Right	設定の説明（ホームボタンの位置）/ 横（右）
    auto scene = CardsGame::createScene();
    
    // run
    director->replaceScene(scene);
};

//------------------------------

Scene* CardsGame::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = CardsGame::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool CardsGame::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() ) {
        return false;
    }
    // サーチパスを設定する
    FileUtils::getInstance()->addSearchPath("TestProjects/CardsGame");
    // シングルタップイベント取得
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(_swallowsTouches);
    //イベント関数の割り当て
    listener->onTouchBegan = CC_CALLBACK_2(CardsGame::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(CardsGame::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(CardsGame::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(CardsGame::onTouchCancelled, this);
    //イベントを追加する
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    // ゲームを初期化する
    initGame();
    return true;
}

void CardsGame::update(float dt)
{
    //時間の積算
    _timer += dt;
    auto timerLabel = (Label*)getChildByTag(TAG_TIMER_LABEL);
    if (timerLabel){
        //時間の表示
        timerLabel->setString(StringUtils::format("%0.2fs", _timer));
    }
    //ゲーム終了判定
    bool finish = true;
    for (int tag = 1; tag <= 10; tag++){
        auto node = getChildByTag(tag);
        if (node){
            //場にカードがある
            finish = false;
            break;
        }
    }
    if (finish){
        //ゲーム終了
        unscheduleUpdate();
    }
}

// カードを初期化する
void CardsGame::initCards()
{
    // カード情報のクリア
    _cards.clear();
    // 4種類　x　13枚分のループ
    for(int type = 0; type < static_cast<int>(CARD_TYP_ENUM); type++){
        for(int number = 1; number <= CARD_NUM; number++){
            // カード情報の作成
            Card card;
            card.number = number;
            card.type = static_cast<CardType>(type);
            // カードを追加する
            _cards.push_back(card);
        }
    }
}

// カードを取得する
Card CardsGame::getCard()
{
    std::random_device rd;
    std::mt19937 rand = std::mt19937(rd());
    // インデックスをランダムに取得する
    int index = std::uniform_int_distribution<int>(0, static_cast<int>(_cards.size()) - 1)(rand);
    // カードの数値を取得する
    auto card = _cards[index];
    // 一時カードから削除する
    _cards.erase(_cards.begin() + index);
    //Zオーダーを変更する
    if (_firstCard){
        _firstCard->setLocalZOrder(ZORDER_SHOW_CARD);
    }
    return card;
}

// カードを作成する
void CardsGame::createCard(PosIndex posIndex)
{
    float posX = CARD_1_POS_X;
    float posY = CARD_1_POS_Y - CARD_DISTANCE_Y;
    // 新しいカードを作成する
    auto card = CardSprite::create();
    card->setCard(getCard());
    card->setPosition(posX, posY);
    card->setPosIndex(posIndex);
    card->moveToInitPos();
    addChild(card, ZORDER_SHOW_CARD);
}

// ゲーム開始時にカードを10枚表示する
void CardsGame::showInitCards()
{
    for(int tag = 1; tag <= 10; tag++){
        auto card = getChildByTag(tag);
        if(card){
            // カードが残っている場合は、削除する
            card->removeFromParent();
        }
    }
    // 5列 x 2行分のループ
    for(int x = 0; x < 5; x++){
        for(int y = 0; y < 2; y++){
            PosIndex posIndex;
            posIndex.x = x;
            posIndex.y = y;
            // カードの生成
            createCard(posIndex);
        }
    }
}

// ゲームの初期化
void CardsGame::initGame()
{
    //裏向いているカードを表示する
    showBackCards();
    //時間を表示する
    showTimerLabel();
    //ボタンを表示する
    showButton();
}

CardSprite* CardsGame::getTouchCard(Touch *touch)
{
    for (int tag = 1; tag <= 10; tag++){
        //表示されているカードを取得する
        auto card = (CardSprite*)getChildByTag(tag);
        if (card && card != _firstCard && card->getBoundingBox().containsPoint(touch->getLocation())){
            //タップされたカードの場合は、そのカードを返す
            return card;
        }
    }
    return nullptr;
}

bool CardsGame::onTouchBegan(Touch *touch, Event *unused_event)
{
    //タップされたカードを取得する
    _firstCard = getTouchCard(touch);
    if (_firstCard){
        //場に出ているカードがタップされた場合
        //Zオーダーを変更する
        _firstCard->setLocalZOrder(ZORDER_MOVING_CARD);
        return true;
    }
    return false;
}

void CardsGame::onTouchMoved(Touch *touch, Event *unused_event)
{
    //スワイプしているカードの位置を変更
    _firstCard->setPosition(_firstCard->getPosition() + touch->getDelta());
}

void CardsGame::onTouchEnded(Touch *touch, Event *unused_event)
{
    bool success = false;
    //タップしたカードの取得
    auto _secondSprite = getTouchCard(touch);
    if (_secondSprite){
        //2枚選択
        if (_firstCard->getCard().number + _secondSprite->getCard().number == 13){
            //2枚のカードを足して13になる
            success = true;
        }
    }
    else{
        //1枚選択
        if (_firstCard->getCard().number == 13){
            //1枚のカードで13になる
            success = true;
        }
    }
    if (success){
        //新しいカードを配置する
        if (static_cast<int>(_cards.size()) > 0){
            createCard(_firstCard->getPosIndex());
        }
        //カードを捨てる
        _firstCard->moveToTrash();
        if (_secondSprite){
            //もう1枚の新しいカードを配置する
            if (static_cast<int>(_cards.size()) > 0){
                createCard(_secondSprite->getPosIndex());
            }
            //カードを捨てる
            _secondSprite->moveToTrash();
        }
        if (static_cast<int>(_cards.size()) <= 0){
            //カードの山を削除する
            removeChildByTag(TAG_BACK_CARD);
        }
    }
    else{
        //カードを元の位置に戻す
        _firstCard->moveBackToInitPos();
        _firstCard->setLocalZOrder(ZORDER_SHOW_CARD);
    }
    //タップしているカードの指定を外す
    _firstCard = nullptr;
}

void CardsGame::onTouchCancelled(Touch *touch, Event *unused_event)
{
    //タップ終了と同じ処理を行う
    onTouchEnded(touch, unused_event);
}

void CardsGame::showButton()
{
    //ボタンを作成する
    auto button = ControlButton::create(Scale9Sprite::create("button.png"));    // TODO
    //画像を引き延ばさない設定
    button->setAdjustBackgroundImage(false);
    //ボタンの位置設定
    button->setPosition(BUTTON_POS_X, BUTTON_POS_Y);
    //ボタンをタップしたときに呼び出す関数の設定
    button->addTargetWithActionForControlEvents(this,
                                                cccontrol_selector(CardsGame::onTapButton),
                                                Control::EventType::TOUCH_UP_INSIDE);
    //ボタンに表示する文字
    button->setTitleForState("Start", Control::State::NORMAL);
    //画面に追加する
    addChild(button);
}

void CardsGame::initTrash()
{
    while (true){
        //ゴミカードがなくなるまでループする
        auto card = getChildByTag(TAG_TRUSH_CARD);
        if (card){
            //ゴミカードが見つかったら削除する
            card->removeFromParent();
        }
        else{
            break;
        }
    }
}

void CardsGame::showBackCards()
{
    auto backCards = getChildByTag(TAG_BACK_CARD);
    if (!backCards){
        //表示されていない場合
        float posX = CARD_1_POS_X;
        float posY = CARD_1_POS_Y - CARD_DISTANCE_Y;
        //カードの山を表示する
        backCards = Sprite::create("card_back.png");
        backCards->setPosition(posX, posY);
        backCards->setTag(TAG_BACK_CARD);
        addChild(backCards);
    }
}

void CardsGame::showTimerLabel()
{
    _timer = 0;
    auto timerLabel = (Label*)getChildByTag(TAG_TIMER_LABEL);
    if (!timerLabel){
        //時間のラベルを表示する
        timerLabel = Label::createWithSystemFont("", "Arial", 48);
        timerLabel->setPosition(TIMER_LABEL_POS_X, TIMER_LABEL_POS_Y);
        timerLabel->setTag(TAG_TIMER_LABEL);
        addChild(timerLabel);
    }
    timerLabel->setString(StringUtils::format("%0.2fs", _timer));
}

void CardsGame::onTapButton(Ref* sender, Control::EventType controlEvent)
{
    //update関数の呼び出しを停止
    unscheduleUpdate();
    //カードを初期化する
    initCards();
    //時間を表示する
    showTimerLabel();
    //カードを表示する
    showInitCards();
    //カードの山を表示する
    showBackCards();
    //ゴミ箱を初期化する
    initTrash();
    //update関数の呼び出しを開始
    scheduleUpdate();
}

//-------------------------------------------------------

// on "init" you need to initialize your instance
bool CardSprite::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Sprite::init() ){
        return false;
    }
    // サーチパスを設定する
    FileUtils::getInstance()->addSearchPath("CardsGame");
    return true;
}

// 表画像ファイル名取得
std::string CardSprite::getFileName(CardType cardtyp)
{
    // ファイル名の取得
    std::string filename;
    switch (cardtyp) {
        case CLUBS:    filename = "card_clubs.png";    break;
        case DIAMONDS: filename = "card_diamonds.png"; break;
        case HEARTS:   filename = "card_hearts.png";   break;
        case SPADES:   filename = "card_spades.png";   break;
        default: break;
    }
    return filename;
}

// カードのマークと数字を表示
void CardSprite::showNumber()
{
    // 表示する数字の取得
    std::string numberString;
    switch (_card.number) {
        case 1:  numberString = "A"; break;
        case 11: numberString = "J"; break;
        case 12: numberString = "Q"; break;
        case 13: numberString = "K"; break;
        default: numberString = StringUtils::format("%d", _card.number); break;
    }
    // 表示する文字の色を取得
    Color4B textColor;
    switch (_card.type) {
        case CLUBS:
        case SPADES:
            textColor = Color4B::BLACK;
            break;
        case DIAMONDS:
        case HEARTS:
            textColor = Color4B::RED;
            break;
        default:
            break;
    }
    // ラベルの表示
    auto number = Label::createWithSystemFont(numberString, "Arial", 96);
    number->setPosition(Point(getContentSize() / 2));
    number->setTextColor(textColor);
    addChild(number);
    
}

// 表示前の処理
void CardSprite::onEnter()
{
    Sprite::onEnter();
    //画像の表示
    setTexture("card_back.png");
    //カードの位置とタグを指定
    setTag(_posIndex.x + _posIndex.y * 5 + 1);
}

// 元の位置に移動する
void CardSprite::moveBackToInitPos()
{
    // カードの位置とタグを指定
    float posX = CARD_1_POS_X + CARD_DISTANCE_X * _posIndex.x;
    float posY = CARD_1_POS_Y + CARD_DISTANCE_Y * _posIndex.y;
    auto move = MoveTo::create(MOVING_TIME, Point(posX, posY));
    // アニメーションの実行
    runAction(move);
}

// カードを捨てる
void CardSprite::moveToTrash()
{
    // カードの位置とタグを指定
    float posX = CARD_1_POS_X + CARD_DISTANCE_X * _posIndex.x;
    float posY = CARD_1_POS_Y + CARD_DISTANCE_Y * _posIndex.y;
    auto move = MoveTo::create(MOVING_TIME, Point(posX, posY));
    // アニメーション後に呼び出す関数の作成
    auto func = CallFunc::create([&](){
        this->setTag(TAG_TRUSH_CARD);
    });
    // アニメーションの直列結合
    auto seq = Sequence::create(move, func, nullptr);
    // アニメーションの実行
    runAction(seq);
}

// 最初の位置に移動する
void CardSprite::moveToInitPos()
{
    //移動アニメーションの作成
    float posX = CARD_1_POS_X + CARD_DISTANCE_X * _posIndex.x;
    float posY = CARD_1_POS_Y + CARD_DISTANCE_Y * _posIndex.y;
    auto move = MoveTo::create(MOVING_TIME, Point(posX, posY));
    //カード回転アニメーションの作成
    auto scale1 = ScaleTo::create(MOVING_TIME / 2, 0, 1);
    auto func1 = CallFunc::create([&](){
        //画像の表示
        setTexture(getFileName(_card.type));
        //数字の表示
        showNumber();
    });
    auto scale2 = ScaleTo::create(MOVING_TIME / 2, 1, 1);
    auto seq1 = Sequence::create(scale1, func1, scale2, nullptr);
    //アクションの並列結合
    auto spawn = Spawn::create(move, seq1, nullptr);
    //アニメーションの実行
    runAction(spawn);
}

//-------------------------------------------------------
