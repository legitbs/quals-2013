#include "sharedfuncs.h"
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <string.h>
#include <time.h>

#define SUCCESS 1
#define FAIL 0
#define ERROR -1

void sysc(void)
{
	asm( 
		"mov r7, r0\n\t"
		"mov r0, r1\n\t"
		"mov r1, r2\n\t"
		"mov r2, r3\n\t"
		"mov r3, r4\n\t"
		"mov r4, r5\n\t"
		"svc 0"
	);
}

typedef struct
{
	char *q[5];
} que, *pque;

class questions
{
   public:
      void parse_questions( char * filename );
      int ask_question();
      int read_name();
      int main_loop();
      questions();
      void setsock(int fd);
   private:
      std::vector<pque> qs;
      int num_correct;
      int max_correct;  
      int sockfd;
};

void questions::setsock(int fd)
{
	this->sockfd = fd;
	return;
}

questions::questions()
{
	this->qs.clear();
	this->num_correct = 0;
	this->sockfd = -1;
   this->max_correct = 5;
	return;
}

int questions::main_loop()
{
   int retval = SUCCESS;

	while ( this->num_correct < this->max_correct )
	{
      retval = this->ask_question();

		if ( retval == SUCCESS )
		{
			this->num_correct += 1;
		} else
		{
			return FAIL;
		}
	}

	this->read_name();
	this->num_correct = 0;
	return SUCCESS;
}

int questions::read_name()
{
	char name[10];
	memset(name, 0x00, 10);
	
	if ( send_string( this->sockfd, (char*)"What is your name: ") == ERROR )
   {
      return FAIL;
   }

	recv_until(this->sockfd, name, 0x100, '\n');
	send_string( this->sockfd, (char*)"Congrats ");
	send_string( this->sockfd, name );

	return SUCCESS;
}

int questions::ask_question()
{
	int correct = 0;
	int index = 0;
	std::vector<int> i;
	int j;
	int k;
	int start;
	int end;
	int mylist[] = { 1, 2, 3, 4};
	int answer = 0;
	char askme[0x400];	
	srand(time(NULL));
	
	memset( askme, 0x00, 0x400);

	if ( this->qs.size() == 0 )
	{
		return ERROR;
	}

	index = rand() % this->qs.size();

	for (j = 0; j < 100; j++)
	{
		start = rand() % 4;
		end = rand() % 4;
		k = mylist[start];
		mylist[start] = mylist[end];
		mylist[end] = k;
	}

	snprintf( askme, 0x400, "%s\n1) %s\n2) %s\n3) %s\n4) %s\n\nAnswer: ", qs[index]->q[0], qs[index]->q[mylist[0]], qs[index]->q[mylist[1]], qs[index]->q[mylist[2]], qs[index]->q[mylist[3]]);
 
	if ( send_string( this->sockfd, askme ) == ERROR )
	{
		return ERROR;
	}

	for ( j = 0; j < 4; j++)
	{
		if ( mylist[j] == 4 )
		{
			correct = j+1;
		}
	}

	qs.erase(qs.begin()+index);

	if ( recvdata( this->sockfd, (char*)&answer, 2) <= 0 )
	{
		return ERROR;
	}

	answer = (answer&0xff)-0x30;
	if (answer == correct)
	{
		if ( send_string(this->sockfd, (char*)"Correct!!\n") == ERROR )
		{
			return ERROR;
		} else
		{
			return SUCCESS;
		}
	} else
	{
		send_string(this->sockfd, (char*)"Wrong!!\n");
		return FAIL;	
	}

	return SUCCESS;	
}

void questions::parse_questions( char * filename )
{
	std::ifstream f;
	std::string line;
	int num = 0;
	pque q = NULL;
	int start = 0;
	int end = 0;
	int i = 0;

	if ( filename == NULL )
	{
		return;
	}

	f.open((const char*)filename, std::ios::in);
	
	if (!f.is_open())
	{
		return;
	}

	std::getline(f, line);
	num = atoi(line.c_str());

	while ( num-- )
	{
		start = 0;
		end = 0;

		std::getline(f, line);
		q = (pque)malloc(sizeof(que));

		if (q == NULL)
		{
			return;
		}

		memset(q, 0x00, sizeof(que));

		for ( i =0; i <5; i++)
		{

			end = line.find_first_of(';', start);

			if ( end == std::string::npos)
			{
				return;
			}
		
			q->q[i] = (char *)malloc((end-start)+1);
			memset( q->q[i], 0x00, (end-start)+1);
			memcpy(q->q[i], line.substr(start, end-start).c_str(), end-start);

			start = end + 1;
		}

		qs.push_back(q);
	}
		
	return;
}

int ff( int connfd )
{
   questions q;
   char cont[2];

   q.setsock(connfd);

   cont[0] = 1;
   while ( cont[0] )
   {
      q.parse_questions((char*)"questions.txt");

      if ( q.main_loop() == 1 )
      {
         send_string( connfd, (char*)"Would you like to try again (y/n): " );

         recv_until( connfd, (char*)&cont, 2, '\n' );

         if ( cont[0] == 'y' )
         { 
            cont[0] = 1;      
         } else
         {
            cont[0] = 0;
         }
      } else
      {
         send_string( connfd, (char*)"Don't worry, I'm the Doctor." );
         close( connfd );
         cont[0] = 0;
      }
   }

   return 0;
}

int main(int argc, char **argv)
{
   int sockfd;

   sockfd = SetupSock( 7263, AF_INET );

	accept_loop( sockfd, ff );

	return 0;
}
