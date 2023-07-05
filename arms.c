/* it's a scratch program from 2017 what do you want from me */

#include "include/ribbon.h"
#include "include/slider.h"
#include "include/win32FileDialog.h"
#include <time.h>

typedef struct { // shared state
    char mode; // 0 is for editor, 1 is for 3D viewer
    int grid; // number of grid squares
    int FOV; // fov for 3D renderer (perspective projection)
    double mx; // mouseX but bounded
    double my; // mouseY bounded
    double d1; // direction1 (3D viewer)
    double d2; // direction2 (3D viewer)
    list_t* points; // raw point data
    list_t* pointsPlus; // point data conformed to grid size
    list_t* points3D; // 3D point data
    char *filename; // filepath
    char *filenameAbbr; // filename
} arms;

void armsInit(arms *selfp) { // initialise state
    arms self = *selfp;
    self.mode = 0;
    self.mx = 0;
    self.my = 0;
    self.d1 = -90;
    self.d2 = 0;
    self.grid = 50;
    self.FOV = 175;
    self.points = list_init();
    self.pointsPlus = list_init();
    self.points3D = list_init();
    self.filename = strdup("null");
    self.filenameAbbr = strdup("null");
    *selfp = self;
}

void pointsPlus(arms *selfp) {
    arms self = *selfp;
    list_clear(self.pointsPlus);
    int i = 0;
    while (i < self.points -> length) {
        if (self.points -> type[i] == 'c') {
            list_append(self.pointsPlus, (unitype) 'p', 'c');
            i += 1;
        }
        if (self.grid % 2 == 1) {
            list_append(self.pointsPlus, (unitype) (round((self.points -> data[i].d + 40 - (160.0 / self.grid)) / (320.0 / self.grid)) * (320.0 / self.grid) + (160.0 / self.grid) - 40), 'd');
            list_append(self.pointsPlus, (unitype) (round((self.points -> data[i + 1].d - (160.0 / self.grid)) / (320.0 / self.grid)) * (320.0 / self.grid) + (160.0 / self.grid)), 'd');
        } else {
            list_append(self.pointsPlus, (unitype) (round((self.points -> data[i].d + 40) / (320.0 / self.grid)) * (320.0 / self.grid) - 40), 'd');
            list_append(self.pointsPlus, (unitype) (round(self.points -> data[i + 1].d / (320.0 / self.grid)) * (320.0 / self.grid)), 'd');
        }
        list_append(self.pointsPlus, (unitype) 0, 'd');
        i += 3;
    }
    *selfp = self;
}

void renderGrid(arms *selfp) {
    arms self = *selfp;
    turtlePenShape("square");
    turtlePenColor(0, 0, 0);
    turtlePenSize(1);
    double x;
    double y = -160;
    for (int i = 0; i < (self.grid + 1); i++) {
        if (y < 159.0 / self.grid && y > -161.0 / self.grid) {
            turtlePenSize(2);
        } else {
            turtlePenSize(1);
        }
        x = -200;
        turtleGoto(x, y);
        turtlePenDown();
        x = 120;
        turtleGoto(x, y);
        turtlePenUp();
        y += 320.0 / self.grid;
    }
    x = -200;
    for (int i = 0; i < (self.grid + 1); i++) {
        if (x < 159.0 / self.grid - 40 && x > -161.0 / self.grid - 40) {
            turtlePenSize(2);
        } else {
            turtlePenSize(1);
        }
        y = -160;
        turtleGoto(x, y);
        turtlePenDown();
        y = 160;
        turtleGoto(x, y);
        turtlePenUp();
        x += 320.0 / self.grid;
    }
    // no shared state is changed in this function
}

void mouseEdit(arms *selfp, double mouseX, double mouseY) { // binds mouse coordinates to in the editing window
    arms self = *selfp;
    self.mx = mouseX;
    self.my = mouseY;
    if (mouseX > 120) {
        self.mx = 120;
    }
    if (mouseX < -200) {
        self.mx = -200;
    }
    if (mouseY > 160) {
        self.my = 160;
    }
    if (mouseY < -160) {
        self.my = -160;
    }
    *selfp = self;
}

