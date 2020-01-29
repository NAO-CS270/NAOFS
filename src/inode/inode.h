#include<time.h>
#include<stdio.h>

#include "../dsk/mdisk.h"

#define BLOCK_PTRS_PER_INODE_STRUCT 13

/**
 * Bit masks for the type of file whose data is stored in the inode.
 * To test for a regular file:
 *  stat(pathname, &sb);
 *      if ((sb.st_mode & S_IFMT) == S_IFREG) {
 *             // Handle regular file
 * }
 * POSIX refers to the stat.st_mode bits corresponding to the mask
 * S_IFMT (see below) as the file type, the 12 bits corresponding to the
 * mask 07777 as the file mode bits and the least significant 9 bits
 * (0777) as the file permission bits.
 */
typedef enum INodeType {
    S_IFMT = 0170000,   // file type
    S_IFSOCK = 0140000, // socket
    S_IFLNK = 0120000,  // symbolic link
    S_IFREG = 0100000,  // regular file
    S_IFBLK = 0060000,  // block device
    S_IFDIR = 0040000,  // directory
    S_IFCHR = 0020000,  // character device
    S_IFIFO = 0010000,  // FIFO
} INodeType;


// for more details, refer - http://man7.org/linux/man-pages/man7/inode.7.html
typedef enum INodeMode {
    S_ISUID = 04000,  // set-user-ID bit (see execve(2))
    S_ISGID = 02000,  // set-group-ID bit
    S_ISVTX = 01000,  // sticky bit

    S_IRWXU = 00700,  // owner has read, write, and execute permission
    S_IRUSR = 00400,  // owner has read permission
    S_IWUSR = 00200,  // owner has write permission
    S_IXUSR = 00100,  // owner has execute permission

    S_IRWXG = 00070,  // group has read, write, and execute permission
    S_IRGRP = 00040,  // group has read permission
    S_IWGRP = 00020,  // group has write permission
    S_IXGRP = 00010,  // group has execute permission

    S_IRWXO = 00007,  // others (not in group) have read, write, and execute permission
    S_IROTH = 00004,  // others have read permission
    S_IWOTH = 00002,  // others have write permission
    S_IXOTH = 00001,  // others have execute permission
} INodeMode;


typedef struct INode {
    // assuming there are more than 1 file systems
    uint32_t device_number;

    // unique to each filesystem
    uint32_t inode_number;

    // file/directory/symlink
    INodeType type;

    // number of hard links to the file
    uint32_t nlink;

    uint32_t owner_uid;

    // ID of the group owner of the file
    uint32_t gid;

    // size of the file in bytes
    uint32_t size;

    // number of blocks allocated to the file
    uint32_t num_blocks;

    // inode allocated/needs to be written
    uint16_t status;

    // pointers to disk blocks containing data
    disk_block data_blocks[BLOCK_PTRS_PER_INODE_STRUCT];

    // file's last access timestamp
    time_t access_time;

    // file creation time(not returned in the inode stat struct)
    time_t birth_time;

    // file's last modified timestamp(timestamp of a directory is changed by the creation
    // or deletion of files in that directory)
    time_t modified_time;

    // permissions for the file
    INodeMode mode;
} INode;

