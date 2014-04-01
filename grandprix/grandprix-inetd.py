#!/usr/bin/python

import sys
import time
import random
import socket
import thread
import threading
import signal

class ClientThread():
    def alarmHandler(self, signum, frame):
      print("Too slow!")
      sys.exit(-1)

    def generateNextState( self, checkstate, newupos ):
        newstate = []
        for i in checkstate:
            newstate.append(i[:])
        
        for i in range(9, 1, -1):
            newstate[i] = newstate[i-1]

        newstate[1] = [ '|', 'X', 'X', 'X', 'X', 'X', '|' ]
        newstate[9][newupos] = 'u'
        
        return newstate

    def getSafeMovesInASingleState( self, checkstate, upos ):
        safemoves = []
        if checkstate[8][upos-1] == ' ' or checkstate[8][upos-1] == '=':
            safemoves.append('l')
        if checkstate[8][upos] == ' ' or checkstate[8][upos] == '=':
            safemoves.append('g')
        if checkstate[8][upos+1] == ' ' or checkstate[8][upos+1] == '=':
            safemoves.append('r')
            
        return safemoves

    def getSafeMoves( self, checkstate, upos, level ):
        safeMoves = self.getSafeMovesInASingleState( checkstate, upos )
        toremove = []
                    
        if level == 0:
            return safeMoves
        
        for i in safeMoves:
            if i == 'l':
                nextState = self.generateNextState( checkstate, upos-1)
                nummoves = self.getSafeMoves( nextState, upos-1, level-1)
                if len(nummoves) == 0:
                    toremove.append( 'l' )
            if i == 'g':
                nextState = self.generateNextState( checkstate, upos)
                nummoves = self.getSafeMoves( nextState, upos, level-1)
                if len(nummoves) == 0:
                    toremove.append( 'g' )
            if i == 'r':
                nextState = self.generateNextState( checkstate, upos+1)
                nummoves = self.getSafeMoves( nextState, upos+1, level-1)
                if len(nummoves) == 0:
                    toremove.append( 'r' )

        for i in toremove:
            safeMoves.remove(i)
            
        return safeMoves
    
    def initstate( self ):
        self.state = []
        self.state.append( ['|', '-', '-', '-', '-', '-', '|' ] )
        self.state.append( ['|', ' ', ' ', ' ', ' ', ' ', '|' ] )
        self.state.append( ['|', ' ', ' ', ' ', ' ', ' ', '|' ] )
        self.state.append( ['|', ' ', ' ', ' ', ' ', ' ', '|' ] )
        self.state.append( ['|', ' ', ' ', ' ', ' ', ' ', '|' ] )
        self.state.append( ['|', ' ', ' ', ' ', ' ', ' ', '|' ] )
        self.state.append( ['|', ' ', ' ', ' ', ' ', ' ', '|' ] )
        self.state.append( ['|', ' ', ' ', ' ', ' ', ' ', '|' ] )
        self.state.append( ['|', ' ', ' ', ' ', ' ', ' ', '|' ] )
        self.state.append( ['|', ' ', ' ', 'u', ' ', ' ', '|' ] )
        self.state.append( ['|', '-', '-', '-', '-', '-', '|' ] )
        self.upos = 3
        self.crash = None
        self.rows = 0
        self.rowtarget = random.randint(800, 1300)
        
    def generateNextLine( self ):
        random.jumpahead(random.randint(100,1000))
        
        line = []
        line.append( '|' )

        # This will be true for every row after the finish
        # line is generated.
        if self.rows > self.rowtarget:
            for i in range(0, 5):
                line.append(' ')
            line.append('|')
            return line

        # This will only be true when the target rows
        # have been hit
        if self.rows == self.rowtarget:
            for i in range(0, 5):
                line.append('=')
            line.append('|')
            return line

        # 1% chance to generate aroadblock
        if random.randint(0, 100) > 99:
            for i in range(0, 5):
                line.append( 'X' )
            line.append('|')
            # Needs at least one open space next to another one
            x = random.randint(0, 5)
            while self.state[2][x] != ' ':
                x = random.randint(0, 5)
            line[x] = ' '
            return line
        
        # Generate a normal line with 14% chance of an obstruction
        for i in range(0, 5):
            if random.randint(0, 100) > 86:
                type = random.randint(0, 5)
                if type == 0:
                    line.append( 'r' )
                elif type == 1:
                    line.append( 'c' )
                elif type == 2:
                    line.append( 'T' )
                elif type == 3:
                    line.append( 'P' )
                elif type == 4:
                    line.append( '~' )
                else:
                    line.append( 'Z' )
            else:
                line.append( ' ' )
            
        line.append( '|' )
        
        return line

    def updateState( self, move ):
        # move the rows down by one
        for i in range( 9, 1, -1):
            self.state[i] = self.state[i-1]

        # Generate a safe line though I only try a few
        # times since it is possible that the player drove into a corner
        safemoves = []
        ct = 0
        while len(safemoves) == 0 and ct < 10:
            ct = ct + 1
            line = self.generateNextLine( )
            self.state[1] = line
            safemoves = self.getSafeMoves( self.state, self.upos, 7)
        
        if move == 'l':
            self.upos = self.upos - 1
        if move == 'r':
            self.upos = self.upos + 1

        if self.state[9][self.upos] != ' ':
            self.crash = self.state[9][self.upos]

        self.state[9][self.upos] = 'u'

    def sendState( self ):
        data = ''

        # Hiding lines will happen here
        for i in self.state:
            data += ''.join(i)
            data += '\n'

        #self.s.send(data)
        sys.stdout.write(data)
        sys.stdout.flush()

    def playGame( self ):        
        self.initstate()
        self.sendState( )
        while self.crash == None:
            #move = self.s.recv(1)
            move = sys.stdin.read(1)
            if move != "\n":
                sys.stdin.read(1)
            self.updateState( move )
            self.sendState(  )
 
            if self.crash != None:
                if self.crash == '|':
                    #self.s.send("You hit a wall")
                    sys.stdout.write("You hit a wall\n")
                    sys.stdout.flush()
                    self.lost = 1
                elif self.crash == '~':
                    #self.s.send("You hit a snake")
                    sys.stdout.write("You hit a snake\n")
                    sys.stdout.flush()
                    self.lost = 1
                elif self.crash == 'T':
                    #self.s.send("You hit a tree")
                    sys.stdout.write("You hit a tree\n")
                    sys.stdout.flush()
                    self.lost = 1
                elif self.crash == 'r':
                    #self.s.send("You hit a rock")
                    sys.stdout.write("You hit a rock\n")
                    sys.stdout.flush()
                    self.lost = 1
                elif self.crash == 'P':
                    #self.s.send("You hit a person")
                    sys.stdout.write("You hit a person\n")
                    sys.stdout.flush()
                    self.lost = 1
                elif self.crash == 'c':
                    #self.s.send("You hit a car")
                    sys.stdout.write("You hit a car\n")
                    sys.stdout.flush()
                    self.lost = 1
                elif self.crash == 'Z':
                    #self.s.send("You hit a big zebra")
                    sys.stdout.write("You hit a big zebra\n")
                    sys.stdout.flush()
                    self.lost = 1
                elif self.crash == 'X':
                    #self.s.send("You hit a road block")
                    sys.stdout.write("You hit a road block\n")
                    sys.stdout.flush()
                    self.lost = 1
                elif self.crash == '=':
                    #self.s.send("You won this game. Push a key to play again")
                    sys.stdout.write("You won this game. Push a key to play again\n")
                    sys.stdout.flush()
                    #self.s.recv(1)
                    sys.stdin.read(1)

            self.rows = self.rows + 1
            