void drawPoints(arms *selfp) {
    arms self = *selfp;
    turtlePenShape("circle");
    turtlePenColor(3, 17, 169);
    turtlePenSize(300.0 / (self.grid + 1));
    int i = 0;
    while (i < self.pointsPlus -> length) {
        if (self.pointsPlus -> length > i + 4 && self.pointsPlus -> type[i + 3] == 'c') {
            turtleGoto(self.pointsPlus -> data[i].d, self.pointsPlus -> data[i + 1].d);
            turtlePenDown();
            i += 4;
        } else {
            turtleGoto(self.pointsPlus -> data[i].d, self.pointsPlus -> data[i + 1].d);
            turtlePenDown();
            turtlePenUp();
            i += 3;
        }
    }
    if (turtleKeyPressed(GLFW_KEY_SPACE) && self.pointsPlus -> length > 1) {
        turtleGoto(self.pointsPlus -> data[self.pointsPlus -> length - 3].d, self.pointsPlus -> data[self.pointsPlus -> length - 2].d);
        turtlePenDown();
    }
    if (turtools.mouseX < 145 && turtools.mouseX > -210 && turtools.mouseY < 170 && ribbonRender.mainselect[2] == -1) {
        if (self.grid % 2 == 1) {
            turtleGoto(round((self.mx + 40 - (160.0 / self.grid)) / (320.0 / self.grid)) * (320.0 / self.grid) + (160.0 / self.grid) - 40, round((self.my - (160.0 / self.grid)) / (320.0 / self.grid)) * (320.0 / self.grid) + (160.0 / self.grid));
        } else {
            turtleGoto(round((self.mx + 40) / (320.0 / self.grid)) * (320.0 / self.grid) - 40, round(self.my / (320.0 / self.grid)) * (320.0 / self.grid));
        }
        turtlePenDown();
        turtlePenUp();
    }
    *selfp = self;
}

void exchange(arms *selfp) {
    arms self = *selfp;
    list_clear(self.points3D);
    int i = 0;
    while (i < self.pointsPlus -> length) {
        if (self.pointsPlus -> type[i] == 'c') {
            list_append(self.points3D, (unitype) 'p', 'c');
            i += 1;
        }
        list_append(self.points3D, (unitype) (self.pointsPlus -> data[i].d + 40), 'd');
        i += 1;
        list_append(self.points3D, (unitype) self.pointsPlus -> data[i].d, 'd');
        i += 1;
        list_append(self.points3D, (unitype) self.pointsPlus -> data[i].d, 'd');
        i += 1;
    }
    *selfp = self;
}

void transform(arms *selfp, double xChange, double yChange) {
    arms self = *selfp;
    int i = 0;
    while (i < self.points -> length) {
        if (self.points -> type[i] == 'c') {
            i += 1;
        }
        if ((self.points -> data[i].d + 320.0 * xChange / self.grid) >= -200 && (self.points -> data[i].d + 320.0 * xChange / self.grid) <= 120)
            self.points -> data[i] = (unitype) (self.points -> data[i].d + 320.0 * xChange / self.grid);
        i += 1;
        if ((self.points -> data[i].d + 320.0 * yChange / self.grid) >= -160 && (self.points -> data[i].d + 320.0 * yChange / self.grid) <= 160)
            self.points -> data[i] = (unitype) (self.points -> data[i].d + 320.0 * yChange / self.grid);
        i += 1;
        i += 1;
    }
    *selfp = self;
}

void render3D(arms *selfp) {
    arms self = *selfp;
    turtlePenShape("circle");
    turtlePenColor(0, 0, 0);
    turtlePenSize(4);
    turtlePenUp();
    int i = 0;
    double x = self.points3D -> data[i].d;
    double y = self.points3D -> data[i + 1].d;
    double z = self.points3D -> data[i + 2].d - 10;
    double sd1 = sin(self.d1);
    double sd2 = sin(self.d2);
    double cd1 = cos(self.d1);
    double cd2 = cos(self.d2);
    while (i < self.points3D -> length) {
        double inter = self.FOV / (((cd1 * y + sd1 * x) * sd2 + self.FOV) - (cd2 * z));
        double xgoto = ((sd2 * z) + (cd1 * y + sd1 * x) * cd2) * inter;
        double ygoto = ((cd1 * x) - (sd1 * y)) * inter;
        turtleGoto(xgoto, ygoto);
        turtlePenDown();
        z += 20;
        inter = self.FOV / (((cd1 * y + sd1 * x) * sd2 + self.FOV) - (cd2 * z));
        turtleGoto(((sd2 * z) + (cd1 * y + sd1 * x) * cd2) * inter, ((cd1 * x) - (sd1 * y)) * inter);
        char j;
        if (self.points3D -> length > i + 4 && self.points3D -> type[i + 3] == 'c') {
            i += 1;
            j = 1;
        } else {
            turtlePenUp();
            j = 0;
        }
        i += 3;
        x = self.points3D -> data[i].d;
        y = self.points3D -> data[i + 1].d;
        z = self.points3D -> data[i + 2].d + 10;
        inter = self.FOV / (((cd1 * y + sd1 * x) * sd2 + self.FOV) - (cd2 * z));
        turtleGoto(((sd2 * z) + (cd1 * y + sd1 * x) * cd2) * inter, ((cd1 * x) - (sd1 * y)) * inter);
        turtlePenUp();
        turtleGoto(xgoto, ygoto);
        if (j == 1) {
            turtlePenDown();
        }
        z -= 20;
    }
    // no shared state is changed in this function
}

