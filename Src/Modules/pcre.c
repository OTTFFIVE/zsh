/*
 * pcre.c - interface to the PCRE library
 *
 * This file is part of zsh, the Z shell.
 *
 * Copyright (c) 2001 Clint Adams
 * All rights reserved.
 *
 * Permission is hereby granted, without written agreement and without
 * license or royalty fees, to use, copy, modify, and distribute this
 * software and to distribute modified versions of this software for any
 * purpose, provided that the above copyright notice and the following
 * two paragraphs appear in all copies of this software.
 *
 * In no event shall Clint Adams or the Zsh Development Group be liable
 * to any party for direct, indirect, special, incidental, or consequential
 * damages arising out of the use of this software and its documentation,
 * even if Andrew Main and the Zsh Development Group have been advised of
 * the possibility of such damage.
 *
 * Clint Adams and the Zsh Development Group specifically disclaim any
 * warranties, including, but not limited to, the implied warranties of
 * merchantability and fitness for a particular purpose.  The software
 * provided hereunder is on an "as is" basis, and Andrew Main and the
 * Zsh Development Group have no obligation to provide maintenance,
 * support, updates, enhancements, or modifications.
 *
 */

/**/
#if defined(HAVE_PCRE_COMPILE) && defined(HAVE_PCRE_EXEC)

#include "pcre.mdh"
#include "pcre.pro"
#include <pcre.h>

static pcre *pcre_pattern;
static pcre_extra *pcre_hints;

/**/
static int
bin_pcre_compile(char *nam, char **args, char *ops, int func)
{
    int pcre_opts = 0, pcre_errptr;
    const char *pcre_error;
    
    if(ops['a']) pcre_opts |= PCRE_ANCHORED;
    if(ops['i']) pcre_opts |= PCRE_CASELESS;
    if(ops['m']) pcre_opts |= PCRE_MULTILINE;
    if(ops['x']) pcre_opts |= PCRE_EXTENDED;
    
    pcre_pattern = pcre_compile(*args, pcre_opts, &pcre_error, &pcre_errptr, NULL);
    
    if (pcre_pattern == NULL)
    {
	zwarnnam(nam, "error in regex: %s", pcre_error, 0);
	return 1;
    }
    
    return 0;
}

/**/
#ifdef HAVE_PCRE_STUDY

/**/
static int
bin_pcre_study(char *nam, char **args, char *ops, int func)
{
    const char *pcre_error;
    
    pcre_hints = pcre_study(pcre_pattern, 0, &pcre_error);
    if (pcre_error != NULL)
    {
	zwarnnam(nam, "error while studying regex: %s", pcre_error, 0);
	return 1;
    }
    
    return 0;
}

/**/
#endif /* HAVE_PCRE_STUDY */

/**/
static int
bin_pcre_match(char *nam, char **args, char *ops, int func)
{
#define PCRE_OVEC_SIZE 50
    
    int ovec[PCRE_OVEC_SIZE];   /* throwing this away now, but will be useful someday */
    
    return !(pcre_exec(pcre_pattern, pcre_hints, *args, strlen(*args), 0, 0, ovec, PCRE_OVEC_SIZE) >= 0);
}

static struct builtin bintab[] = {
    BUILTIN("pcre_compile", 0, bin_pcre_compile, 1, 1, 0, "aimx",  NULL),
#ifdef HAVE_PCRE_STUDY
    BUILTIN("pcre_study",   0, bin_pcre_study,   0, 0, 0, NULL,    NULL),
#endif
    BUILTIN("pcre_match",   0, bin_pcre_match,   1, 1, 0, NULL,    NULL)
};

/**/
int
setup_(Module m)
{
    return 0;
}

/**/
int
boot_(Module m)
{
    return !addbuiltins(m->nam, bintab, sizeof(bintab)/sizeof(*bintab));
}

/**/
int
cleanup_(Module m)
{
    deletebuiltins(m->nam, bintab, sizeof(bintab)/sizeof(*bintab));
    return 0;
}

/**/
int
finish_(Module m)
{
    return 0;
}

/**/
#endif /* HAVE_PCRE_COMPILE && HAVE_PCRE_EXEC */
