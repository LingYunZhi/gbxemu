#pragma once


class IDisplay {
public:
    IDisplay() { };
    virtual ~IDisplay() { };

    struct VIDEO_MODE
    {
        unsigned int adapter;
        unsigned int width;
        unsigned int height;
        unsigned int bitDepth;
        unsigned int frequency;
    };

    virtual bool initialize( int w, int h ) = 0;
    virtual void cleanup() = 0;

    virtual void render() = 0;
    virtual void clear() = 0;
    virtual bool changeRenderSize(int w, int h) { return true; };
    virtual void resize(int w, int h) {};
    virtual void setOption(const char *option, int value) {};
    virtual bool selectFullScreenMode( VIDEO_MODE &mode ) = 0;
};
