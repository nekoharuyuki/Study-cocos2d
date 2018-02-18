//
//  TitleScene.h
//  KawazJet
//
//  Created by giginet on 2014/8/1.
//
//

#ifndef __KawazJet__TitleScene__
#define __KawazJet__TitleScene__

#include "cocos2d.h"

#include "BaseTest.h"

DEFINE_TEST_SUITE(KawazJetPage);

class KawazJetTitleScene :public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();
    
    KawazJetTitleScene();
    virtual ~KawazJetTitleScene();
    
    bool init() override;
    void onEnterTransitionDidFinish() override;
    CREATE_FUNC(KawazJetTitleScene);
};

#endif /* defined(__KawazJet__TitleScene__) */
