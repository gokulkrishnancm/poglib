#pragma once
#include "../ecs/entitymanager.h"
#include "../ds/map.h"
#include "./assetmanager.h"
#include "./action.h"

//TODO: Implement actions
/*=========================================
            -- SCENE --
=========================================*/


typedef struct scene_t scene_t ;

scene_t     scene_init(const char *scene_label);
#define     scene_get_type(PSCENE) (PSCENE)->__enum_id
void        scene_add_action(scene_t *scene, const action_t action);
void        scene_destroy(scene_t *scene);



#ifndef IGNORE_POGGEN_SCENE_IMPLEMENTATION

#define MAX_ACTIONS_ALLOWED_PER_SCENE 64

struct scene_t {

    const char          *label;
    void                *poggen;
    entitymanager_t     manager;
    bool                is_paused;
    bool                is_over;
    map_t               actionmap;

    void (*init)        (struct scene_t *);
    void (*update)      (struct scene_t *);
    void (*doaction)    (struct scene_t *);
    void (*render)      (struct scene_t *);
    void (*destroy)     (struct scene_t *);

};

void scene_add_action(scene_t *scene, const action_t action)
{
    assert(scene);
    if (scene->doaction == NULL) eprint("`%s` scene is missing a doaction() function", scene->label);

    map_insert(&scene->actionmap, action.label, action.key);
}

scene_t scene_init(const char *scene_label)
{
    scene_t scene = {
        .label      = scene_label,
        .manager    = entitymanager_init(10),
        .is_paused  = false,
        .is_over    = false,

        .init       = NULL,
        .update     = NULL,
        .render     = NULL,
        .destroy    = NULL 
    };

    return scene;
}


void scene_destroy(scene_t *scene)
{
    assert(scene);

    entitymanager_destroy(&scene->manager);
    scene->destroy(scene);

    scene->poggen = NULL;
    scene->label = NULL;
    scene->init  = NULL;
    scene->update = NULL;
    scene->render = NULL;
    scene->destroy = NULL;
}

#endif