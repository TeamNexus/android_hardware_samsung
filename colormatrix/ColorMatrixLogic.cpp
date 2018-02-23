/*
 * Copyright (C) 2018 TeamNexus
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
#define LOG_TAG "libexynoscolormatrix"
#define LOG_NDEBUG 0

#include <stdio.h>

#include <math/mat4.h>
#include <utils/Errors.h>
#include <utils/Log.h>

// device-dependent configurations
#include <hardware/ColorMatrixLogic.h>

// implementation header
#include <surfaceflinger/ColorMatrixLogic.h>

#define FIXUP_RGB_VALUE(val) ((val) < 0 ? 0 : ((val) > 255 ? 255 : (val)))

namespace android {

/**
 * Initializes the logic
 */
ColorMatrixLogic::ColorMatrixLogic() { }

/**
 * Cleans up the logic
 */
ColorMatrixLogic::~ColorMatrixLogic() { }

/**
 * Requests an color-manipulation
 */
int ColorMatrixLogic::update(const mat4 colorMatrix) {
    int ret = NO_ERROR;

    /*
     * The color-matrices are passed in the follow format
     *
     *     [ R - - - ]
     *     [ - G - - ]
     *     [ - - B - ]
     *     [ - - - - ]
     *
     * as an one-based value (meaning 1 is 255, 0.5 is 127.5, etc.)
     */
    int r = FIXUP_RGB_VALUE((int)(colorMatrix[0][0] * 255.0F));
    int g = FIXUP_RGB_VALUE((int)(colorMatrix[1][1] * 255.0F));
    int b = FIXUP_RGB_VALUE((int)(colorMatrix[2][2] * 255.0F));

    ALOGV("dumping color-matrix [ %f %f %f %f ]", colorMatrix[0][0], colorMatrix[0][1], colorMatrix[0][2], colorMatrix[0][3]);
    ALOGV("                     [ %f %f %f %f ]", colorMatrix[1][0], colorMatrix[1][1], colorMatrix[1][2], colorMatrix[1][3]);
    ALOGV("                     [ %f %f %f %f ]", colorMatrix[2][0], colorMatrix[2][1], colorMatrix[2][2], colorMatrix[2][3]);
    ALOGV("                     [ %f %f %f %f ]", colorMatrix[3][0], colorMatrix[3][1], colorMatrix[3][2], colorMatrix[3][3]);

    ALOGV("opening ColorMatrixLogic control file:  %s", TARGET_COLOR_MATRIX_SENSOR_RGB);
    FILE *fd = fopen(TARGET_COLOR_MATRIX_SENSOR_RGB, "w");
    if (fd == nullptr) {
        ALOGE("failed to open ColorMatrixLogic control file: %d (%s)", errno, strerror(errno));
        return INVALID_OPERATION;
    }

    ALOGV("updating color-matrix [ R=%d, G=%d, B=%d ]", r, g, b);
    int rc = fprintf(fd, "%d %d %d\n", r, g, b);
    if (rc < 0) {
        ALOGE("failed to write to ColorMatrixLogic control file: %d (%s)", -rc, strerror(-rc));
        ret = INVALID_OPERATION;
    }

    fclose(fd);
    return ret;
}

}
