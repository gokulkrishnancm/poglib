#pragma once
#include "../basic.h"
#include "../math/la.h"
#include "../ds/hashtable.h"
#include "../ds/list.h"
#include "../simple/gl/types.h"



typedef struct entity_t entity_t ;
typedef struct entitycomponent_t entitycomponent_t ;

#ifndef entity_type
    typedef u64 entity_type;
#endif

typedef enum entitycomponent_type {

    ECT_c_transform_t,
    ECT_c_boxcollider2d_t, 
    ECT_c_lifespan_t,
    ECT_c_input_t,
    ECT_c_shape_t,

    // Add new component types here

    ECT_COUNT

} entitycomponent_type;



entity_t *      entity_init(entity_type tag);
#define         entity_add_component(PENTITY, COMPONENT, TYPE)          __impl_entity_add_component((PENTITY), &(COMPONENT), sizeof(COMPONENT), ECT_type(TYPE), sizeof(TYPE))
void            entity_destroy(entity_t *e);

void *          entity_get_component(entity_t *e, entitycomponent_type type);







#ifndef IGNORE_ENTITY_IMPLEMENTATION


static_assert(ECT_COUNT == 5, "U FORGOR 💀 TO UPDATE THE STATIC ASSERT IN `entity.h` ");


#define ECT_type(TYPE) ECT_ ## TYPE

struct entity_t {

    // Members
    const u64   id;
    entity_type tag; 
    bool        is_alive;

    // dynamic list that holds pointers to components
    list_t      components;

    // opengl data (cache)
    glvertex_t *__vertices;
    
    // index buffer for grabbing components of the list
    u64        __indices[ECT_COUNT];
};


struct entitycomponent_t {

    entitycomponent_type    type;
    void                    *cmp;
};


void entity_destroy(entity_t *e)
{
    assert(e);

    // Freeing the component list of all its component before destroying the list
    list_t *cmps = &e->components;
    for (int i = 0; i < cmps->len; i++)
    {
        entitycomponent_t *ec = (entitycomponent_t *)list_get_element_by_index(cmps, i);
        assert(ec);

        void *cmp = ec->cmp; 
        assert(cmp);
        free(cmp);

        cmp = NULL;
        ec = NULL;
    }
    list_destroy(cmps);
    cmps = NULL;

    // Freeing glvertices pointer 
    free(e->__vertices);
    e->__vertices = NULL;

    // Zeroing in the indices buffer
    memset(e->__indices, 0, sizeof(e->__indices));

    // Freeing the entity itself 
    free(e);
    e = NULL;
}

entity_t * entity_init(entity_type tag)
{
    entity_t *e = (entity_t *)malloc(1 * sizeof(entity_t));
    assert(e);

    entity_t tmp = (entity_t ) {
        .id = (u64)e,
        .tag = tag,
        .is_alive = true,
        .components = list_init(4, entitycomponent_t ),
        .__vertices = NULL,
        .__indices = {0}
    };

    memcpy(e, &tmp, sizeof(tmp));

    return e;
}

void __impl_entity_add_component(entity_t *e, void * ecmp_ref, u64 ecmp_size, entitycomponent_type type, u64 type_size)
{
    assert(e);
    assert(ecmp_ref);
    assert(ecmp_size == type_size);

    // Heap allocated component
    void *cmp = calloc(1, ecmp_size);
    assert(cmp);
    memcpy(cmp, ecmp_ref, ecmp_size);

    // Wrapping the component into a entitycomponent_t type for better safety
    entitycomponent_t ec = (entitycomponent_t ) {
        .type = type,
        .cmp = cmp
    };

    // Appending the wrapper to the entity components list
    list_t *list = &e->components;
    list_append(list, ec);

    // Updating the indices buffer
    e->__indices[type] = list->len - 1;
}


void *entity_get_component(entity_t *e, entitycomponent_type type)
{
    assert(e);

    u64 index = e->__indices[type];
    assert(index < e->components.len);

    entitycomponent_t *ec = (entitycomponent_t *)list_get_element_by_index(&e->components, index);
    assert(ec);

    return ec->cmp;
}

#endif
