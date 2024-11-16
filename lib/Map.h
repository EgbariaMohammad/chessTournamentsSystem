//
// Created by Baraa Egbaria.
// Copyright © 2022 Baraa Egbaria All rights reserved.
//
// Map.h
//

#ifndef Map_h
#define Map_h

#include <stdbool.h>

/**
* @file Map.h
* @brief Generic Map Container
*
* Implements a map container type.
* The map has an internal iterator for external use.
* For all functions where the state of the iterator after calling that function is not stated,
* you should assume that the state of the iterator is undefined.
*
* The following functions are available:
*   mapCreate() - Creates a new empty map
*   mapDestroy() - Deletes an existing map and frees all resources
*   mapCopy() - Copies an existing map
*   mapGetSize() - Returns the size of a given map
*   mapContains() - Checks if a key exists inside the map. This resets the internal iterator.
*   mapPut() - Gives a specific key a given value. If the key exists, the value is overridden. This resets the internal iterator.
*   mapGet() - Returns the data paired to a key which matches the given key. Iterator status unchanged
*   mapRemove() - Removes a pair of (key,data) elements for which the key matches a given element (by the key compare function). This resets the internal iterator.
*   mapGetFirst() - Sets the internal iterator to the first (smallest) key in the map, and returns a copy of it.
*   mapGetNext() - Advances the internal iterator to the next key and returns a copy of it.
*   mapClear() - Clears the contents of the map. Frees all the elements of the map using the free function.
*   MAP_FOREACH - A macro for iterating over the map's elements. The iterator needs to be deallocated (freed) after each iteration.
 */


typedef struct Map_t *Map;


/**
 Type used for returning error codes from map functions
 */
typedef enum MapResult_t {
    MAP_SUCCESS, /**< Operation completed successfully */
    MAP_ERROR, /**< An unspecified error occurred */
    MAP_OUT_OF_MEMORY, /**< Operation failed due to memory allocation failure */
    MAP_NULL_ARGUMENT, /**< Operation failed due to a null argument */
    MAP_ITEM_ALREADY_EXISTS, /**< An attempt was made to insert an item that already exists in the map */
    MAP_ITEM_DOES_NOT_EXIST /**< An attempt was made to access or remove an item that does not exist in the map */
} MapResult;


typedef void *MapDataElement;
typedef void *MapKeyElement;

typedef MapDataElement(*copyMapDataElements)(MapDataElement);
typedef MapKeyElement(*copyMapKeyElements)(MapKeyElement);
typedef void(*freeMapDataElements)(MapDataElement);
typedef void(*freeMapKeyElements)(MapKeyElement);

/**
 * @brief Type of function used by the map to identify equal key elements.
 *
 * should @return
 *  - A positive integer if the first element is greater;
 *  - 0 if they're equal;
 *  - A negative integer if the second element is greater.
 */
typedef int(*compareMapKeyElements)(MapKeyElement , MapKeyElement);

/**
 * @brief Allocates a new empty map.
 *
 * @param copyDataElement Function pointer to be used for copying data elements into the map or when copying the map.
 * @param copyKeyElement Function pointer to be used for copying key elements into the map or when copying the map.
 * @param freeDataElement Function pointer to be used for removing data elements from the map.
 * @param freeKeyElement Function pointer to be used for removing key elements from the map.
 * @param compareKeyElements Function pointer to be used for comparing key elements inside the map. Used to check if new elements already exist in the map.
 * @return A new Map in case of success, NULL if one of the parameters is NULL or allocations failed.
 */
Map mapCreate(copyMapDataElements copyDataElement,
              copyMapKeyElements copyKeyElement,
              freeMapDataElements freeDataElement,
              freeMapKeyElements freeKeyElement,
              compareMapKeyElements compareKeyElements);

/**
 * @brief Deallocates an existing map.
 */
void mapDestroy(Map map);


/**
 * @brief Creates a copy of target map.
 * Iterator values for both maps is undefined after this operation.
 *
 * @return
 *  - A Map containing the same elements as map
 *  - NULL if a NULL was sent or a memory allocation failed.
 */
Map mapCopy(Map map);

/**
 * @brief Returns the number of elements in a map.
 */
int mapGetSize(Map map);

/**
 * @brief Checks if a key element exists in the map. Iterator status unchanged.
 *
 * @param map The map to search in.
 * @param element The element to look for. Will be compared using the comparison function.
 * @return
 *  - true if the key element was found in the map
 *  - false if one or more of the inputs is null, or if the key element was not found.
 */
bool mapContains(Map map, MapKeyElement element);

/**
 * @brief Gives a specified key a specific value.
 * Iterator's value is undefined after this operation.
 *
 * @param map The map for which to reassign the data element.
 * @param keyElement The key element which needs to be reassigned.
 * @param dataElement The new data element to associate with the given key. A copy of the element will be inserted
 * @return
 *  - MAP_SUCCESS if the paired elements had been inserted successfully,
 *  - MAP_NULL_ARGUMENT if a NULL was sent as map or keyElement or dataElement,
 *  - MAP_OUT_OF_MEMORY if an allocation failed (Meaning the function for copying an element or a key failed).
 */
MapResult mapPut(Map map, MapKeyElement keyElement, MapDataElement dataElement);

/**
 * @brief Returns the data associated with a specific key in the map. Iterator status unchanged.
 *
 * @param map The map from which to get the data element.
 * @param keyElement The key element which needs to be found and whose data we want to get.
 * @return
 *  - The data element associated with the key
 *  - NULL if a NULL pointer was sent or if the map does not contain the requested key.
 */
MapDataElement mapGet(Map map, MapKeyElement keyElement);

/**
 * @brief Removes a pair of key and data elements from the map.
 * Iterator's value is undefined after this operation.
 *
 * @param map The map to remove the elements from.
 * @param keyElement The key element to find and remove from the map.
 * @return
 *  - MAP_SUCCESS if the paired elements had been removed successfully
 *  - MAP_NULL_ARGUMENT if a NULL was sent to the function
 *  - MAP_ITEM_DOES_NOT_EXIST if an equal key item does not already exist in the map.
 */
MapResult mapRemove(Map map, MapKeyElement keyElement);

/**
 * @brief Sets the internal iterator (also called current key element) to the smallest key element in the map and returns a copy of it.
 * Use this to start iterating over the map. To continue iteration use mapGetNext.
 *
 * @param map The map for which to set the iterator and return a copy of the first key element.
 * @return
 *  - The first key element of the map
 *  - NULL if a NULL pointer was sent or the map is empty or allocation fails.
 */
MapKeyElement mapGetFirst(Map map);

/**
 * @brief Advances the map iterator to the next key element and returns a copy of it.
 *
 * @param map The map for which to advance the iterator.
 * @return
 *  - The next key element on the map in case of success
 *  - NULL if reached the end of the map, or the iterator is at an invalid state, or a NULL sent as argument or allocation fails.
 */
MapKeyElement mapGetNext(Map map);

/**
 * @brief Removes all key and data elements from target map.
 *
 * @param map Target map to remove all elements from.
 * @return
 *  - MAP_SUCCESS if all elements were successfully removed
 *  - MAP_NULL_ARGUMENT if a NULL pointer was sent.
 */
MapResult mapClear(Map map);

/**
 * @brief Macro for iterating over a map. Declares a new iterator for the loop. Iterator needs to be deallocated (freed) each iteration.
 */
#define MAP_FOREACH(type, iterator, map) \
    for(type iterator = (type) mapGetFirst(map) ; \
        iterator ;\
        iterator = mapGetNext(map))

#endif /* Map_h */

