/**
 ******************************************************************************
 * @file    maze_viewer.c
 * @author  'Mihail Kozlov'
 * @date
 * @brief Модуль для построения сгенерированного лабиринта при помощи клеточных автоматов.
 * Для отображения используется игровой движок Allegro-5.
 * Программа последовательно читает команды из стандартно ввода и строит изображение
 * Результат построения зависит от:
 * ---------------------------------------------------------------------------------
 * N | имя параметра                                 | аргумент cmd | комментарий
 * ---------------------------------------------------------------------------------
 * 1.| размера поля                                  | -r WxH       | W и H -целые числа размеры поля
 * 2.| включить таймер для пролистывания кадров      | -t arg       | arg - float, время в секундах между пролистываниями кадров 
 * 3.| включить режим покомандного отображения       | -d           | в данном режиме будут последователь отображаться команды для построения фигур
 ******************************************************************************
 */

#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

enum {
    DEFAULT_XRES = 640,     // разрешение экрана по умолчани.
    DEFAULT_YRES = 480,
    STATUS_HEAIGHT = 10
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

static inline ALLEGRO_COLOR Colour_transformToAl(Colour c) {
    return al_map_rgb(c.r_, c.g_, c.b_);
}

/**
 * @brief интерфейс "абстрактной фигуры"
*/
typedef struct sFigure {
    void* self_;
    void (*draw_)(void* const self);
} Figure;


/**
 * @brief ADT точки
*/
typedef struct sDot {
    float x1_;
    float y1_;
    float x2_;
    float y2_;
    Colour colour_;
} Dot;

void Dot_init(Dot *self, const char* str, float xScale, float yScale);
void Dot_raw(void *const self);


void Dot_init(Dot *self, const char* str, float xScale, float yScale) {
    size_t res = sscanf(
        str,
        "dot %f %f %"SCNu8" %"SCNu8" %"SCNu8"", 
        &self->x1_,
        &self->y1_,
        &self->colour_.r_,
        &self->colour_.g_,
        &self->colour_.b_
    );

    if(res != 5){
        char temp[strlen(str) + 50];
        sprintf(temp, "invalid data for dot construction [%s]", str);
        error(temp);
    }

    self->x1_ *= xScale;
    self->y1_ *= yScale;

    self->x2_ = self->x1_ + xScale;
    self->y2_ = self->y1_ + yScale;
}

void Dot_draw(void* const self) {
    assert(self);
    Dot* dot = self;
    al_draw_filled_rectangle(dot->x1_, dot->y1_, dot->x2_, dot->y2_, Colour_transformToAl(dot->colour_));
}

/**
 * @brief ADT прямоугольника
*/
typedef struct sRectangle {
    float x1_;
    float y1_;
    float x2_;
    float y2_;
    Colour colour_;
} Rectangle;

void Rectangle_init(Rectangle *self, const char* str, float xScale, float yScale);
void Rectangle_draw(void *const self);

void Rectangle_init(Rectangle *self, const char* str, float xScale, float yScale) {
    uint16_t x, y, w, h;
    
    size_t res = sscanf(
        str,
        "rectangle %"SCNu16" %"SCNu16" %"SCNu16" %"SCNu16" %"SCNu8" %"SCNu8" %"SCNu8"", 
        &x,
        &y,
        &w,
        &h,
        &self->colour_.r_,
        &self->colour_.g_,
        &self->colour_.b_
    );

    if(res != 7){
        char temp[strlen(str) + 50];
        sprintf(temp, "invalid data for dot construction [%s]", str);
        error(temp);
    }

    self->x1_ = x * xScale;
    self->y1_ = y * yScale;

    self->x2_ = self->x1_ + w * xScale;
    self->y2_ = self->y1_ + h * yScale;
}

void Rectangle_draw(void *const self) {
    assert(self);
    Rectangle* rec = self;
    al_draw_filled_rectangle(rec->x1_, rec->y1_, rec->x2_, rec->y2_, Colour_transformToAl(rec->colour_));
}

/**
 * @brief фабрика для фигур
*/
Figure createFigure(const char* str, float xscale, float yscale);

/**
 * @brief "конструктор" точки
 * @note испольует кучу для создания точки
*/
Figure createDot(const char* str, float xScale, float yScale);
/**
 * @brief "конструктор" прямоугольника
 * @note испольует кучу для создания точки
*/
Figure createRectangle(const char* str, float xScale, float yScale);
/**
 * @brief обертка над рисованием абстрактной фигуры
*/
void drawFigure(Figure* const f);
/**
 * @brief освобождение ресурсов, занимаемых фигурой
*/
void destroyFigure(Figure* const f);

Figure createFigure(const char* str, float xscale, float yscale) {
    Figure ret;
    
    if (strstr(str, "dot"))
        ret = createDot(str, xscale, yscale);
    else if (strstr(str, "rectangle"))
        ret = createRectangle(str, xscale, yscale);
    else{
        fputs(str, stderr);
        error("invalid figure");
    }

    return ret;
}

Figure createDot(const char* str, float xScale, float yScale) {
    Dot *dot = malloc(sizeof(Dot));

    if(dot == NULL)
        error("out of memory");

    Dot_init(dot, str, xScale, yScale);
    
    return (Figure){
        .self_ = (void *)dot,
        .draw_ = Dot_draw
    };
}

Figure createRectangle(const char* str, float xScale, float yScale) {
    Rectangle *rec = malloc(sizeof(Rectangle));

    if(rec == NULL)
        error("out of memory");

    Rectangle_init(rec, str, xScale, yScale);
    
    return (Figure){
        .self_ = (void *)rec,
        .draw_ = Rectangle_draw
    };
}

void drawFigure(Figure *const f) {
    f->draw_(f->self_);
}

void destroyFigure(Figure *const f) {
    free(f->self_);
    f->draw_ = NULL;
}

/**
 * @brief класс - загрузчик фигур. читает данные с потока до delim (метка кадра) или до конца файла
*/
typedef struct sLoader {
    FILE* src_;
    bool isEof_;
    float xScale_;
    float yScale_;
} Loader;

void Loader_init(Loader* const self, FILE* src, float xScale, float yScale);
size_t Loader_loadFigureArray(Loader* const self, Figure* fa, size_t size);
bool Loader_isLoadingComplete(const Loader *const self);
void freeFigureArray(Figure *fa, size_t size);

void Loader_init(Loader* const self, FILE* src, float xScale, float yScale) {
    assert(src);
    self->src_ = src;
    self->isEof_ = false;
    self->xScale_ = xScale;
    self->yScale_ = yScale;
}

size_t Loader_loadFigureArray(Loader* const self, Figure* fa, size_t size) {
    size_t ret = 0;
    char temp[255] = "";    // буфер для загрузки строки из файла

    for (; ret < size; ++ret) {
        if(fgets(temp, 255, self->src_) == NULL) {
            self->isEof_ = true;
            break;
        }

        if(strstr(temp, "delim"))
            break;

        fa[ret] = createFigure(temp, self->xScale_, self->yScale_);
    }

    return ret;
}

bool Loader_isLoadingComplete(const Loader* const self) {
    return self->isEof_;
}

void freeFigureArray(Figure* fa, size_t size) {
    for (size_t i = 0; i < size; ++i){
        destroyFigure(fa + i);
    }
}

/**
 * @brief ADT строки состояния
*/
typedef struct sStatusString {
    float y1_;      // смещение относительно НК
    float y2_;
    float width_;   // длина статусной строки
    ALLEGRO_FONT* font_;
} StatusString;

void SS_init(StatusString* const self, float shift, float width);
void SS_deinit(StatusString* const self);
float SS_getHeight(const StatusString* const self);
void SS_print(StatusString* const self, const char* str);

void SS_init(StatusString* const self, float shift, float width) {
    self->font_ = al_create_builtin_font();
    self->y1_ = shift + 2;
    self->y2_ = shift + STATUS_HEAIGHT + 2;
    self->width_ = width;
}

void SS_deinit(StatusString* const self) {
    al_destroy_font(self->font_);
}

float SS_getHeight(const StatusString* const self) {
    return self->y2_ - self->y1_;
}

void SS_print(StatusString* const self, const char* str) {
    al_draw_filled_rectangle(0, self->y1_, self->width_, self->y2_, al_map_rgb(0, 0, 0));
    al_draw_text(self->font_, al_map_rgb(255, 255, 255), 5, self->y1_, 0, str);
}

struct sDisplayHandler;

typedef void (*DH_DrawFunctionT)(struct sDisplayHandler* self, Figure* fa, size_t size);

typedef struct sDisplayHandler {
    ALLEGRO_DISPLAY* display_;
    ALLEGRO_EVENT_QUEUE* queue_;
    ALLEGRO_TIMER* timer_;
    DH_DrawFunctionT draw_;
    StatusString statusString_;
    size_t frameNum_;
} DisplayHandler;

/**
 * @brief исходная инициализация ADT handler'а графического отображения
 * задает разрешение по умолчанию и покадровое перелистывание по нажатию клавиши
*/
void DH_init(DisplayHandler* const self, uint16_t w, uint16_t h, Colour dc);

// в deinit будем ожидать закрытия окна
void DH_deinit(DisplayHandler* const self);

void DH_enableTimer(DisplayHandler* const self, float time);
void DH_enablePrimitiveMode(DisplayHandler* const self);

void DH_drawPerPrimitive(DisplayHandler* const self, Figure* fa, size_t size);
void DH_drawPerFrame(DisplayHandler* const self, Figure* fa, size_t size);


void DH_init(DisplayHandler *const self, uint16_t w, uint16_t h, Colour dc){
    al_init();
    al_init_primitives_addon();

    self->frameNum_ = 0;
    SS_init(&self->statusString_, h, w);


    self->display_ = al_create_display(w, h + SS_getHeight(&self->statusString_));
    self->queue_ = al_create_event_queue();
    self->timer_ = NULL;

    al_install_keyboard();

    al_register_event_source(self->queue_, al_get_display_event_source(self->display_));
    al_register_event_source(self->queue_, al_get_keyboard_event_source());

    al_clear_to_color(Colour_transformToAl(dc));

    self->draw_ = DH_drawPerFrame;
    SS_print(&self->statusString_, "init");

    al_flip_display();
}

void DH_deinit(DisplayHandler *const self) {
    char temp[100];
    sprintf(temp, "%s: %lu (finished)", self->draw_ == DH_drawPerFrame ? "frame" : "diff", self->frameNum_);
    SS_print(&self->statusString_, temp);
    al_flip_display();

    ALLEGRO_EVENT event;
    do{
        al_wait_for_event(self->queue_, &event);
    }
    while(event.type != ALLEGRO_EVENT_DISPLAY_CLOSE);

    SS_deinit(&self->statusString_);
    al_destroy_display(self->display_);
    al_uninstall_keyboard();
    
    if(self->timer_)
        al_destroy_timer(self->timer_);
    
    al_destroy_event_queue(self->queue_);
}


void DH_enableTimer(DisplayHandler* const self, float time) {
    self->timer_ = al_create_timer(time);
    al_register_event_source(self->queue_, al_get_timer_event_source(self->timer_));
    al_start_timer(self->timer_);
}

void DH_enablePrimitiveMode(DisplayHandler* const self) {
    self->draw_ = DH_drawPerPrimitive;
}

static inline void waitTimerOrKeyUp(ALLEGRO_EVENT_QUEUE* q) {
    ALLEGRO_EVENT event;
    
    do{
        al_wait_for_event(q, &event);
    }
    while(event.type != ALLEGRO_EVENT_TIMER && event.type != ALLEGRO_EVENT_KEY_UP);
}

void DH_drawPerPrimitive(DisplayHandler* const self, Figure* fa, size_t size) {
    char temp[32];

    for(size_t i = 0; i < size; ++i){
        sprintf(temp, "diff: %lu", ++self->frameNum_);
        SS_print(&self->statusString_, temp);

        drawFigure(fa + i);
        waitTimerOrKeyUp(self->queue_);
        al_flip_display();
    }
}

void DH_drawPerFrame(DisplayHandler* const self, Figure* fa, size_t size) {
    char temp[32];
    sprintf(temp, "frame: %lu", ++self->frameNum_);
    SS_print(&self->statusString_, temp);

    for(size_t i = 0; i < size; ++i)
        drawFigure(fa + i);

    waitTimerOrKeyUp(self->queue_);
    al_flip_display();
}



int main(int argc, char* argv[]) {
    uint16_t width, height;
    Colour colour;
    // считаем размер поля лабиринта и исходный цвет
    uint32_t ctrl = scanf("map %"SCNu16" %"SCNu16" %"SCNu8" %"SCNu8" %"SCNu8"\n", &width, &height, &colour.r_, &colour.g_, &colour.b_);
    assert (ctrl == 5);

    int xres = DEFAULT_XRES;
    int yres = DEFAULT_YRES;

    if(argc > 1 && getopt(argc, argv, "r:"))
        sscanf(optarg, "%dx%d", &xres, &yres);

    float xscale = (float)xres / width;
    float yscale = (float)yres / height;

    DisplayHandler dh;
    DH_init(&dh, xres, yres, colour);

    optind = 1;
    char ch;
    while ((ch = getopt(argc, argv, "rdt:")) != EOF) {
        switch (ch) {
        case 'd':
            DH_enablePrimitiveMode(&dh);
            break;
        case 't':
            float time = atof(optarg);
            DH_enableTimer(&dh, time);
            break;
        default:
            break;
        }
    }

    Loader loader;
    Loader_init(&loader, stdin, xscale, yscale);

    size_t fgArraySize = width * height + 1;
    Figure fgArray[fgArraySize];

    while (!Loader_isLoadingComplete(&loader)) {
        size_t lSize = Loader_loadFigureArray(&loader, fgArray, fgArraySize);

        dh.draw_(&dh, fgArray, lSize);
        freeFigureArray(fgArray, lSize);
    }

    DH_deinit(&dh);

    return 0;
}





