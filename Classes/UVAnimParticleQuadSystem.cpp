//
//  UVAnimParticleQuadSystem.h
//
//  Created by jbyu on 2016/01/07.
//  Copyright 2016 GEEKSBY. All rights reserved.
//

#include "UVAnimParticleQuadSystem.h"
#include "2d/CCParticleBatchNode.h"
#include "renderer/CCTextureAtlas.h"

NS_CC_BEGIN

UVAnimParticleQuadSystem::~UVAnimParticleQuadSystem() {
	freeResources();
}

UVAnimParticleQuadSystem* UVAnimParticleQuadSystem::create(const std::string& filename)
{
    UVAnimParticleQuadSystem *ret = new UVAnimParticleQuadSystem();
    if (ret && ret->initWithFile(filename)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return ret;
}

void UVAnimParticleQuadSystem::freeResources() {
	if (m_pTexCoords) delete [] m_pTexCoords;
	if (m_pUVRects) delete [] m_pUVRects;
}

void UVAnimParticleQuadSystem::setTextureWithRectForAnimation(Texture2D *texture, const Rect& rect, 
	int tileWidth, int tileHeight, int number_Frames_per_Second, bool NeedsToRemoveParticleAfterAniamtion) {
    m_nItemWidth = tileWidth;
    m_nItemHeight = tileHeight;
    m_bNeedsToRemoveParticleAfterAniamtion = NeedsToRemoveParticleAfterAniamtion;

	if (texture && tileWidth > 0 && tileHeight > 0 && number_Frames_per_Second > 0) {
	    const Size& s = texture->getContentSizeInPixels();
		m_nItemsPerColumn = (int)(s.height / m_nItemHeight);
		m_nItemsPerRow = (int)(s.width / m_nItemWidth);
		m_nFrameCount = m_nItemsPerRow * m_nItemsPerColumn;
        m_fFrameRate = 1.f / number_Frames_per_Second;
		
	    const float inv_wide = 1.f/texture->getPixelsWide();
        const float inv_high = 1.f/texture->getPixelsHigh();

		freeResources();

		const int UVCount = (m_nItemsPerColumn+1) * (m_nItemsPerRow+1);
		m_pTexCoords = new Tex2F[UVCount];
		Tex2F *texPtr = m_pTexCoords;
		// Important. Texture in cocos2d are inverted, so the Y component should be inverted
		for (int j = 0; j <= m_nItemsPerRow; ++j) {
			const float y = (rect.origin.y + m_nItemHeight*j)*inv_high;
			for (int i = 0; i <= m_nItemsPerColumn; ++i) {
				texPtr->u = (rect.origin.x + m_nItemWidth*i)*inv_wide;
				texPtr->v = y;
				++texPtr;
			}
		}

		m_pUVRects = new UVRect[m_nFrameCount];
		UVRect *rectPtr = m_pUVRects;
		for (int j = 0; j < m_nItemsPerRow; ++j) {
			Tex2F *texPtr = m_pTexCoords + j * (m_nItemsPerColumn+1);
			for (int i = 0; i < m_nItemsPerColumn; ++i) {
				Tex2F *ptr = texPtr + i;
				rectPtr->tl = ptr;
				rectPtr->tr = ptr + 1;
				rectPtr->bl = ptr + m_nItemsPerColumn + 1;
				rectPtr->br = ptr + m_nItemsPerColumn + 2;
				++rectPtr;
			}
		}

		ParticleSystemQuad::setTextureWithRect(texture, Rect(rect.origin.x, rect.origin.y, tileWidth, tileHeight));
	} else {
		ParticleSystemQuad::setTextureWithRect(texture, rect);
	}
}

void UVAnimParticleQuadSystem::updateQuadWithParticle(tParticle* particle,  const Vec2& newPosition)  {
    ParticleSystemQuad::updateQuadWithParticle(particle, newPosition);
	if (!m_pUVRects)
		return;

	V3F_C4B_T2F_Quad *quad;
    if (_batchNode) {
        V3F_C4B_T2F_Quad *batchQuads = _batchNode->getTextureAtlas()->getQuads();
        quad = &(batchQuads[_atlasIndex+particle->atlasIndex]);
    } else {
        quad = &(_quads[_particleIdx]);
    }

	while (particle->elapsedTime >= m_fFrameRate) {
		particle->elapsedTime -= m_fFrameRate;
		++particle->frameIndex;
		if (m_nFrameCount <= particle->frameIndex) {
			if (m_bNeedsToRemoveParticleAfterAniamtion) {
				particle->timeToLive = 0;
				return;
			} else {
				particle->frameIndex = 0;
			}
		}
	}
	const UVRect& rect = m_pUVRects[particle->frameIndex];
	quad->bl.texCoords = *rect.bl;
    quad->br.texCoords = *rect.br;
    quad->tl.texCoords = *rect.tl;
    quad->tr.texCoords = *rect.tr;
}
    
NS_CC_END
