#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <signal.h>
#include "common.h"
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

// array to hold the shellcode
char winnings[MAX_WINNINGS];
int total_bets = 0;

// drink arrays
char *drink1[] = {
	"long island ",
	"jager ",
	"mojito ",
	"shirley temple ",
	"virgin ",
	"cosmopolitan ",
	"barbie "
	"dark and stormy "
	"cinderella "
};
#define MAX_DRINK1 9
char *drink2[] = {
	"bomb.\n",
	"diablo.\n",
	"daiquiri\n",
	"shot.\n",
	"spritzer.\n",
	"rum.\n"
	"fiz.\n"
	"tini.\n"
	"punch.\n"
	"cooler.\n"
	"mudslide.\n"
	"smoothie.\n"
};
#define MAX_DRINK2 12

// no tip array
char *notips[] = {
	"She pours the drink in your lap.\n\n",
	"She says, \"Looser\" and walks away.\n\n",
	"She pours the drink on your head.\n\n",
	"She spits in your drink.\n\n",
	"She flips you off.\n\n"
};
#define MAX_NOTIPS 5

// tip array
char *tips[] = {
	"She says, \"Thanks\".\n\n",
	"She winks at you.\n\n",
	"She slips your her number on a cocktail napkin.\n\n",
	"She winks at you.\n\n"
};
#define MAX_TIPS 4
int tip = 0;

// starting cash
int cash = 100;

// max bet
int MAX_BET = 127;

void handle_client(int);

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
}

void PrintWinnings(int sock) {
	int i;
	char buf[100];

	sendit(sock, "Bets so far:\n");
	for (i = 0; i < total_bets; i++) {
		snprintf(buf, 99, "%d (%02x)\n", winnings[i], winnings[i] & 0xFF);
		sendit(sock, buf);
	}
}

int CalculateHandValue(char *hand) {
	int i;
	int aces = 0;
	int value = 0;

	for (i = 0; i < 21; i++) {
		if (hand[i] == 0) {
			break;
		}

		// see if we have an ace
		if (hand[i] == 1) {
			value += 11;
			aces++;
		} else if (hand[i] == 11) {
			value += 10;
		} else if (hand[i] == 12) {
			value += 10;
		} else if (hand[i] == 13) {
			value += 10;
		} else {
			value += hand[i];
		}
	}

	// if we're less than or equal to 21, return it
	if (value <= 21) {
		return(value);
	}

	// see if any of their Aces can be set to value '1' to
	// fix any hand values over 21
	while (aces--) {
		value -= 10;
		if (value < 21) {
			return(value);
		}
	}

	return(value);
}

void PrintHand(int sock, char *hand, int hide_first) {
	int i;
	char buf[10];

	for (i = 0; i < 21; i++) {
		if (hide_first && i == 0) {
			sendit(sock, "X ");
			continue;
		}
		if (hand[i] == 0) {
			break;
		}

		if (hand[i] == 1) {
			sendit(sock, "A ");
		} else if (hand[i] == 11) {
			sendit(sock, "J ");
		} else if (hand[i] == 12) {
			sendit(sock, "Q ");
		} else if (hand[i] == 13) {
			sendit(sock, "K ");
		} else {
			snprintf(buf, 9, "%d ", hand[i]);
			sendit(sock, buf);
		}
	}

	if (!hide_first) {
		snprintf(buf, 9, "(%d)", CalculateHandValue(hand));
		sendit(sock, buf);
	}
}

int TipWaitress(int sock) {
	char buf[100];
	int rnd;

	sendit(sock, "But it's not all bad.  The waitress just showed up with your ");
	rnd = rand() % MAX_DRINK1;
	sendit(sock, drink1[rnd]);
	rnd = rand() % MAX_DRINK2;
	sendit(sock, drink2[rnd]);
	sendit(sock, "Would you like to tip $1 (y/n)? ");
	read_until(sock, buf, 99);
	if (buf[0] == 'y' || buf[0] == 'Y') {
		rnd = rand() % MAX_TIPS;
		sendit(sock, tips[rnd]);
		return(1);
	}

	rnd = rand() % MAX_NOTIPS;
	sendit(sock, notips[rnd]);
	return(0);
}
		
