#include "textGL.h"
#include <time.h>

typedef struct {
    unsigned char marginSize;
    char mainselect[4]; // 0 - select, 1 - mouseHover, 2 - selected, 3 - free
    char subselect[4]; // like mainselect but for the sublists (only one sublist supported)
    char output[3]; // 0 - toggle, 1 - mainselect, 2 - subselect
    double ribbonSize;
    list_t *options;
    list_t* lengths;
} ribbon;

ribbon ribbonRender;

int ribbonInit(GLFWwindow* window, const char *filename) { // read from config file
    ribbonRender.marginSize = 10; // number of pixels between different items in the ribbon (not affected by ribbonSize)
    ribbonRender.mainselect[0] = -1;
    ribbonRender.mainselect[1] = -1;
    ribbonRender.mainselect[2] = -1;
    ribbonRender.subselect[0] = -1;
    ribbonRender.subselect[1] = -1;
    ribbonRender.subselect[2] = -1;
    ribbonRender.output[0] = 0;
    ribbonRender.output[1] = -1;
    ribbonRender.output[2] = -1;
    ribbonRender.ribbonSize = 1; // 1 is default, below 1 is smaller, above 1 is larger (scales as a multiplier, 0.1 is 100x smaller than 10)
    ribbonRender.options = list_init();
    ribbonRender.lengths = list_init();
    ribbonRender.output[0] = -1;
    ribbonRender.output[1] = -1;

    FILE* configFile = fopen(filename, "r"); // load from config file
    if (configFile == NULL) {
        printf("EribbonRenderor: file %s not found\n", filename);
        return -1;
    }
    list_t* sublist = list_init();
    int checksum = 0;
    char throw[256]; // maximum size of any option or sub-option (characters)
    int j = 0;
    list_clear(sublist);
    while (checksum != EOF) {
        checksum = fscanf(configFile, "%[^,\n]%*c,", throw);
        char whitespace = 0;
        if (throw[0] == ' ') {
            whitespace += 1;
        } else {
            if (j != 0) {
                list_t* appendList = list_init();
                list_copy(sublist, appendList);
                list_clear(sublist);
                list_append(ribbonRender.options, (unitype) appendList, 'r');
            }
        }
        list_append(sublist, (unitype) (throw + whitespace), 's');
        j++;
    }
    list_pop(sublist);
    list_t* appendList = list_init();
    list_copy(sublist, appendList);
    list_clear(sublist);
    list_append(ribbonRender.options, (unitype) appendList, 'r');
    list_free(sublist);
    fclose(configFile);

    for (int i = 0; i < ribbonRender.options -> length; i++) {
        list_append(ribbonRender.lengths, (unitype) textGLGetStringLength(ribbonRender.options -> data[i].r -> data[0].s, strlen(ribbonRender.options -> data[i].r -> data[0].s), 7 * ribbonRender.ribbonSize), 'd');
        double max = 0;
        for (int j = 1; j < ribbonRender.options -> data[i].r -> length; j++) {
            double cur = textGLGetStringLength(ribbonRender.options -> data[i].r -> data[j].s, strlen(ribbonRender.options -> data[i].r -> data[0].s), 7 * ribbonRender.ribbonSize);
            if (cur > max) {
                max = cur;
            }
        }
        list_append(ribbonRender.lengths, (unitype) max, 'd');
    }
    return 0;
}

