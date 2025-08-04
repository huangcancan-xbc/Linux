#pragma once
#include <iostream>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <cstring>
#include <stdlib.h>
using namespace std;

struct my_msgbuf
{
    long mtype;
    char mtext[1024];
};

const int key = ftok("./comm.hpp", 66);