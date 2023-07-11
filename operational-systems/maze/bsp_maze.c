#include <unistd.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <inttypes.h>
#include <time.h>

enum {
    DEFAULT_HEIGHT = 100,
    DEFAULT_WIDTH = 100,
    DEFAULT_MIN_LEAF_SIZE = 20,  // DEFAULT_MIN_LEAF_SIZE / 2 - минимальный размер комнаты
    MIN_ROOM_SIZE = DEFAULT_MIN_LEAF_SIZE / 2,
    DEFAULT_MAX_LEAF_SIZE = 20,
    DEFAULT_MIN_ROOM_MARGIN = 1 // 2x + MIN_ROOM_SIZE не должны превышать DEFAULT_MIN_LEAF_SIZE
};

void error(const char* str) {
    fprintf(stderr, "error has occured: %s\n", str);
    exit(EXIT_FAILURE);
}

typedef struct sColour {
    uint8_t r_;
    uint8_t g_;
    uint8_t b_;
} Colour;


static const Colour LIGHT_COLOUR = {.r_ = 255, .g_ = 255, .b_ = 255};

typedef struct sRectangle {
    uint16_t x_;
    uint16_t y_;
    uint16_t w_;
    uint16_t h_;
} Rectangle;

static inline void printColouredRectangle(FILE* file, const Rectangle r, const Colour c) {
    fprintf(
        file, 
        "rectangle %"PRIu16" %"PRIu16" %"PRIu16" %"PRIu16" %"PRIu8" %"PRIu8" %"PRIu8"\n",
        r.x_,
        r.y_,
        r.w_,
        r.h_,
        c.r_,
        c.g_,
        c.b_
    );
}

static inline void printLightRectangle(FILE* file, const Rectangle r) {
    printColouredRectangle(file, r, LIGHT_COLOUR);
}

typedef struct sRoomsHadnler {
    Rectangle* arr_;
    size_t size_;
} RoomsHadnler;

void RH_init(RoomsHadnler* const self, size_t size);
void RH_deinit(RoomsHadnler* const self);

void RH_addRoom(RoomsHadnler* const self,Rectangle r);
void RH_calculateHalls(RoomsHadnler* const self);
void createHall(Rectangle lroom, Rectangle rroom);

void RH_init(RoomsHadnler* const self, size_t maxSize) {
    self->arr_ = malloc (sizeof(Rectangle) * maxSize);
    self->size_ = 0;
}

void RH_deinit(RoomsHadnler* const self) {
    free(self->arr_);
}

void RH_addRoom(RoomsHadnler* const self,Rectangle r) {
    self->arr_[self->size_++] = r;
}

void RH_calculateHalls(RoomsHadnler* const self) {
    for(size_t i = 1; i < self->size_; ++i)
        createHall(self->arr_[i - 1], self->arr_[i]);
}


