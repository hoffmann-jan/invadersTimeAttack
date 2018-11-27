#ifndef ENUMS_H
#define ENUMS_H

typedef enum
{
    KEY_NONE = 0,
    KEY_w = 119,
    KEY_W = 87,
    KEY_a = 97,
    KEY_A = 65,
    KEY_s = 115,
    KEY_S = 83,
    KEY_d = 100,
    KEY_D = 68,
    KEY_Left = 260,
    KEY_Right = 261,
    KEY_Up = 259,
    KEY_Down = 258,
    KEY_Space = 32,
    KEY_ESC = 27

} Key;

typedef enum
{
    UP,
    DOWN,
    LEFT,
    RIGTH
} Direction;

typedef enum
{
    ONE,
    TWO,
    THREE,
    FOUR
} Symbol;

#endif
