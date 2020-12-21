/*
blkdump -- dumps blocks from a file
Written by Matthew Veety, circa 2010.
Under BSD license
*/

#include <u.h>
#include <libc.h>

int
printblock(u8int *block, uint len)
{
	u16int i;

	for(i = 0; i < len; i++){
		if(i%16 == 0){
			if(i != 0)
				print("\n");
			print("  %u4.hx: ", i);
		}
		print("%u2.hhx ", block[i]);
	}
	return 0;
}

void
usage(void)
{
	fprint(2, "usage: blkdmp -f file [-o offset] [-n nblocks] [-b bsize]\n");
	exits("usage");
}

void
main(int argc, char *argv[])
{
	uint nblocks = 1;
	char *fn = nil;
	uint bsize = 512;
	u64int offset = 0;

	int fd;
	u8int *blockbf;
	uint i;

	ARGBEGIN{
	case 'f': // filename
		fn = strdup(EARGF(usage()));
		break;
	case 'o': // offset
		offset = atoi(EARGF(usage()));
		break;
	case 'n':
		nblocks = atoi(EARGF(usage()));
		break;
	case 'b':
		bsize = atoi(EARGF(usage()));
		break;
	case 'h':
	default:
		usage();
	}ARGEND;
	if(!fn)
		usage();
	fd = open(fn, OREAD);
	if(fd < 0){
		perror(nil);
		exits("open");
	}
	blockbf = mallocz(bsize, 1);
	if(!blockbf){
		perror(nil);
		exits("malloc");
	}
	print("dumping %ud blocks at %ulld from %s\n", nblocks, offset, fn);
	for(i = 0; i < nblocks; i++){
		if(pread(fd, blockbf, bsize, (offset+i)*bsize) < bsize){
			fprint(2, "error: short read: %r\n");
			exits("short read");
		}
		print("block %ulld (size = %ud) {\n", offset+i, bsize);
		printblock(blockbf, bsize);
		print("\n}\n\n");
		memset(blockbf, 0, bsize);
	}
	close(fd);
	free(blockbf);
	free(fn);
	exits(nil);
}
