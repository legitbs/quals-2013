#include "sharedfuncs.h"
#include "hjalloc.h"

typedef struct Message_t
{
    uint64_t junk;
    int msg_id;
    char from[0x10];
    char *msg;
} mess, *pmess;

typedef struct User_t
{
    uint64_t junk;
    char name[0x10];
    char pass[0x10];
    uint32_t id;
    uint32_t msg_cnt;
    pmess msgs[0x10];
    char description[0x20];
} user, *puser;

typedef struct UserList_t
{
    uint64_t junk;
    struct UserList_t *next;
    puser user_s;		  
} ulist, *pulist;

int port = 3647;
int authd = 0;
int next_id = 1;
char *menu_main = "\n1) Login\n2) Create User\n3) List Users\n4) Exit\n\n-> ";
char *menu_authd = "\n1) Add Description\n2) Send message\n3) Read Messages\n4) Delete Messages\n5) Delete User\n6) Logout\n\n-> ";
pulist ulist_head;
puser cntxt = NULL;
pheap progheap = NULL;

int ff( int connfd );
int get_resp( int connfd );
int handle_main_menu( int connfd );
int handle_authd_menu( int connfd );
puser create_user( int connfd );
puser get_user_by_name( char *name );
int add_user( puser new_user );
int init_user_list( );
int list_users( int connfd);
int check_user_exists( char * name );
int handle_login( int connfd );
int delete_user( );
int free_user_messages( puser user_x );
int add_desc( int connfd );
int send_message( int connfd );
int read_messages( int connfd );
int delete_message( int connfd );
int send_message_list( int connfd, puser ctxt );

int main( int argc, char **argv )
{
		  int sockfd = SetupSock( port, AF_INET, "em0" );

		  accept_loop( sockfd, ff, "yolo" );

		  return 0;
}

int handle_authd_menu( int connfd )
{
    int retval = 0;

    retval = send_string( connfd, menu_authd );

    if ( retval == -1 )
    {
        return retval;
    }

    retval = get_resp( connfd );

    if ( retval < 1 || retval > 6 )
    {
        send_string( connfd, "Invalid response\n" );
        return -1;
    }

    switch ( retval )
    {
        case 1:
            add_desc( connfd );
            break;
        case 2:
            send_message( connfd );
            break;
        case 3:
            read_messages( connfd );
            break;
        case 4:
            delete_message( connfd );
            break;
        case 5:
            authd = 0;
            delete_user( );
            cntxt = NULL;
            send_string( connfd, "User deleted. Logged out\n\n");
            return -1;
            break;
        case 6:
            authd = 0;
            send_string( connfd, "Logout successful\n\n");
            cntxt = NULL;
            return -1;
            break;
        default:
            break;
    };

    return -1;
}

int send_message_list( int connfd, puser ctxt )
{
    int retval = 0;
    char header[0x30];
    int count = 0;

    while( count < 0x10 )
    {
        if ( ctxt->msgs[count] != NULL )
        {
            memset(header, 0x00, 0x30);
            snprintf( header, 0x30, "Msg Id: %d From: %s\n", count + 1, (ctxt->msgs[count]->from));
            retval = send_string( connfd, header );

            if ( retval == -1 )
            {
                return -1;
            }
        }

        count++;
    }

    return 0;
}

int delete_message( int connfd )
{
    char readbuff[0x10];
    int msg_id = 0;
    int retval = 0;

    if ( cntxt->msg_cnt == 0 )
    {
        send_string( connfd, "\nMailbox empty!!!\n");
        return 0;
    } else
    {
        send_message_list( connfd, cntxt );
    }
    while ( 1 )
    {
        retval = send_string( connfd, "\n\nMessage Id or \'q\' to return to previous menu: " );

        if ( retval == -1 )
        {
            return -1;
        }

        memset(readbuff, 0x00, 0x10);
        retval = recv_until( connfd, readbuff, 0x10, 0x0a );

        if ( retval == -1 )
        {
            return -1;
        }

        if ( readbuff[0] == 'q' )
        {
            break;
        }

        msg_id = atoi((const char*)readbuff);

        if ( msg_id < 1 || msg_id > 0x10 )
        {
            send_string( connfd, "Invalid message ID\n");
            return -1;
        }

        if ( cntxt->msgs[msg_id-1] == NULL )
        {
            send_string( connfd, "Invalid message ID\n");
        } else
        {
            hjfree( progheap, cntxt->msgs[msg_id-1]->msg, sizeof(user) );
            hjfree( progheap, cntxt->msgs[msg_id-1], sizeof(mess));
            cntxt->msgs[msg_id-1] = NULL;
            cntxt->msg_cnt--;
        }
    }

    return 0;
}

