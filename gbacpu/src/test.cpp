#include "gbacpu.h"

class DummyMem : public IMemory {
    virtual bool store32( u32 data, u32 address ) {
        return true;
    }

    virtual bool store16( u16 data, u32 address ) {
        return true;
    }

    virtual bool store8( u8 data, u32 address ) {
        return true;
    }

    virtual bool load32( u32 *data, u32 address ) {
        *data = 0;
        return true;
    }

    virtual bool load16( u16 *data, u32 address ) {
        *data = 0;
        return true;
    }

    virtual bool load8( u8 *data, u32 address ) {
        *data = 0;
        return true;
    }
};

int main() {
    DummyMem *mem = new DummyMem();
    Gbacpu *cpu = new Gbacpu( *mem );
    return 0;
}
