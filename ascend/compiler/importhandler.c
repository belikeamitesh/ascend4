/*	ASCEND modelling environment
	Copyright (C) 2006, 2010 Carnegie Mellon University

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
	Handle the Import Handler library, which is a hash table of additional
	handlers for external scripts in the IMPORT statement.
*//*
	by John Pye
	Created Sept 26, 2006
*/

#include "importhandler.h"

#include <ascend/utilities/config.h>
#include <ascend/utilities/error.h>
#include <ascend/utilities/ascDynaLoad.h>
#include <ascend/general/panic.h>
#include <ascend/utilities/ascEnvVar.h>
#include <ascend/general/table.h>

#include <string.h>

/* #define SEARCH_DEBUG */
/* #define FIND_DEBUG */
/* #define IMPORTHANDLER_VERBOSE */
/* #define IMPORT_DEBUG */

/*
	Maximum number of importhandlers possible in one session. Hard to imagine
	that you would want more than this.
*/
#define IMPORTHANDLER_MAX 10

/**
	List of import handlers currently in effect. @TODO this shouldn't be a global,
	but unfortunately such globals are 'The ASCEND Way'.
*/
struct ImportHandler **importhandler_library=NULL;

/**
	Table of registered pointers for use in passing GUI data out to external scripts.
*/
struct Table *importhandler_sharedpointers=NULL;

ASC_DLLSPEC int importhandler_add(struct ImportHandler *handler){
	int i;
	if(handler==NULL){
		ERROR_REPORTER_HERE(ASC_PROG_ERR,"Handler is NULL");
		return 2;
	}
	if(importhandler_library == NULL){
		importhandler_createlibrary();
	}
	for(i=0; i< IMPORTHANDLER_MAX; ++i){
		if(importhandler_library[i] == NULL)break;
		if(importhandler_library[i]->name == handler->name){
			ERROR_REPORTER_HERE(ASC_USER_NOTE,"Handler already loaded");
			return 0;
		}
	}
	if(i==IMPORTHANDLER_MAX){
		ERROR_REPORTER_HERE(ASC_PROG_ERR,"Too many import handlers register (IMPORTHANDLER_MAX=%d)",IMPORTHANDLER_MAX);
		return 1;
	}
	importhandler_library[i] = handler;
#ifdef IMPORTHANDLER_VERBOSE
	ERROR_REPORTER_HERE(ASC_USER_NOTE,"New import hander '%s' added",handler->name);
#endif
	return 0;
}

/* Function to attempt import of an external script
	@param partialname Name of the external script (without extension), relative to PATH.
	@param defaultpath Default value of file search PATH. Is trumped by value of pathenvvar if present in environment.
	@param pathenvvar Environment variable containing the user's preferred file search path value.
	@return 0 on success
*/
int importhandler_attemptimport(const char *partialname,const char *defaultpath, const char *pathenvvar){
	ERROR_REPORTER_HERE(ASC_PROG_ERR,"%s not implemented",__FUNCTION__);
	return 1;
}

/*------------------------------------------------------------------------------
  DEFAULT IMPORT HANDLER FOR DLL/SO FILES
*/

/**
	Create a filename for an external library (DLL/SO) based on a
	partial filename.

	@param partialname The partial filename (eg 'mylib')
	@return Complete filename (eg 'libmylib.so' or 'mylib.dlll', etc)
*/
char *importhandler_extlib_filename(const char *partialname){
	char *buffer;
	buffer = ASC_NEW_ARRAY(char,PATH_MAX);

#if defined(ASC_EXTLIBSUFFIX) && defined(ASC_EXTLIBPREFIX)
	/*
		this is the preferred operation: SCons reports what the local system
		uses as its shared library file extension.
	*/
	SNPRINTF(buffer,PATH_MAX,"%s%s%s",ASC_EXTLIBPREFIX,partialname,ASC_EXTLIBSUFFIX);
#else
#ifdef __GNUC__
# warning "You should be using Use ASC_EXTLIBPREFIX and ASC_EXTLIBSUFFIX!"
#endif
	/**
		@DEPRECATED

		If we don't have ASC_EXTLIB-SUFFIX and -PREFIX then we can do some
		system-specific stuff here, but it's not as general.
	*/
# ifdef __WIN32__
	SNPRINTF(buffer,PATH_MAX,"%s.dll",partialname);
# elif defined(linux)
	SNPRINTF(buffer,PATH_MAX,"lib%s.so",partialname); /* changed from .o to .so -- JP */
# elif defined(sun) || defined(solaris)
	SNPRINTF(buffer,PATH_MAX,"%s.so.1.0",partialname);
# elif defined(__hpux)
	SNPRINTF(buffer,PATH_MAX,"%s.sl",partialname);
# elif defined(_SGI_SOURCE)
	SNPRINTF(buffer,PATH_MAX,"%s.so",partialname);
# else
#  error "Unknown system type (please define ASC_EXTLIBSUFFIX and ASC_EXTLIBPREFIX)"
# endif
#endif

	return buffer;
}

