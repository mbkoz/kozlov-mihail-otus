#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>

/**
 * @brief программа читает со стандартного ввода поток команд для строительства лабиринта
 * тактики отображения:
 * покомандная смена кадров (нужен расчет частоты смены для построения лабиринта за 10 секунд)
 * покадровая смена кадров (нужен расчет частоты смены для построения лабиринта за 10 секунд)
 * смена кадров по нажатию клавиши пробел
 * кадры друг от друга отделены указанием delim
 * 
 * в качестве параметров командной строки использовать:
 * 1. разрешение экрана
 * 2. способ прокрутки кадров (покомандно, покадрово, по нажатию, по таймеру)
 * 3. длительноть анимации для таймера
*/

enum {
    DEFAULT_HRES = 480,
    DEFAULT_WRES = 640,
};

typedef struct sColour {
    uint8_t r_;
    uint8_t g_;
    uint8_t b_;
} Colour;

/**
 * @brief интерфейс "абстрактной фигуры"
*/
typedef struct sFigure {
    void *const self_;
    void (*draw_)(void *const self);
} Figure;

void drawFigure(const Figure* const f) {
    f->draw_(f->self_);
}


/**
 * @brief ADT точки
*/
typedef struct sDot {

} Dot;

/**
 * @brief фабрика для фигур
*/
Figure createFigure(const char *str, uint8_t xscale, uint8_t yscale);

int main() {
    uint32_t width, height;
    uint8_t red, green, blue;

    uint32_t ctrl = scanf("map %"SCNu32" %"SCNu32" %"SCNu8" %"SCNu8" %"SCNu8"", &width, &height, &red, &green, &blue);
    assert(ctrl == 5);

    al_init();
    al_init_primitives_addon();
    
    ALLEGRO_DISPLAY* display = al_create_display(DEFAULT_WRES, DEFAULT_HRES);
    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    ALLEGRO_TIMER *timer = al_create_timer(1.0 / 60);

    al_install_keyboard();

    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_timer_event_source(timer));

    al_clear_to_color(al_map_rgb(red, green, blue));
    //al_draw_filled_rectangle(100, 100, 50, 200, al_map_rgb(0, 0, 0));

    al_start_timer(timer);

    float xscale = (float)DEFAULT_WRES / width;
    float yscale = (float)DEFAULT_HRES / height;

    char temp[255];
    while (fgets(temp, 255, stdin)) {
        if(!strstr(temp, "dot"))
            continue;

        //fputs(temp, stdout);

        ALLEGRO_EVENT event;

        uint32_t x, y;
        uint8_t r, g, b;

        sscanf(temp, "dot %"SCNu32" %"SCNu32" %"SCNu8" %"SCNu8" %"SCNu8"", &x, &y, &r, &g, &b);
        al_draw_filled_rectangle(x * xscale, y * yscale, x * xscale + xscale, y * yscale + yscale, al_map_rgb(r, g, b));

        //printf("%d %d %d %d %d\n", x, y, r, g, b);

        al_wait_for_event(queue, &event);
        if(event.type == ALLEGRO_EVENT_TIMER)
            al_flip_display();
    }

    //al_clear_to_color(al_map_rgb(0, 255, 0));

    bool running = true;
    while(running) {
        ALLEGRO_EVENT event;
        al_wait_for_event(queue, &event);

        if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE || event.type == ALLEGRO_EVENT_KEY_UP)
            running = false;

        if(event.type == ALLEGRO_EVENT_TIMER)
            al_flip_display();
    }

    al_destroy_display(display);
    al_uninstall_keyboard();
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);

    return 0;
}