void PlayBlackjack(int sock) {
	int done = 0;
	char buf[100];
	int bet;
	char dealer[21];
	int dealer_hand_value;
	char player[21];
	int player_hand_value;
	int player_cards;
	int dealer_cards;

	while (!done) {

		// start with empty hands
		bzero(dealer, 21);
		bzero(player, 21);

		// print out current cash
		snprintf(buf, 99, "You have $%d\n", cash);
		sendit(sock, buf);

		// take bets
		bet = -1;
		while (bet < 0 || bet > MAX_BET) {
			sendit(sock, "How much would you like to bet (-1 to exit)? ");
			read_until(sock, buf, 99);
			bet = atoi(buf);
			if (bet == -1) {
				return;
			}
			if (bet < 0 || bet > MAX_BET) {
				snprintf(buf, 99, "Table limit is $%d.  Bet again.\n", MAX_BET);
				sendit(sock, buf);
			}
			if (bet == 0) {
				sendit(sock, "Well, I suppose we can let a noob blackjack player learn the ropes without risking any money.  Don't tell my pit boss.\n");
			}
			if (bet > cash) {
				snprintf(buf, 99, "No lines of credit in this casino.  Bet again.\n", MAX_BET);
				sendit(sock, buf);
				bet = -1;
			}
		} 

		// deal inital hand
		player[0] = (rand() % 13)+1;
		dealer[0] = (rand() % 13)+1;
		player[1] = (rand() % 13)+1;
		dealer[1] = (rand() % 13)+1;

		// calc inital hand values
		player_hand_value = CalculateHandValue(player);
		dealer_hand_value = CalculateHandValue(dealer);

		// print out the hands
		sendit(sock, "Dealer: "); PrintHand(sock, dealer, 1); sendit(sock, "\n");
		sendit(sock, "Player: "); PrintHand(sock, player, 0); sendit(sock, "\n");

		player_cards = 2;
		while (player_cards <= 21) {
			// hit or stand
			sendit(sock, "Hit or Stand (H/S)? ");
			read_until(sock, buf, 99);
			if (buf[0] == 'H') {
				player[player_cards++] = (rand() % 13)+1;
				// print out the hand
				sendit(sock, "Player: "); PrintHand(sock, player, 0); sendit(sock, "\n");
			} else if (buf[0] == 'S') {
				break;
			}
				
			// see if busted or win
			player_hand_value = CalculateHandValue(player);
			if (player_hand_value > 21) {
				break;
			} //else if (player_hand_value == 21) {
//				break;
//			}
		}

		dealer_cards = 2;
		while (player_hand_value <= 21 && dealer_cards < 21) {
			// dealer hit or stand
			if (dealer_hand_value < 17) {
				sendit(sock, "Dealer hits\n");
				dealer[dealer_cards++] = (rand() % 13)+1;

				// print out the hand
				sendit(sock, "Dealer: "); PrintHand(sock, dealer, 0); sendit(sock, "\n");
			} else {
				sendit(sock, "Dealer stands\n");
				sendit(sock, "Dealer: "); PrintHand(sock, dealer, 0); sendit(sock, "\n");
				break;
			}

			// see if busted
			dealer_hand_value = CalculateHandValue(dealer);
			if (dealer_hand_value > 21) {
				break;
			} else if (dealer_hand_value == 21) {
				break;
			}
		}

		// see who won
		if (player_hand_value == 21 && dealer_hand_value != 21) {
			sendit(sock, "You win!\n\n");
			cash += bet;
			winnings[total_bets++] = bet;
		} else if (dealer_hand_value == 21 && player_hand_value != 21) {
			sendit(sock, "You lose!\n");
			if (bet == 127 && TipWaitress(sock)) {
				bet++;
			}
			cash -= bet;
			winnings[total_bets++] = 0-bet;
		} else if (player_hand_value > 21) {
			sendit(sock, "You lose!\n\n");
			if (bet == 127 && TipWaitress(sock)) {
				bet++;
			}
			cash -= bet;
			winnings[total_bets++] = 0-bet;
		} else if (dealer_hand_value > 21) {
			sendit(sock, "You win!\n\n");
			cash += bet;
			winnings[total_bets++] = bet;
		} else if (player_hand_value > dealer_hand_value) {
			sendit(sock, "You win!\n\n");
			cash += bet;
			winnings[total_bets++] = bet;
		} else if (player_hand_value == dealer_hand_value) {
			sendit(sock, "It's a draw.\n\n");
		} else if (player_hand_value < dealer_hand_value) {
			sendit(sock, "You lose!\n\n");
			if (bet == 127 && TipWaitress(sock)) {
				bet++;
			}
			cash -= bet;
			winnings[total_bets++] = 0-bet;
		}

		//PrintWinnings(sock);

		if (total_bets == MAX_WINNINGS) {
			sendit(sock, "The pit boss is tired of you flirting with the waitresses...you're outta here\n");
			break;
		}

		if (cash <= 0) {
			break;
		}
	}
}