/**
	Perform the actual importing of an external DLL/SO in to ASCEND. Can assume
	that the file exists and is readable.

	@param fp Location of DLL/SO file
	@param initfunc Name of registration function, preferably NULL (so that ASCEND automatically determines it)
	@param partialpath as specified in 'IMPORT' statement, for creation of auto_initfunc name
	@return 0 on success
*/
int importhandler_extlib_import(const struct FilePath *fp,const char *initfunc,const char *partialpath){

	struct FilePath *fp1;
	char *stem;
	char *path;
	char auto_initfunc[PATH_MAX];
	int result;

	fp1 = ospath_getabs(fp);
	ospath_cleanup(fp1);
	path = ospath_str(fp1);
	ospath_free(fp1);

	if(path==NULL){
		ERROR_REPORTER_HERE(ASC_PROG_ERR,"File path is NULL");
		return 1;
	}
#ifdef IMPORT_DEBUG
	CONSOLE_DEBUG("Importing extlib with path '%s'",path);
#endif

	if(initfunc==NULL){
		fp1 = ospath_new(partialpath);
		stem = ospath_getbasefilename(fp1);
		strncpy(auto_initfunc,stem,PATH_MAX);
		ospath_free(fp1);
		ASC_FREE(stem);

		strncat(auto_initfunc,"_register",PATH_MAX-strlen(auto_initfunc));
		/* CONSOLE_DEBUG("Created auto-initfunc name '%s'",auto_initfunc); */
		result = Asc_DynamicLoad(path,auto_initfunc);
	}else{
		result = Asc_DynamicLoad(path,initfunc);
	}

#ifdef IMPORT_DEBUG
	if(result){
		CONSOLE_DEBUG("FAILED TO IMPORT '%s' (error %d)",partialpath,result);
	}else{
		if(initfunc==NULL){
	  		CONSOLE_DEBUG("'%s' OK (%s)",auto_initfunc,path);
		}else{
			CONSOLE_DEBUG("'%s' OK (explicitly named, got file '%s')",initfunc,path);
		}
	}
#endif

	ASC_FREE(path);
	return result;
}

/*------------------------------------------------------------------------------
  LIST-BASED FUNCTIONS related to IMPORT handler 'library'
*/

int importhandler_createlibrary(){
	int i;
	struct ImportHandler *extlib_handler;

	if(importhandler_library!=NULL){
		/* ERROR_REPORTER_HERE(ASC_PROG_ERR,"Already created"); */
		return 0;
	};
	importhandler_library=ASC_NEW_ARRAY(struct ImportHandler *,IMPORTHANDLER_MAX);
	for(i=0; i < IMPORTHANDLER_MAX; ++i){
		importhandler_library[i] = NULL;
	}
	/* CONSOLE_DEBUG("ImportHandler library created"); */

	extlib_handler = ASC_NEW(struct ImportHandler);
	extlib_handler->name ="extlib";
	extlib_handler->filenamefn = &importhandler_extlib_filename;
	extlib_handler->importfn = &importhandler_extlib_import;
	extlib_handler->destroyfn = NULL;
	if(importhandler_add(extlib_handler)){
		ERROR_REPORTER_HERE(ASC_PROG_ERR,"Failed to create 'extlib' import handler");
		return 1;
	}

	return 0;
}

int importhandler_destroy(struct ImportHandler *handler){
	int res = 0;
	if(handler->destroyfn){
		res = (*(handler->destroyfn))(handler);
	}/* else: nothing inside needs destroying */
	if(!res)ASC_FREE(handler);
	return res;
}

