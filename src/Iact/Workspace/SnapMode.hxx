
#ifndef SnapMode_HXX
#define SnapMode_HXX

// ²¶×½Ä£Ê½Ã¶¾Ù
enum class SnapMode
{
    None,
    Grid,
    Vertex,
    Edge,
    Face
};

DEFINE_ENUM_FLAGS(SnapModes, SnapMode)

#endif // ! SnapMode_HXX
