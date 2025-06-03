#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <string.h>
#include <unistd.h>

#define DEVICE "/dev/lcd_ioctl"

#define LCD_IOCTL_MAGIC  'M'
#define LCD_IOCTL_LINE1  _IOW(LCD_IOCTL_MAGIC, 1, char *)
#define LCD_IOCTL_LINE2  _IOW(LCD_IOCTL_MAGIC, 2, char *)

int main()
{
    int fd;
    char msg1[17] ;
    char msg2[17] ;

    fd = open(DEVICE, O_RDWR);
    if (fd < 0) {
	perror("open");
	return 1;
    }
    while(1)
    {
	printf("\nEnter Line1 for LCD (max 16 chars) : ");
	scanf(" %[^\n]",msg1);
	printf("Enter Line2 for LCD (max 16 chars): ");
	scanf(" %[^\n]",msg2);

	if (ioctl(fd, LCD_IOCTL_LINE1, msg1) < 0) {
	    perror("ioctl LINE1");
	}

	if (ioctl(fd, LCD_IOCTL_LINE2, msg2) < 0) {
	    perror("ioctl LINE2");
	}
    }

    close(fd);
    return 0;
}