int loadData(arms *selfp, const char *filename) { // loads weights and biases from a file (custom format)
    arms self = *selfp;
    if (strcmp(self.filename, filename) != 0) { // check if filename is self.filename
        free(self.filename);
        self.filename = strdup(filename);
        self.filenameAbbr = realloc(self.filenameAbbr, strlen(filename));
        int i = strlen(filename) - 1;
        while (filename[i] != '\\' && i > 0) {
            i--;
        }
        if (i != 0)
            i += 1;
        int j = i;
        for (; i < strlen(filename); i++) {
            self.filenameAbbr[i - j] = filename[i];
        }
        self.filenameAbbr[i - j] = '\0';
    }

    FILE* fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Error: file %s not found\n", self.filename);
        return -1;
    }
    int checksum;
    char throw[50];
    double doub;
    list_clear(self.points);
    checksum = fscanf(fp, "%s", throw);
    if (checksum == EOF) {
        printf("Error reading file %s\n", self.filename);
        self.filename = strdup("null"); // set filename to null if load was unsuccessful
        self.filenameAbbr = strdup("null");
        fclose(fp);
        *selfp = self;
        return -1;
    }
    rewind(fp);
    checksum = fscanf(fp, "%s", throw);
    while (checksum != EOF) {
        if (strcmp(throw, "p") == 0) {
            list_append(self.points, (unitype) 'p', 'c');
        } else {
            sscanf(throw, "%lf", &doub);
            list_append(self.points, (unitype) doub, 'd');
        }
        checksum = fscanf(fp, "%s", throw);
    }
    fclose(fp);
    *selfp = self;
    return 0;
}

int saveData(arms *selfp, const char *filename) {
    arms self = *selfp;
    if (strcmp(self.filename, filename) != 0) { // check if filename is self.filename
        free(self.filename);
        self.filename = strdup(filename);
        self.filenameAbbr = realloc(self.filenameAbbr, strlen(filename));
        int i = strlen(filename) - 1;
        while (filename[i] != '\\' && i > 0) {
            i--;
        }
        if (i != 0)
            i += 1;
        int j = i;
        for (; i < strlen(filename); i++) {
            self.filenameAbbr[i - j] = filename[i];
        }
        self.filenameAbbr[i - j] = '\0';
    }

    FILE* fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("Error: writing to file %s\n", self.filename);
        free(self.filename);
        self.filename = strdup("null"); // set filename to null if save was unsuccessful
        free(self.filenameAbbr);
        self.filenameAbbr = strdup("null");
        fclose(fp);
        *selfp = self;
        return -1;
    } else {
        for (int i = 0; i < self.points -> length; i++) {
            if (self.points -> type[i] == 'c') {
                fprintf(fp, "%c", 'p');
            } else {
                fprintf(fp, "%lf", self.points -> data[i].d);
            }
            if (i != self.points -> length - 1) {
                fprintf(fp, "\n");
            }
        }
        fclose(fp);
        *selfp = self;
        return 0;
    }
}