struct ImportHandler *importhandler_lookup(const char *name){
	int i;
	for(i=0; i < IMPORTHANDLER_MAX && importhandler_library[i] != NULL; ++i){
		if(importhandler_library[i]->name
			&& 0==strcmp(name,importhandler_library[i]->name)
		){
			return importhandler_library[i];
		}
	}
	return NULL;
}

/** @return 0 on success */
int importhandler_destroylibrary(){
	int i;
	int err = 0, thiserr;
#ifdef IMPORT_DEBUG
	CONSOLE_DEBUG("Destroying importhandler library...");
#endif
	///importhandler_printlibrary(stderr);
	if(importhandler_library!=NULL){
		for(i=IMPORTHANDLER_MAX - 1; i >= 0; --i){
			if(importhandler_library[i]==NULL)continue;
			thiserr = importhandler_destroy(importhandler_library[i]);
			if(!thiserr){
				importhandler_library[i] = NULL;
#ifdef IMPORT_DEBUG
				CONSOLE_DEBUG("Destroyed import handler");
#endif
			}
			err = err | thiserr;
		}
		if(!err){
			ASC_FREE(importhandler_library);
			importhandler_library = NULL;
		}
	}
	if(err)ERROR_REPORTER_HERE(ASC_PROG_WARNING,"Failed to destroy importhandler library");
	return err;
}

/** @return 0 on success */
int importhandler_printlibrary(FILE *fp){
	int i;
	if(importhandler_library==NULL){
		fprintf(fp,"# importhandler_printlibrary: empty\n");
		return 0;
	}else{
		fprintf(fp,"# importhandler_printlibrary: start\n");
		for(i=0; i < IMPORTHANDLER_MAX && importhandler_library[i] != NULL; ++i){
			fprintf(fp,"%s\n",importhandler_library[i]->name);
		}
		fprintf(fp,"# importhandler_printlibrary: end\n");
		return 0;
	}
}

int importhandler_printhandler(FILE *fp, struct ImportHandler *handler){
	ERROR_REPORTER_HERE(ASC_PROG_ERR,"%s not implemented",__FUNCTION__);
	CONSOLE_DEBUG("NOT IMPLEMENTED");
	return 1;
}

/*------------------------------------------------------------------------------
  PATH SEARCH ROUTINES
*/

/**
	A little structure to help with searching for import files

	@see test_importsearch
*/
struct ImportHandlerSearch{
	char *partialname; /**< for example 'myext' */
	struct FilePath *relativedir; /**< for example 'path/to' */
	struct FilePath *foundpath; /**< the complete filepath located, for example '/home/john/path/to/libmyext.so' */
	struct ImportHandler *handler; /**< pointer to the import handler identified for this file */
};

FilePathTestFn importhandler_search_test;

