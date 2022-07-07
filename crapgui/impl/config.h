#pragma once
#include <poglib/crapgui/decl.h>
#include <stddef.h>
#include "frame.h"

//FIXME: read the gui, member by member

void __crapgui_saveall_to_config(crapgui_t *gui)
{
    assert(gui);
    file_t *file = file_init(DEFAULT_CRAPGUI_CONFIG_PATH, "w");

    file_save_map(file, gui->frames);
    map_dump(&gui->frames);
    map_iterator(&gui->frames, iter01)
    {
        frame_t *frame = (frame_t *)iter01;
        slot_dump(&frame->uis);
        file_save_slot(file, frame->uis);
    }
    file_destroy(file);
}

crapgui_t __crapgui_init_from_config(void)
{    
    file_t *file = file_init(DEFAULT_CRAPGUI_CONFIG_PATH, "r");

    crapgui_t gui   = __crapgui_init();
    gui.frames      = file_load_map(file);

    map_dump(&gui.frames);
    map_iterator(&gui.frames, iter01)
    {
        frame_t *frame  = (frame_t *)iter01;
        frame->__cache  = __frame_cache_init();
        frame->uis      = file_load_slot(file);

        slot_dump(&frame->uis);
        slot_iterator(&frame->uis, iter02)
        {
            ui_t *ui = (ui_t *)iter02;
            ui->__cache = __ui_cache_init();
        }
    }
    file_destroy(file);

    return gui;
}

