// Gerald Arocena
// CSCI E-28, Spring 2020
// 3-14-2020
// hw3

/* *
 * sttyl.c
 * Implements some of the Unix stty command. If no arguments are given, then
 * some settings for stdin are displayed. If arguments are given then the
 * settings for the corresponding arguments are changed in stdin. The following
 * settings can be modified: erase, kill, intr, icrnl, hupcl, echo, echoe,
 * opost, icanon, and isig 
 * note: referenced "showtty.c" from lecture 5 
 * usage: ./sttyl [SETTING]...
 */

#include    <stdio.h>
#include    <termios.h>
#include    <stdlib.h>
#include    <sys/ioctl.h>
#include    <fcntl.h>
#include    <unistd.h>
#include    <string.h>
#include    <stdbool.h>
#include    <ctype.h>
#include    <errno.h>

struct flagInfo { tcflag_t fl_value;              // to hold settings' flag info
                  tcflag_t *field;
                  char *fl_name; };
struct charInfo { char *settingName;              // to hold settings' char info
                  cc_t *settingChar; };
                  
static struct termios ttyinfo;	                  // to hold tty info	                  

/* *
 * Table to define/manage tty settings and associated flags
 */ 
static struct flagInfo flags[] = {		
    { ICRNL,  &ttyinfo.c_iflag,	"icrnl"  },
    { HUPCL,  &ttyinfo.c_cflag, "hupcl"  },
    { ECHO,   &ttyinfo.c_lflag, "echo"   },
    { ECHOE,  &ttyinfo.c_lflag,	"echoe"  },    
    { OPOST,  &ttyinfo.c_oflag, "opost"  },
    { ICANON, &ttyinfo.c_lflag, "icanon" },  
    { ISIG,   &ttyinfo.c_lflag, "isig"   },    
    { 0,      NULL,             NULL     }
};

/* *
 * Table to define/manage tty settings and their corresponding character keys
 */ 
static struct charInfo settingChars[] = {
    { "intr",  &ttyinfo.c_cc[VINTR]  },
    { "erase", &ttyinfo.c_cc[VERASE] },
    { "kill",  &ttyinfo.c_cc[VKILL]  },
    { NULL,    0                     }
};

/* *
 * main( int ac, char *av[] )
 * purpose: handles command line arguments. If no args given, then
 *          displaySettings() is called. Otherwise, checkSettingChars() or
 *          checkFlags() changes the given settings. Settings are retrieved and
 *          set with tcgetattr() and tcsetattr() respectively
 * args: the number of command line arguments and the arguments themselves
 * rets: exit status code
 */ 
int main( int ac, char *av[] )
{    
    void    displaySettings();
    bool    checkSettingChars( int *, char **[] ),  
            checkFlags();
    
    if ( tcgetattr( 0 , &ttyinfo ) == -1 )    // get tty info on stdin
    {
        perror( "could not get terminal parameters" ); 
	exit(1);
    }  

    if ( ac == 1 )                            // if no arguments entered
        displaySettings();   

    while (--ac)                              // handle any arguments one by one
    {
        av++;       
        
        if ( !checkSettingChars( &ac, &av ) && !checkFlags( *av ) ) {
            fprintf( stderr, "sttyl: invalid argument '%s'\n", *av );
            exit(1);
        }            
    }

    if ( tcsetattr( 0, TCSANOW, &ttyinfo ) == -1 ) // set settings & any changes
        fprintf( stderr, "sttyl: settings not modified: %s\n"
                    , strerror( errno ) );
	
    return 0;
}

/* *
 * displaySettings()
 * purpose: displays some terminal settings via utility functions
 * args: none
 * rets: none
 */
void displaySettings()
{
    void    showBaud( int ),
            showWinSize(),
            showOtherSettings(),
            showFlagset();
    
    int speed = cfgetospeed( &ttyinfo );
    if (speed != -1 )
        showBaud( speed );	                  // get and show baud rate
    else
        fprintf( stderr, "sttyl: could not show baud speed: %s\n"
                    , strerror( errno ) );
    showWinSize();                            // prints rows and cols
    showOtherSettings();                      // show some other settings
    showFlagset();                            // show flag info  
}

/* *
 * showBaud( int thespeed )
 * purpose: utility function for displaySettings(). Prints the baud rate speed
 *          in English to stdout
 * args: the output baud rate speed
 * rets: none
 */
void showBaud( int thespeed )
{
    printf("speed ");
	switch ( thespeed ) {
	case B0:      printf("0");      break;
	case B50:     printf("50");     break;
	case B75:     printf("75");     break;
	case B110:    printf("110");	break;
	case B134:    printf("134");	break;
	case B150:    printf("150");	break;
	case B200:    printf("200");	break;
	case B300:    printf("300");	break;
	case B600:    printf("600");	break;
	case B1200:   printf("1200");   break;
	case B1800:   printf("1800");   break;
	case B2400:   printf("2400");   break;
	case B4800:   printf("4800");   break;
	case B9600:   printf("9600");   break;
	case B19200:  printf("19200");	break;
	case B38400:  printf("38400");	break;
	case B57600:  printf("57600");	break;
	case B115200: printf("115200"); break;
	case B230400: printf("230400"); break;
	default:      printf("Fast");   break;
	}
    printf(" baud; ");
}

