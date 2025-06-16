/*
 * Copyright (C) 2024 Red Hat, Inc.
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include <config.h>
#include <unistd.h>

#include "internal.h"
#include "virfile.h"
#include "virmock.h"

static bool (*real_virFileExists)(const char *path);
static int (*real_access)(const char *path, int mode);
static int (*real_virFileReadAll)(const char *path, int maxlen, char **buf);

static void
init_syms(void)
{
    if (real_virFileExists && real_access && real_virFileReadAll)
        return;

    VIR_MOCK_REAL_INIT(virFileExists);
    VIR_MOCK_REAL_INIT(access);
    VIR_MOCK_REAL_INIT(virFileReadAll);
}

bool
virFileExists(const char *path)
{
    init_syms();

    /* Mock EGM device paths for testing */
    if (g_str_has_prefix(path, "/dev/egm") ||
        g_str_has_prefix(path, "/sys/class/egm/"))
        return true;

    return real_virFileExists(path);
}

int
access(const char *path, int mode)
{
    init_syms();

    /* Mock EGM device paths for testing */
    if (g_str_has_prefix(path, "/dev/egm") ||
        g_str_has_prefix(path, "/sys/class/egm/"))
        return 0;  /* success */

    return real_access(path, mode);
}

int
virFileReadAll(const char *path, int maxlen, char **buf)
{
    init_syms();

    /* Mock EGM GPU device file for testing */
    if (g_str_has_prefix(path, "/sys/class/egm/") &&
        g_str_has_suffix(path, "/gpu_devices")) {
        *buf = g_strdup("0000:01:00.0\n");
        return strlen(*buf);
    }

    return real_virFileReadAll(path, maxlen, buf);
}
