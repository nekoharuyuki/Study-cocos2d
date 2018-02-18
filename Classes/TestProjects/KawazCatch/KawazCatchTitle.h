#ifndef __KAWAZCATCH_TITLESCENE_H__
#define __KAWAZCATCH_TITLESCENE_H__

#include "cocos2d.h"
#include "BaseTest.h"

DEFINE_TEST_SUITE(KawazCatchPage);

class KawazCatchTitle : public cocos2d::Layer
{
protected:
	virtual ~KawazCatchTitle();
	bool init() override;
	
public:
    KawazCatchTitle();
    
    static cocos2d::Scene* createScene();
    void onEnterTransitionDidFinish() override;
    CREATE_FUNC(KawazCatchTitle);
};

#endif // __KAWAZCATCH_TITLESCENE_H__
