/*
 * Copyright (C) 2024 The pgmoneta community
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list
 * of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this
 * list of conditions and the following disclaimer in the documentation and/or other
 * materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may
 * be used to endorse or promote products derived from this software without specific
 * prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 * TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef PGMONETA_DEQUE_H
#define PGMONETA_DEQUE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <value.h>

#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>

/** @struct deque_node
 * Defines a deque node
 */
struct deque_node
{
   struct value* data;      /**< The value */
   char* tag;               /**< The tag */
   struct deque_node* next; /**< The next pointer */
   struct deque_node* prev; /**< The previous pointer */
};

/** @struct deque
 * Defines a deque
 */
struct deque
{
   uint32_t size;            /**< The size of the deque */
   bool thread_safe;         /**< If the deque is thread safe */
   pthread_rwlock_t mutex;   /**< The mutex of the deque */
   struct deque_node* start; /**< The start node */
   struct deque_node* end;   /**< The end node */
};

/** @struct deque_iterator
 * Defines a deque iterator
 */
struct deque_iterator
{
   struct deque* deque;      /**< The deque */
   struct deque_node* cur;   /**< The current deque node */
   char* tag;                /**< The current tag */
   struct value* value;      /**< The current value */
};

/**
 * Create a deque
 * @param thread_safe If the deque needs to be thread safe
 * @param deque The deque
 * @return 0 if success, otherwise 1
 */
int
pgmoneta_deque_create(bool thread_safe, struct deque** deque);

/**
 * Add a node to deque's tail, the tag will be copied, but the data will not
 * This function is thread safe
 * @param deque The deque
 * @param tag The tag,optional
 * @param data The data
 * @param type The data type
 * @return 0 if success, otherwise 1
 */
int
pgmoneta_deque_add(struct deque* deque, char* tag, uintptr_t data, enum value_type type);

/**
 * Retrieve value and remove the node from deque's head.
 * Note that if the value was copied into node,
 * this function will return the original value and tag
 * rather than making a copy of it.
 * This function is thread safe, but the returned value is not protected
 * @param deque The deque
 * @param tag [out] Optional, tag will be returned through if not NULL
 * @return The value data if deque's not empty, otherwise 0
 */
uintptr_t
pgmoneta_deque_poll(struct deque* deque, char** tag);

/**
 * Retrieve value without removing the node from deque's head.
 * Note that if the value was copied into node,
 * this function will return the original value and tag
 * rather than making a copy of it.
 * This function is thread safe, but the returned value is not protected
 * @param deque The deque
 * @param tag [out] Optional, tag will be returned through if not NULL
 * @return The value data if deque's not empty, otherwise 0
 */
uintptr_t
pgmoneta_deque_peek(struct deque* deque, char** tag);

/**
 * Get the data for the specified tag
 * @param deque The deque
 * @param tag The tag
 * @return The data, or 0
 */
uintptr_t
pgmoneta_deque_get(struct deque* deque, char* tag);

/**
 * Check if deque contains certain tag
 * @param deque The deque
 * @param tag The tag
 * @return true if the tag exists, false if otherwise
 */
bool
pgmoneta_deque_contains_tag(struct deque* deque, char* tag);

/**
 * Set the value of a tag if it exists
 * @param deque The deque
 * @param tag The tag
 * @param val The value data
 * @param type The value type
 * @return 0 if success, 1 if otherwise
 */
int
pgmoneta_deque_set(struct deque* deque, char* tag, uintptr_t val, enum value_type type);

/**
 * Get the next deque node
 * The function is thread safe for put/add but not for polling,
 * meaning that the returned node could get destroyed by other thread
 * @param deque The deque
 * @param node The current node
 * @return The next node if there is a next node, NULL if otherwise
 */
struct deque_node*
pgmoneta_deque_next(struct deque* deque, struct deque_node* node);

/**
 * Get the previous deque node
 * The function is thread safe for put/add but not for polling,
 * meaning that the returned node could get destroyed by other thread
 * @param deque The deque
 * @param node The current node
 * @return The next node if there is a previous node, NULL if otherwise
 */
struct deque_node*
pgmoneta_deque_prev(struct deque* deque, struct deque_node* node);

/**
 * Get the head of the deque.
 * The function is thread safe, but it doesn't protect the head from being removed
 * @param deque The deque
 * @return The head, or NULL if deque is empty
 */
struct deque_node*
pgmoneta_deque_head(struct deque* deque);

/**
 * Get the tail of the deque.
 * The function is thread safe, but it doesn't protect the head from being removed
 * @param deque The deque
 * @return The tail, or NULL if deque is empty
 */
struct deque_node*
pgmoneta_deque_tail(struct deque* deque);

/**
 * Remove a node from the deque.
 * The function is thread safe, but it does not protect the returned node from being removed
 * @param deque The deque
 * @param node The node
 * @return Next node of the deleted node, or NULL if there is no next node
 */
struct deque_node*
pgmoneta_deque_remove(struct deque* deque, struct deque_node* node);

/**
 * Get the size of the deque
 * @param deque The deque
 * @return The size
 */
uint32_t
pgmoneta_deque_size(struct deque* deque);

/**
 * Check if the deque is empty
 * @param deque The deque
 * @return true if deque size is 0, otherwise false
 */
bool
pgmoneta_deque_empty(struct deque* deque);

/**
 * List the nodes in the deque
 * @param deque The deque
 */
void
pgmoneta_deque_list(struct deque* deque);

/**
 * Destroy the deque and free its and its nodes' memory
 * @param deque The deque
 */
void
pgmoneta_deque_destroy(struct deque* deque);

/**
 * Convert what's inside deque to string
 * @param deque The deque
 * @param format The format
 * @param tag [Optional] The tag, which will be applied before the content if not null
 * @param indent The current indentation
 * @return The string
 */
char*
pgmoneta_deque_to_string(struct deque* deque, int32_t format, char* tag, int indent);

/**
 * Create a deque iterator
 * @param deque The deque
 * @param iter [out] The iterator
 * @return 0 on success, 1 if otherwise
 */
int
pgmoneta_deque_iterator_init(struct deque* deque, struct deque_iterator** iter);

/**
 * Destroy a deque iterator
 * @param iter The iterator
 */
void
pgmoneta_deque_iterator_destroy(struct deque_iterator* iter);

/**
 * Get the next deque value
 * @param iter The iterator
 * @return true if has next, false if otherwise
 */
bool
pgmoneta_deque_iterator_next(struct deque_iterator* iter);

/**
 * Check if deque has next value
 * @param iter The iterator
 * @return true if has next, false if otherwise
 */
bool
pgmoneta_deque_iterator_has_next(struct deque_iterator* iter);

#ifdef __cplusplus
}
#endif

#endif