void parseRibbonOutput(arms *selfp) {
    arms self = *selfp;
    if (ribbonRender.output[0] == 1) {
        ribbonRender.output[0] = 0; // untoggle
        if (ribbonRender.output[1] == 0) { // file
            if (ribbonRender.output[2] == 1) { // new
                list_clear(self.points);
            }
            if (ribbonRender.output[2] == 2) { // save
                if (strcmp(self.filename, "null") == 0) {
                    if (win32FileDialogPrompt(1, "") != -1) {
                        saveData(&self, win32FileDialog.filename);
                        printf("Successfully saved to: %s\n", self.filenameAbbr);
                    }
                } else {
                    saveData(&self, self.filename);
                    printf("Successfully saved to: %s\n", self.filenameAbbr);
                }
            }
            if (ribbonRender.output[2] == 3) { // save as
                if (win32FileDialogPrompt(1, self.filenameAbbr) != -1) {
                    saveData(&self, win32FileDialog.filename);
                    printf("Successfully saved to: %s\n", self.filenameAbbr);
                }
            }
            if (ribbonRender.output[2] == 4) { // load
                if (win32FileDialogPrompt(0, "") != -1) {
                    loadData(&self, win32FileDialog.filename);
                    printf("Loaded data from: %s\n", self.filenameAbbr);
                    if (self.mode == 1) {
                        pointsPlus(&self);
                        exchange(&self);
                    }
                }
            }
        }
        if (ribbonRender.output[1] == 1) { // edit
            if (ribbonRender.output[2] == 1 && self.mode == 0) { // undo
                list_pop(self.points);
                list_pop(self.points);
                list_pop(self.points);
                if (self.points -> length > 1 && self.points -> type[self.points -> length - 1] == 'c')
                    list_pop(self.points);
            }
            if (ribbonRender.output[2] == 2 && self.mode == 0) { // clear
                list_clear(self.points);
            }
        }
        if (ribbonRender.output[1] == 2) { // view
            if (ribbonRender.output[2] == 1) { // view grid
                self.mode = 0;
            } 
            if (ribbonRender.output[2] == 2) { // view 3D
                exchange(&self);
                self.mode = 1;
            } 
        }
    }
    *selfp = self;
}

