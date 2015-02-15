/*
 * File:		/src/bool.h
 *
 * Project:     nix-compress aka lz1337
 * Author:		Sebastian Büttner
 * Created:		01/05/2012
 *
 */

#ifndef BOOL_H
#define BOOL_H

#ifdef _WIN32
	typedef enum bool {false, true} bool;
#else
	#include <stdbool.h>
#endif

#endif /* BOOL_H */