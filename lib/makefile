# 
# Copyright (c) 2021 Actility. All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
# 
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
# 3. All advertising materials mentioning features or use of this software
#    must display the following acknowledgement:
#    This product includes software developed by ACTILITY.
# 4. Neither the name of ACTILITY  nor the names of its contributors may be
#    used to endorse or promote products derived from this software without
#    specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY ACTILITY  "AS IS" AND ANY EXPRESS OR IMPLIED 
# WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL ACTILITY  BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
# OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
# OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
# ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
# 
################################################################################
include $(ROOTACT)/rtbase/base/makefile.def

################################################################################
#LIB=$(PRJLIB)
INC=-I../include

################################################################################
LIB=librtbase.a
LIBSO=librtbase.so


OBJLIB=\
	$(LIB)(trace.o)\
	$(LIB)(poll.o)\
	$(LIB)(epoll.o)\
	$(LIB)(message.o)\
	$(LIB)(hashtbl.o)\
	$(LIB)(basename.o)\
	$(LIB)(tools.o)\
	$(LIB)(dir.o)\
	$(LIB)(mkdirp.o)\
	$(LIB)(timems.o)\
	$(LIB)(timemsmono.o)\
	$(LIB)(dolpath.o)\
	$(LIB)(dolpath2.o)\
	$(LIB)(base64.o)\
	$(LIB)(iso8601.o)\
	$(LIB)(ini.o)\
	$(LIB)(inihashvar.o)\
	$(LIB)(vasprintf.o)\
	$(LIB)(btree.o)\
	$(LIB)(xlap.o)\
	$(LIB)(elap.o)\
	$(LIB)(spinlockgcc.o)\
	$(LIB)(md5.o)\
	$(LIB)(version.o)

################################################################################

all	: $(OBJLIB) $(LIBSO)

$(LIB) : $(OBJ)
	$(AR) $(ARFLAGS) $(LIB) $(OBJ)

version.o	: version.c ../include/_whatstr.h
	$(CC) $(CCFLAGS) $(INC) -c $<

#%.o	: %.c
#	$(CC) $(CCFLAGS) $(INC) -c $<

.c.a :
	$(CC) $(CFLAGS) $(LIBCFLAGS) $(CFULLWARN) $(INC) -c $*.c
	@$(AR) $(ARFLAGS) $(LIB) $*.o
	@$(RM) $*.o

clean	:
	@(rm *.x *.o *.a core 2>/dev/null || exit 0)
	@$(RM) $(LIBSO)
	@$(RM) $(ROOTACT)/shlib/$(LIBSO)

$(LIBSO) : $(OBJLIB)
	@$(RM) $(ROOTACT)/shlib/$(LIBSO)
	@$(RM) *.o
	$(AR) -x $(LIB)
	$(LD) $(LDSOFLAGS) -Wl,-soname=$(LIBSO) -o $(LIBSO) *.o
	@$(RM) *.o
	$(MKSHLIB) -p rtbase -l lib/$(LIBSO)
