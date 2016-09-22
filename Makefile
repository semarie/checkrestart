
PROG=	checkrestart
CFLAGS+= -Wall -Wmissing-prototypes -Wno-uninitialized -Wstrict-prototypes
NOMAN=

LDADD+=	-lkvm
DPADD+=	${LIBKVM}

.include <bsd.prog.mk>
