#ifndef PMA_ERR_H
#define PMA_ERR_H

#define SUCCESS		0	// No error
#define ENORMERR	-1	// normal error

#define	ENOENT		2	/* No such file or directory */
#define	EIO			5	/* I/O error */
#define	ENOMEM		12	/* Out of memory */
#define	EACCES		13	/* Permission denied */
#define	EFAULT		14	/* Bad address */
#define	EEXIST		17	/* File exists */
#define	ENODEV		19	/* No such device */
#define	EINVAL		22	/* Invalid argument */
#define	ENFILE		23	/* File table overflow */


//DEBUG LEVEL
#define LEVEL_ERR		0 //all errors are printed
#define LEVEL_WARNING	1 //all warnings are printed
#define LEVEL_INFO		2 //all information are printed

#define PTR_ERR		((void *)-1) //error pointer

#define NO_ERR 0
#define NO_MEM_ERR -1
#define NOT_FOUND_ERR -2
#define ALREADY_EXISTS_ERR -3
#define N0_DEPENDENCE_ERR -4
#define WR_FROMAT_ERR -5
#define CONTAINER_EMPTY_ERR -6
#define ASSERT_ERR -7
#define ILLEGAL_ARGS_ERR -8
#define IO_ERR -9

#define SERIOUS_ERR -255
#endif
