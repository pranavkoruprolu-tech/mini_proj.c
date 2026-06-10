#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define WIDTH 60
#define HEIGHT 20
#define MAX_OBJECTS 100

typedef enum {
    OBJ_NONE = 0,
    OBJ_LINE,
    OBJ_RECTANGLE,
    OBJ_CIRCLE,
    OBJ_TRIANGLE
} ObjectType;

typedef struct {
    int id;
    ObjectType type;
    int x1, y1;
    int x2, y2;
    int x3, y3;
    int radius;
} GraphicObject;

char canvas[HEIGHT][WIDTH];
GraphicObject objects[MAX_OBJECTS];
int objectCount = 0;
int nextId = 1;

void initCanvas(void) {
    int r, c;
    for (r = 0; r < HEIGHT; ++r) {
        for (c = 0; c < WIDTH; ++c) {
            canvas[r][c] = '_';
        }
    }
}

void setPixel(int x, int y, char ch) {
    if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
        canvas[y][x] = ch;
    }
}

void drawLine(int x0, int y0, int x1, int y1) {
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    while (1) {
        setPixel(x0, y0, '*');
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

void drawRectangle(int x0, int y0, int x1, int y1) {
    int left = x0 < x1 ? x0 : x1;
    int right = x0 < x1 ? x1 : x0;
    int top = y0 < y1 ? y0 : y1;
    int bottom = y0 < y1 ? y1 : y0;

    {
        int x;
        for (x = left; x <= right; ++x) {
            setPixel(x, top, '*');
            setPixel(x, bottom, '*');
        }
    }
    {
        int y;
        for (y = top; y <= bottom; ++y) {
            setPixel(left, y, '*');
            setPixel(right, y, '*');
        }
    }
}

void drawCircle(int cx, int cy, int radius) {
    if (radius < 0) return;
    int x = radius;
    int y = 0;
    int err = 0;

    while (x >= y) {
        setPixel(cx + x, cy + y, '*');
        setPixel(cx + y, cy + x, '*');
        setPixel(cx - y, cy + x, '*');
        setPixel(cx - x, cy + y, '*');
        setPixel(cx - x, cy - y, '*');
        setPixel(cx - y, cy - x, '*');
        setPixel(cx + y, cy - x, '*');
        setPixel(cx + x, cy - y, '*');

        y++;
        if (err <= 0) {
            err += 2 * y + 1;
        }
        if (err > 0) {
            x--;
            err -= 2 * x + 1;
        }
    }
}

void drawTriangle(int x0, int y0, int x1, int y1, int x2, int y2) {
    drawLine(x0, y0, x1, y1);
    drawLine(x1, y1, x2, y2);
    drawLine(x2, y2, x0, y0);
}

void drawObject(const GraphicObject *obj) {
    if (!obj) return;
    switch (obj->type) {
        case OBJ_LINE:
            drawLine(obj->x1, obj->y1, obj->x2, obj->y2);
            break;
        case OBJ_RECTANGLE:
            drawRectangle(obj->x1, obj->y1, obj->x2, obj->y2);
            break;
        case OBJ_CIRCLE:
            drawCircle(obj->x1, obj->y1, obj->radius);
            break;
        case OBJ_TRIANGLE:
            drawTriangle(obj->x1, obj->y1, obj->x2, obj->y2, obj->x3, obj->y3);
            break;
        default:
            break;
    }
}

void refreshCanvas(void) {
    int i;
    initCanvas();
    for (i = 0; i < objectCount; ++i) {
        drawObject(&objects[i]);
    }
}

void displayCanvas(void) {
    int r, c;
    printf("\n");
    for (r = 0; r < HEIGHT; ++r) {
        for (c = 0; c < WIDTH; ++c) {
            putchar(canvas[r][c]);
        }
        putchar('\n');
    }
}

void listObjects(void) {
    int i;
    if (objectCount == 0) {
        printf("No objects in the picture.\n");
        return;
    }
    printf("\nObjects:\n");
    for (i = 0; i < objectCount; ++i) {
        GraphicObject *obj = &objects[i];
        printf("ID %d: ", obj->id);
        switch (obj->type) {
            case OBJ_LINE:
                printf("Line from (%d,%d) to (%d,%d)\n", obj->x1, obj->y1, obj->x2, obj->y2);
                break;
            case OBJ_RECTANGLE:
                printf("Rectangle between (%d,%d) and (%d,%d)\n", obj->x1, obj->y1, obj->x2, obj->y2);
                break;
            case OBJ_CIRCLE:
                printf("Circle center (%d,%d) radius %d\n", obj->x1, obj->y1, obj->radius);
                break;
            case OBJ_TRIANGLE:
                printf("Triangle vertices (%d,%d), (%d,%d), (%d,%d)\n", obj->x1, obj->y1, obj->x2, obj->y2, obj->x3, obj->y3);
                break;
            default:
                printf("Unknown object\n");
                break;
        }
    }
}

GraphicObject *findObjectById(int id) {
    int i;
    for (i = 0; i < objectCount; ++i) {
        if (objects[i].id == id) {
            return &objects[i];
        }
    }
    return NULL;
}

void addObject(void) {
    if (objectCount >= MAX_OBJECTS) {
        printf("Cannot add more objects: limit reached.\n");
        return;
    }

    printf("Choose object type:\n");
    printf("1. Line\n");
    printf("2. Rectangle\n");
    printf("3. Circle\n");
    printf("4. Triangle\n");
    printf("Enter choice: ");
    int choice;
    if (scanf("%d", &choice) != 1) {
        while (getchar() != '\n');
        return;
    }

    GraphicObject obj;
    obj.id = nextId++;
    obj.type = (ObjectType)choice;
    obj.x1 = obj.y1 = obj.x2 = obj.y2 = obj.x3 = obj.y3 = obj.radius = 0;

    switch (choice) {
        case 1:
            printf("Enter x1 y1 x2 y2: ");
            scanf("%d %d %d %d", &obj.x1, &obj.y1, &obj.x2, &obj.y2);
            break;
        case 2:
            printf("Enter top-left x y and bottom-right x y: ");
            scanf("%d %d %d %d", &obj.x1, &obj.y1, &obj.x2, &obj.y2);
            break;
        case 3:
            printf("Enter center x y and radius: ");
            scanf("%d %d %d", &obj.x1, &obj.y1, &obj.radius);
            break;
        case 4:
            printf("Enter x1 y1 x2 y2 x3 y3: ");
            scanf("%d %d %d %d %d %d", &obj.x1, &obj.y1, &obj.x2, &obj.y2, &obj.x3, &obj.y3);
            break;
        default:
            printf("Invalid type selection.\n");
            return;
    }

    objects[objectCount++] = obj;
    refreshCanvas();
    printf("Object added with ID %d.\n", obj.id);
}

void deleteObject(void) {
    if (objectCount == 0) {
        printf("No objects to delete.\n");
        return;
    }
    printf("Enter object ID to delete: ");
    int id;
    if (scanf("%d", &id) != 1) {
        while (getchar() != '\n');
        return;
    }
    int index = -1;
    {
        int i;
        for (i = 0; i < objectCount; ++i) {
            if (objects[i].id == id) {
                index = i;
                break;
            }
        }
    }
    if (index == -1) {
        printf("Object with ID %d not found.\n", id);
        return;
    }
    memmove(&objects[index], &objects[index + 1], sizeof(GraphicObject) * (objectCount - index - 1));
    objectCount--;
    refreshCanvas();
    printf("Object %d deleted.\n", id);
}

void modifyObject(void) {
    if (objectCount == 0) {
        printf("No objects to modify.\n");
        return;
    }
    printf("Enter object ID to modify: ");
    int id;
    if (scanf("%d", &id) != 1) {
        while (getchar() != '\n');
        return;
    }
    GraphicObject *obj = findObjectById(id);
    if (!obj) {
        printf("Object with ID %d not found.\n", id);
        return;
    }
    printf("Modifying object ID %d.\n", id);

    switch (obj->type) {
        case OBJ_LINE:
            printf("Enter new x1 y1 x2 y2: ");
            scanf("%d %d %d %d", &obj->x1, &obj->y1, &obj->x2, &obj->y2);
            break;
        case OBJ_RECTANGLE:
            printf("Enter new top-left x y and bottom-right x y: ");
            scanf("%d %d %d %d", &obj->x1, &obj->y1, &obj->x2, &obj->y2);
            break;
        case OBJ_CIRCLE:
            printf("Enter new center x y and radius: ");
            scanf("%d %d %d", &obj->x1, &obj->y1, &obj->radius);
            break;
        case OBJ_TRIANGLE:
            printf("Enter new x1 y1 x2 y2 x3 y3: ");
            scanf("%d %d %d %d %d %d", &obj->x1, &obj->y1, &obj->x2, &obj->y2, &obj->x3, &obj->y3);
            break;
        default:
            printf("Unknown object type.\n");
            return;
    }
    refreshCanvas();
    printf("Object %d updated.\n", id);
}

void showMenu(void) {
    printf("\n2D Graphics Editor\n");
    printf("1. Display picture\n");
    printf("2. List objects\n");
    printf("3. Add object\n");
    printf("4. Delete object\n");
    printf("5. Modify object\n");
    printf("6. Clear picture\n");
    printf("7. Exit\n");
    printf("Enter choice: ");
}

int main(void) {
    initCanvas();
    int choice;
    do {
        showMenu();
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            choice = 0;
        }

        switch (choice) {
            case 1:
                displayCanvas();
                break;
            case 2:
                listObjects();
                break;
            case 3:
                addObject();
                break;
            case 4:
                deleteObject();
                break;
            case 5:
                modifyObject();
                break;
            case 6:
                objectCount = 0;
                initCanvas();
                printf("Picture cleared.\n");
                break;
            case 7:
                printf("Exiting.\n");
                break;
            default:
                printf("Invalid choice. Please select a valid option.\n");
                break;
        }
    } while (choice != 7);

    return 0;
}