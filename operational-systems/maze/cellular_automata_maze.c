#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <inttypes.h>

/**
 * TODO параметры командной строки:
 * 1. размер поля
 * 2. вероятность начать жизнь в клетке
 * 3. количество поколений автомата
 * 4. предел рождения
 * 5. предел смерти
*/

enum {
    LIFE_CHANCE_MULT = 1000, // коэффициент для приведения float [0; 1] в единицы 0.1%/LSB
    LIFE_CHANCE_RAND_LIM = LIFE_CHANCE_MULT + 1,
    DEFAULT_LIFE_CHANCE = 400, // вероятность жизни в клетке по умолчанию 45.1%
    DEFAULT_HEIGHT = 100,
    DEFAULT_WIDTH = 100,
    DEFAULT_ITER_NUMBER = 4,
    DEFAULT_BIRTH_LIMIT = 4,
    DEFAULT_DEATH_LIMIT = 3
};

typedef struct sCoord {
    uint32_t x_;
    uint32_t y_;
} Coord;

static inline void printDarkDot(FILE* file, Coord coord) {
    fprintf(file, "dot %"PRIu32" %"PRIu32" 0 0 0\n", coord.x_, coord.y_);
}

static inline void printLightDot(FILE* file, Coord coord) {
    fprintf(file, "dot %"PRIu32" %"PRIu32" 255 255 255\n", coord.x_, coord.y_);
}

typedef struct sCellmap {
    uint32_t width_;
    uint32_t height_;
    void *mapptr_;
    FILE *output_;
} Cellmap;

static inline uint32_t CellmapGetsize(Cellmap* const cellmap) {
    return sizeof(bool[cellmap->width_][cellmap->height_]);
}

/**
 * @brief функция инициализации ADT поля клеток
 * @param cellmap указать на структуру для инициализации
 * @param width ширина поля
 * @param height выстоа поля
 * @param output указатель на поток вывода лога создания лабиринта
 * @param lifeProp вероятность того, что клетка - живая
 * @note использует кучу, необходима деинициализация
*/
void CellmapInit(Cellmap *const cellmap, uint32_t width, uint32_t height, FILE* output, uint32_t lifeChance);
/**
 * @brief посчитать количество живых клеток вокруг координаты
*/
uint32_t CellmapCountAlive(const Cellmap *const cellmap, Coord target);
/**
 * @brief произвести 1 итерацию клеточного автомата
*/
void CellmapDoIteration(Cellmap *const cellmap, uint32_t birthLim, uint32_t deathLim);
/**
 * @brief деаллокация ресурсов
*/
void CellmapDeinit(Cellmap *const cellmap);


int main(int argc, char* argv[]) {
    Cellmap cellmap;

    CellmapInit(
        &cellmap,
        DEFAULT_WIDTH,
        DEFAULT_HEIGHT,
        stdout,
        DEFAULT_LIFE_CHANCE
    );

    for (uint32_t i = 0; i < DEFAULT_ITER_NUMBER; ++i){
        CellmapDoIteration(&cellmap, DEFAULT_BIRTH_LIMIT, DEFAULT_DEATH_LIMIT);
    }

    CellmapDeinit(&cellmap);

    return 0;
}

void CellmapInit(Cellmap *const cellmap, uint32_t width, uint32_t height, FILE* output, uint32_t lifeChance) {
    assert(cellmap);
    assert(width);
    assert(height);
    assert(output);

    *cellmap = (Cellmap){
        .height_ = height,
        .width_ = width,
        .output_ = output,
        .mapptr_ = malloc(sizeof(bool[width][height]))
    };

    fprintf(cellmap->output_, "map %"PRIu32" %"PRIu32" 0 0 0\n", width, height);

    srand(time(NULL));
    bool(*temp)[height] = (bool (*)[height])cellmap->mapptr_;

    for (uint32_t x = 0; x < width; ++x)
        for (uint32_t y = 0; y < height; ++y) {
            if(rand() % LIFE_CHANCE_RAND_LIM < lifeChance) {
                temp[x][y] = true;
                printLightDot(cellmap->output_, (Coord){.x_ = x, .y_ = y});
            }
            else
                temp[x][y] = false;
        }

    fprintf(cellmap->output_, "delim\n");
}

uint32_t CellmapCountAlive(const Cellmap *const cellmap, Coord target) {
    uint32_t leftLim = target.x_ > 0 ? target.x_ - 1 : 0;
    uint32_t rigthLim = target.x_ + 2 <= cellmap->width_ ? target.x_ + 2 : cellmap->width_;

    uint32_t lowLim = target.y_ > 0 ? target.y_ - 1 : 0;
    uint32_t upLim = target.y_ + 2 <= cellmap->height_ ? target.y_ + 2 : cellmap->height_;

    uint32_t i = 0, ret = 0;
    bool (*temp)[cellmap->height_] = (bool (*)[cellmap->height_])cellmap->mapptr_;

    for (uint32_t x = leftLim; x < rigthLim; ++x)
        for (uint32_t y = lowLim; y < upLim; ++y) 
            if(x != target.x_ || y != target.y_){
                ret = temp[x][y] ? ret + 1 : ret;
                ++i;
            }
    // прибавим пограничные клетки
    ret += (8 - i);

    return ret;
}

void CellmapDoIteration(Cellmap *const cellmap, uint32_t birthLim, uint32_t deathLim) {
    bool(*newmap)[cellmap->height_] = malloc(CellmapGetsize(cellmap));
    bool(*oldmap)[cellmap->height_] = cellmap->mapptr_;

    for (uint32_t x = 0; x < cellmap->width_; ++x)
        for (uint32_t y = 0; y < cellmap->height_; ++y) {
            uint32_t ln = CellmapCountAlive(cellmap, (Coord){.x_ = x, .y_ = y});

            if(oldmap[x][y]) {
                if(ln < deathLim){
                    newmap[x][y] = false;
                    printDarkDot(cellmap->output_, (Coord){.x_ = x, .y_ = y});
                }
                else {
                    newmap[x][y] = true;
                }
            }
            else {
                if(ln > birthLim) {
                    newmap[x][y] = true;
                    printLightDot(cellmap->output_, (Coord){.x_ = x, .y_ = y});
                }
                else {
                    newmap[x][y] = false;
                }
            }
        }

    fprintf(cellmap->output_, "delim\n");

    free(oldmap);
    cellmap->mapptr_ = newmap;
}

void CellmapDeinit(Cellmap *const cellmap) {
    free(cellmap->mapptr_);
}
