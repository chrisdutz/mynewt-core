/**
 * Copyright (c) 2015 Stack Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#include "os/os.h" 

#include <string.h> 

TAILQ_HEAD(, os_callout) g_callout_list = 
    TAILQ_HEAD_INITIALIZER(g_callout_list); 

void 
os_callout_init(struct os_callout *c)
{
    memset(c, 0, sizeof(*c));
    c->c_ev.ev_type = OS_EVENT_T_TIMER; 
}

void
os_callout_stop(struct os_callout *c) 
{
    os_sr_t sr; 

    OS_ENTER_CRITICAL(sr);

    if (OS_CALLOUT_QUEUED(c)) {
        TAILQ_REMOVE(&g_callout_list, c, c_next);
        c->c_flags &= ~OS_CALLOUT_F_QUEUED;
    }

    if (c->c_evq) {
        os_eventq_remove(c->c_evq, &c->c_ev); 
    }

    OS_EXIT_CRITICAL(sr);
}


int 
os_callout_reset(struct os_callout *c, int32_t ticks, struct os_eventq *evq, 
        void *arg) 
{
    struct os_callout *entry; 
    os_sr_t sr;
    int rc;

    if (ticks < 0) {
        rc = OS_EINVAL;
        goto err;
    }

    OS_ENTER_CRITICAL(sr);

    os_callout_stop(c);

    if (ticks == 0) {
        ticks = 1; 
    }

    c->c_ticks = os_time_get() + ticks; 
    c->c_flags |= OS_CALLOUT_F_QUEUED; 
    c->c_evq = evq;
    c->c_ev.ev_arg = arg;

    entry = NULL;
    TAILQ_FOREACH(entry, &g_callout_list, c_next) {
        if (OS_TIME_TICK_LT(c->c_ticks, entry->c_ticks)) {
            break;
        }
    }

    if (entry) {
        TAILQ_INSERT_BEFORE(entry, c, c_next);
    } else {
        TAILQ_INSERT_TAIL(&g_callout_list, c, c_next);
    }

    OS_EXIT_CRITICAL(sr);

    return (0);
err:
    return (rc);
}

int
os_callout_func_reset(struct os_callout_func *cf, int32_t ticks, 
        struct os_eventq *evq, os_callout_func_t func, void *arg)
{
    int rc; 

    cf->cf_func = func;
    cf->cf_arg = arg; 

    rc = os_callout_reset(&cf->cf_c, ticks, evq, cf);

    return (rc);
}

<<<<<<< HEAD
void 
os_callout_tick(void)
{
    os_sr_t sr;
=======
/* XXX: assume called from interrupt context, no need to disable interrupts
 */
void 
os_callout_tick(void)
{
>>>>>>> 5aae54c3cfdb6e772bedf9235b354855aa7f536a
    struct os_callout *c; 
    uint32_t now;

    now = os_time_get();

<<<<<<< HEAD
    while (1) {
        OS_ENTER_CRITICAL(sr);
        c = TAILQ_FIRST(&g_callout_list);
        if (c) {
            if (OS_TIME_TICK_GEQ(now, c->c_ticks)) {
                TAILQ_REMOVE(&g_callout_list, c, c_next);
                c->c_flags &= ~OS_CALLOUT_F_QUEUED;
            } else {
                c = NULL;
            }
        }
        OS_EXIT_CRITICAL(sr);

        if (c) {
=======
    c = NULL;
    TAILQ_FOREACH(c, &g_callout_list, c_next) {
        if (OS_TIME_TICK_GEQ(now, c->c_ticks)) {
            TAILQ_REMOVE(&g_callout_list, c, c_next);
            c->c_flags &= ~OS_CALLOUT_F_QUEUED;
>>>>>>> 5aae54c3cfdb6e772bedf9235b354855aa7f536a
            os_eventq_put2(c->c_evq, &c->c_ev, 1);
        } else {
            break;
        }
    }
}
