//
// Created by Baraa Egbaria.
// Copyright © 2022 Baraa Egbaria All rights reserved.
//
// Map.h
//

#include "Map.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>


typedef struct node_t {
    MapDataElement data;
    MapKeyElement key;
    struct node_t *next;
} *Node;

struct Map_t
{
    Node head;
    Node tail;
    Node iterator;
    int size;
    copyMapDataElements copyData;
    copyMapKeyElements copyKey;
    freeMapDataElements freeData;
    freeMapKeyElements freeKey;
    compareMapKeyElements compareKeys;
};


static Node NodeCreate(Map map, MapKeyElement keyElement, MapDataElement dataElement);
static Node NodeCopy(Map map, Node node);
static void NodeDestroy(Node node, Map map);

static Node NodeCreate(Map map, MapKeyElement keyElement, MapDataElement dataElement)
{
    Node newNode = malloc(sizeof(*newNode));
    if(newNode == NULL)
        return NULL;

    newNode->key = map->copyKey(keyElement);
    if(newNode->key == NULL)
    {
        free(newNode);
        return NULL;
    }

    newNode->data = map->copyData(dataElement);
    if(newNode->data == NULL)
    {
        map->freeKey(newNode->key);
        free(newNode);
        return NULL;
    }

    newNode -> next = NULL;
    return newNode;
}

static Node NodeCopy(Map map, Node node)
{
    Node newNode = NodeCreate(map, node->key, node->data);
    if(newNode == NULL)
        return NULL;
    return  newNode;
}

static void NodeDestroy(Node node, Map map)
{
    map->freeData(node->data);
    map->freeKey(node->key);
    free(node);
}

static void deleteNodesList(Map map)
{
    while(map->head != NULL)
    {
        Node toDelete = map->head;
        map->head = map->head->next;
        NodeDestroy(toDelete, map);
    }
}


Map mapCreate(copyMapDataElements copyDataElement, copyMapKeyElements copyKeyElement, freeMapDataElements freeDataElement,
              freeMapKeyElements freeKeyElement, compareMapKeyElements compareKeyElements)
{
    if(!copyDataElement || !copyKeyElement || !freeDataElement || !freeKeyElement || !compareKeyElements)
        return NULL;

    Map map = malloc(sizeof(*map));
    if(map == NULL)
        return NULL;

    // properties initialization
    map->head = NULL;
    map->tail = NULL;
    map->iterator = NULL;
    map->size = 0;

    // utils initialization
    map->copyKey = copyKeyElement;
    map->copyData = copyDataElement;
    map->freeKey = freeKeyElement;
    map->freeData = freeDataElement;
    map->compareKeys = compareKeyElements;

    return map;
}

Map mapCopy(Map map)
{
    if(map == NULL)
        return NULL;

    Map newMap = mapCreate(map->copyData, map->copyKey, map->freeData,
                            map->freeKey, map->compareKeys);
    if(newMap == NULL)
        return NULL;

    for(Node iter = map->head; iter != NULL; iter= iter->next)
    {
        if(mapPut(newMap, iter->key, iter->data) != MAP_SUCCESS)
        {
            mapDestroy(newMap);
            return NULL;
        }
    }
    return newMap;
}

static Node findElement(Map map, MapKeyElement element)
{
    for(Node iter = map->head; iter != NULL; iter= iter->next)
        if(map->compareKeys(iter->key, element) == 0)
            return iter;
    return NULL;
}

static void removeExistingNode(Map map, Node toRemove)
{
    if(map->head == toRemove)
    {
        map->head = (map->head != map->tail) ? map->head->next : NULL;
        map->tail = (map->head != map->tail) ? map->tail : NULL;
        NodeDestroy(toRemove, map);
        return;
    }

    Node prev = map->head;
    for(Node curr = map->head->next; curr != NULL; prev = curr, curr = curr->next)
    {
        if(map->compareKeys(curr->key, toRemove->key) != 0)
            continue;

        prev->next = curr->next;
        if(curr == map->tail)
            map->tail = prev;

        NodeDestroy(toRemove, map);
        return;
    }
}

//mapRemove: Removes a pair of key and data elements from the map.
MapResult mapRemove(Map map, MapKeyElement keyElement)
{
    if(map == NULL || keyElement == NULL)
    {
        return MAP_NULL_ARGUMENT;
    }
    Node toRemove = findElement(map, keyElement);
    if(toRemove == NULL)
        return MAP_ITEM_DOES_NOT_EXIST;

    removeExistingNode(map, toRemove);
    map->size -= 1;
    return MAP_SUCCESS;
}

static void addNonExistingNode(Map map, Node node)
{
    if(mapGetSize(map) == 0)
    {
        map->head = node;
        map->tail = node;
        return;
    }

    Node prev = NULL;
    for(Node curr = map->head; curr != NULL; prev = curr, curr = curr->next)
    {
        if(map->compareKeys(node->key, curr->key) < 0)
        {
            if(curr == map->head)
            {
                node->next = map->head;
                map->head = node;
            }
            else
            {
                node->next = curr;
                prev->next = node;
            }
            return;
        }
    }
    map->tail->next = node;
    node->next = NULL;
    map->tail = node;
}

MapResult mapPut(Map map, MapKeyElement keyElement, MapDataElement dataElement)
{
    if( !map || !keyElement || !dataElement)
        return MAP_NULL_ARGUMENT;

    Node node = findElement(map, keyElement);
    if( node == NULL )
    {
        node = NodeCreate(map, keyElement, dataElement);
        if(node == NULL)
            return MAP_OUT_OF_MEMORY;
        addNonExistingNode(map, node);
        map->size += 1;
    }
    else
    {
        map->freeData(node->data);
        node->data = map->copyData(dataElement);
    }
    return MAP_SUCCESS;
}

MapDataElement mapGet(Map map, MapKeyElement keyElement)
{
    if( !map || !keyElement|| !findElement(map, keyElement))
        return NULL;
    return findElement(map, keyElement)->data;
}

void mapDestroy(Map map)
{
    if(map == NULL)
        return;

    deleteNodesList(map);
    free(map);
}

MapResult mapClear(Map map)
{
    if(map == NULL)
        return MAP_NULL_ARGUMENT;

    deleteNodesList(map);
    map->head = map->tail = NULL;
    map->size = 0;
    return MAP_SUCCESS;
}

int mapGetSize(Map map) { return !map ? -1 : map->size; }

bool mapContains(Map map, MapKeyElement element)
{
    if( !map || !element)
        return false;
    return findElement(map, element) == NULL ? false : true;
}


MapKeyElement mapGetFirst(Map map)
{
    if( !map|| !map->head)
        return NULL;

    map->iterator = map->head;
    return map->iterator == NULL ? NULL :  map->copyKey(map->iterator->key);
}


MapKeyElement mapGetNext(Map map)
{
    if( !map|| !map->iterator)
        return NULL;

    map->iterator = map->iterator->next;
    return map->iterator == NULL ? NULL :  map->copyKey(map->iterator->key);
}

