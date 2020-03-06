// Gerald Arocena
// CSCI E-28, Spring 2020
// hw3

/* sttyl.c
 * TODO: description
 * note: referenced "showtty.c" from lecture 5
 */

#include	<stdio.h>
#include	<termios.h>
#include	<stdlib.h>
#include    <sys/ioctl.h>
#include    <fcntl.h>
#include    <unistd.h>

struct flaginfo { tcflag_t fl_value; char	*fl_name; };

void handleArgs();
void showBaud( int thespeed );
void showWinSize();
void showOtherSettings( struct termios *ttyp );
void showSomeFlags( struct termios *ttyp );

int main()
{    
	struct termios ttyinfo, orig;	                         // to hold tty info						
	if ( tcgetattr( 0 , &ttyinfo ) == -1 ) {            // get tty info on stdin
		perror( "could not get terminal parameters" );
		exit(1);
	}    
    orig = ttyinfo;                                    // save original tty info
	
    // TODO
    handleArgs();
    
    showBaud( cfgetospeed( &ttyinfo ) );	           // get and show baud rate

    showWinSize();                                       // prints rows and cols

    showOtherSettings( &ttyinfo );

    showSomeFlags( &ttyinfo );                                 // show flag info		   

    // TODO
    ttyinfo.c_cc[VKILL] = 'd';
    tcsetattr(0,TCSANOW, &ttyinfo);
    printf("kill = ^%c; \n", ttyinfo.c_cc[VKILL] );

    // TODO: necessary? -- return to original settings
    tcsetattr(0,TCSANOW, &orig);

	return 0;
}

void handleArgs()
{

}

/*
 *	prints the speed in english
 */
void showBaud( int thespeed )
{
	printf("speed ");
	switch ( thespeed ) {
        case B0: printf("0");	        break;
        case B50: printf("50");         break;
        case B75: printf("75");         break;
        case B110: printf("110");	    break;
        case B134: printf("134");	    break;
        case B150: printf("150");	    break;
        case B200: printf("200");	    break;
		case B300: printf("300");	    break;
		case B600: printf("600");	    break;
		case B1200: printf("1200");     break;
		case B1800:	printf("1800");     break;
		case B2400:	printf("2400");     break;
		case B4800:	printf("4800");     break;
		case B9600:	printf("9600");     break;
        case B19200: printf("19200");	break;
        case B38400: printf("38400");	break;
        case B57600: printf("57600");	break;
        case B115200: printf("115200"); break;
        case B230400: printf("230400"); break;
		default: printf("Fast");        break;
	}
    printf(" baud; ");
}

/*
 *
 * note: referenced "set_term_dims.c" from lecture 5
 */
void showWinSize()
{
    int	fd;
	struct winsize w;	
	int	rv;    
    if ( (fd = open( "/dev/tty", O_RDONLY )) == -1 ) {
		perror("/dev/tty");
		exit(1);
	}
    rv = ioctl(fd, TIOCGWINSZ, &w);
    if ( rv == -1 ) {
		perror("/dev/tty");
		exit(1);
	}
	printf("rows %d; cols %d; ", w.ws_row, w.ws_col);
    if ( close(fd) == -1 ) {
        perror("/dev/tty");
    }
}

void showOtherSettings( struct termios *ttyinfo ) {
    printf("intr = ^%c; ", ttyinfo->c_cc[VINTR] + 'A' - 1);
    printf("erase = ^%c; ", ttyinfo->c_cc[VERASE] + 'A' - 1);
    printf("kill = ^%c; ", ttyinfo->c_cc[VKILL] + 'A' - 1);
    printf("start = ^%c; ", ttyinfo->c_cc[VSTART] + 'A' - 1);
    printf("stop = ^%c; ", ttyinfo->c_cc[VSTOP] + 'A' - 1);
    printf("werase = ^%c; ", ttyinfo->c_cc[VWERASE] + 'A' - 1);
    printf("\n");
}

void showSomeFlags( struct termios * ttyp )
{
    void showFlagset( int thevalue, struct flaginfo thebitnames[] );

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

    showFlagset( ttyp->c_iflag, input_flags );
	showFlagset( ttyp->c_lflag, local_flags );
    showFlagset( ttyp->c_cflag, controlFlags );
    showFlagset( ttyp->c_oflag, outputFlags );
    printf("\n");
}

/*
 * 
 */
void showFlagset( int thevalue, struct flaginfo thebitnames[] )
{	
	for ( int i = 0; thebitnames[i].fl_value != 0 ; i++ ) {
		if ( thevalue & thebitnames[i].fl_value )
			printf("%s ", thebitnames[i].fl_name);
		else
			printf("-%s ", thebitnames[i].fl_name);
	}
}