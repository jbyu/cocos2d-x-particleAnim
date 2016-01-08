# cocos2d-x-particleAnim
sprite sheet animation for particle of cocos2d-x

```
	auto *particle = UVAnimParticleQuadSystem::create("particle_texture.plist");
	auto texture = Director::getInstance()->getTextureCache()->addImage("explosion.png");
	if (texture) {
		const Size& s = texture->getContentSize();
		particle->setTextureWithRectForAnimation(texture, Rect(0, 0, s.width, s.height), 64, 64, 25, true);
	}
```