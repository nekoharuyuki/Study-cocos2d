//
//  Stage.cpp
//  KawazJet
//
//  Created by giginet on 7/9/14.
//
//

#include "KawazJet_Stage.h"

USING_NS_CC;

KawazJetStage::KawazJetStage()
: _level(0)
, _tiledMap(nullptr)
, _player(nullptr)
{
}

KawazJetStage::~KawazJetStage()
{
    CC_SAFE_RELEASE_NULL(_tiledMap);
    CC_SAFE_RELEASE_NULL(_player);
}

KawazJetStage * KawazJetStage::createWithLevel(int level)
{
    KawazJetStage *ret = new KawazJetStage();
    if (ret->initWithLevel(level))
    {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool KawazJetStage::initWithLevel(int level)
{
    if (!Layer::init())
    {
        return false;
    }
    
    // 現在のステージ番号をセット
    _level = level;
    
    auto winSize = Director::getInstance()->getWinSize();
    auto map = TMXTiledMap::create(StringUtils::format("map/stage%d.tmx", level));
    
    if (Director::getInstance()->getContentScaleFactor() == 1.0) {
        map->setScale(0.5);
    }
    
    this->addChild(map);
    
    this->setTiledMap(map);
    
    // 地形レイヤーを取得する
    auto terrainLayer = map->getLayer("Terrain");
    // オブジェクトレイヤーを取得する
    auto objectLayer = map->getLayer("Object");
    
    // マップのサイズ
    auto mapSize = map->getMapSize();
    for (int x = 0; x < mapSize.width; ++x) {
        for (int y = 0; y < mapSize.height; ++y) {
            auto coordinate = Vec2(x, y);
            
            this->addPhysicsBody(terrainLayer, coordinate);
            this->addPhysicsBody(objectLayer, coordinate);
            
        }
    }
    
    // Playerの生成
    auto player = KawazJetPlayer::create();
    player->setPosition(Vec2(100, winSize.height / 2.0));
    this->addChild(player);
    this->setPlayer(player);
    
    // Playerの移動に画面を追従させる
    this->runAction(Follow::create(player, Rect(0,
                                                0,
                                                _tiledMap->getContentSize().width * _tiledMap->getScale(),
                                                winSize.height)));

    this->scheduleUpdate();
    
    return true;
}

void KawazJetStage::update(float dt)
{
}

Sprite* KawazJetStage::addPhysicsBody(cocos2d::TMXLayer *layer, cocos2d::Vec2 &coordinate)
{
    // タイル1枚の大きさを取り出す
    auto tileSize = _tiledMap->getTileSize();
    
    // タイルのスプライトを取り出す
    auto sprite = layer->getTileAt(coordinate);
    if (sprite) {
        // タイルのIDを取り出す
        auto gid = layer->getTileGIDAt(coordinate);
        // タイルのプロパティをmapで取り出す
        auto property = _tiledMap->getPropertiesForGID(gid);
        if (property.isNull() || property.getType() != Value::Type::MAP) {
            return nullptr;
        }
        auto properties = property.asValueMap();
        // プロパティの中からcategoryの値をintとして取り出す
        auto category = properties.at("category").asInt();
        
        auto material = PhysicsMaterial();
        material.friction = 0;
        material.restitution = 0.1;
        
        // 剛体を設置する
        auto physicsBody = PhysicsBody::createBox(sprite->getContentSize(), material);
        // 剛体を固定する
        physicsBody->setDynamic(false);
        // 剛体にカテゴリをセットする
        physicsBody->setCategoryBitmask(category);
        // 剛体と接触判定を取るカテゴリを指定する
        physicsBody->setContactTestBitmask(static_cast<int>(TileType::PLAYER));
        // 剛体と衝突を取るカテゴリを指定する
        physicsBody->setCollisionBitmask(static_cast<int>(TileType::PLAYER));
        
        // アニメーションを付ける
        // プロパティにanimationの値があったら
        if (!properties["animation"].isNull()) {
            auto animationSprite = properties["animation"].asString();
            auto animationCount = properties["animationCount"].asInt();
        
            sprite->removeFromParent();
            this->addChild(sprite);
            
            Vector<SpriteFrame *> frames;
            auto scale = 0.5;
            for (int i = 0; i < animationCount; ++i) {
                auto frame = SpriteFrame::create(animationSprite, Rect(tileSize.width * i * scale, 0, tileSize.width * scale, tileSize.height * scale));
                frames.pushBack(frame);
            }
            auto animation = Animation::createWithSpriteFrames(frames);
            animation->setDelayPerUnit(0.15);
            sprite->runAction(RepeatForever::create(Animate::create(animation)));
        }
        sprite->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
        sprite->setPhysicsBody(physicsBody);
        
        return sprite;
    }
    return nullptr;
}
