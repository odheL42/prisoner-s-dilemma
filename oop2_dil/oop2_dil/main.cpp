#define CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <string>

#include <list>
#include <random>
#include <ctime>

//STRATEGIES
//factory of strategies:
enum Move {
	HELP,
	BETRAY
};

using std::list;

class BaseStrategy
{
public:
	BaseStrategy(uint32_t playerNumber) {
		PlayerNumber = playerNumber;
		PreferredMove = new Move[PlayerNumber];
	}
	~BaseStrategy() {
		delete[] PreferredMove;
	}

	Move* makeMove(const list<Move**>& moveList, uint32_t me) {
		if (!moveList.empty())
			analysMove(moveList, me);
		return PreferredMove;
	}
protected:
	virtual void analysMove(const list<Move**>& moveList, uint32_t me) = 0;
	uint32_t playerNumber() const {
		return PlayerNumber;
	}

	void setPreferredMove(Move* mv) {
		PreferredMove = mv;
	}
private:
	Move* PreferredMove;
	uint32_t PlayerNumber;
};


//“–»¬»¿À‹Õ€≈ —“–¿“≈√»»
class Agressor : //‚ÒÂ„‰‡ Ì‡Ô‡‰‡ÂÚ
	public BaseStrategy
{
public:
	Agressor(uint32_t playerNumber) : BaseStrategy(playerNumber) {
		Move* mv = new Move[playerNumber];
		for (uint32_t i = 0; i < playerNumber; i++) {
			mv[i] = BETRAY;
		}
		setPreferredMove(mv);
	}
protected:
	void analysMove(const list<Move**>& moveList, uint32_t me) {
		Move* mv = new Move[playerNumber()];
		for (uint32_t i = 0; i < playerNumber(); i++) {
			mv[i] = BETRAY;
		}
		setPreferredMove(mv);
	}
};


class Pacific :
	public BaseStrategy
{
public:
	Pacific(uint32_t playerNumber) : BaseStrategy(playerNumber) {
		Move* mv = new Move[playerNumber];
		for (uint32_t i = 0; i < playerNumber; i++) {
			mv[i] = HELP;
		}
		setPreferredMove(mv);
	}
protected:
	virtual void analysMove(const list<Move**>& moveList, uint32_t me) {
		Move* mv = new Move[playerNumber()];
		for (uint32_t i = 0; i < playerNumber(); i++) {
			mv[i] = HELP;
		}
		setPreferredMove(mv);
	}
};

class Random :
	public BaseStrategy
{
public:
	Random(uint32_t playerNumber) : BaseStrategy(playerNumber) {
		std::srand((std::time(nullptr)));
	}
protected:
	void analysMove(const list<Move**>& moveList, uint32_t me) {
		Move* mv = new Move[playerNumber()];
		for (uint32_t i = 0; i < playerNumber(); i++) {
			uint32_t r = std::rand();
			if (r % 2 == 0)
				mv[i] = HELP;
			else
				mv[i] = BETRAY;
		}
		setPreferredMove(mv);
	}
};


class Voter :
	public BaseStrategy
{
public:
	Voter(uint32_t playerNumber) : BaseStrategy(playerNumber) { ; }
protected:
	void analysMove(const list<Move**>& moveList, uint32_t me) {
		int32_t* sum = new int32_t[playerNumber()];
		memset(sum, 0, sizeof(int32_t) * playerNumber());

		for (auto move : moveList) {
			for (uint32_t i = 0; i < playerNumber(); i++) {
				sum[i] += move[i][me];
			}
		}

		Move* mv = new Move[playerNumber()];
		for (uint32_t i = 0; i < playerNumber(); i++) {
			mv[i] = sum[i] >= 0 ? HELP : BETRAY;
		}
		setPreferredMove(mv);
	}
};


