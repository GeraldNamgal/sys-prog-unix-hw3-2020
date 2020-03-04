// https://cscie28.dce.harvard.edu/~lib215/lectures/lect_p/lect05/5_Code/showtty.c

#include	<stdio.h>
#include	<termios.h>
#include	<stdlib.h>
#include    <sys/ioctl.h>
#include    <fcntl.h>
#include    <unistd.h>

void showbaud( int thespeed );
void show_some_flags( struct termios *ttyp );

int main()
{    
	struct termios ttyinfo;	                            // to hold tty info						
	if ( tcgetattr( 0 , &ttyinfo ) == -1 ) {            // get tty info on stdin
		perror( "could not get terminal parameters" );
		exit(1);
	}						
	showbaud( cfgetospeed( &ttyinfo ) );	           // get and show baud rate

    // https://cscie28.dce.harvard.edu/~lib215/lectures/lect_p/lect05/5_Code/set_term_dims.c
    int	fd;
	struct winsize w;	
	int	rv;    
    if ( (fd = open( "/dev/tty", O_RDONLY )) == -1 ) {
		perror("/dev/tty");
		return 1;
	}
    rv = ioctl(fd, TIOCGWINSZ, &w);
    if ( rv == -1 ) {
		perror("/dev/tty");
		return 1;
	}
	printf("rows %d; cols %d; ", w.ws_row, w.ws_col);
    if ( close(fd) == -1 ) {
        perror("/dev/tty");
    }
    printf("intr = ^%c; ", ttyinfo.c_cc[VINTR] + 'A' - 1);
    printf("erase = ^%c; ", ttyinfo.c_cc[VERASE] + 'A' - 1);
    printf("kill = ^%c; ", ttyinfo.c_cc[VKILL] + 'A' - 1);
    printf("start = ^%c; ", ttyinfo.c_cc[VSTART] + 'A' - 1);
    printf("stop = ^%c; ", ttyinfo.c_cc[VSTOP] + 'A' - 1);
    printf("werase = ^%c; ", ttyinfo.c_cc[VWERASE] + 'A' - 1);
    printf("\n");

	show_some_flags( &ttyinfo );		   // show settings from hw list 
    printf("\n");

	return 0;
}

void showbaud( int thespeed )
/*
 *	prints the speed in english
 */
{
	printf("speed ");
	switch ( thespeed ){
        case B0: printf("0");	break;
        case B50: printf("50"); break;
        case B75: printf("75"); break;
        case B110: printf("110");	break;
        case B134: printf("134");	break;
        case B150: printf("150");	break;
        case B200: printf("200");	break;
		case B300: printf("300");	break;
		case B600: printf("600");	break;
		case B1200: printf("1200"); break;
		case B1800:	printf("1800"); break;
		case B2400:	printf("2400"); break;
		case B4800:	printf("4800"); break;
		case B9600:	printf("9600"); break;
        case B19200: printf("19200");	break;
        case B38400: printf("38400");	break;
        case B57600: printf("57600");	break;
        case B115200: printf("115200"); break;
        case B230400: printf("230400"); break;
		default: printf("Fast"); break;
	}
    printf(" baud; ");
}

struct flaginfo { tcflag_t fl_value; char	*fl_name; };

static struct flaginfo input_flags[] = {		
		{ ICRNL  ,	"icrnl" },
		{ 0      ,	NULL 	}
};

static struct flaginfo controlFlags[] = {
        { HUPCL , "hupcl" },
        { 0, NULL }
};

static struct flaginfo outputFlags[] = {
        { OPOST , "opost" },
        { 0, NULL }
};

static struct flaginfo local_flags[] = {
		{ ISIG   ,	"isig" },
		{ ICANON ,	"icanon" } , 
		{ ECHO   ,	"echo" },
		{ ECHOE  ,	"echoe" }, 
		{ 0	,	NULL }
};

void show_flagset( int thevalue, struct flaginfo thebitnames[] );

void show_some_flags( struct termios *ttyp )
/*
 *	show the values of two of the flag sets_: c_iflag and c_lflag
 *	adding c_oflag and c_cflag is pretty routine - just add new
 *	tables above and a bit more code below.
 */
{
	show_flagset( ttyp->c_iflag, input_flags );
	show_flagset( ttyp->c_lflag, local_flags );
    show_flagset( ttyp->c_cflag, controlFlags );
    show_flagset( ttyp->c_oflag, outputFlags );
}

void show_flagset( int thevalue, struct flaginfo thebitnames[] )
/*
 * check each bit pattern and display descriptive title
 */
{
	int	i;
	
	for ( i=0; thebitnames[i].fl_value != 0 ; i++ ) {
		if ( thevalue & thebitnames[i].fl_value )
			printf("%s ", thebitnames[i].fl_name);
		else
			printf("-%s ", thebitnames[i].fl_name);
	}
}