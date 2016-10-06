
PROG=	checkrestart
CFLAGS+= -Wall -Wmissing-prototypes -Wno-uninitialized -Wstrict-prototypes
MAN=	checkrestart.8

LDADD+=	-lkvm
DPADD+=	${LIBKVM}

README.md: checkrestart.8
	mandoc -T ascii checkrestart.8 | sed -e 's/.//g' -e '/^CHECKRESTART.*/d' -e '/^OpenBSD /d' -e 's/^\([A-Z]\)/# \1/' >$@

.include <bsd.prog.mk>