class Changer :
	public BaseStrategy
{
public:
	Changer(uint32_t playerNumber) : BaseStrategy(playerNumber) { ; }
protected:
	void analysMove(const list<Move**>& moveList, uint32_t me) {
		Move** last = moveList.back();
		Move* nw = new Move[playerNumber()];
		for (uint32_t i = 0; i < playerNumber(); i++) {
			nw[i] = last[me][i] == HELP ? BETRAY : HELP;
		}
		setPreferredMove(nw);
	}
};

class Repeater :
	public BaseStrategy
{
public:
	Repeater(uint32_t playerNumber) : BaseStrategy(playerNumber) { ; }
protected:
	void analysMove(const list<Move**>& moveList, uint32_t me) {
		Move** last = moveList.back();
		Move* nw = new Move[playerNumber()];
		for (uint32_t i = 0; i < playerNumber(); i++) {
			nw[i] = last[i][me];
		}
		setPreferredMove(nw);
	}
};




//==============================================Strategy Holder============================================
const uint32_t StrategiesNumber = 6;
enum StrategyIdx {
	PACIFIC = 0,
	AGRESSOR,
	REPEATER,
	CHANGER,
	VOTER,
	RANDOM
};

class StrategyHolder
{
public:
	StrategyHolder(uint32_t playerNumber) {
		Strategies = new BaseStrategy * [StrategiesNumber];

		Strategies[0] = new Pacific(playerNumber);
		Strategies[1] = new Agressor(playerNumber);
		Strategies[2] = new Repeater(playerNumber);
		Strategies[3] = new Changer(playerNumber);
		Strategies[4] = new Voter(playerNumber);
		Strategies[5] = new Random(playerNumber);
	}

	~StrategyHolder() {
		delete Strategies[0];
		delete Strategies[1];
		delete Strategies[2];
		delete Strategies[3];
		delete Strategies[4];
		delete Strategies[5];

		delete[] Strategies;
	}

	std::string getStrategyString(StrategyIdx idx) {
		std::string str;

		switch (idx) {
		case PACIFIC: str = "Pacific"; break;
		case AGRESSOR: str = "Agressor"; break;
		case REPEATER: str = "Repeater"; break;
		case CHANGER: str = "Changer"; break;
		case VOTER: str = "Voter"; break;
		case RANDOM: str = "Random"; break;
		}

		return str;
	}

	Move* callStrategy(StrategyIdx idx, const list<Move**>& moveList, uint32_t currentPlayer = 0) const {
		return Strategies[idx]->makeMove(moveList, currentPlayer);
	}
private:
	BaseStrategy** Strategies;
};




//GAME


class Player
{
	friend class Game;
private:
	Player(uint32_t playerId, StrategyIdx preferredStrategy = PACIFIC) {
		PreferredStrategy = preferredStrategy;
		Score = 0;
		Id = playerId;
	}

	void addScore(int32_t score) {
		Score += score;
	}

	int32_t score() const {
		return Score;
	}

	StrategyIdx strategy() const {
		return PreferredStrategy;
	}

	StrategyIdx PreferredStrategy;
	int32_t Score;
	uint32_t Id;
};





const uint32_t MoveBufferSize = 5;

namespace moveScore {
	int32_t help_help = 1;
	int32_t help_betray = -2;
	int32_t betray_help = 2;
	int32_t betray_betray = -1;
}

class Game
{
public:
	Game(uint32_t playerNumber, StrategyIdx* preferredStrategies);
	~Game();
	void rounds(uint32_t roundNumber = 1);
	void calculateMove();
	std::string getInfo() const;
	std::string getLastRound() const;
private:
	Player** Players;
	uint32_t PlayerNumber;
	StrategyHolder* StratHolder;
	list<Move**> MoveList;
};

Game::Game(uint32_t playerNumber, StrategyIdx* preferredStrategies) {
	PlayerNumber = playerNumber;

	Players = new Player * [PlayerNumber];
	for (uint32_t i = 0; i < PlayerNumber; i++) {
		Players[i] = new Player(i, preferredStrategies[i]);
	}
	StratHolder = new StrategyHolder(playerNumber);
	rounds();
}

