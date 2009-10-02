#ifndef CDRIVER_GRAPHICS_H
#define CDRIVER_GRAPHICS_H

// graphics driver interface
class CDriver_Graphics
{
public:
    virtual ~CDriver_Graphics() { };
    virtual bool displayFrame( void *data ) = 0;
};


// dummy graphics driver
class CDummyDriver_Graphics : public CDriver_Graphics
{
public:
    bool displayFrame( void *data ) { return true; };
};

#endif // CDRIVER_GRAPHICS_H
