#include "VLCPlayer.h"

USING_NS_CC;

void *lock(void *data, void **p_pixels) {
    auto player = static_cast<VLCPlayer*>(data);
    *p_pixels = player->m_videobuf;
	return NULL;
}

void unlock(void *data, void *id, void *const *p_pixels) {
	assert(id == NULL);
}

void display(void *data, void *id) {
    auto player = static_cast<VLCPlayer*>(data);
    player->m_readyToShow = true;
	assert(id == NULL);
}

void endReached(const struct libvlc_event_t *event, void *data) {
    if (libvlc_MediaPlayerEndReached == event->type) {
        VLCPlayer *self = (VLCPlayer *)data;
        self->m_isEndReached = true;
    }
}

VLCPlayer::~VLCPlayer() {
	free(m_videobuf);
	libvlc_media_player_stop(vlc_player);
	libvlc_media_player_release(vlc_player);
	libvlc_release(vlc);
}

VLCPlayer* VLCPlayer::create(Size size) {
    auto player = new VLCPlayer;
    if (player && player->init(size)) {
        player->autorelease();
    } else {
        CC_SAFE_DELETE(player);
    }
    return player;
}

bool VLCPlayer::init(Size &size) {
	vlc = libvlc_new(0, NULL);
	vlc_player = libvlc_media_player_new(vlc);
	width = size.width;
	height = size.height;
	m_videobuf = (char *)malloc((width * height) << 2);
	memset(m_videobuf, 0, (width * height) << 2);
    libvlc_video_set_callbacks(vlc_player, lock, unlock, display, this);
	libvlc_video_set_format(vlc_player, "RGBA", width, height, width << 2);
    libvlc_event_attach(
		libvlc_media_player_event_manager(vlc_player), 
		libvlc_MediaPlayerEndReached, 
		endReached,
		(void *)this);

	Texture2D *texture = new Texture2D();
	texture->initWithData(m_videobuf, (width * height) << 2, Texture2D::PixelFormat::RGBA8888, width, height, size);
    texture->autorelease();
    initWithTexture(texture);
    scheduleUpdate();
    return true;
}

void VLCPlayer::o_play(const std::string &path, bool repeat) {
    m_isEndReached = false;
    m_curMedia = path;
    m_repeat = repeat;
    m_readyToShow = false;
    libvlc_media_t *media = libvlc_media_new_path(vlc, path.c_str());
    libvlc_media_player_set_media(vlc_player, media);
    libvlc_media_release(media);
    libvlc_media_player_play(vlc_player);
}

void VLCPlayer::o_stop(void)    { libvlc_media_player_stop(vlc_player); }

void VLCPlayer::o_resume(void)  { libvlc_media_player_set_pause(vlc_player, 0); }

void VLCPlayer::o_pause(void)   { libvlc_media_player_pause(vlc_player); }

bool VLCPlayer::isPlaying(void) { return (1 == libvlc_media_player_is_playing(vlc_player)) ? true : false; }

bool VLCPlayer::isEndReached()  { return m_isEndReached; }

void VLCPlayer::draw(Renderer *renderer, const Mat4 &transform, uint32_t flags) {
    _insideBounds = (flags & FLAGS_TRANSFORM_DIRTY) ? renderer->checkVisibility(transform, _contentSize) : _insideBounds;
    if(_insideBounds) {
        if (m_readyToShow) {
            m_readyToShow = false;
			ccGLBindTexture2D( _texture->getName() );  
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, (uint8_t *) m_videobuf);  
        }
        _quadCommand.init(_globalZOrder, _texture->getName(), getGLProgramState(), _blendFunc, &_quad, 1, transform);
        renderer->addCommand(&_quadCommand);

#if CC_SPRITE_DEBUG_DRAW
        _debugDrawNode->clear();
        Vec2 vertices[4] = {
            Vec2( _quad.bl.vertices.x, _quad.bl.vertices.y ),
            Vec2( _quad.br.vertices.x, _quad.br.vertices.y ),
            Vec2( _quad.tr.vertices.x, _quad.tr.vertices.y ),
            Vec2( _quad.tl.vertices.x, _quad.tl.vertices.y ),
        };
        _debugDrawNode->drawPoly(vertices, 4, true, Color4F(1.0, 1.0, 1.0, 1.0));
#endif //CC_SPRITE_DEBUG_DRAW
    }
}

void VLCPlayer::update(float dt) {
    if (m_repeat && isEndReached()) {
        o_play(m_curMedia);
    }
}
