/* 
 *    (c) Jens Hermans, 2004
 */
 
#include <fuse.h>
#include "bstrlib.h"
#define CFS_INTERNAL
#include "cfs.h"
#include <dirent.h> 
#include <sys/stat.h>
#include <errno.h>
 
static int cfs_fuse_statfs(struct fuse_statfs *fst){
    fst->block_size = 1024;
    fst->blocks = 0;
    fst->blocks_free= 0;
    fst->files = 0;
    fst->files_free = 1000;
    fst->namelen = 512;
    return 0;
}

static int cfs_fuse_getattr(const char * path, struct stat *st){
    bstring q;
    cfile fd;
    
    memset(st, 0, sizeof(struct stat));
    if((fd = cnf_find_entry(rootnode, path)) == NULL){
	return -ENOENT;
    }
    st->st_uid  = 0;
    st->st_gid  = 0;
    st->st_size = blen(_c_read(fd));
    if(fd->cnfnode->first_child){
        st->st_mode= S_IFDIR | S_IWUSR | S_IRUSR | S_IXUSR;
    } else {
	st->st_mode= S_IFREG | S_IWUSR | S_IRUSR;
    }
    bdestroy(q);
    return 0;
}

static int cfs_fuse_getdir(const char * path, fuse_dirh_t h, fuse_dirfil_t filler){
    cfile p;
    int i, res = 0;
    char tmpdir[2];
    cfile cur;
    
    p = cnf_find_entry(rootnode, path);
    if(p == NULL){
	return -ENOENT;
    }
    cur = p->cnfnode->first_child;
    
    while(cur){
	if(cur->first_child)
	    res = filler(h,q->data, DT_DIR);
	else
	    res = filler(h,q->data, DT_REG);
	if(res != 0)
	    break;
	cur = cur->next;
    }
    return res;
}

static int cfs_fuse_open(const char * path, int flags){
    if((cnf_find_entry(rootnode, path)) == NULL){
	return -ENOENT;
    }
    bdestroy(q);
    return 0;
}

static int cfs_fuse_utime(const char * path, struct utimbuf *buf){
    return 0;
}

static int cfs_fuse_readlink(const char *path, char *buf, size_t size){
    return -1;
}

static int cfs_fuse_mknod(const char *path, mode_t mode, dev_t rdev){
    cfile fd;

    if(cnd_find_node(rootnode, path)->cnfnode){
	return 0;
    }
    cnf_add_branch(rootnode,path,0);
    return 0;
}

static int cfs_fuse_mkdir(const char *path, mode_t mode){
    cfile fd;

    if(cnd_find_node(rootnode, path)->cnfnode){
	return 0;
    }
    cnf_add_branch(rootnode,path,0);
    return 0;
}

static int cfs_fuse_symlink(const char *path, const char *to){
    return -EPERM;
}

static int cfs_fuse_unlink(const char *path){
    bstring q;
    int ret;
    
    ret = c_rm(q=cstr2bstr(path));
    bdestroy(q);
    return ret;
}

static int cfs_fuse_rmdir(const char *path){
    bstring q;
    int ret;
    
    ret = c_rm(q=cstr2bstr(path));
    bdestroy(q);
    return ret;
}

static int cfs_fuse_link(const char *from, const char *to){
    return -EPERM;
}

static int cfs_fuse_rename(const char *from, const char *to){
    bstring f;
    bstring t;
    
    if(c_rename(f=cstr2bstr(from), t=cstr2bstr(to)) != 0){
	bdestroy(f);
	bdestroy(t);
	return -EIO;
    }
    return 0;
}

static int cfs_fuse_chown(const char *path, uid_t uid, gid_t gid){
    return 0;
}

static int cfs_fuse_chmod(const char *path, mode_t mode){
    return 0;
}

static int cfs_fuse_truncate(const char *path, off_t size){
    cfile fd;
    bstring p;
    bstring olddata;
    bstring newdata;
    
    
    if((fd = c_open(p = cstr2bstr(path))) == NULL){
	bdestroy(p);
	return -ENOENT;
    }
    
    //olddata can be NULL
    if((olddata = c_read(fd)) == NULL){
	newdata = cstr2bstr("\0");
	balloc(newdata, size);
	c_write(fd, newdata);
	bdestroy(newdata);
	return 0;
    }

    //nothing should really happen here, since \0's don't have a meaning and the file is dynamicly resized on write
    if(blen(olddata) <= size){
	return 0;
    }
    
    newdata = bmidstr(olddata,0,size);
    c_write(fd, newdata);
    bdestroy(newdata);
    bdestroy(olddata);
    return 0;
}

static int cfs_fuse_read(const char *path, char *buf, size_t size, off_t offset){
    cfile fd;
    bstring data;
    bstring p;
    int retlen;
    char *ret;
    
    fd = c_open(p=cstr2bstr(path));
    bdestroy(p);
    if(!fd)
	return -ENOENT;
	
    p = c_read(fd);
    if(!p)
	return -EIO;
	
    data = bmidstr(p, offset, size);
    retlen = blen(data);

    ret = bstr2cstr(data, '_');
    memcpy(buf,ret, retlen);
    buf[retlen] = '\0';
    bdestroy(data);
    free(ret);
    return size;
}

static int cfs_fuse_write(const char *path, const char *buf, size_t size, off_t offset){
    cfile fd;
    bstring data, newdata;
    bstring p;
    int retlen;
    char *ret;
    
    fd = c_open(p=cstr2bstr(path));
    bdestroy(p);
    if(!fd)
	return -ENOENT;
	
    data = c_read(fd);
    if(!data)
	return -EIO;
    
    newdata = blk2bstr(buf, size);
    bsetstr(data, offset, newdata, '\0');    
    c_write(fd,data);
    bdestroy(data);
    bdestroy(newdata);
    return size;
}

static int cfs_fuse_release(const char *path, int flags){
    return 0;
}

static int cfs_fuse_fsync(const char *path, int isdatasync){
    return 0;
}

static struct fuse_operations cfs_fuse_ops = {
    .statfs	= cfs_fuse_statfs,
    .getdir	= cfs_fuse_getdir,
    .open	= cfs_fuse_open,
    .getattr	= cfs_fuse_getattr,
    .utime	= cfs_fuse_utime,
    .readlink	= cfs_fuse_readlink,
    .mknod	= cfs_fuse_mknod,
    .mkdir	= cfs_fuse_mkdir,
    .symlink	= cfs_fuse_symlink,
    .unlink	= cfs_fuse_unlink,
    .rmdir	= cfs_fuse_rmdir,
    .rename	= cfs_fuse_rename,
    .link	= cfs_fuse_link,
    .chmod	= cfs_fuse_chmod,
    .chown	= cfs_fuse_chown,
    .truncate	= cfs_fuse_truncate,
    .utime	= cfs_fuse_utime,
    .read	= cfs_fuse_read,
    .write	= cfs_fuse_write,
    .release	= cfs_fuse_release
//    .fsync	= cfs_fuse_fsync    
};

int cfs_fuse_main(int argc, char ** argv, cnfmodule * cmod){
    FILE * f;
    f = fopen("./test.cfg","r");
    rootnode = cmod->parse(f);
    fuse_main(argc, argv, &cfs_fuse_ops);
    return 0;
}
