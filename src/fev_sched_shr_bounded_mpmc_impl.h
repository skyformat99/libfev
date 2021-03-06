/*
 * Copyright 2020 Patryk Stefanski
 *
 * Licensed under the Apache License, Version 2.0, <LICENSE-APACHE or
 * http://apache.org/licenses/LICENSE-2.0> or the MIT license <LICENSE-MIT or
 * http://opensource.org/licenses/MIT>, at your option. This file may not be
 * copied, modified, or distributed except according to those terms.
 */

#ifndef FEV_SCHED_SHR_BOUNDED_MPMC_IMPL_H
#define FEV_SCHED_SHR_BOUNDED_MPMC_IMPL_H

#include "fev_sched_intf.h"

#include <stdbool.h>
#include <stdint.h>

#include <queue.h>

#include "fev_assert.h"
#include "fev_bounded_mpmc_queue.h"
#include "fev_compiler.h"
#include "fev_fiber.h"

FEV_NONNULL(1, 2)
static inline void fev_push_one(struct fev_sched_worker *worker, struct fev_fiber *fiber)
{
  struct fev_sched *sched = worker->sched;
  bool pushed = fev_bounded_mpmc_queue_push(&sched->run_queue, fiber);
  if (FEV_UNLIKELY(!pushed))
    fev_push_one_fallback(sched, fiber);
}

FEV_NONNULL(1, 2)
static inline void fev_push_stq(struct fev_sched_worker *worker, fev_fiber_stq_head_t *fibers,
                                uint32_t num_fibers)
{
  struct fev_sched *sched = worker->sched;
  uint32_t n = num_fibers;

  FEV_ASSERT(!STAILQ_EMPTY(fibers));
  FEV_ASSERT(num_fibers > 0);

  fev_bounded_mpmc_queue_push_stq(&sched->run_queue, fibers, &n);
  if (FEV_UNLIKELY(n != num_fibers)) {
    FEV_ASSERT(n < num_fibers);
    fev_push_stq_fallback(sched, fibers, num_fibers - n);
  }
}

#endif /* !FEV_SCHED_SHR_BOUNDED_MPMC_IMPL_H */