int check_len( int size, int *rez, char*data)
{
	int sz = 0;

	if (rez == NULL)
	{
		return -1;
	}

	if ( data == NULL )
	{
		return -1;
	}

	while( data[sz] != '\x00' && sz<=size)
	{
		sz++;
	}

	if ( sz > size )
	{
		*rez = -1;
	}

	*rez = 1;

	return 1;
}
	 
int read_messages( int connfd )
{
    char data[0x100];
    int msg_id = 2;
    int retval = 0;

    if ( cntxt->msg_cnt == 0 )
    {
        send_string( connfd, "\nMailbox empty!!!\n");
        return 0;
    } else
    {
        send_message_list( connfd, cntxt );
    }

    while ( 1 )
    {
        if ( send_string( connfd, "\n\nMessage Id or \'q\' to return to previous menu: " ) == -1)
        {
            return -1;
        }

        memset(data, 0x00, 0x10);
        if ( recv_until( connfd, data, 0x10, 0x0a ) == -1 )
        {
            return -1;
        }

        if ( data[0] == 'q' )
        {
            break;
        }

	check_len( 2, &retval, data);

	if (retval)
	{ 
        	msg_id = atoi((const char *)data);
	} else
	{
		send_string( connfd, "Invalid response\n");
		return -1;
	}

        if ( msg_id < 1 || msg_id > 0x10 )
        {
            send_string( connfd, "Invalid message ID\n");
            return -1;
        }

        if ( cntxt->msgs[msg_id-1] == NULL )
        {
            send_string( connfd, "Invalid message ID\n");
        } else
        {
            memset(data, 0x00, 0xe0);
            memcpy( data, cntxt->msgs[msg_id-1]->msg, sizeof(user));
            send_data( connfd, data, 0xe0 );
        }
    }

    return 0;
}

int send_message( int connfd )
{
    char to[0x10];
    int retval = 0;
    puser dest = NULL;
    pmess new_mess = NULL;
    int index = 0;

    memset(to, 0x00, 0x10);

    retval = send_string( connfd, "To: " );

    if ( retval == -1 )
    {
        return retval;
    }

    retval = recv_until( connfd, to, 0x10, 0x0a );

    if ( retval == -1 )
    {
        return retval;
    }

    dest = get_user_by_name( to );

    if ( dest == NULL )
    {
        send_string( connfd, "User does not exist\n\n" );
        return -1;
    } 

    if ( dest->msg_cnt == 0x10 )
    {
        send_string( connfd, "User mailbox is full\n\n");
        return -1;
    }

    retval = send_string( connfd, "Message: " );

    if ( retval == -1 )
    {
        return retval;
    }

    new_mess = (pmess)hjalloc(progheap, sizeof(mess));

    if ( new_mess == NULL )
    {
        return -1;
    }

    memset(new_mess, 0x00, sizeof(mess));

    memcpy( new_mess->from, cntxt->name, 0x10 );
    new_mess->msg = (char*)hjalloc(progheap, sizeof(user));

    if ( new_mess->msg == NULL )
    {
        hjfree( progheap, new_mess, sizeof(mess) );
        return -1;
    }

    memset(new_mess->msg, 0x00, sizeof(user));
 
    retval = recv_until( connfd, new_mess->msg, sizeof(user), 0x0a );

    if ( retval == -1 )
    {
        hjfree( progheap, new_mess->msg, sizeof(user));
        hjfree( progheap, new_mess, sizeof(mess) );
        return retval;
    }

    while ( index < 0x10 )
    {
        if ( dest->msgs[index] == NULL )
        {
            dest->msgs[index] = new_mess;
            dest->msg_cnt++;
            break;    
        } else
        {
            index++;
        }
    }

    send_string( connfd, "Message sent\n\n");
 
    return 0;
}