/**
	A FilePath 'test' function for passing to the ospath_searchpath_iterate function.
	This test function will return a match when an importable file having the
	required name is present in the fully resolved path.

	@param path the search path component
	@param userdata will be an ImportHandlerSearch object
	@return 1 if found
*/
int importhandler_search_test(struct FilePath *path, void *userdata){
	/*  user data = the relative path, plus a place
		to store the full path when found */
	FILE *f;
	char *filename;
#ifdef SEARCH_DEBUG
	char *fullpath;
#endif
	struct ImportHandlerSearch *searchdata;
	struct FilePath *fp, *fp1, *fp2;
	int i;
	ospath_stat_t buf;

	searchdata = (struct ImportHandlerSearch *)userdata;

#ifdef SEARCH_DEBUG
	char *pathcomponent;
	pathcomponent = ospath_str(path); //eg '/home/john'
	CONSOLE_DEBUG("In directory '%s'...",pathcomponent);
	ASC_FREE(pathcomponent);
#endif

	asc_assert(importhandler_library!=NULL);

	for(i=0; i<IMPORTHANDLER_MAX && importhandler_library[i]!=NULL; ++i){

		filename = (*(importhandler_library[i]->filenamefn))(searchdata->partialname); /* eg 'myext' -> 'libmyext.so' */
		if(filename==NULL){
#ifdef SEARCH_DEBUG
			CONSOLE_DEBUG("Unable to create filename from partialname '%s'",searchdata->partialname);
#endif
			continue;
		}
#ifdef SEARCH_DEBUG
		CONSOLE_DEBUG("Filename '%s'",filename);
#endif
		fp = ospath_new_noclean(filename); /* eg 'libmyext.so' */
		ASC_FREE(filename);
		asc_assert(fp!=NULL);

#ifdef SEARCH_DEBUG
		fullpath = ospath_str(searchdata->relativedir);
		CONSOLE_DEBUG("Relative dir is '%s'",fullpath);
		ASC_FREE(fullpath);
#endif
		fp1 = ospath_concat(path,searchdata->relativedir); /* eg '/home/john/path/to' */
		asc_assert(fp1!=NULL);


#ifdef SEARCH_DEBUG
		fullpath = ospath_str(fp1);
		CONSOLE_DEBUG("Path is '%s'",fullpath);
		ASC_FREE(fullpath);

		fullpath = ospath_str(fp);
		CONSOLE_DEBUG("Filename is '%s'",fullpath);
		ASC_FREE(fullpath);
#endif

		fp2 = ospath_concat(fp1,fp); /* eg '/home/john/path/to/libmyext.so' */
		asc_assert(fp2!=NULL);
		ospath_free(fp1);
		ospath_free(fp);

#ifdef SEARCH_DEBUG
		fullpath = ospath_str(fp2);
		CONSOLE_DEBUG("Checking for readable '%s'",fullpath);
		ASC_FREE(fullpath);
#endif

		if(0==ospath_stat(fp2,&buf) && NULL!=(f = ospath_fopen(fp2,"r"))){
			fclose(f);
			searchdata->foundpath = fp2;
			searchdata->handler = importhandler_library[i];
			return 1; /* success */
		}

		ospath_free(fp2);
	}
	return 0; /* failed */
}

