#ifndef __KAWAZCATCH_MAINSCENE_H__
#define __KAWAZCATCH_MAINSCENE_H__

#include "cocos2d.h"

class KawazCatchMain : public cocos2d::Layer
{
protected:
	KawazCatchMain();
	virtual ~KawazCatchMain();
	bool init() override;
	
public:
    static cocos2d::Scene* createScene();
    void update(float dt) override;
    void onEnterTransitionDidFinish() override;
    CREATE_FUNC(KawazCatchMain);
    
private:
    /** ゲームの状態
     * PLAYING : プレイ中
     * RESULT : スコア表示
     */
    enum class GameState
    {
        /// 開始前
        READY,
        /// ゲーム中
        PLAYING,
        /// 終了演出中
        ENDING,
        /// スコア表示
        RESULT,
    };
    
    /** ゲームが終了した時に呼び出す
     */
    void onResult();
    
    /** ゲーム開始の文字
     */
    void addReadyLabel();
    
public:
    // _変数とget()メソッド、set(Sprite *)メソッドが自動的に実装される
    CC_SYNTHESIZE(cocos2d::Vector<cocos2d::Sprite *>, _fruits, Fruits);
    CC_SYNTHESIZE(int, _score, Score);
    CC_SYNTHESIZE(float, _second, Second);
    CC_SYNTHESIZE(GameState, _state, State);
    CC_SYNTHESIZE(bool, _isCrash, IsCrash);
    CC_SYNTHESIZE_RETAIN(cocos2d::Sprite *, _player, Player);
    CC_SYNTHESIZE_RETAIN(cocos2d::Label *, _scoreLabel, ScoreLabel);
    CC_SYNTHESIZE_RETAIN(cocos2d::Label *, _secondLabel, SecondLabel);
    CC_SYNTHESIZE_RETAIN(cocos2d::SpriteBatchNode *, _fruitsBatchNode, FruitsBatchNode);
    
private:
    //フルーツの種類
    enum class FruitType
    {
        APPLE = 0,  // リンゴ
        GRAPE,      // ぶどう
        ORANGE,     // みかん
        BANANA,     // バナナ
        CHERRY,     // さくらんぼ
        GOLDEN,     // 黄金のフルーツ
        BOMB,       // 爆弾
        COUNT,      // 最大値
    };
    
    /** 画面にフルーツを新たに配置して、それを返す
     * @return 新たに作成されたフルーツ
     */
    cocos2d::Sprite* addFruit();
    
    /** マップからフルーツを取り除く
     * @param fruit 削除するフルーツ
     * @return 正しく削除されたか
     */
    bool removeFruit(cocos2d::Sprite *fruit);
    
    /** フルーツを取得する
     * @param fruit 取得するフルーツ
     */
    void catchFruit(cocos2d::Sprite *fruit);
    
    /** 爆弾を取得した時の処理
     */
    void onCatchBomb();
    
};

#endif // __KAWAZCATCH_MAINSCENE_H__