puser get_user_by_name( char *name )
{
    pulist walker = ulist_head;

    if ( name == NULL )
    {
        return NULL;
    }

    while ( walker != NULL )
    {
        if ( walker->user_s != NULL )
        {
            if ( strncmp( (const char *)name, (const char *)(walker->user_s->name), 0x10 ) == 0 )
            {
                return walker->user_s; 
            }
        }

        walker = walker->next;
    }

    return NULL;
}

int add_desc( int connfd )
{
    int retval = 0;

    retval = send_string( connfd, "Enter description: " );

    if ( retval == -1 )
    {
        return retval;
    }

    retval = recv_until( connfd, cntxt->description, 0x28, 0x0a );

    if ( retval == -1 )
    {
        return retval;
    }
    
    return 0;
}

int delete_user( )
{
    pulist walker = ulist_head;
    pulist temp = NULL;

    if ( walker->user_s == NULL )
    {
        return 0;
    }

    if ( walker->user_s->id == cntxt->id )
    {
        free_user_messages( walker->user_s );
        ulist_head = walker->next;
        hjfree( progheap, walker->user_s, sizeof(user) );
        hjfree( progheap, walker, sizeof(ulist) );
        return 0;
    }

    while ( (walker->user_s->id != cntxt->id) )
    {
        temp = walker;
        walker = walker->next;
    }

    free_user_messages( walker->user_s );
    temp->next = walker->next;
    hjfree( progheap, walker->user_s, sizeof(user) );
    hjfree( progheap, walker, sizeof(ulist) );
    
    return 0;
}

int free_user_messages( puser user_x )
{
    int index = 0;

    if ( user_x == NULL )
    {
        return -1;
    }


    while( index < 0x10 )
    {
        if ( user_x->msgs[index] != NULL )
        {
            hjfree( progheap, user_x->msgs[index]->msg, sizeof(user) );
            hjfree( progheap, user_x->msgs[index], sizeof(mess) );
            user_x->msgs[index] = NULL;
        }
        index++;
    } 

    return 0;
}

int get_resp( int connfd )
{
		  char buff[4];
		  int32_t retval = 0;

		  memset(buff, 0x00, 0x04);

		  retval = recv_until( connfd, buff, 2, 0x0a );

		  if (retval == 2 )
		  {
					 retval = buff[0] - 0x30;
		  } else
		  {
					 retval = -1;
		  }

		  return retval;
}

int ff( int connfd )
{
    int retval = -1;
    init_user_list();

    progheap = create_heap( 0x8000, 0x0000 );

    while( 1 )
    {    
        while ( retval != 0 )
        {
            retval = handle_main_menu( connfd );
        }

        while ( authd != 0 )
        {
            retval = handle_authd_menu( connfd );
        }
    }

    return 0;
}

int check_user_exists( char * name )
{
		  pulist walker = ulist_head;
		  int retval = -1;

		  if ( name == NULL )
		  {
					 return -1;
		  }
		  
		  while ( walker != NULL )
		  {
					 if ( walker->user_s == NULL )
					 {
								return 0;
					 }

					 retval = strncmp( (const char*)name, (const char*)walker->user_s, 0x0f );

					 if ( retval == 0 )
					 {
								return -1;
					 }

					 walker = walker->next;
		  }

		  return 0;
}

int add_user( puser new_user )
{
    pulist walker = ulist_head;
    pulist new_ulink = NULL;
    
    if ( new_user == NULL )
    {
        return -1;
    }

    new_ulink = (pulist)hjalloc(progheap, sizeof(ulist));    

    if ( new_ulink == NULL )
    {
        return -1;
    }
    
    memset(new_ulink, 0x00, sizeof(ulist));
    new_ulink->user_s = new_user;

    // handle init
    if ( walker == NULL )
    {
        ulist_head = new_ulink;
    } else
    {
        while ( walker->next != NULL )
        {
            walker = walker->next;
        }

        walker->next = new_ulink;
    }

    return 0;
}