Game::~Game() {
	// delete[] Players;
	delete StratHolder;
}

void Game::rounds(uint32_t roundNumber) {
	for (uint32_t i = 0; i < roundNumber; i++) {
		Move** currentMove = new Move * [PlayerNumber];
		for (uint32_t j = 0; j < PlayerNumber; j++) {
			currentMove[j] = StratHolder->callStrategy(Players[j]->strategy(), MoveList, j);
		}

		MoveList.push_back(currentMove);
		if (MoveList.size() > MoveBufferSize) {
			MoveList.pop_front();
		}

		calculateMove();
	}
}

void Game::calculateMove() {
	Move** last = MoveList.back();
	for (uint32_t i = 0; i < PlayerNumber - 1; i++) {
		for (uint32_t j = i + 1; j < PlayerNumber; j++) {
			if (last[i][j] == HELP && last[j][i] == HELP) {
				Players[i]->addScore(moveScore::help_help);
				Players[j]->addScore(moveScore::help_help);
			}
			else if (last[i][j] == HELP && last[j][i] == BETRAY) {
				Players[i]->addScore(moveScore::help_betray);
				Players[j]->addScore(moveScore::betray_help);
			}
			else if (last[i][j] == BETRAY && last[j][i] == HELP) {
				Players[i]->addScore(moveScore::betray_help);
				Players[j]->addScore(moveScore::help_betray);
			}
			else {
				Players[i]->addScore(moveScore::betray_betray);
				Players[j]->addScore(moveScore::betray_betray);
			}
		}
	}
}

std::string Game::getInfo() const {
	std::string info = "";
	info += "PLAYER\t\tSCORE\t\tSTRATEGY\n";
	for (uint32_t i = 0; i < PlayerNumber; i++) {
		info += "Player " + std::to_string(i + 1) + ":\t";
		info += std::to_string(Players[i]->score()) + "\t";
		info += StratHolder->getStrategyString(Players[i]->strategy()) + "\n";
	}

	return info;
}

std::string Game::getLastRound() const {
	std::string round;
	round = "   ";
	for (uint32_t i = 0; i < PlayerNumber; i++) {
		round += std::to_string(i + 1) + "  ";
	}
	round += "\n";

	Move** last = MoveList.back();

	for (uint32_t i = 0; i < PlayerNumber; i++) {
		round += std::to_string(i + 1) + "  ";
		for (uint32_t j = 0; j < PlayerNumber; j++) {
			if (i == j)
				round += "N  ";
			else
				round += std::string(last[i][j] == HELP ? "1" : "0") + "  ";
		}
		round += "\n";
	}

	return round;
}







//MENU INTERFACE

enum MenuState {
	MENU,
	PREGAME,
	GAME,
	QUIT
};

namespace strategies {
	std::string pacific = "Pacific";
	std::string agressor = "Agressor";
	std::string changer = "Changer";
	std::string random = "Random";
	std::string repeater = "Repeater";
	std::string voter = "Voter";
}

namespace commands {
	namespace menu {
		std::string newGameCommand = "newGame";
	}
	namespace pregame {
		std::string setPlayers = "setPlayerNumber";
		std::string setStrategy = "setStrategy";
		std::string playerNumber = "playerNumber";
		std::string startGameCommand = "start";
	}
	namespace game {
		std::string roundCommand = "round";
		std::string roundsCommand = "rounds";
		std::string infoCommand = "info";
	}

	std::string helpCommand = "help";
	std::string quitCommand = "quit";
	std::string defaultGame = "!def";
}

class Menu
{
public:
	static void start() {
		Menu menu = Menu();
		menu.stateMachine();
	}
private:
	Menu();

	void stateMachine();

	std::string getCommand();

	void interpretCommand(std::string& command);