void createHall(Rectangle lroom, Rectangle rroom) {
    typedef struct sPoint {
        uint16_t x_;
        uint16_t y_;
    } Point;

    Point point1 = (Point){
        .x_ = lroom.x_ + 1 + rand() % (lroom.w_ - 2),
        .y_ = lroom.y_ + 1 + rand() % (lroom.h_ - 2),
    };

    Point point2 = (Point){
        .x_ = rroom.x_ + 1 + rand() % (rroom.w_ - 2),
        .y_ = rroom.y_ + 1 + rand() % (rroom.h_ - 2),
    };

    int32_t w = point2.x_ - point1.x_;
    int32_t h = point2.y_ - point1.y_;

    if (w < 0) {
        if (h < 0) {
            if (rand() % 1000 < 500) {
                printLightRectangle(stdout, (Rectangle){point2.x_, point1.y_, abs(w) + 1, 1});
                printLightRectangle(stdout, (Rectangle){point2.x_, point2.y_, 1, abs(h) + 1});
            }
            else {
                printLightRectangle(stdout, (Rectangle){point2.x_, point2.y_, abs(w) + 1, 1});
                printLightRectangle(stdout, (Rectangle){point1.x_, point2.y_, 1, abs(h) + 1});
            }
        }
        else if (h > 0) {
            if (rand() % 1000 < 500) {
                printLightRectangle(stdout, (Rectangle){point2.x_, point1.y_, abs(w) + 1, 1});
                printLightRectangle(stdout, (Rectangle){point2.x_, point1.y_, 1, abs(h) + 1});
            }
            else {
                printLightRectangle(stdout, (Rectangle){point2.x_, point2.y_, abs(w) + 1, 1});
                printLightRectangle(stdout, (Rectangle){point1.x_, point1.y_, 1, abs(h) + 1});
            }
        }
        else {
            printLightRectangle(stdout, (Rectangle){point2.x_, point2.y_, abs(w) + 1, 1});
        }
    }
    else if (w > 0) {
        if (h < 0) {
            if (rand() % 1000 < 500) {
                printLightRectangle(stdout, (Rectangle){point1.x_, point2.y_, abs(w) + 1, 1});
                printLightRectangle(stdout, (Rectangle){point1.x_, point2.y_, 1, abs(h) + 1});
            }
            else {
                printLightRectangle(stdout, (Rectangle){point1.x_, point1.y_, abs(w) + 1, 1});
                printLightRectangle(stdout, (Rectangle){point2.x_, point2.y_, 1, abs(h) + 1});
            }
        }
        else if (h > 0) {
            if (rand() % 1000 < 500) {
                printLightRectangle(stdout, (Rectangle){point1.x_, point1.y_, abs(w) + 1, 1});
                printLightRectangle(stdout, (Rectangle){point2.x_, point1.y_, 1, abs(h) + 1});
            }
            else {
                printLightRectangle(stdout, (Rectangle){point1.x_, point2.y_, abs(w) + 1, 1});
                printLightRectangle(stdout, (Rectangle){point1.x_, point1.y_, 1, abs(h) + 1});
            }
        }
        else {
            printLightRectangle(stdout, (Rectangle){point1.x_, point1.y_, abs(w) + 1, 1});
        }
    }
    else {
        if (h < 0) {
            printLightRectangle(stdout, (Rectangle){point2.x_, point2.y_, 1, abs(h) + 1});
        }
        else if (h > 0) {
            printLightRectangle(stdout, (Rectangle){point1.x_, point1.y_, 1, abs(h) + 1});
        }
    }
}

/**
 * @brief ADT деления для размещения комнаты
*/

typedef struct sLeaf {
    Rectangle leafr_;
    struct sLeaf* lChild_;
    struct sLeaf* rChild_;
} Leaf;

typedef void (*ModifyLeafFunctionT)(Leaf* const leaf);

void Leaf_init(Leaf* const self, Rectangle const leafr);
Leaf* Leaf_create(Rectangle const leafr);
void Leaf_destroy(Leaf* const leaf);
bool Leaf_split(Leaf* const self);
void Leaf_splitToPrimitives(Leaf* const self, RoomsHadnler* rh);
//void Leaf_makeRoomForTopLeaf(Leaf* const);
//Rectangle Leaf_getRoom(const Leaf* self);

void createRoom(Leaf* const leaf, RoomsHadnler* rh);


void Leaf_init(Leaf* const self, Rectangle const leafr) {
    self->leafr_ = leafr;
    self->lChild_ = NULL;
    self->rChild_ = NULL;
}

Leaf* Leaf_create(Rectangle const leafr) {
    Leaf* ret = malloc(sizeof(Leaf));
    if(!ret)
        error("out of memory");
    Leaf_init(ret, leafr);
}

void Leaf_destroy(Leaf* const leaf) {
    if(leaf->lChild_)
        Leaf_destroy(leaf->lChild_);
    if(leaf->rChild_)
        Leaf_destroy(leaf->rChild_);

    free(leaf);
}

