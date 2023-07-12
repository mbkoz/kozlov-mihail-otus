/**
 ******************************************************************************
 * @file    cellular_automata_maze.c
 * @author  'Mihail Kozlov'
 * @date
 * @brief Модуль для генерации лабиринта при помощи клеточных автоматов.
 * Результат генерации зависит от:
 * ---------------------------------------------------------------------------------
 * N | имя параметра                                 | аргумент cmd | комментарий
 * ---------------------------------------------------------------------------------
 * 1.| размера поля                                  | -r WxH       | W и H -целые числа размеры поля
 * 2.| вероятности начать жизнь в клетке при старте  | -l arg       | arg - 0...1 
 * 3.| количества итераций                           | -n arg       | arg - целое число
 * 4.| порога количества соседей для рождения клетки | -b arg       | arg - целое число
 * 5.| порога количества соседей для смерти клетки   | -d arg       | arg - целое число
 ******************************************************************************
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <inttypes.h>
#include <unistd.h>
#include <getopt.h>

// параметры по умолчанию (DEFAULT_) и именованные константы
enum {
    LIFE_CHANCE_MULT = 1000,                        // коэффициент для приведения float [0; 1] в единицы 0.1%/LSB
    LIFE_CHANCE_RAND_LIM = LIFE_CHANCE_MULT + 1,
    DEFAULT_LIFE_CHANCE = 420,                      // вероятность жизни в клетке по умолчанию 42.0%
    DEFAULT_HEIGHT = 100,
    DEFAULT_WIDTH = 100,
    DEFAULT_ITER_NUMBER = 4,
    DEFAULT_BIRTH_LIMIT = 4,
    DEFAULT_DEATH_LIMIT = 3
};

/**
 * @brief аварийное завершение программы
 * @param str сообщение выведенное в stderr
*/
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

/**
 * @brief конструктор структуры cellmap
 * @note использует кучу, требует вызова Cellmap_deinit после завершения работы
*/
static inline void Cellmap_init(Cellmap* const self, Dimensions size) {
    assert(size.x_ && size.y_);

    self->size_ = size;
    self->mapptr_ = malloc(sizeof(bool [size.x_][size.y_]));

    if(self->mapptr_ == NULL)
        error("out of memory");
}

static inline void Cellmap_deinit(Cellmap *const self) {
    free(self->mapptr_);
}

/**
 * @brief проверка того, является ли клетка живой
 * @return true - живая, false - мертвая
*/
static inline bool Cellmap_isCellAlive(const Cellmap* const self, Dimensions coord) {
    assert(isCoordInside(self->size_, coord));
    return ((bool(*)[self->size_.y_])self->mapptr_)[coord.x_][coord.y_];
}

/**
 * @brief установка состояния клетки
 * @param self - указатель на объект Cellmap
 * @param coord - координата клетки
 * @param state - состояние. true - живая, false - мертвая
*/
static inline void Cellmap_setCellState(Cellmap* const self, Dimensions coord, bool state) {
    assert(isCoordInside(self->size_, coord));
    ((bool(*)[self->size_.y_])self->mapptr_)[coord.x_][coord.y_] = state;
}

/**
 * @brief убить клетку по координатам
*/
static inline void Cellmap_killCell(Cellmap* const self, Dimensions coord) {
    Cellmap_setCellState(self, coord, false);
}

/**
 * @brief оживить клетку по координатам
*/
static inline void Cellmap_birthCell(Cellmap* const self, Dimensions coord) {
    Cellmap_setCellState(self, coord, true);
}
/**
 * @brief получить глубокую копию поля клеток
 * @return копия аргумента. Использует кучу
 * @note требует вызова деконструктора Cellmap_deinit
*/
static inline Cellmap Cellmap_clone(Cellmap* const self) {
    Cellmap ret;
    Cellmap_init(&ret, self->size_);
    memcpy(ret.mapptr_, self->mapptr_, sizeof(bool[ret.size_.x_][ret.size_.y_]));
    return ret;
}

static inline Dimensions Cellmap_getSize(const Cellmap* const self) {
    return self->size_;
}

