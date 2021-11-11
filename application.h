#pragma once


#include "simple/window.h"
#include "application/stopwatch.h"
#include "simple/gl_renderer2d.h"






typedef struct application_t application_t;

typedef u8 state_t;

#define         application_init(PWIN, PRENDERER) __impl_application_init((PWIN), (PRENDERER))
void            application_run(application_t *app);

#define         application_update_state(PAPP, STATE) (PAPP)->state = STATE
#define         application_get_dt(PAPP)  (PAPP)->timer.dt
#define         application_get_fps(PAPP) (PAPP)->timer.fps
#define         application_get_whandle(PAPP) (PAPP)->__window_handle





#ifndef IGNORE_APPLICATION_IMPLEMENTATION

struct application_t {

    window_t *__window_handle;
    gl_renderer2d_t *__renderer2d;
    
    stopwatch_t timer;

    state_t state;

    void (* init)(struct application_t*);
    void (*update)(struct application_t*);
    void (*render)(struct application_t*);
};


application_t __impl_application_init(window_t *window, gl_renderer2d_t *renderer)
{
    return (application_t) {
        .__window_handle = window,
        .__renderer2d = renderer,
        .timer = stopwatch_init(), 
        .state = 0,
        .init = NULL,
        .update = NULL,
        .render = NULL
    };
}



void application_run(application_t *app)
{
    if (app == NULL) eprint("application argument is null");
    
    window_t *win = app->__window_handle;
    stopwatch_t *timer = &app->timer;

    // Initialize the content in the application
    app->init(app);

    // states
    state_t st_current  = app->state, 
            st_previous = 0; 

    // Update the content in the application
    while(win->is_open)
    {
        u64 perf_start  = SDL_GetPerformanceCounter();

        timer->__last   = timer->__now;
        timer->__now    = (f32)SDL_GetTicks();
        timer->dt = (timer->__now - timer->__last) / 1000.0f;

        if (timer->dt > 0.25f) timer->dt = 0.25f;

        timer->accumulator += timer->dt;

        while(timer->accumulator >= timer->dt && timer->dt != 0.0f)
        {
            st_previous = st_current;

            application_update_state(app, st_current);
            window_update_user_input(win);

            app->update(app);

            timer->accumulator -= timer->dt;
        }

        const f32 alpha = timer->accumulator / timer->dt;


        state_t state   = st_current * alpha + st_previous * (1.0 - alpha);
        application_update_state(app, state);


        app->render(app);


        // NOTE: for now i am capping at 60 fps 
        /*
         * u64 perf_end = SDL_GetPerformanceCounter();
         * timer->fps = 1.0f / ((perf_end - perf_start) / (f32)SDL_GetPerformanceFrequency());
         */

        //NOTE: here is a simple dt to fps converter dont think its accurate for physics stuff
        //need to do more research on delta time, especially the math behind it
        //
        timer->fps = 1.0f / timer->dt;
        SDL_Delay(floor(1000/60 - timer->dt));
    }
}

#endif 
