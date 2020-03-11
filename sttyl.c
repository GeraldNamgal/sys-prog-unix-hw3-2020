// Gerald Arocena
// CSCI E-28, Spring 2020
// hw3

/* sttyl.c
 * TODO: description stuff, etc.
 * note: referenced "showtty.c" from lecture 5
 */

#include	<stdio.h>
#include	<termios.h>
#include	<stdlib.h>
#include    <sys/ioctl.h>
#include    <fcntl.h>
#include    <unistd.h>
#include    <string.h>
#include    <stdbool.h>

#define MAX_STR_SIZE 40

struct flaginfo { tcflag_t fl_value;              // to hold settings' flag info
                  tcflag_t *field;
                  char *fl_name; };
struct charInfo { char *settingName;              // to hold settings' char info
                  cc_t *settingChar; };
static struct termios ttyinfo;	                             // to hold tty info	                  

// TODO: double check that flags match, e.g., iflag vs cflag
/* Table to define/manage tty settings and associated flags
 */ 
static struct flaginfo flags[] = {		
    { ICRNL, &ttyinfo.c_iflag,	"icrnl" },
    { HUPCL, &ttyinfo.c_cflag, "hupcl" },
    { ECHO, &ttyinfo.c_lflag, "echo" },
    { ECHOE, &ttyinfo.c_lflag,	"echoe" },    
    { OPOST, &ttyinfo.c_oflag, "opost" },
    { ICANON, &ttyinfo.c_lflag, "icanon" },  
    { ISIG, &ttyinfo.c_lflag, "isig" },    
    { 0, NULL, NULL }
};

/* Table to define/manage tty settings and their corresponding character keys
 */ 
static struct charInfo settingChars[] = {
    { "intr", &ttyinfo.c_cc[VINTR] },
    { "erase", &ttyinfo.c_cc[VERASE] },
    { "kill", &ttyinfo.c_cc[VKILL] },
    { NULL, 0 }
};

/* main( int ac, char *av[] )
 *
 */ 
int main( int ac, char *av[] )
{    
	void displaySettings();
    bool checkSettingChars( int *, char **[] );  
    bool checkFlags();
    
    if ( tcgetattr( 0 , &ttyinfo ) == -1 )              // get tty info on stdin
	{
        perror( "could not get terminal parameters" ); 
		exit(1);
	}  

    if ( ac == 1 )                                    // if no arguments entered
        displaySettings();   

    while (--ac)                              // handle any arguments one by one
    {
        av++;       
        
        if ( !checkSettingChars( &ac, &av ) && !checkFlags( *av ) )
        {
            fprintf( stderr, "sttyl: invalid argument '%s'\n", *av );
            exit(1);
        }            
    }

    tcsetattr(0,TCSANOW, &ttyinfo);                              // set settings

	return 0;
}

/* displaySettings()
 *
 */
void displaySettings()
{
    void showBaud( int );
    void showWinSize();
    void showOtherSettings();
    void showFlagset( struct flaginfo [] );
    
    showBaud( cfgetospeed( &ttyinfo ) );	       // get and show baud rate
    showWinSize();                                   // prints rows and cols
    showOtherSettings();                         // show some other settings
    showFlagset( flags );                                  // show flag info  
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
    
    if ( (fd = open( "/dev/tty", O_RDONLY )) == -1 )
    {
		perror("/dev/tty");
		exit(1);
	}
    
    rv = ioctl(fd, TIOCGWINSZ, &w);
    if ( rv == -1 )
    {
		perror("/dev/tty");
		exit(1);
	}

	printf("rows %d; cols %d; ", w.ws_row, w.ws_col);
    
    if ( close(fd) == -1 )    
        perror("/dev/tty");    
}

/*
 *
 */
void showOtherSettings()
{
    for (int i = 0; settingChars[i].settingName != NULL; i++)
    {
        printf( "%s = ^%c; ", settingChars[i].settingName,
                *settingChars[i].settingChar + 'A' - 1 );
    }

    printf("\n");
}

/*
 * 
 */
void showFlagset( struct flaginfo flags[] )
{	
	for ( int i = 0; flags[i].fl_value != 0 ; i++ )
    {
		if ( *flags[i].field & flags[i].fl_value )
			printf("%s ", flags[i].fl_name);
		
        else
			printf("-%s ", flags[i].fl_name);
	}

    printf("\n");
}

/*
 *
 */ 
bool checkSettingChars( int *ac, char **av[] )
{
    for (int i = 0; settingChars[i].settingName != NULL; i++)
        
        if ( strcmp( **av, settingChars[i].settingName ) == 0 )
        {
            if ( *ac > 1 )
            {
                if ( strlen( *++*av ) == 1 )
                {
                    *settingChars[i].settingChar = **av[0];                    
                    (*ac)--;                   
                    return true;
                }
                else
                {
                    fprintf( stderr, "sttyl: invalid integer argument: %s\n"
                            , **av );
                    exit(1);
                }
            }
            else
            {
                fprintf( stderr, "sttyl: missing argument to '%s'\n", **av );
                exit(1);
            }                
        }

    return false;
}

bool checkFlags( char *av )
{
    for ( int i = 0; flags[i].fl_value != 0 ; i++ )
    {
		char strBuff[MAX_STR_SIZE] = "-";
        strcat( strBuff, flags[i].fl_name );
        
        if ( strcmp( av, strBuff ) == 0 )
        {           
            *flags[i].field &= ~flags[i].fl_value;
            return true;
        }

        else if ( strcmp( av, flags[i].fl_name ) == 0 )
        {
            *flags[i].field |= flags[i].fl_value;
            return true;
        }
	}

    return false;
}

