#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>

int drop_privs( char * name )
{
	struct passwd * pw = NULL;

	if ( name == NULL )
	{
		exit(-1);
	}

	pw = getpwnam( name );

	if ( pw == NULL )
	{
		exit(-1);
	}

	if ( init_user( pw ) != 0 )
	{
		exit(-1);
	}

	return 0;
}

int init_user( struct passwd * pw )
{
	uid_t procuid = 0;
	gid_t procgid = 0;

	if (pw == NULL )
	{
		return -1;
	}

	procuid = getuid( );
	procgid = getgid( );

	if ( initgroups( pw->pw_name, pw->pw_gid ) != 0 )
	{
		return -1;
	}

	if ( setresgid( pw->pw_gid, pw->pw_gid, pw->pw_gid ) != 0 )
	{
		printf("setresgid failed\n");
		return -1;
	}

	if ( setresuid( pw->pw_uid, pw->pw_uid, pw->pw_uid ) != 0 )
	{
		printf("setresuid failed\n");
		return -1;
	}

	if ( procgid != pw->pw_gid )
	{
		if ( setgid( procgid ) != -1 )
		{
			printf("setgid failed\n");
			return -1;
		}

		if ( setegid( procgid ) != -1 )
		{
			printf("setegid failed\n");
			return -1;
		}
	}

	if ( procuid != pw->pw_uid )
	{
		if ( setuid( procuid ) != -1 )
		{
			printf("setuid failed\n");
			return -1;
		}

		if ( seteuid( procuid ) != -1 )
		{
			printf("seteuid failed\n");
			return -1;
		}
	}

	if ( getgid( ) != pw->pw_gid )
	{
		return -1;
	}

	if ( getegid( ) != pw->pw_gid )
	{
		return -1;
	}

	if ( getuid( ) != pw->pw_uid )
	{
		return -1;
	}

	if ( geteuid( ) != pw->pw_uid )
	{
		return -1;
	}

	if ( chdir( pw->pw_dir ) != 0 )
	{
		printf("chdir failed\n");
		return -1;
	}

	return 0;
}

void sendit(int sock, char *line) {
	write(sock, line, strlen(line));
}

void read_until(int sock, char *buffer, int max) {
	int i = 0;
	char local_char[2];
	int n;

	while ((n = read(sock,local_char,1)) == 1) {
		if (local_char[0] == '\n' || i == max) {
			break;
		}
		buffer[i++] = local_char[0];
	}
	buffer[i] = '\0';

	#ifdef MY64
	asm("mov    %rsi,%rdi");
	asm("ret");
	#endif

}

void handle_client(void) {
	char buf1[2048];
	int done = 0;

	while (!done) {
		if (fgets(buf1, 2304, stdin) == NULL) {
			return;
		}

		// make sure there are no %n's anywhere in the text...no 'n' characaters at all
		if (strchr(buf1, 'n')) {
			printf("I don't think so...\n");
			continue;
		}
			
		// printf vuln
		printf(buf1);
		fflush(stdout);
	}
}

void main(void) {
	drop_privs("fmtstr");
	handle_client();
}

