/*
LGMP - Looking Glass Memory Protocol
Copyright (C) 2020 Geoffrey McRae <geoff@hostfission.com>
https://looking-glass.hostfission.com

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place, Suite 330, Boston, MA 02111-1307 USA
*/

#ifndef LGMP_PRIVATE_HEADERS_H
#define LGMP_PRIVATE_HEADERS_H

#include <stdint.h>
#include <stdatomic.h>

#define LGMP_PROTOCOL_MAGIC   0x504d474c
#define LGMP_PROTOCOL_VERSION 2
#define LGMP_MAX_QUEUES       5

#define LGMP_SUBS_ON(x)          (uint32_t)((x) >> 32)
#define LGMP_SUBS_BAD(x)         (uint32_t)((x) >>  0)
#define LGMP_SUBS_OR_BAD(x, bad) ((x) | (bad))
#define LGMP_SUBS_CLEAR(x, cl)   ((x) & ~((cl) | ((uint64_t)(cl) << 32)))
#define LGMP_SUBS_SET(x, st)     ((x) | ((uint64_t)(st) << 32))

#define LGMP_QUEUE_LOCK(hq) \
  while (atomic_flag_test_and_set_explicit(&hq->lock, memory_order_acquire)) { ; }

#define LGMP_QUEUE_TRY_LOCK(hq) \
  (!atomic_flag_test_and_set_explicit(&hq->lock, memory_order_acquire))

#define LGMP_QUEUE_UNLOCK(hq) \
  atomic_flag_clear_explicit(&hq->lock, memory_order_release);

struct LGMPHeaderMessage
{
  uint32_t udata;
  uint32_t size;
  uint32_t offset;
  uint32_t pendingSubs;
};

struct LGMPHeaderQueue
{
  uint32_t queueID;
  uint32_t numMessages;
  _Atomic(uint32_t) newSubCount;
  uint32_t maxTime;

  _Atomic(uint32_t) position;
  uint32_t messagesOffset;
  uint64_t timeout[32];

  /* the lock MUST be held to use the following values */
  atomic_flag lock;
  _Atomic(uint64_t) subs; // see LGMP_SUBS_* macros
  uint32_t start;
  _Atomic(uint64_t) msgTimeout;
  _Atomic(uint32_t) count;
};

struct LGMPHeader
{
  uint32_t magic;
  uint32_t version;
  uint32_t sessionID;
  uint64_t timestamp;
  uint32_t numQueues;
  struct LGMPHeaderQueue queues[LGMP_MAX_QUEUES];
  uint32_t udataSize;
  uint8_t  udata[0];
};

#endif