	void printAvaiableStrategies();
	void readPlayerNumber();
	void readStrategy();

	uint32_t askRoundNumber();
	void printInfo();

	void quitCommand();
	void printHelp();

	void printCommandError();

	void defaultGame();

	Game* CurrentGame;
	MenuState CurrentState;

	uint32_t PlayerNumber;
	StrategyIdx* PreferredStrategies;
};

Menu::Menu() {
	CurrentGame = nullptr;
	PreferredStrategies = nullptr;
	PlayerNumber = 0;
	CurrentState = MENU;
}

void Menu::stateMachine() {
	while (CurrentState != QUIT) {
		std::string command = getCommand();
		interpretCommand(command);
	}
}

std::string Menu::getCommand() {
	std::cout << "\nEnter the command: ";
	std::string command;
	std::cin >> command;
	return command;
}

void Menu::interpretCommand(std::string& command) {
	if (command == commands::quitCommand) {
		quitCommand();
	}
	else if (command == commands::helpCommand) {
		printHelp();
	}
	else if (command == commands::defaultGame) {
		defaultGame();
	}
	else if (CurrentState == MENU) {
		if (command == commands::menu::newGameCommand) {
			CurrentState = PREGAME;
			printHelp();
		}
		else {
			printCommandError();
		}
	}
	else if (CurrentState == PREGAME) {
		if (command == commands::pregame::setPlayers) {
			readPlayerNumber();
			PreferredStrategies = new StrategyIdx[PlayerNumber];
		}
		else if (command == commands::pregame::setStrategy) {
			readStrategy();
		}
		else if (command == commands::pregame::playerNumber) {
			std::cout << "\tNumber of players: " << PlayerNumber << std::endl;
		}
		else if (command == commands::pregame::startGameCommand) {
			if (PlayerNumber > 0 && PreferredStrategies != nullptr) {
				CurrentGame = new Game(PlayerNumber, PreferredStrategies);
				CurrentState = GAME;
				printHelp();
			}
			else {
				std::cout << "Need to have a number of players and choose strategy first";
			}
		}
		else {
			printCommandError();
		}
	}
	else if (CurrentState == GAME) {
		if (command == commands::game::roundCommand) {
			CurrentGame->rounds();
		}
		else if (command == commands::game::roundsCommand) {
			CurrentGame->rounds(askRoundNumber());
		}
		else if (command == commands::game::infoCommand) {
			printInfo();
		}
		else {
			printCommandError();
		}
	}
}

void Menu::printAvaiableStrategies() {
	std::cout << "\t\tPacific - all moves turned to help" << std::endl;
	std::cout << "\t\tAgressor - all moves turned to betray" << std::endl;
	std::cout << "\t\tChanger - change turn every move" << std::endl;
	std::cout << "\t\tRepeater - repeat previous turn of enemy" << std::endl;
	std::cout << "\t\tVoter - check how much you betray him and decide what turn choose" << std::endl;
	std::cout << "\t\tRandom - random turn every move" << std::endl;
}

void Menu::readPlayerNumber() {
	std::cout << "\tNumber of players: ";
	int32_t plNum;
	std::cin >> plNum;
	if (plNum <= 0) {
		while (plNum <= 0) {
			std::cout << "\tNumber of players must be positive\n";
			std::cout << "\tNumber of players: ";
			std::cin >> plNum;
		}
	}

	PlayerNumber = plNum;
}