/* *
 * showWinSize()
 * purpose: utility function for displaySettings(). Gets and prints the terminal
 *          window's dimensions to stdout
 * args: none
 * rets: none
 * note: referenced "set_term_dims.c" from lecture 5
 */
void showWinSize()
{
    int	     fd,
             rv;
    struct   winsize w;	                  // see termios.h for more info   
    
    if ( (fd = open( "/dev/tty", O_RDONLY )) == -1 )  // open terminal
    {
	perror("/dev/tty");
	exit(1);
    }
    
    rv = ioctl(fd, TIOCGWINSZ, &w);           // puts terminal dimensions into w
    if ( rv == -1 )
    {
	perror("/dev/tty");
	exit(1);
    }

    printf("rows %d; cols %d; ", w.ws_row, w.ws_col);
    
    if ( close(fd) == -1 )    
        perror("/dev/tty");    
}

/* *
 * showOtherSettings()
 * purpose: utility function for displaySettings(). Prints some terminal
 *          settings to stdout
 * args: none
 * rets: none
 */
void showOtherSettings()
{
    int deleteASCII = 127;

    for (int i = 0; settingChars[i].settingName != NULL; i++)  // traverse table
    {
        if ( *settingChars[i].settingChar == deleteASCII )     // if del char
            printf( "%s = ^%c; ", settingChars[i].settingName, '?' );        

        else if ( *settingChars[i].settingChar == _POSIX_VDISABLE )  // if undef
            printf( "%s = <undef>; ", settingChars[i].settingName );
        
        else if ( iscntrl( *settingChars[i].settingChar ) )    // if cntrl char
            printf( "%s = ^%c; ", settingChars[i].settingName,
                *settingChars[i].settingChar + 'A' - 1 );    

        else                                                   // default
            printf( "%s = %c; ", settingChars[i].settingName,
                *settingChars[i].settingChar );        
    }

    printf("\n");
}

/* *
 * showFlagset( struct flagInfo flags[] )
 * purpose: utility function for displaySettings(). Prints if setting is on or
 *          off
 * args: the table of settings/flags to show
 * rets: none
 */
void showFlagset()
{	
    for ( int i = 0; flags[i].fl_value != 0 ; i++ )  // traverse flags table
    {
	if ( *flags[i].field & flags[i].fl_value )   // if setting is on
	    printf("%s ", flags[i].fl_name);
		
        else                                         // if setting is off
	    printf("-%s ", flags[i].fl_name);
    }

    printf("\n");
}

/* *
 * checkSettingChars( int *ac, char **av[] )
 * purpose: changes the execution character of a setting. Checks first if the
 *          args given are valid 
 * args: the number of command line arguments and the arguments themselves
 * rets: if a setting was changed (true or false)
 */ 
bool checkSettingChars( int *ac, char **av[] )
{
    for (int i = 0; settingChars[i].settingName != NULL; i++)  // traverse table
        
        if ( strcmp( **av, settingChars[i].settingName ) == 0 )   // matches arg
        {
            if ( *ac > 1 )                            // if there's another arg
            {
                if ( strlen( *++*av ) == 1 ) {        // if next arg is a char
                    *settingChars[i].settingChar = **av[0];       // change char                  
                    (*ac)--;                   
                    return true;
                }
                else if ( strcmp( "undef", **av ) == 0 ) {  // if next arg undef
                    *settingChars[i].settingChar = _POSIX_VDISABLE;  // chg char                  
                    (*ac)--;                   
                    return true;
                }
                else {                                // if next arg is invalid
                    fprintf( stderr, "sttyl: invalid integer argument: %s\n"
                            , **av );
                    exit(1);
                }
            }
            else                                      // if no other args follow
            {
                fprintf( stderr, "sttyl: missing argument to '%s'\n", **av );
                exit(1);
            }                
        }

    return false;
}

/* *
 * checkFlags( char *av )
 * purpose: turns a terminal setting on or off
 * args: name of setting
 * rets: if a setting was modified (true or false)
 */
bool checkFlags( char *av )
{    
    for ( int i = 0; flags[i].fl_value != 0 ; i++ )      // traverse flags table
    {
        // if matches [-]/'off' flag
        if ( av[0] == '-' && strcmp( &av[1], flags[i].fl_name ) == 0 )
        {
            *flags[i].field &= ~flags[i].fl_value;       // turn off setting
            return true;
        }

        else if ( strcmp( av, flags[i].fl_name ) == 0 )  // if matches 'on' flag 
        {
            *flags[i].field |= flags[i].fl_value;        // turn on setting
            return true;
        }
    }

    return false;
}

