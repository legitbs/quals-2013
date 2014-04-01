#!/usr/bin/python

import sys
import socket
import thread
import threading
import random
import time
import signal

class ClientThread():
#    def __init__( self, s, addr ):
#        self.s = s
#        self.addr = addr
#        threading.Thread.__init__( self )
 
    def alarmHandler(self, signum, frame):
      print("Too slow!")
      sys.exit(-1)

    def run( self ):
        signal.signal(signal.SIGALRM, self.alarmHandler)
        random.seed()
        printB = 1
        self.numgames = random.randint(50, 60)
        signal.alarm(self.numgames * 3)
        while self.numgames > 0:
            self.initBoard2( )
            while(1):
                if(printB):
                    self.printBoard()
                sys.stdout.flush()
                #self.updateBoard( self.s.recv(1) )
                printB = self.updateBoard( sys.stdin.read(1) )

                if ( self.checkSolved( ) == 1 ):
                    #self.s.send("Solved\nPress a key to start again: ")
                    sys.stdout.write("Solved\nPress a key to start again: ")
                    #self.s.recv(1)                    
                    sys.stdout.flush()
                    sys.stdin.read(2)
                    self.numgames = self.numgames - 1
                    break;

        f = open('/home/pieceofeight/key', 'r')
        key = f.read()
        #self.s.send(key)
        sys.stdout.write(key)
        f.close()
        #self.s.close()
        
        return

    def initBoard2( self ):
        self.board = []
        self.board.append([ 1, 2, 3 ] )
        self.board.append([ 4, 5, 6 ] )
        self.board.append([ 7, 8, ' ' ] )

        self.nullloc = 8

        for i in range( 0, random.randint(600, 1000) ):
            a = random.randint(0, 3)
            if a == 0:
                move = 'l'
            elif a == 1:
                move = 'r'
            elif a == 2:
                move = 'u'
            elif a == 3:
                move = 'd'

            self.updateBoard( move )
                          
    def initBoard( self ):
        self.board = []
        self.numlist = []
        sortlist = [ 0, 1, 2, 3, 4, 5, 6, 7, 8 ]
        
        for i in range( len( sortlist) ):
            element = random.choice(sortlist)
            sortlist.remove(element)
            if element == 0:
                element = ' '
                self.nullloc = i
            self.numlist.append(element)

        self.board.append([ self.numlist[0], self.numlist[1], self.numlist[2] ] )
        self.board.append([ self.numlist[3], self.numlist[4], self.numlist[5] ] )
        self.board.append([ self.numlist[6], self.numlist[7], self.numlist[8] ] )

    def checkSolved( self ):
        if self.board[0][0] == 1 and self.board[0][1] == 2 and self.board[0][2] == 3:
            if self.board[1][0] == 4 and self.board[1][1] == 5 and self.board[1][2] == 6:
                if self.board[2][0] == 7 and self.board[2][1] == 8 and self.board[2][2] == ' ':
                    return 1
        return 0
    
    def updateBoard( self, move ):
        new_row = self.nullloc / 3
        new_col = self.nullloc % 3
        
        if move == 'l':
            if new_col == 2:
                return 1

            self.board[new_row][new_col] = self.board[new_row][new_col+1]
            self.board[new_row][new_col+1] = ' '
            self.nullloc = (new_row)*3 +(new_col+1)
            return 1

        if move == 'r':
            if new_col == 0:
                return 1
            self.board[new_row][new_col] = self.board[new_row][new_col-1]
            self.board[new_row][new_col-1] = ' '
            self.nullloc = (new_row)*3 +(new_col-1)
            return 1

        if move == 'u':
            if new_row == 2:
                return 1
            self.board[new_row][new_col] = self.board[new_row+1][new_col]
            self.board[new_row+1][new_col] = ' '
            self.nullloc = (new_row+1)*3 +(new_col)
            return 1
            
        if move == 'd':
            if new_row == 0:
                return 1
            self.board[new_row][new_col] = self.board[new_row-1][new_col]
            self.board[new_row-1][new_col] = ' '
            self.nullloc = (new_row-1)*3 +(new_col)
            return 1

        if move == '\n':
            return 0

        if move == 'q':
            #self.s.send("Leaving")
            sys.stdout.write("Leaving\n")
            sys.stdout.flush()
#            self.s.close()
            sys.exit(0)
            
    def printBoard( self ):
        board = ''

        for i in range(0, 3):
            board += '\n' + '-'*19+'\n'
            board += '|     '*3+'|\n'
            board += '|  ' + str(self.board[i][0]) + '  '
            board += '|  ' + str(self.board[i][1]) + '  '
            board += '|  ' + str(self.board[i][2]) + '  |\n'
            board += '|     '*3+'|\n'

        
        board += '-'*19+'\n'
        #self.s.send(board)
        sys.stdout.write(board)

def openSock( port ):
    s = socket.socket( socket.AF_INET, socket.SOCK_STREAM)
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s.bind(('',port))
    s.listen(3000)

    return s

if __name__ == '__main__':
    random.seed()

#    s = openSock( 2222 )
#    while True:
#        conn, addr = s.accept()
#        ClientThread( conn, addr ).start() 
#lol, not a thread anymore.
    ClientThread().run()