void Menu::readStrategy() {
	if (PlayerNumber == 0) {
		std::cout << "\tEnter number of players first\n";
		return;
	}
	std::cout << "\tPlayer Id: ";
	uint32_t plId;
	std::cin >> plId;
	if (plId <= 0) {
		while (plId <= 0) {
			std::cout << "\tPlayer Id must be positive\n";
			std::cout << "\tPlayer Id: ";
			std::cin >> plId;
		}
	}
	plId--;

	std::string strat;
	bool correctStrategy = false;
	std::cout << "\tStrategy: ";
	std::cin >> strat;
	while (!correctStrategy) {
		if (strat == strategies::pacific) {
			PreferredStrategies[plId] = PACIFIC;
			correctStrategy = true;
		}
		else if (strat == strategies::agressor) {
			PreferredStrategies[plId] = AGRESSOR;
			correctStrategy = true;
		}
		else if (strat == strategies::random) {
			PreferredStrategies[plId] = RANDOM;
			correctStrategy = true;
		}
		else if (strat == strategies::voter) {
			PreferredStrategies[plId] = VOTER;
			correctStrategy = true;
		}
		else if (strat == strategies::repeater) {
			PreferredStrategies[plId] = REPEATER;
			correctStrategy = true;
		}
		else if (strat == strategies::changer) {
			PreferredStrategies[plId] = CHANGER;
			correctStrategy = true;
		}
		else {
			std::cout << "\tWrong strategy. Avaiable strategies: " << std::endl;
			printAvaiableStrategies();
			std::cout << "\tStrategy: ";
			std::cin >> strat;
		}
	}
}

uint32_t Menu::askRoundNumber() {
	int32_t rounds = -1;
	std::cout << "\tNumber of rounds: ";
	std::cin >> rounds;
	if (rounds <= 0) {
		while (rounds <= 0) {
			std::cout << "\tNumber of rounds must be positive" << std::endl;
			std::cout << "\tNumber of rounds: ";
			std::cin >> rounds;
		}
	}

	return rounds;
}

void Menu::printInfo() {
	std::cout << CurrentGame->getLastRound() << std::endl;
	std::cout << CurrentGame->getInfo() << std::endl;
}

void Menu::quitCommand() {
	if (CurrentState == MENU) {
		CurrentState = QUIT;
	}
	else if (CurrentState == PREGAME) {
		if (PreferredStrategies != nullptr)
			delete[] PreferredStrategies;
		PlayerNumber = 0;
		CurrentState = MENU;
	}
	else if (CurrentState == GAME) {
		delete CurrentGame;
		delete[] PreferredStrategies;
		CurrentGame = nullptr;
		PreferredStrategies = nullptr;
		CurrentState = MENU;
	}
	printHelp();
}

void Menu::printHelp() {
	std::cout << "Avaiable commands:" << std::endl;
	if (CurrentState == MENU) {
		std::cout << commands::menu::newGameCommand << "; " <<
			commands::quitCommand << "; " <<
			commands::helpCommand << std::endl;
	}
	else if (CurrentState == PREGAME) {
		std::cout << commands::pregame::playerNumber << "; " <<
			commands::pregame::setPlayers << "; " <<
			commands::pregame::setStrategy << "; " <<
			commands::pregame::startGameCommand << "; " <<
			commands::quitCommand << "; " <<
			commands::helpCommand << std::endl;
	}
	else if (CurrentState == GAME) {
		std::cout << commands::game::infoCommand << "; " <<
			commands::game::roundCommand << "; " <<
			commands::game::roundsCommand << "; " <<
			commands::quitCommand << "; " <<
			commands::helpCommand << std::endl;
	}
	else {
		std::cout << "None" << std::endl;
	}
}

void Menu::printCommandError() {
	std::cout << "Invalid command" << std::endl;
	printHelp();
}

void Menu::defaultGame() {
	PlayerNumber = 3;
	delete[] PreferredStrategies;
	PreferredStrategies = new StrategyIdx[PlayerNumber];
	PreferredStrategies[0] = VOTER;
	PreferredStrategies[1] = RANDOM;
	PreferredStrategies[2] = CHANGER;
	delete CurrentGame;
	CurrentGame = new Game(PlayerNumber, PreferredStrategies);
	CurrentState = GAME;
	printHelp();
}

//start
int main() {
	Menu::start();
	return 0;
}