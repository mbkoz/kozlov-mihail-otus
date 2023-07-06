#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <inttypes.h>
#include <unistd.h>

/**
 * параметры командной строки:
 * размер поля: -r 100x100
 * шанс появления живой клетки: -l 0.45
 * кoличество итераций: -n 4
 * порог смерти: -d 3
 * порог рождения: -b 4
*/

/**
 * параметры по умолчанию (DEFAULT_) и именованные константы
*/
enum {
    LIFE_CHANCE_MULT = 1000, // коэффициент для приведения float [0; 1] в единицы 0.1%/LSB
    LIFE_CHANCE_RAND_LIM = LIFE_CHANCE_MULT + 1,
    DEFAULT_LIFE_CHANCE = 420, // вероятность жизни в клетке по умолчанию 45.1%
    DEFAULT_HEIGHT = 100,
    DEFAULT_WIDTH = 100,
    DEFAULT_ITER_NUMBER = 4,
    DEFAULT_BIRTH_LIMIT = 4,
    DEFAULT_DEATH_LIMIT = 3
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

static const Colour LIGHT_COLOUR = {.r_ = 105, .g_ = 105, .b_ = 105};
static const Colour DARK_COLOUR = {.r_ = 173, .g_ = 255, .b_ = 47};

/**
 * @brief структура для размеров или линейных координат
*/
typedef struct sDimensions {
    uint16_t x_;
    uint16_t y_;
} Dimensions;

static inline bool isCoordInside(Dimensions size, Dimensions coord) {
    return size.x_ > coord.x_ && size.y_ > coord.y_;
}

static inline void printColouredDot(FILE* file, Dimensions coord, Colour colour) {
    fprintf(
        file, 
        "dot %"PRIu16" %"PRIu16" %"PRIu8" %"PRIu8" %"PRIu8"\n", 
        coord.x_, 
        coord.y_,
        colour.r_,
        colour.g_,
        colour.b_
    );
}

static inline void printDarkDot(FILE* file, Dimensions coord) {
    printColouredDot(file, coord, DARK_COLOUR);
}

static inline void printLightDot(FILE* file, Dimensions coord) {
    printColouredDot(file, coord, LIGHT_COLOUR);
}


/**
 * @brief ADT для представления поля с клетками
 * @note использует кучу. После инициализации или клонирования необходима деинициализация
*/
typedef struct sCellmap {
    Dimensions size_;
    void* mapptr_;
} Cellmap;

static inline void cellmapInit(Cellmap* const cm, Dimensions size) {
    assert(size.x_ && size.y_);

    cm->size_ = size;
    cm->mapptr_ = malloc(sizeof(bool [size.x_][size.y_]));

    if(cm->mapptr_ == NULL)
        error("out of memory");
}

static inline void cellmapDeinit(Cellmap *const cm) {
    free(cm->mapptr_);
}

static inline bool cellmapIsCellAlive(const Cellmap* const cm, Dimensions coord) {
    assert(isCoordInside(cm->size_, coord));
    return ((bool(*)[cm->size_.y_])cm->mapptr_)[coord.x_][coord.y_];
}

static inline void cellmapSetCellState(Cellmap* const cm, Dimensions coord, bool state) {
    assert(isCoordInside(cm->size_, coord));
    ((bool(*)[cm->size_.y_])cm->mapptr_)[coord.x_][coord.y_] = state;
}

static inline void cellmapKillCell(Cellmap* const cm, Dimensions coord) {
    cellmapSetCellState(cm, coord, false);
}

static inline void cellmapBirthCell(Cellmap* const cm, Dimensions coord) {
    cellmapSetCellState(cm, coord, true);
}
/**
 * @brief получить глубокую копию поля клеток
*/
static inline Cellmap cellmapClone(Cellmap* const cm) {
    Cellmap ret;
    cellmapInit(&ret, cm->size_);
    memcpy(ret.mapptr_, cm->mapptr_, sizeof(bool[ret.size_.x_][ret.size_.y_]));
    return ret;
}

static inline Dimensions cellmapGetSize(const Cellmap* const cm) {
    return cm->size_;
}

/**
 * @brief посчитать количество живых соседей.
*/
static inline uint8_t CellmapCountAlive(const Cellmap *const cm, Dimensions coord) {
    uint16_t leftLim = coord.x_ > 0 ? coord.x_ - 1 : 0;
    uint16_t rigthLim = coord.x_ + 2 <= cm->size_.x_ ? coord.x_ + 2 : cm->size_.x_;

    uint32_t lowLim = coord.y_ > 0 ? coord.y_ - 1 : 0;
    uint32_t upLim = coord.y_ + 2 <= cm->size_.y_ ? coord.y_ + 2 : cm->size_.y_;

    uint32_t i = 0, ret = 0;

    for (uint32_t x = leftLim; x < rigthLim; ++x)
        for (uint32_t y = lowLim; y < upLim; ++y) 
            if(x != coord.x_ || y != coord.y_){
                ret = cellmapIsCellAlive(cm, (Dimensions){x, y}) ? ret + 1 : ret;
                ++i;
            }
    // прибавим пограничные клетки, 8 - максимальное количество соседей
    ret += (8 - i);

    return ret;
}

/**
 * @brief ADT бизнес логики генератора лабиринта на клеточном автомате
*/
typedef struct sMazeGenerator {
    Cellmap cm_;
    FILE *log_;
    uint8_t birthLim_;
    uint8_t deathLim_;
    uint8_t iterNum_;
} MazeGenerator;

void mazeGeneratorInit(MazeGenerator *const mg, FILE* log, int argc, char* argv[]);
void mazeGeneratorDoIteration(MazeGenerator *const mg);
void mazeGeneratorGenerate(MazeGenerator *const mg);
void mazeGeneratorDeinit(MazeGenerator *const mg);


int main(int argc, char* argv[]) {
    MazeGenerator mg;

    mazeGeneratorInit(&mg, stdout, argc, argv);
    mazeGeneratorGenerate(&mg);
    mazeGeneratorDeinit(&mg);

    return 0;
}


void mazeGeneratorInit(MazeGenerator *const mg, FILE* log, int argc, char* argv[]) {
    uint16_t width = DEFAULT_WIDTH;
    uint16_t height = DEFAULT_HEIGHT;
    uint16_t lifeChance = DEFAULT_LIFE_CHANCE;

    mg->birthLim_ = DEFAULT_BIRTH_LIMIT;
    mg->deathLim_ = DEFAULT_DEATH_LIMIT;
    mg->iterNum_ = DEFAULT_ITER_NUMBER;

    char ch;
    while ((ch = getopt(argc, argv, "r:l:n:d:b:")) != EOF) {
        switch (ch) {
        case 'r':
            sscanf(optarg, "%" SCNu16 "x%" SCNu16 "", &width, &height);
            break;
        case 'n':
            mg->iterNum_ = atoi(optarg);
            break;
        case 'l':
            lifeChance = atof(optarg) * LIFE_CHANCE_MULT;
            break;
        case 'd':
            mg->deathLim_ = atoi(optarg);
            break;
        case 'b':
            mg->birthLim_ = atoi(optarg);
            break;
        default:
            break;
        }
    }

    mg->log_ = log;
    cellmapInit(&mg->cm_, (Dimensions){width, height});

    fprintf(mg->log_, "map %"PRIu32" %"PRIu32" 0 0 0\n", width, height);

    srand(time(NULL));

    for(uint16_t x = 0; x < width; ++x)
        for (uint16_t y = 0; y < height; ++y) {
            if(rand() % LIFE_CHANCE_RAND_LIM < lifeChance) {
                cellmapBirthCell(&mg->cm_, (Dimensions){x, y});
                printLightDot(mg->log_, (Dimensions){x, y});
            }
            else{
                cellmapKillCell(&mg->cm_, (Dimensions){x, y});
                printDarkDot(mg->log_, (Dimensions){x, y});
            }
        }

    fprintf(mg->log_, "delim\n");
}

void mazeGeneratorDoIteration(MazeGenerator *const mg) {
    Cellmap ref = cellmapClone(&mg->cm_);
    Dimensions size = cellmapGetSize(&mg->cm_);

    for (uint16_t x = 0; x < size.x_; ++x) {
        for (uint16_t y = 0; y < size.y_; ++y) {
            uint8_t ln = CellmapCountAlive(&ref, (Dimensions){x, y});
            if(cellmapIsCellAlive(&ref, (Dimensions){x, y}) && ln < mg->deathLim_) {
                cellmapKillCell(&mg->cm_, (Dimensions){x, y});
                printDarkDot(mg->log_, (Dimensions){x, y});
            }
            else if (!cellmapIsCellAlive(&ref, (Dimensions){x, y}) && ln > mg->birthLim_){
                cellmapBirthCell(&mg->cm_, (Dimensions){x, y});
                printLightDot(mg->log_, (Dimensions){x, y});
            }
        }
    }

    fprintf(mg->log_, "delim\n");
    cellmapDeinit(&ref);
}

void mazeGeneratorGenerate(MazeGenerator *const mg) {
    for (size_t i = 0; i < mg->iterNum_; ++i)
        mazeGeneratorDoIteration(mg);
}

void mazeGeneratorDeinit(MazeGenerator *const mg) {
    cellmapDeinit(&mg->cm_);
}
