//
// Created by wmj on 18-11-2.
//

#ifndef COMPLIE_UTILITY_H
#define COMPLIE_UTILITY_H

#define ishex(c) (((c) >= '0' && (c) <= '9') || ((c) >= 'a' && (c) <= 'f') || ((c) >= 'A' && (c) <= 'F'))
#define isoct(c) ((c) >= '0' && (c) <= '7')
#define isdec(c) ((c) >= '0' && (c) <= '9')

int hex2dec(char c);
bool isalnumunder(int c);


#endif //COMPLIE_UTILITY_H
