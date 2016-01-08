//
//  UVAnimParticleQuadSystem.h
//
//  Created by jbyu on 2016/01/07.
//  Copyright 2016 GEEKSBY. All rights reserved.
//
#ifndef __UV_ANIM_PARTICLE_QUAD_SYSTEM_H__
#define __UV_ANIM_PARTICLE_QUAD_SYSTEM_H__

#include "2d/CCParticleSystemQuad.h"

namespace cocos2d {
    class UVAnimParticleQuadSystem : public ParticleSystemQuad {
	protected:
        int	m_nItemWidth;
        int	m_nItemHeight;
        int m_nItemsPerRow;
        int m_nItemsPerColumn;
		int m_nFrameCount;
        bool m_bNeedsToRemoveParticleAfterAniamtion;
        float m_fFrameRate;
		Tex2F *m_pTexCoords;

		struct UVRect {
			Tex2F *bl;
			Tex2F *br;
			Tex2F *tl;
			Tex2F *tr;
		} *m_pUVRects;

		void freeResources();

    public:
        UVAnimParticleQuadSystem()
        :m_nItemWidth(0)
        ,m_nItemHeight(0)
		,m_nItemsPerRow(0)
        ,m_nItemsPerColumn(0)
		,m_nFrameCount(0)
        ,m_bNeedsToRemoveParticleAfterAniamtion(false)
		,m_fFrameRate(0)
		,m_pTexCoords(NULL)
		,m_pUVRects(NULL)
		{}

		virtual ~UVAnimParticleQuadSystem();
        
        void setTextureWithRectForAnimation(Texture2D *texture, const Rect& rect, 
			int tileWidth, int tileHeight, int number_Frames_per_Second, bool NeedsToRemoveParticleAfterAniamtion);

	    virtual void updateQuadWithParticle(tParticle* particle, const Vec2& newPosition) override;

	    static UVAnimParticleQuadSystem* create(const std::string& filename);
    };
    
}// namespace cocos2d

#endif //__UV_ANIM_PARTICLE_QUAD_SYSTEM_H__

