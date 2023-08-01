#ifndef TETRAMINO_TYPE
#define TETRAMINO_TYPE

enum class TetraminoType{
    STRAIGHT=0,
    SQUARE,
    T_SHAPE,
    L_SHAPE,
    J_SHAPE,
    S_SHAPE,
    Z_SHAPE,
    AMOUNT,
    MIN=STRAIGHT,
    MAX=Z_SHAPE,
};

inline bool isValidTetraminoType(TetraminoType test){
    return (TetraminoType::MIN <= test && test <= TetraminoType::MAX);
}

#endif
