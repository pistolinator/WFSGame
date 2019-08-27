//
//  GFWUtility.cpp
//  OpenGLTest
//
//  Created by Fletcher Wells on 12/10/18.
//  Copyright © 2018 Fletcher Wells. All rights reserved.
//

#include "GFWUtility.hpp"
#include <time.h>

float TimeNow () {
    return glutGet(GLUT_ELAPSED_TIME) / 1000.0;
}

// note that this is a non FEH function
float TimeForRand () {
    return time (NULL);
}
