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

#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <kvm.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static void usage(void);

int
main(int argc, char *argv[])
{
	struct kinfo_file *elems = NULL;
	int	 elem_count, i;
	char	 errstr[_POSIX2_LINE_MAX];
	kvm_t	*kd;
	int	 vflag = 0;
	char	*corefile = NULL, *execfile = NULL, *swapfile = NULL;
	int	 kvmflags = KVM_NO_FILES;
	int	 ch;

	/* arguments parsing */
	while ((ch = getopt(argc, argv, "vM:N:W:")) != -1) {
		switch (ch) {
		case 'v':
			vflag = 1;
			break;
		case 'M':
			corefile = optarg;
			kvmflags = O_RDONLY;
			break;
		case 'N':
			execfile = optarg;
			kvmflags = O_RDONLY;
			break;
		case 'W':
			swapfile = optarg;
			kvmflags = O_RDONLY;
			break;
		default:
			usage();
		}
	}
	argc -= optind;

	if (argc != 0)
		usage();

	/* get information via kvm(3) interface */
	if ((kd = kvm_openfiles(execfile, corefile, swapfile, kvmflags, errstr))
	    == 0)
		errx(EXIT_FAILURE, "kvm_openfiles: %s", errstr);

	if ((elems = kvm_getfiles(kd, KERN_FILE_BYPID, -1,
	    sizeof(struct kinfo_file), &elem_count)) == NULL)
		errx(EXIT_FAILURE, "%s", kvm_geterr(kd));

	/* privdrop */
	if (pledge("stdio", NULL) == -1)
		err(EXIT_FAILURE, "pledge");

	/* walk thought elems */
	for (i=0; i < elem_count; i++) {
		struct kinfo_file *kf = &elems[i];

		/* only VTEXT entries */
		if (kf->v_flag != VTEXT)
			continue;

		/* only detached files */
		if (kf->va_nlink != 0)
			continue;

		if (vflag)
			printf("%d	%s	%ld	%s\n",
			    (pid_t)kf->p_pid,
			    (char *)kf->p_comm,
			    (long)kf->va_fileid,
			    (char *)kf->f_mntonname);
		else
			printf("%d	%s\n",
			    (pid_t)kf->p_pid,
			    (char *)kf->p_comm);
	}
	kvm_close(kd);

	return EXIT_SUCCESS;
}

static __dead void
usage()
{
	fprintf(stderr, "%s [-v] [-M core] [-N system] [-W swap]\n",
	    getprogname());
	exit(EXIT_FAILURE);
}