/**
 * @brief посчитать количество живых соседей.
*/
static inline uint8_t Cellmap_countAlive(const Cellmap *const self, Dimensions coord) {
    uint16_t leftLim = coord.x_ > 0 ? coord.x_ - 1 : 0;
    uint16_t rigthLim = coord.x_ + 2 <= self->size_.x_ ? coord.x_ + 2 : self->size_.x_;

    uint32_t lowLim = coord.y_ > 0 ? coord.y_ - 1 : 0;
    uint32_t upLim = coord.y_ + 2 <= self->size_.y_ ? coord.y_ + 2 : self->size_.y_;

    uint32_t i = 0, ret = 0;

    for (uint32_t x = leftLim; x < rigthLim; ++x)
        for (uint32_t y = lowLim; y < upLim; ++y) 
            if(x != coord.x_ || y != coord.y_){
                ret = Cellmap_isCellAlive(self, (Dimensions){x, y}) ? ret + 1 : ret;
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

/**
 * @brief инициализировать бизнес логику создания лабиринта на клеточном автомате
 * @note в данном вызове будут читаться аргументы командной строки или использваться аргументы по умолчанию
 * @note класс использует кучу,требуется вызов MG_deinit
*/
void MG_init(MazeGenerator *const self, FILE* log, int argc, char* argv[]);

/**
 * @brief провести 1 шаг симуляции
*/
void MG_doIteration(MazeGenerator *const self);

/**
 * @brief провести все шаги симуляции
*/
void MG_generate(MazeGenerator *const self);
void MG_deinit(MazeGenerator *const self);


int main(int argc, char* argv[]) {
    MazeGenerator mg;

    MG_init(&mg, stdout, argc, argv);
    MG_generate(&mg);
    MG_deinit(&mg);

    return 0;
}


void MG_init(MazeGenerator *const self, FILE* log, int argc, char* argv[]) {
    uint16_t width = DEFAULT_WIDTH;
    uint16_t height = DEFAULT_HEIGHT;

    uint16_t lifeChance = DEFAULT_LIFE_CHANCE;

    self->birthLim_ = DEFAULT_BIRTH_LIMIT;
    self->deathLim_ = DEFAULT_DEATH_LIMIT;
    self->iterNum_ = DEFAULT_ITER_NUMBER;

    char ch;
    while ((ch = getopt(argc, argv, "r:l:n:d:b:")) != EOF) {
        switch (ch) {
        case 'r':
            sscanf(optarg, "%" SCNu16 "x%" SCNu16 "", &width, &height);
            break;
        case 'n':
            self->iterNum_ = atoi(optarg);
            break;
        case 'l':
            lifeChance = atof(optarg) * LIFE_CHANCE_MULT;
            break;
        case 'd':
            self->deathLim_ = atoi(optarg);
            break;
        case 'b':
            self->birthLim_ = atoi(optarg);
            break;
        default:
            break;
        }
    }

    self->log_ = log;
    Cellmap_init(&self->cm_, (Dimensions){width, height});

    fprintf(self->log_, "map %"PRIu32" %"PRIu32" 0 0 0\n", width, height);

    srand(time(NULL));

    for(uint16_t x = 0; x < width; ++x)
        for (uint16_t y = 0; y < height; ++y) {
            if(rand() % LIFE_CHANCE_RAND_LIM < lifeChance) {
                Cellmap_birthCell(&self->cm_, (Dimensions){x, y});
                printLightDot(self->log_, (Dimensions){x, y});
            }
            else{
                Cellmap_killCell(&self->cm_, (Dimensions){x, y});
                printDarkDot(self->log_, (Dimensions){x, y});
            }
        }

    fprintf(self->log_, "delim\n");
}

void MG_doIteration(MazeGenerator *const self) {
    Cellmap ref = Cellmap_clone(&self->cm_);
    Dimensions size = Cellmap_getSize(&self->cm_);

    for (uint16_t x = 0; x < size.x_; ++x) {
        for (uint16_t y = 0; y < size.y_; ++y) {
            uint8_t ln = Cellmap_countAlive(&ref, (Dimensions){x, y});
            if(Cellmap_isCellAlive(&ref, (Dimensions){x, y}) && ln < self->deathLim_) {
                Cellmap_killCell(&self->cm_, (Dimensions){x, y});
                printDarkDot(self->log_, (Dimensions){x, y});
            }
            else if (!Cellmap_isCellAlive(&ref, (Dimensions){x, y}) && ln > self->birthLim_){
                Cellmap_birthCell(&self->cm_, (Dimensions){x, y});
                printLightDot(self->log_, (Dimensions){x, y});
            }
        }
    }

    fprintf(self->log_, "delim\n");
    Cellmap_deinit(&ref);
}

void MG_generate(MazeGenerator *const self) {
    for (size_t i = 0; i < self->iterNum_; ++i)
        MG_doIteration(self);
}

void MG_deinit(MazeGenerator *const self) {
    Cellmap_deinit(&self->cm_);
}
