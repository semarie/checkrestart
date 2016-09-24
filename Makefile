
PROG=	checkrestart
CFLAGS+= -Wall -Wmissing-prototypes -Wno-uninitialized -Wstrict-prototypes
MAN=	checkrestart.8

LDADD+=	-lkvm
DPADD+=	${LIBKVM}

.include <bsd.prog.mk>
