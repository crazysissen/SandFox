#pragma once

#define E_FLAG(e, flg) e & flg != 0;

// str - name/identifier of function
#define SINGLEINIT(str) static bool initialized = false; if (initialized) { EXC(std::string("Tried to re-initialize ") + str); } initialized = true;

#define PAD(bytes) char pad_0[bytes];

#define PAD1(bytes) char pad_1[bytes];
#define PAD2(bytes) char pad_2[bytes];
#define PAD3(bytes) char pad_3[bytes];
#define PAD4(bytes) char pad_4[bytes];
#define PAD5(bytes) char pad_5[bytes];