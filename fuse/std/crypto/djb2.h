//
// Created by jr on 4/24/23.
//

#ifndef FUSION_DJB2_H
#define FUSION_DJB2_H

unsigned long djb2(unsigned char* str) {
    unsigned long hash = 5381;
    int c;

    while (c = *str++) {
        hash = ((hash << 5) + hash) + c;
    }

    return hash;
}

#endif  //FUSION_DJB2_H
