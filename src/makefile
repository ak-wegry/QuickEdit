CFLAGS = -P -tW -c -w-8004 -w-8027 -w-8057 -w-8012
CC = bcc32 $(CFLAGS)
LFLAGS = -tW
LINK = bcc32 $(LFLAGS)
RCC = brc32

EXEC = QuickEdit.exe
OBJS = QuickEdit.obj CalTime.obj DateCtrl.obj DialogBox.obj Formating.obj OpeString.obj StrData.obj TableInfo.obj TreeInfo.obj Variable.obj $(LIB)\htmlhelp.lib
RESS = QuickEdit.rc

$(EXEC) : $(OBJS) $(RESS)
	$(LINK) $(OBJS)
	$(RCC) $(RESS) $(EXEC)

QuickEdit.obj : QuickEdit.cpp QuickEdit.h resource.h StrData.h OpeString.h TableInfo.h SubProc.h DateCtrl.h Formating.h DialogBox.h TreeInfo.h Variable.h
	$(CC) QuickEdit.cpp

CalTime.obj : CalTime.cpp CalTime.h Variable.h
        $(CC) CalTime.cpp

DateCtrl.obj : DateCtrl.cpp DateCtrl.h
	$(CC) DateCtrl.cpp

DialogBox.obj : DialogBox.cpp QuickEdit.h DialogBox.h
	$(CC) DialogBox.cpp

Formating.obj : Formating.cpp Formating.h
	$(CC) Formating.cpp

OpeString.obj : OpeString.cpp OpeString.h
	$(CC) OpeString.cpp

StrData.obj : StrData.cpp StrData.h
	$(CC) StrData.cpp

TableInfo.obj : TableInfo.cpp TableInfo.h
	$(CC) TableInfo.cpp

TreeInfo.obj : TreeInfo.cpp TreeInfo.h
	$(CC) TreeInfo.cpp

Variable.obj : Variable.cpp Variable.h
	$(CC) Variable.cpp
