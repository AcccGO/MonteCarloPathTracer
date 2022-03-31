#include "emitter.h"

bool Emitter::operator==(const Emitter& other) const
{
    return shapeID == other.shapeID;
}