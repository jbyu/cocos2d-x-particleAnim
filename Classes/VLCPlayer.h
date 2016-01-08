#ifndef __MOVIEPLAYER_H__
#define __MOVIEPLAYER_H__

#include "vlc/vlc.h"
#include "cocos2d.h"

class VLCPlayer : public cocos2d::Sprite {
public:
	~VLCPlayer();

    static VLCPlayer* create(cocos2d::Size size);

    void o_play(const std::string &path, bool repeat = true);

	inline void o_stop(void);

	inline void o_pause(void);

	inline void o_resume(void);

	inline bool isPlaying(void);

    inline bool isEndReached();

    virtual void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags) override;

    virtual void update(float dt) override;

	friend void endReached(const struct libvlc_event_t *event, void *data);

    friend void *lock(void *data, void **p_pixels);

    friend void unlock(void *data, void *id, void *const *p_pixels);

    friend void display(void *data, void *id);

private:
    bool init(cocos2d::Size &size);

	libvlc_instance_t *vlc;
	libvlc_media_player_t *vlc_player;

	unsigned int width;
	unsigned int height;
	char *m_videobuf;
	bool m_isEndReached;

    std::string m_curMedia;

    bool m_repeat;
    bool m_readyToShow;
};

#endif //__MOVIEPLAYER_H__