#    def __init__ (self, s, addr):
#        self.s = s
#        self.addr = addr
#        threading.Thread.__init__( self )

    def run( self ):
        signal.signal(signal.SIGALRM, self.alarmHandler)

        self.lost = 0

        # Initialize number of games won to 0
        self.games = 0

        # Random number of games to win
        #self.gametarget = random.randint(30, 40)
        self.gametarget = 1
        #self.gametarget = random.randint(3, 4)

        #self.s.send("Use 'l' and 'r' to move. Don't crash.\nPress any key to start")
        sys.stdout.write("Use 'l' and 'r' to move. Don't crash.\nPress return to start\n")
        sys.stdout.flush()
        sys.stdin.read(1)
        signal.alarm(120)
        while self.games < self.gametarget and self.lost == 0:
            self.playGame()
            if self.lost == 0:
                self.games = self.games + 1
                print "User won game"
            else:
                print "User lost game"
        if self.games == self.gametarget:
            f = open("/home/grandprix/key", 'r')
            key = f.read()
            #self.s.send(key)
            sys.stdout.write(key)
            f.close()

        #self.s.close()
        return

def openSock( port ):
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s.bind(('', port))
    s.listen(3000)

    return s


    
if __name__ == '__main__':
    random.seed()
#    s = openSock( 2038 )
#    while True:
#        conn, addr = s.accept()
#        ClientThread( conn, addr ).start()
    ClientThread().run()