struct FilePath *importhandler_findinpath(const char *partialname
		, const char *defaultpath, char *envv, struct ImportHandler **handler
){
	struct FilePath *fp, *fp1; /* relative path */
	struct ImportHandlerSearch searchdata;
	char *path, *filename;
	struct FilePath **sp;
	int i;
	ospath_stat_t buf;
	FILE *f;
	const char *epath;
	char *epathmem = NULL;

	fp1 = ospath_new_noclean(partialname); /* eg 'path/to/myext' */
	if(fp1==NULL){
		ERROR_REPORTER_HERE(ASC_USER_ERROR,"Invalid partial path '%s'",partialname);
		return NULL;
	}

	searchdata.partialname = ospath_getbasefilename(fp1);
	if(searchdata.partialname==NULL){
#ifdef FIND_DEBUG
		CONSOLE_DEBUG("Not a filename");
#endif
		ospath_free(fp1);
		return NULL;
	}

	searchdata.relativedir = ospath_getdir(fp1);
	if(searchdata.relativedir ==NULL){
		ERROR_REPORTER_HERE(ASC_PROG_ERR,"unable to retrieve file dir");
		ospath_free(fp1);
		ASC_FREE(searchdata.partialname);
		return NULL;
	}
	ospath_free(fp1);

	searchdata.foundpath = NULL;
	searchdata.handler = NULL;

	/* first, attempt to open without searching in path */

#ifdef FIND_DEBUG
	CONSOLE_DEBUG("SEARCHING RELATIVE TO CURRENT DIRECTORY");
#endif

	for(i=0; i<IMPORTHANDLER_MAX && importhandler_library[i]!=NULL; ++i){

		filename = (*(importhandler_library[i]->filenamefn))(searchdata.partialname); /* eg 'myext' -> 'libmyext.so' */
		if(filename==NULL){
#ifdef FIND_DEBUG
			CONSOLE_DEBUG("Unable to create filename from partialname '%s'",searchdata.partialname);
#endif
			continue;
		}

		fp = ospath_new(filename); /* eg 'libmyext.so' */
		ASC_FREE(filename);
		asc_assert(fp!=NULL);

		path = ospath_str(searchdata.relativedir);
		if(strlen(path)==0){
#ifdef FIND_DEBUG
			CONSOLE_DEBUG("ADDING '.' AT START OF EMPTY RELATIVE PATH");
#endif
			ospath_free(searchdata.relativedir);
			searchdata.relativedir = ospath_new_noclean(".");
			ASC_FREE(path);
			path = ospath_str(searchdata.relativedir);
		}
#ifdef FIND_DEBUG
		CONSOLE_DEBUG("Relative dir is '%s'",path);
#endif
		ASC_FREE(path);

#ifdef FIND_DEBUG
		path = ospath_str(fp);
		CONSOLE_DEBUG("Filename is '%s'",path);
		ASC_FREE(path);
#endif

		fp1 = ospath_concat(searchdata.relativedir,fp); /* eg '/home/john/path/to/libmyext.so' */
		asc_assert(fp1!=NULL);
		ospath_free(fp);

#ifdef FIND_DEBUG
		path = ospath_str(fp1);
		CONSOLE_DEBUG("Checking for readable '%s'",path);
		ASC_FREE(path);
#endif

		if(0==ospath_stat(fp1,&buf) && NULL!=(f = ospath_fopen(fp1,"r"))){
#ifdef FIND_DEBUG
			CONSOLE_DEBUG("Found in current directory!");
#endif
			fclose(f);
			ASC_FREE(searchdata.partialname);
			ospath_free(searchdata.relativedir);
			*handler = importhandler_library[i];
			return fp1;
		}
#ifdef FIND_DEBUG
		else{
			CONSOLE_DEBUG("Not found");
		}
#endif

		ospath_free(fp1);
	}

	/*-----------------------*/

#ifdef FIND_DEBUG
	CONSOLE_DEBUG("SEARCHING ACCORDING TO ENV VAR $%s",envv);
#endif

	epath = defaultpath;
	epathmem=Asc_GetEnv(envv);
	if(epathmem){
		epath = epathmem;
#ifdef FIND_DEBUG
	}else{
		CONSOLE_DEBUG("ENV VAR '%s' NOT FOUND, FALLING BACK TO DEFAULT SEARCH PATH = '%s'",envv,defaultpath);
#endif
	}

#ifdef FIND_DEBUG
	CONSOLE_DEBUG("SEARCHPATH IS %s",epath);
#endif
	sp = ospath_searchpath_new(epath);

	if(NULL==ospath_searchpath_iterate(sp,&importhandler_search_test,&searchdata)){
#ifdef FIND_DEBUG
		CONSOLE_DEBUG("Import not found in searchpath");
#endif
		ospath_free(searchdata.relativedir);
		ASC_FREE(searchdata.partialname);
		ospath_searchpath_free(sp);
		if(epathmem)ASC_FREE(epathmem);
		return NULL;
	}

#ifdef FIND_DEBUG
	CONSOLE_DEBUG("Found in searchpath :-)");
#endif

	ospath_searchpath_free(sp);
	ASC_FREE(searchdata.partialname);
	if(epathmem)ASC_FREE(epathmem);
	ospath_free(searchdata.relativedir);
	*handler = searchdata.handler;
	return searchdata.foundpath;
}

/*------------------------------------------------------------------------------
  SHARED POINTER TABLE
*/

int importhandler_createsharedpointertable(){
	if(importhandler_sharedpointers==NULL){
		/* CONSOLE_DEBUG("CREATED SHARED POINTER TABLE"); */
		importhandler_sharedpointers = CreateTable(31);
	}
	return 0;
}

int importhandler_setsharedpointer(const char *key, void *ptr){
	importhandler_createsharedpointertable();
	if(key==NULL){
		ERROR_REPORTER_HERE(ASC_PROG_ERR,"key is NULL");
		return 1;
	}

	/* woops! 'AddTableData' does *not* overwrite table entries! */
	RemoveTableData(importhandler_sharedpointers,(char *)key);

	AddTableData(importhandler_sharedpointers,ptr,key);
#ifdef IMPORTHANDLER_VERBOSE
	CONSOLE_DEBUG("Set shared pointer '%s' to %p",key, ptr);
#endif
	asc_assert(importhandler_getsharedpointer(key)==ptr);
	return 0;
}

void *importhandler_getsharedpointer(const char *key){
	importhandler_createsharedpointertable();
	if(key==NULL){
		ERROR_REPORTER_HERE(ASC_PROG_ERR,"key is NULL");
		return NULL;
	}
	return LookupTableData(importhandler_sharedpointers,key);
}
