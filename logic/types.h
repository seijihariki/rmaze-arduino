#ifndef TYPES_H_
#define TYPES_H_

typedef char bool;

struct Move
{
    Move(int _dir, int _cnt)
    {
      dir = _dir;
      cnt = _cnt;
    }
    int dir;
    int cnt;
};

#endif
