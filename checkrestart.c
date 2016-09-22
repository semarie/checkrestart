/*
 * Copyright (c) 2016 Sebastien Marie <semarie@online.fr>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <sys/param.h>
#include <sys/sysctl.h>
#include <sys/vnode.h>

#define _KERNEL
#include <ufs/ufs/quota.h>
#include <ufs/ufs/inode.h>
#undef _KERNEL

#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <kvm.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static int get_i_effnlink(kvm_t *, u_long);

int
main(int argc, char *argv[])
{
	int mib[] = {
		CTL_KERN,
		KERN_FILE,
		KERN_FILE_BYPID, -1,	/* all processes */
		sizeof(struct kinfo_file),
		0,			/* number of structures to return */
	};
	struct kinfo_file *elems = NULL;
	size_t elem_len = 0;
	size_t elem_count, i;
	char errstr[_POSIX2_LINE_MAX];
	kvm_t *kd;

	/* root is required */
	if (getuid() != 0)
		errx(EXIT_FAILURE, "needs root privileges");

	/* get required size to allocate */
	if (sysctl(mib, 6, NULL, &elem_len, NULL, 0) == -1)
		err(EXIT_FAILURE, "sysctl");

	if ((elems = malloc(elem_len)) == NULL)
		err(EXIT_FAILURE, NULL);

	/* grab informations on opened files */
	mib[5] = elem_len / sizeof(struct kinfo_file);
	if (sysctl(mib, 6, elems, &elem_len, NULL, 0) == -1)
		err(EXIT_FAILURE, "sysctl");

	elem_count = elem_len / sizeof(struct kinfo_file);

	/* open kvm for information unavailable from kinfo_file */
	if ((kd = kvm_openfiles(NULL, NULL, NULL, O_RDONLY, errstr)) == 0)
		errx(EXIT_FAILURE, "kvm_openfiles: %s", errstr);

	if (pledge("stdio", NULL) == -1)
		err(EXIT_FAILURE, "pledge");

	/* walk thought elems */
	for (i=0; i < elem_count; i++) {
		struct kinfo_file *kf = &elems[i];

		/* only VTEXT entries */
		if (kf->v_flag != VTEXT)
			continue;

		/* only detached files (on UFS) */
		if (kf->v_tag != VT_UFS ||
		    get_i_effnlink(kd, kf->v_data) != 0)
			continue;

		printf("%d	%s	%ld	%s\n",
		    (pid_t)kf->p_pid,
		    (char *)kf->p_comm,
		    (long)kf->va_fileid,
		    (char *)kf->f_mntonname);
	}
	kvm_close(kd);

	return EXIT_SUCCESS;
}

static int
get_i_effnlink(kvm_t *kd, u_long v_data)
{
	struct inode inode, *ip = &inode;

	/* read v_data (struct inode) */
	if (kvm_read(kd, v_data, &inode, sizeof(struct inode))
	    != sizeof(struct inode))
		errx(EXIT_FAILURE, "cannot read: struct inode: %s",
		    kvm_geterr(kd));

	return ip->i_effnlink;
}
