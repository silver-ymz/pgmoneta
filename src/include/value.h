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

#ifndef PGMONETA_VALUE_H
#define PGMONETA_VALUE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>
#include <stdbool.h>

typedef void (*data_destroy_cb)(uintptr_t data);
typedef char* (*data_to_string_cb)(uintptr_t data, char* tag, int indent);

enum value_type {
   ValueInt8,
   ValueUInt8,
   ValueInt16,
   ValueUInt16,
   ValueInt32,
   ValueUInt32,
   ValueInt64,
   ValueUInt64,
   ValueBool,
   ValueString,
   ValueFloat,
   ValueDouble,
   ValueJSON,
   ValueDeque,
   ValueART,
   ValueRef,
   ValueVerifyEntry,
};

/**
 * @struct value
 * Defines a universal value
 */
struct value
{
   enum value_type type;          /**< The type of value data */
   uintptr_t data;                /**< The data, could be passed by value or by reference */
   data_destroy_cb destroy_data;  /**< The callback to destroy data */
   data_to_string_cb to_string;   /**< The callback to convert data to string */
};

/**
 * Create a value based on the data and value type
 * @param type The value type, use ValueRef if you are only storing pointers without the need to manage memory
 * @param data The value data, type cast it to uintptr_t before passing into function
 * @param value [out] The value
 * @return 0 on success, 1 if otherwise
 */
int
pgmoneta_value_create(enum value_type type, uintptr_t data, struct value** value);

/**
 * Destroy a value along with the data within
 * @param value The value
 * @return 0 on success, 1 if otherwise
 */
int
pgmoneta_value_destroy(struct value* value);

/**
 * Get the raw data from the value, which can be casted back to its original type
 * @param value The value
 * @return The value data within
 */
uintptr_t
pgmoneta_value_data(struct value* value);

/**
 * Convert a value to string
 * @param value The value
 * @param tag The optional tag
 * @param indent The indent
 * @return The string
 */
char*
pgmoneta_value_to_string(struct value* value, char* tag, int indent);

#ifdef __cplusplus
}
#endif

#endif