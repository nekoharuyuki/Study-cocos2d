#ifndef __CARDSGAME_SCENE_H__
#define __CARDSGAME_SCENE_H__

#include "cocos2d.h"
#include "extensions/cocos-ext.h"
#include "BaseTest.h"

DEFINE_TEST_SUITE(CardsGamePage);

#define CARD_NUM      13    // 1種類あたりのカードの枚数
#define CARD_TYPE_NUM 4     // カードの種類

// カードとボタンの位置関連
#define CARD_1_POS_X 200      // 1番のカード位置（x）
#define CARD_1_POS_Y 320      // 1番のカード位置（y）
#define CARD_DISTANCE_X 140   // カード間の距離（x方向）
#define CARD_DISTANCE_Y 200   // カード間の距離（y方向）
#define BUTTON_POS_X 340      // ボタンの位置（x）
#define BUTTON_POS_Y 120      // ボタンの位置（y）
#define TIMER_LABEL_POS_X 550 // ラベルの位置(x)
#define TIMER_LABEL_POS_Y 120 // ラベルの位置(y)

#define ZORDER_SHOW_CARD   1  // 表示しているカードのZオーダー
#define ZORDER_MOVING_CARD 2  // 移動しているカードのZオーダー

#define TAG_TRUSH_CARD  11    // 捨てられたカードのタグ
#define TAG_BACK_CARD   12    // カードの山のタグ
#define TAG_TIMER_LABEL 13    // 時間ラベルのタグ

#define MOVING_TIME 0.3       // カードのアニメーションの時間

/** カードの種類
 * CLUBS    : クラブ
 * DIAMONDS : ダイヤモンド
 * HEARTS   : ハート
 * SPADES   : スペード
 */
enum CardType
{
    CLUBS = 0,  // クラブ
    DIAMONDS,   // ダイヤモンド
    HEARTS,     // ハート
    SPADES,     // スペード
    CARD_TYP_ENUM,
};

// カード
struct Card
{
    int number;
    CardType type;
};

// カードを表示する位置
struct PosIndex
{
    int x;
    int y;
};

//-------------------------------------------------------

class CardSprite : public cocos2d::Sprite
{
protected:
    // 表画像ファイル名取得
    std::string getFileName(CardType cardtyp);
    // カードのマークと数字を表示
    void showNumber();
    
public:
    // 初期化処理
    virtual bool init() override;
    // 表示前の処理
    void onEnter() override;
    // implement the "static create()" method manually
    CREATE_FUNC(CardSprite);
    
    CC_SYNTHESIZE(Card, _card, Card);               // カードの情報
    CC_SYNTHESIZE(PosIndex, _posIndex, PosIndex);   // 表示位置
    
    void moveBackToInitPos();       // 元の位置に移動する
    void moveToTrash();             // カードを捨てる
    void moveToInitPos();           // 最初の位置に移動する
};

//-------------------------------------------------------

class CardsGame : public cocos2d::Layer
{
protected:
    // カード情報
    std::vector<Card> _cards;
    // 最初にタップされたカード
    CardSprite* _firstCard;
    //経過時間
    float _timer;
    
    // カードを初期化する
    void initCards();
    // カードを取得する
    Card getCard();
    // カードを作成する
    void createCard(PosIndex posIndex);
    // ゲーム開始時にカードを10枚表示する
    void showInitCards();
    // ゲームの初期化
    void initGame();
    // タップされたカードを取得
    CardSprite* getTouchCard(cocos2d::Touch *touch);
    // ボタンを表示する
    void showButton();
    // ゴミカードを初期化する
    void initTrash();
    // ボタンがタップされた時に呼ばれる
    void onTapButton(cocos2d::Ref* sender, cocos2d::extension::Control::EventType controlEvent);
    // カードの山を表示する
    void showBackCards();
    //経過時間を表示
    void showTimerLabel();
    
public:
    static cocos2d::Scene* createScene();
    
    virtual bool init() override;
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
    
    // implement the "static create()" method manually
    CREATE_FUNC(CardsGame);
    
    // タップイベント
    virtual bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *unused_event) override;
    virtual void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *unused_event) override;
    virtual void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *unused_event) override;
    virtual void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *unused_event) override;
    
    //毎フレーム呼ばれる関数
    void update(float dt) override;
};

//-------------------------------------------------------

#endif // __CARDSGAME_SCENE_H__
