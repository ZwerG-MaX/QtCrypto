/*
 * qca_export.h - Qt Cryptographic Architecture
 * Copyright (C) 2003-2005  Justin Karneges <justin@affinix.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef QCA_EXPORT_H
#define QCA_EXPORT_H

#include <QtGlobal>

#ifdef Q_OS_WIN32
# ifndef QCA_STATIC
#  ifdef QCA_MAKEDLL
#   define QCA_EXPORT __declspec(dllexport)
#  else
#   define QCA_EXPORT __declspec(dllimport)
#  endif
# endif
#endif
#ifndef QCA_EXPORT
# define QCA_EXPORT
#endif

#ifdef Q_OS_WIN32
# ifdef QCA_PLUGIN_DLL
#  define QCA_PLUGIN_EXPORT extern "C" __declspec(dllexport)
# else
#  define QCA_PLUGIN_EXPORT extern "C" __declspec(dllimport)
# endif
#endif
#ifndef QCA_PLUGIN_EXPORT
# define QCA_PLUGIN_EXPORT extern "C"
#endif

#endif