int main(int argc, char *argv[]) {
    GLFWwindow* window;
    /* Initialize glfw */
    if (!glfwInit()) {
        return -1;
    }
    glfwWindowHint(GLFW_SAMPLES, 4); // MSAA (Anti-Aliasing) with 4 samples (must be done before window is created (?))

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(1280, 720, "arms", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetWindowSizeLimits(window, 128, 72, 1280, 720);
    gladLoadGL();

    /* initialize turtle/turtools */
    turtleInit(window, -320, -180, 320, 180);
    /* initialise textGL */
    textGLInit(window, "include/fontBez.tgl");
    /* initialise ribbon */
    ribbonInit(window, "include/ribbonConfig.txt");
    /* initialise win32FileDialog */
    win32FileDialogInit();
    win32FileDialogAddExtension("txt"); // add txt to extension restrictions (this isn't the best system since you can only add extensions and not remove them, plus it's not so easy to make multiple "profiles". 
    // I guess that's what the COMDLG_FILTERSPEC is for and it makes sense but im not dealing with garbage names and wide strings)

    int tps = 60; // ticks per second (locked to fps in this case)
    arms obj;
    armsInit(&obj);

    /* create slider (assumes turtle and textGL are initialised) */
    slider gridSlider;
    sliderInit(&gridSlider, (unitype*) &obj.grid, 'i', "grid", "vertical", 200, 0, 30, 200, 10, 50);

    if (argc > 1) {
        if (strcmp(argv[1], "-f") == 0 || strcmp(argv[1], "-F") == 0) {
            if (argc > 2) {
                printf("Loading data from: %s\n", argv[2]);
                loadData(&obj, argv[2]);
            } else {
                printf("no file supplied\n");
                return -1;
            }
        } else {
            printf("Loading data from: %s\n", argv[1]);
            loadData(&obj, argv[1]);
        }
    }

    clock_t start;
    clock_t end;

    double turnSpeed = 0.03;
    char keys[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    while (turtools.close == 0) { // main loop
        start = clock();
        turtleGetMouseCoords(); // get the mouse coordinates (turtools.mouseX, turtools.mouseY)
        turtleClear();
        if (turtleKeyPressed(GLFW_KEY_G)) {
            if (keys[1] == 0) {
                keys[1] = 1;
                if (obj.mode == 0) {
                    exchange(&obj);
                    obj.mode = 1;
                } else {
                    obj.mode = 0;
                }
            }
        } else {
            keys[1] = 0;
        }
        if (obj.mode == 0) {
            if (turtleMouseDown()) {
                if (keys[0] == 0) {
                    keys[0] = 1;
                    // printf("%d\n", ribbonRender.mainselect[0]);
                    if (turtools.mouseX < 145 && turtools.mouseX > -210 && turtools.mouseY < 170 && ribbonRender.mainselect[2] == -1) {
                        if (turtleKeyPressed(GLFW_KEY_SPACE) && obj.points -> length > 0) {
                            list_append(obj.points, (unitype) 'p', 'c');
                        }
                        if (obj.grid % 2 == 1) {
                            list_append(obj.points, (unitype) (round((obj.mx + 40 - (160.0 / obj.grid)) / (320.0 / obj.grid)) * (320.0 / obj.grid) + (160.0 / obj.grid) - 40), 'd');
                            list_append(obj.points, (unitype) (round((obj.my - (160.0 / obj.grid)) / (320.0 / obj.grid)) * (320.0 / obj.grid) + (160.0 / obj.grid)), 'd');
                        } else {
                            list_append(obj.points, (unitype) (round((obj.mx + 40) / (320.0 / obj.grid)) * (320.0 / obj.grid) - 40), 'd');
                            list_append(obj.points, (unitype) (round(obj.my / (320.0 / obj.grid)) * (320.0 / obj.grid)), 'd');
                        }
                        list_append(obj.points, (unitype) 0, 'd');
                    }
                }
            } else {
                keys[0] = 0;
            }
            if (turtleKeyPressed(GLFW_KEY_C)) { // clear
                if (keys[2] == 0) {
                    keys[2] = 1;
                    if(obj.mode == 0)
                        list_clear(obj.points);
                }
            } else {
                keys[2] = 0;
            }
            if (turtleKeyPressed(GLFW_KEY_Z)) {
                if (keys[3] == 0) { // undo
                    keys[3] = 1;
                    if (obj.mode == 0) {
                        list_pop(obj.points);
                        list_pop(obj.points);
                        list_pop(obj.points);
                        if (obj.points -> length > 1 && obj.points -> type[obj.points -> length - 1] == 'c')
                            list_pop(obj.points);
                    }
                }
            } else {
                keys[3] = 0;
            }
            if (turtleKeyPressed(GLFW_KEY_W) || turtleKeyPressed(GLFW_KEY_UP)) {
                if (keys[4] == 0) {
                    keys[4] = 1;
                    transform(&obj, 0, 1);
                }
            } else {
                keys[4] = 0;
            }
            if (turtleKeyPressed(GLFW_KEY_S) || turtleKeyPressed(GLFW_KEY_DOWN)) {
                if (keys[5] == 0) {
                    keys[5] = 1;
                    transform(&obj, 0, -1);
                }
            } else {
                keys[5] = 0;
            }
            if (turtleKeyPressed(GLFW_KEY_D) || turtleKeyPressed(GLFW_KEY_RIGHT)) {
                if (keys[6] == 0) {
                    keys[6] = 1;
                    transform(&obj, 1, 0);
                }
            } else {
                keys[6] = 0;
            }
            if (turtleKeyPressed(GLFW_KEY_A) || turtleKeyPressed(GLFW_KEY_LEFT)) {
                if (keys[7] == 0) {
                    keys[7] = 1;
                    transform(&obj, -1, 0);
                }
            } else {
                keys[7] = 0;
            }
            pointsPlus(&obj);
            renderGrid(&obj);
            mouseEdit(&obj, turtools.mouseX, turtools.mouseY);
            drawPoints(&obj);
            sliderRender(&gridSlider);
        } else {
            if (turtleKeyPressed(GLFW_KEY_W) || turtleKeyPressed(GLFW_KEY_UP)) {
                obj.d1 += turnSpeed;
            }
            if (turtleKeyPressed(GLFW_KEY_S) || turtleKeyPressed(GLFW_KEY_DOWN)) {
                obj.d1 -= turnSpeed;
            }
            if (turtleKeyPressed(GLFW_KEY_D) || turtleKeyPressed(GLFW_KEY_RIGHT)) {
                obj.d2 += turnSpeed;
            }
            if (turtleKeyPressed(GLFW_KEY_A) || turtleKeyPressed(GLFW_KEY_LEFT)) {
                obj.d2 -= turnSpeed;
            }
            render3D(&obj);
        }
        ribbonDraw();
        parseRibbonOutput(&obj);
        turtleUpdate(); // update the screen
        end = clock();
        while ((double) (end - start) / CLOCKS_PER_SEC < (1.0 / tps)) {
            end = clock();
        }
    }
    turtleFree();
    glfwTerminate();
    return 0;
}