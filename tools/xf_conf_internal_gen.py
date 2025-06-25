#!/usr/bin/env python3

# ------------------------------------------------------------------------------
# @brief Generates xf_conf_internal.h from xf_conf_template.h to provide default values
# SPDX-FileCopyrightText: 2025 LVGL LLC
# SPDX-License-Identifier: MIT licence
# SPDX-FileCopyrightText: 2025 CompanyNameMagicTag
# SPDX-License-Identifier: Apache-2.0
# ------------------------------------------------------------------------------

'''
Generates xf_conf_internal.h from xf_conf_template.h to provide default values
'''

import os
import sys
import re

SCRIPT_DIR = os.path.dirname(__file__)
XF_CONF_TEMPLATE = os.path.join(SCRIPT_DIR, "..", "xf_conf_template.h")
XF_CONF_INTERNAL = os.path.join(SCRIPT_DIR, "..", "src", "xf_conf_internal.h")

if sys.version_info < (3,6,0):
  print("Python >=3.6 is required", file=sys.stderr)
  exit(1)

def check_for_tabs(file_path):
    errors = []
    with open(file_path, 'r') as file:
        for line_number, line in enumerate(file, 1):
            if '\t' in line:
                errors.append(f" {file_path}:{line_number}")

    if errors:
        print(f"Tabs found in the following files:", file=sys.stderr)
        for error in errors:
            print(error, file=sys.stderr)
        print("Please replace tabs with spaces.", file=sys.stderr)
        exit(1)

check_for_tabs(XF_CONF_TEMPLATE)

fin = open(XF_CONF_TEMPLATE)
fout = open(XF_CONF_INTERNAL, "w", newline='')

fout.write(
'''/**
 * GENERATED FILE, DO NOT EDIT IT!
 * @file xf_conf_internal.h
 * This file ensures all defines of xf_conf.h have a default value.
 */

/* *INDENT-OFF* */

#ifndef __XF_CONF_INTERNAL_H__
#define __XF_CONF_INTERNAL_H__

/* Handle special Kconfig options */
#if !defined(XF_KCONFIG_IGNORE)
    #include "xf_conf_kconfig.h"
    #if defined(CONFIG_XF_CONF_SKIP) && !defined(XF_CONF_SKIP)
        #define XF_CONF_SKIP
    #endif
#endif

/* If "xf_conf.h" is available from here try to use it later. */
#ifdef __has_include
    #if __has_include("xf_conf.h")
        #ifndef XF_CONF_INCLUDE_SIMPLE
            #define XF_CONF_INCLUDE_SIMPLE
        #endif
    #endif
#endif

/* If xf_conf.h is not skipped include it */
#if !defined(XF_CONF_SKIP) || defined(XF_CONF_PATH)
    #ifdef XF_CONF_PATH                           /* If there is a path defined for xf_conf.h, use it */
        #include XF_CONF_PATH                     /* Note: Make sure to define custom CONF_PATH as a string */
    #elif defined(XF_CONF_INCLUDE_SIMPLE)         /* Or simply include xf_conf.h is enabled. */
        #include "xf_conf.h"
    #else
        #include "../../xf_conf.h"                /* Else assume xf_conf.h is next to the xtiny folder. */
    #endif
    #if !defined(XF_CONF_H) && !defined(XF_CONF_SUPPRESS_DEFINE_CHECK)
        /* #include will sometimes silently fail when __has_include is used */
        /* https://gcc.gnu.org/bugzilla/show_bug.cgi?id=80753 */
        #pragma message("Possible failure to include xf_conf.h, please read the comment in this file if you get errors")
    #endif
#endif

#if defined(CONFIG_XF_CONF_SKIP)
    #define XF_KCONFIG_PRESENT
#endif

/*----------------------------------
 * Start parsing xf_conf_template.h
 -----------------------------------*/
'''
)

started = 0

for line in fin.read().splitlines():
  if not started:
    if '#define __XF_CONF_H__' in line:
      started = 1
      continue
    else:
      continue

  if '/* --END OF __XF_CONF_H__-- */' in line: break

  #Is there a #define in this line?
  r = re.search(r'^([\s]*)#[\s]*(undef|define)[\s]+([^\s]+).*$', line)   # \s means any white space character

  if r:
    indent = r[1]

    name = r[3]
    name = re.sub(r'\(.*?\)', '', name, count=1)    #remove parentheses from macros. E.g. MY_FUNC(5) -> MY_FUNC

    line = re.sub(r'[\s]*', '', line, count=1)

    #If the value should be 1 (enabled) by default use a more complex structure for Kconfig checks because
    #if a not defined CONFIG_... value should be interpreted as 0 and not the LVGL default
    is_one = re.search(r'#[\s]*define[\s]*[A-Z0-9_]+[\s]+1([\s]*$|[\s]+)', line)
    if is_one:
      #1. Use the value if already set from xf_conf.h or anything else (i.e. do nothing)
      #2. In Kconfig environment use the CONFIG_... value if set, else use 0
      #3. In not Kconfig environment use the LVGL's default value

      fout.write(
        f'{indent}#ifndef {name}\n'
        f'{indent}    #ifdef XF_KCONFIG_PRESENT\n'
        f'{indent}        #ifdef CONFIG_{name.upper()}\n'
        f'{indent}            #define {name} CONFIG_{name.upper()}\n'
        f'{indent}        #else\n'
        f'{indent}            #define {name} 0\n'
        f'{indent}        #endif\n'
        f'{indent}    #else\n'
        f'{indent}        {line}\n'
        f'{indent}    #endif\n'
        f'{indent}#endif\n'
      )
    else:
      #1. Use the value if already set from xf_conf.h or anything else  (i.e. do nothing)
      #2. Use the Kconfig value if set
      #3. Use the LVGL's default value

      fout.write(
        f'{indent}#ifndef {name}\n'
        f'{indent}    #ifdef CONFIG_{name.upper()}\n'
        f'{indent}        #define {name} CONFIG_{name.upper()}\n'
        f'{indent}    #else\n'
        f'{indent}        {line}\n'
        f'{indent}    #endif\n'
        f'{indent}#endif\n'
      )

  elif re.search('^ *typedef .*;.*$', line):
    continue   #ignore typedefs to avoid redeclaration
  else:
    fout.write(f'{line}\n')

fout.write(
r'''/*----------------------------------
 * End of parsing xf_conf_template.h
 -----------------------------------*/

#if XF_ENABLE_EXTENDED
#include XF_CONF_EXTENDED_PATH
#endif

#endif /* __XF_CONF_INTERNAL_H__ */

/* *INDENT-ON* */
'''
)

fin.close()
fout.close()
