/*	ASCEND modelling environment
	Copyright (C) 1990, 1993, 1994 Thomas Guthrie Epperly
	Copyright (C) 2006 Carnegie Mellon University

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2, or (at your option)
	any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*//**
	@file
	Temporary name output routine.
*//*
	by Tom Epperly
	Last in CVS: $Revision: 1.5 $ $Date: 1998/04/16 00:43:29 $ $Author: ballan $
*/

#ifndef ASC_NAMEIO_H
#define ASC_NAMEIO_H

#include <stdio.h>
#include <ascend/general/platform.h>
#include <ascend/general/dstring.h>
#include <ascend/compiler/compiler.h>
#include <ascend/compiler/expr_types.h>
#include <ascend/compiler/symtab.h>

/**	@addtogroup compiler_stmt Compiler Statements
	@{
*/

extern void WriteName(FILE *f, CONST struct Name *n);
/**<
 *  Write n to file f.  No leading or trailing spaces are added,
 *  and no trailing newline is added.
 */

extern void WriteNameNode(FILE *f, CONST struct Name *n);
/**<
 *  Write just this one name node, and not any of the ones following it.
 */

ASC_DLLSPEC char*WriteNameString(CONST struct Name *n);
/**<
 * Return a string containing the name.
 * User is responsible for freeing string.
 */

extern void WriteName2Str(Asc_DString *dstring, CONST struct Name *n);
/**<
 *  Write n to dstring.  No leading or trailing spaces are added,
 *  and no trailing newline is added.
 */

extern void WriteNameNode2Str(Asc_DString *dstring,CONST struct Name *n);
/**<
 *  Write just this one name node, and not any of the ones following it.
 */

/* @} */

#endif  /* ASC_NAMEIO_H */