// разделение на 2 потомка
bool Leaf_split(Leaf* const self) {
    if(self->lChild_ != NULL || self->rChild_ != NULL)
        return false; // we're already split! Abort!

    // определим способ разделения (по вертикали или по горизонтали)
    bool splitH;

    if ((float)(self->leafr_.w_) / self->leafr_.h_ >= 1.25)
        splitH = false;
    else if ((float)(self->leafr_.h_) / self->leafr_.w_ >= 1.25)
        splitH = true;
    else
        splitH = (rand() % 1000 >= 500);

    // проверка того, что части не будут меньше минимального порога
    uint16_t max = (splitH ? self->leafr_.h_ : self->leafr_.w_) - DEFAULT_MIN_LEAF_SIZE;
    if (max <= DEFAULT_MIN_LEAF_SIZE)
        return false;

    uint16_t split = rand() % (max - DEFAULT_MIN_LEAF_SIZE + 1) + DEFAULT_MIN_LEAF_SIZE; // determine where we're going to split
    // разделение на части
    if (splitH) {
        self->lChild_ = Leaf_create((Rectangle){self->leafr_.x_, self->leafr_.y_, self->leafr_.w_, split});
        self->rChild_ = Leaf_create((Rectangle){self->leafr_.x_, self->leafr_.y_ + split, self->leafr_.w_, self->leafr_.h_ - split});
    } else {
        self->lChild_ = Leaf_create((Rectangle){self->leafr_.x_, self->leafr_.y_, split, self->leafr_.h_});
        self->rChild_ = Leaf_create((Rectangle){self->leafr_.x_ + split, self->leafr_.y_, self->leafr_.w_ - split, self->leafr_.h_});
    }

    return true; // успех
}

// разделить рекурсивно
void Leaf_splitToPrimitives(Leaf* const self, RoomsHadnler* rh) {
    if(self->leafr_.w_ > DEFAULT_MAX_LEAF_SIZE || self->leafr_.h_ > DEFAULT_MAX_LEAF_SIZE || rand() % 1001 > 250) {
        if(Leaf_split(self)) {
            Leaf_splitToPrimitives(self->lChild_, rh);
            Leaf_splitToPrimitives(self->rChild_, rh);
        }
        else
            createRoom(self, rh);
    }
    else
        createRoom(self, rh);
}

void createRoom(Leaf* const leaf, RoomsHadnler* rh) {
    Rectangle r;
    r.w_ = rand() % (leaf->leafr_.w_ - 2 * DEFAULT_MIN_ROOM_MARGIN - MIN_ROOM_SIZE + 1) + MIN_ROOM_SIZE;
    r.h_ = rand() % (leaf->leafr_.h_ - 2 * DEFAULT_MIN_ROOM_MARGIN - MIN_ROOM_SIZE + 1) + MIN_ROOM_SIZE;
    r.x_ = leaf->leafr_.x_ + DEFAULT_MIN_ROOM_MARGIN + rand() % (leaf->leafr_.w_ - 2 * DEFAULT_MIN_ROOM_MARGIN - r.w_ + 1);
    r.y_ = leaf->leafr_.y_ + DEFAULT_MIN_ROOM_MARGIN + rand() % (leaf->leafr_.h_ - 2 * DEFAULT_MIN_ROOM_MARGIN - r.h_ + 1);
    printLightRectangle(stdout, r);
    RH_addRoom(rh, r);
}

//Rectangle Leaf_getRoom(const Leaf* self) {
//    // iterate all the way through these leafs to find a room, if one exists.
//    if (self->lChild_ == NULL && self->rChild_ == NULL) // 
//        return self->roomr_;
//    else {
//        Rectangle lroom;
//        Rectangle rroom;
//
//        if (self->lChild_ != NULL) {
//            lroom = Leaf_getRoom(self->lChild_);
//        }
//        if (self->rChild_ != NULL) {
//            rroom = Leaf_getRoom(self->rChild_);
//        }
//        if (lroom.w_ == 0 && rroom.w_ == 0)
//            return (Rectangle){0};
//        else if (rroom.w_ == 0)
//            return lroom;
//        else if (lroom.w_ == 0)
//            return rroom;
//        else if (rand() % 1000 > 499)
//            return lroom;
//        else
//            return rroom;
//    }
//
//}



int main(int argc, char* argv[]) {
    printf("map %"PRIu32" %"PRIu32" 0 0 0\n", DEFAULT_WIDTH, DEFAULT_HEIGHT);
    srand(time(NULL));

    Leaf* root = Leaf_create((Rectangle){0, 0, DEFAULT_WIDTH, DEFAULT_HEIGHT});
    RoomsHadnler rh;
    RH_init(&rh, (DEFAULT_WIDTH * DEFAULT_HEIGHT) / (DEFAULT_MIN_LEAF_SIZE * DEFAULT_MIN_LEAF_SIZE));

    Leaf_splitToPrimitives(root, &rh);
    //Leaf_makeRoomForTopLeaf(root);
    RH_calculateHalls(&rh);

    Leaf_destroy(root);
    RH_deinit(&rh);

    return 0;
}




