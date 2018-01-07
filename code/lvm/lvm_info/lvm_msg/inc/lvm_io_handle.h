/*
 * The IO handle functions
 *
 * Copyright (C) 2014-2017, Joachim Metz <joachim.metz@gmail.com>
 *
 * Refer to AUTHORS for acknowledgements.
 *
 * This software is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this software.  If not, see <http://www.gnu.org/licenses/>.
 */

#if !defined( _LIBVSLVM_IO_HANDLE_H )
#define _LIBVSLVM_IO_HANDLE_H

#include <common.h>
#include <types.h>

#include "libvslvm_libbfio.h"
#include "libvslvm_libcdata.h"
#include "libvslvm_libcerror.h"
#include "libvslvm_libfcache.h"
#include "libvslvm_libfdata.h"

#if defined( __cplusplus )
extern "C" {
#endif

typedef struct libvslvm_io_handle libvslvm_io_handle_t;

struct libvslvm_io_handle
{
	/* The number of bytes per sector
	 */
	size_t bytes_per_sector;

	/* Value to indicate if abort was signalled
	 */
	int abort;
};

int libvslvm_io_handle_initialize(
     libvslvm_io_handle_t **io_handle,
     libcerror_error_t **error );

int libvslvm_io_handle_free(
     libvslvm_io_handle_t **io_handle,
     libcerror_error_t **error );

int libvslvm_io_handle_clear(
     libvslvm_io_handle_t *io_handle,
     libcerror_error_t **error );

int libvslvm_io_handle_read_chunk_data(
     intptr_t *data_handle,
     libbfio_pool_t *file_io_pool,
     libfdata_vector_t *vector,
     libfcache_cache_t *cache,
     int element_index,
     int element_data_file_index,
     off64_t element_data_offset,
     size64_t element_data_size,
     uint32_t element_data_flags,
     uint8_t read_flags,
     libcerror_error_t **error );

#if defined( __cplusplus )
}
#endif

#endif /* !defined( _LIBVSLVM_IO_HANDLE_H ) */