void ribbonDraw() {
    char shapeSave = turtools.penshape;
    double sizeSave = turtools.pensize;
    turtlePenSize(20);
    turtlePenShape("square");
    turtleGetMouseCoords(); // get the mouse coordinates (turtools.mouseX, turtools.mouseY)
    turtlePenColor(200, 200, 200);
    turtleGoto(-240, 175);
    turtlePenDown();
    turtleGoto(240, 175);
    turtlePenUp();
    turtlePenColor(0, 0, 0);
    double cutoff = -240 + ribbonRender.marginSize;
    ribbonRender.mainselect[0] = -1;
    ribbonRender.subselect[0] = -1;
    for (int i = 0; i < ribbonRender.options -> length; i++) {
        double prevCutoff = cutoff;
        if (i == ribbonRender.mainselect[2]) {
            double xLeft = prevCutoff - ribbonRender.marginSize / 2.0;
            double xRight = prevCutoff + ribbonRender.lengths -> data[i * 2 + 1].d + ribbonRender.marginSize / 2.0;
            double yDown = 170 - 15 * (ribbonRender.options -> data[i].r -> length - 1) - ribbonRender.marginSize / 2.0;
            turtleQuad(xLeft, 170, xRight, 170, xRight, yDown, xLeft, yDown, 140.0 / 255, 140.0 / 255, 140.0 / 255, 0.0);
            for (int j = 1; j < ribbonRender.options -> data[i].r -> length; j++) {
                if (turtools.mouseY > 170 - 15 * j - ribbonRender.marginSize / 4.0 && turtools.mouseY < 170 && turtools.mouseX > xLeft && turtools.mouseX < xRight && ribbonRender.subselect[0] == -1) {
                    turtleQuad(xLeft, 170 - 15 * (j - 1) - ribbonRender.marginSize / 4.0, xRight, 170 - 15 * (j - 1) - ribbonRender.marginSize / 4.0, xRight, 170 - 15 * j - ribbonRender.marginSize / 3.0, xLeft, 170 - 15 * j - ribbonRender.marginSize / 3.0, 100.0 / 255, 100.0 / 255, 100.0 / 255, 0.0);
                    ribbonRender.subselect[0] = j;
                }
                textGLWriteString(ribbonRender.options -> data[i].r -> data[j].s, prevCutoff, 174.5 - j * 15, 7 * ribbonRender.ribbonSize, 0);
            }
        }
        cutoff += ribbonRender.lengths -> data[i * 2].d + ribbonRender.marginSize;
        if (turtools.mouseY > 170 && turtools.mouseY < 180 && turtools.mouseX > -240 + ribbonRender.marginSize / 2.0 && turtools.mouseX < cutoff - ribbonRender.marginSize / 2.0 && ribbonRender.mainselect[0] == -1) { // -217, -195, -164
            // turtlePenSize(17);
            // turtlePenColor(140, 140, 140);
            // turtleGoto(prevCutoff, 175);
            // turtlePenDown();
            // turtleGoto(cutoff - ribbonRender.marginSize, 175);
            // turtlePenUp();
            // turtlePenSize(20);
            turtleQuad(prevCutoff - ribbonRender.marginSize / 2.0, 179, cutoff - ribbonRender.marginSize / 2.0, 179, cutoff - ribbonRender.marginSize / 2.0, 171, prevCutoff - ribbonRender.marginSize / 2.0, 171, 140.0 / 255, 140.0 / 255, 140.0 / 255, 0.0);
            ribbonRender.mainselect[0] = i;
        }
        textGLWriteString(ribbonRender.options -> data[i].r -> data[0].s, prevCutoff, 174.5, 7 * ribbonRender.ribbonSize, 0);
    }
    if (turtleMouseDown()) {
        if (ribbonRender.subselect[0] == ribbonRender.subselect[1] && ribbonRender.subselect[0] != -1) {
            ribbonRender.subselect[2] = ribbonRender.subselect[0];
            ribbonRender.output[0] = 1;
            ribbonRender.output[1] = ribbonRender.mainselect[2];
            ribbonRender.output[2] = ribbonRender.subselect[2];
        }
        if (ribbonRender.mainselect[0] == ribbonRender.mainselect[1]) {
            ribbonRender.mainselect[2] = ribbonRender.mainselect[0];
        }
    } else {
        ribbonRender.mainselect[1] = ribbonRender.mainselect[0];
        ribbonRender.subselect[1] = ribbonRender.subselect[0];
    }
    turtools.penshape = shapeSave;
    turtools.pensize = sizeSave;
}