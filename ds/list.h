#pragma once
#include "../basic.h"


typedef struct list_t list_t;


#define         list_init(CAPACITY, TYPE)                       __impl_list_init(CAPACITY, sizeof(TYPE))

#define         list_append(PLIST, VALUE)                       __impl_list_append((PLIST), &(VALUE), sizeof(VALUE)) 
void            list_delete(list_t *list, const u64 index);

void            list_destroy(list_t *list);

void            list_dump(const list_t *list);
void            list_print(const list_t *list, void (*print)(void*));

#define         list_get_element_by_index(PLIST, INDEX)         ((PLIST)->__array + (INDEX) * (PLIST)->__elem_size)
#define         list_clear(PLIST)                               __impl_list_clear(PLIST)






#ifndef IGNORE_LIST_IMPLEMENTATION

struct list_t {

    // length of the list
    u64 len;

    u8 *__array;
    i64 __top;
    u64 __capacity;
    u64 __elem_size;
};


void __impl_list_clear(list_t *list)
{
    assert(list);
    list->__top = -1;
    list->len = 0;
    memset(list->__array, 0, list->__elem_size * list->__capacity);
}

list_t __impl_list_init(const u64 capacity, u64 elem_size) 
{
    return (list_t )
    {
        .len = 0,
        .__array = (u8 *)calloc(capacity, elem_size),
        .__top = -1,
        .__capacity = capacity,
        .__elem_size = elem_size
    };
}

void __impl_list_append(list_t *list, void *value_addr, u64 value_size)
{
    assert(list);
    assert(value_addr);
    assert(value_size == list->__elem_size);

    if (value_size != list->__elem_size) eprint("trying to push a value of size %lu to slot of size %lu", value_size, list->__elem_size);


    if (list->__top == (i64)(list->__capacity - 1)) {

        list->__capacity = list->__capacity * 2;

        list->__array = (u8 *)realloc(list->__array, list->__capacity * list->__elem_size);

    }

    list->len = ++list->__top + 1;

    memcpy(list->__array + list->__top * list->__elem_size, value_addr, list->__elem_size);
}


void list_delete(list_t *list, const u64 index)
{
    assert(list);
    if(list->__top == -1) eprint("Trying to delete an element from an empty list\n");
    assert((i64)index <= list->__top);


    if ((i64)index != list->__top) {

        memcpy(list->__array + index * list->__elem_size, 
                list->__array + (index + 1) * list->__elem_size, 
                list->__elem_size * (list->__top - index)); 
    } 

    list->len = --list->__top + 1;
} 

void list_print(const list_t *list, void (*print)(void*))
{
    assert(list);
    if (list->__top == -1) {
        printf("list is empty\n");
        return ;
    }
    for (u64 i = 0; i < list->len; i++)
    {
        print(list->__array + i * list->__elem_size);
    }
    printf("\n");
}

void list_dump(const list_t *list)
{
    assert(list);

    printf("\n len = %ld\n arr = %p\n top = %ld\n capacity = %ld\n elem_size = %ld\n", 
            list->len, 
            list->__array, 
            list->__top, 
            list->__capacity, 
            list->__elem_size);

    printf(" contents = [ ");
    for (u64 i = 0; i < list->__capacity; i++)
    {
        printf("%p, ",list->__array + i * list->__elem_size);
    }
    printf("]\n");

}

void list_destroy(list_t *list)
{
    free(list->__array);
    list->__array = NULL;
    list->__capacity = 0;
    list->__top = -1;
    list->len = 0;
    list->__elem_size = 0;
}
#endif