int list_users( int connfd )
{
    char buff[0x60];
    pulist walker = ulist_head;

    while ( walker != NULL )
    {
        memset(buff, 0x00, 0x60);
        if ( walker->user_s == NULL)
        {
            break;
        }

        snprintf(buff, 0x60, "%d: %s -- %s\n", walker->user_s->id, walker->user_s->name, walker->user_s->description);
        send_string( connfd, buff );
        walker = walker->next;
    }

    return 0;
}

int init_user_list( )
{
    ulist_head = NULL;
    return 0;
}

int handle_main_menu( int connfd )
{
		  int retval =0;

		  // if not authenticated display login menu
		  retval = send_string( connfd, menu_main );

		  if ( retval == -1 )
		  {
					 return retval;
		  }

		  retval = get_resp( connfd );
		 
		  if ( retval < 1 || retval > 4 )
		  {
					 send_string( connfd, "Invalid response\n");
					 return -1;
		  }

		  switch ( retval )
		  {
					 case 1:
                                retval = handle_login( connfd );
                                if ( retval == 0 )
                                {
                                    send_string( connfd, "Login success\n\n" );
                                    return 0;
                                } else
                                {
                                    send_string( connfd, "Invalid login\n\n" );
                                    return -1;
                                }
								break;
					 case 2:
								retval = add_user( create_user( connfd ) );
								return 1;
								break;
					 case 3:
								retval = list_users( connfd );
								return 2;
								break;
					 case 4:
								send_string( connfd, "So long\n");
								close( connfd );
								exit(0);
					 default:
								break;
		  };

		  return -1;		   
}

puser create_user( int connfd )
{
		  int retval = 0;
		  puser new_user = (puser)hjalloc(progheap, sizeof(user));

		  if ( new_user == NULL )
		  {
					 return new_user;
		  }

		  memset(new_user, 0x00, sizeof(user));

		  do
		  {
					 retval = send_string( connfd, "Username: ");

					 if (retval == -1)
					 {
								hjfree( progheap, new_user, sizeof(user) );
								return NULL;
					 }

					 retval = recv_until( connfd, new_user->name, 0x0f, 0x0a );

					 if (retval == -1)
					 {
								hjfree( progheap, new_user, sizeof(user) );
								return NULL;
					 }
					 
					 if ( check_user_exists( new_user->name ) == -1 )
					 {
								send_string( connfd, "User exists. Try again\n\n");
								memset(new_user->name, 0x00, 0x10);
					 } else
					 {
								break;
					 }
		  } while (1);

		  retval = send_string( connfd, "Password: ");

		  if (retval == -1)
		  {
				     hjfree( progheap, new_user, sizeof(user) );
					 return NULL;
		  }

		  retval = recv_until( connfd, new_user->pass, 0x0f, 0x0a);

		  if (retval == -1)
		  {
				     hjfree( progheap, new_user, sizeof(user) );
					 return NULL;
		  }

		  new_user->id = next_id;
		  next_id += 1;

		  return new_user;
}

int handle_login( int connfd )
{
    int retval = 0;
    pulist walker = ulist_head;

    char username[0x10];
    char password[0x10];

    memset(password, 0x00, 0x10);
    memset(username, 0x00, 0x10);

    retval = send_string( connfd, "Username: ");

    if ( retval == -1 )
    {
        return -1;
    }

    retval = recv_until( connfd, username, 0x10, 0x0a );

    if ( retval == -1 )
    {
        return -1;
    }

    retval = send_string( connfd, "Password: ");

    if ( retval == -1 )
    {
        return -1;
    }

    retval = recv_until( connfd, password, 0x10, 0x0a);

    if ( retval == -1 )
    {
        return -1;
    }

    while ( walker != NULL )
    {
        if ( walker->user_s != NULL )
        {
            if ( strncmp( (const char *)walker->user_s->name, (const char *)username, 0x10 ) == 0 )
            {
                if ( strncmp( (const char *)walker->user_s->pass, (const char *)password, 0x10 ) == 0 )
                {
                    authd = walker->user_s->id;
                    cntxt = walker->user_s;
                    return 0;
                } else
                {
                    return -1;
                }
            }
        }

        walker = walker->next;
    }    

    return -1;    
}
