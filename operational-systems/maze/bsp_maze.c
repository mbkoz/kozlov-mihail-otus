#include <unistd.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <inttypes.h>
#include <time.h>

/**
 * параметры командной строки:
 * 1. размер поля
 * 2. минимальный размер деления
 * 3. максимальный размер деления
 * 4. вероятность разбить комнату большего размера на 2 меньше
 * 5. размер padding'а
*/

enum {
    DEFAULT_HEIGHT = 100,
    DEFAULT_WIDTH = 100,
    DEFAULT_MIN_LEAF_SIZE = 6,  // DEFAULT_MIN_LEAF_SIZE / 2 - минимальный размер комнаты
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
//static const Colour DARK_COLOUR = {.r_ = 0, .g_ = 0, .b_ = 0};

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

static inline void setMargin(Rectangle* const r, uint16_t margin){
    r->x_ += margin;
    r->y_ += margin;
    r->w_ -= 2 * margin;
    r->h_ -= 2 * margin;
}

/**
 * @brief ADT деления для размещения комнаты
*/

typedef struct sLeaf {
    Rectangle leafr_;
    Rectangle roomr_;
    //Rectangle halls_[10];
    struct sLeaf* lChild_;
    struct sLeaf* rChild_;
} Leaf;

typedef void (*ModifyLeafFunctionT)(Leaf* const leaf);

void Leaf_init(Leaf* const self, Rectangle const leafr);
Leaf* Leaf_create(Rectangle const leafr);
void Leaf_destroy(Leaf* const leaf);
bool Leaf_split(Leaf* const self);
void Leaf_splitToPrimitives(Leaf* const self);
void Leaf_forEachTopLeaf(Leaf* const, ModifyLeafFunctionT modifyLeaf);

void createRoom(Leaf* const leaf);

void Leaf_init(Leaf* const self, Rectangle const leafr) {
    self->leafr_ = leafr;
    self->roomr_ = (Rectangle){0};
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

bool Leaf_split(Leaf* const self) {
    // begin splitting the leaf into two children

    if(self->lChild_ != NULL || self->rChild_ != NULL)
        return false; // we're already split! Abort!

    // determine direction of split
    // if the width is >25% larger than height, we split vertically
    // if the height is >25% larger than the width, we split horizontally
    // otherwise we split randomly

    bool splitH;

    if ((float)(self->leafr_.w_) / self->leafr_.h_ >= 1.25)
        splitH = false;
    else if ((float)(self->leafr_.h_) / self->leafr_.w_ >= 1.25)
        splitH = true;
    else
        splitH = (rand() % 1000 >= 500);

    // check MIN_LEAF_SIZE
    uint16_t max = (splitH ? self->leafr_.h_ : self->leafr_.w_) - DEFAULT_MIN_LEAF_SIZE;
    if (max <= DEFAULT_MIN_LEAF_SIZE)
        return false; // the area is too small to split any more...

    uint16_t split = rand() % (max - DEFAULT_MIN_LEAF_SIZE + 1) + DEFAULT_MIN_LEAF_SIZE; // determine where we're going to split
    // create our left and right children based on the direction of the split
    if (splitH) {
        self->lChild_ = Leaf_create((Rectangle){self->leafr_.x_, self->leafr_.y_, self->leafr_.w_, split});
        self->rChild_ = Leaf_create((Rectangle){self->leafr_.x_, self->leafr_.y_ + split, self->leafr_.w_, self->leafr_.h_ - split});
    } else {
        self->lChild_ = Leaf_create((Rectangle){self->leafr_.x_, self->leafr_.y_, split, self->leafr_.h_});
        self->rChild_ = Leaf_create((Rectangle){self->leafr_.x_ + split, self->leafr_.y_, self->leafr_.w_ - split, self->leafr_.h_});
    }

    return true; // split successful!
}

void Leaf_splitToPrimitives(Leaf* const self) {
    if(self->leafr_.w_ > DEFAULT_MAX_LEAF_SIZE || self->leafr_.h_ > DEFAULT_MAX_LEAF_SIZE || rand() % 1001 > 250) {
        if(Leaf_split(self)) {
            Leaf_splitToPrimitives(self->lChild_);
            Leaf_splitToPrimitives(self->rChild_);
        }
    }
}

void Leaf_forEachTopLeaf(Leaf* const leaf, ModifyLeafFunctionT modifyLeaf) {
    if(leaf->lChild_)
        Leaf_forEachTopLeaf(leaf->lChild_, modifyLeaf);
    if(leaf->rChild_)
        Leaf_forEachTopLeaf(leaf->rChild_, modifyLeaf);

    if(!leaf->lChild_ && !leaf->rChild_)
        modifyLeaf(leaf);
}

void createRoom(Leaf* const leaf) {
    Rectangle r;
    r.w_ = rand() % (leaf->leafr_.w_ - 2 * DEFAULT_MIN_ROOM_MARGIN - MIN_ROOM_SIZE + 1) + MIN_ROOM_SIZE;
    r.h_ = rand() % (leaf->leafr_.h_ - 2 * DEFAULT_MIN_ROOM_MARGIN - MIN_ROOM_SIZE + 1) + MIN_ROOM_SIZE;
    r.x_ = leaf->leafr_.x_ + DEFAULT_MIN_ROOM_MARGIN + rand() % (leaf->leafr_.w_ - 2 * DEFAULT_MIN_ROOM_MARGIN - r.w_ + 1);
    r.y_ = leaf->leafr_.y_ + DEFAULT_MIN_ROOM_MARGIN + rand() % (leaf->leafr_.h_ - 2 * DEFAULT_MIN_ROOM_MARGIN - r.h_ + 1);
    leaf->roomr_ = r;
    printLightRectangle(stdout, r);
}

int main(int argc, char* argv[]) {
    printf("map %"PRIu32" %"PRIu32" 0 0 0\n", DEFAULT_WIDTH, DEFAULT_HEIGHT);
    srand(time(NULL));

    Leaf* root = Leaf_create((Rectangle){0, 0, DEFAULT_WIDTH, DEFAULT_HEIGHT});

    Leaf_splitToPrimitives(root);
    Leaf_forEachTopLeaf(root, createRoom);

    Leaf_destroy(root);

    return 0;
}




