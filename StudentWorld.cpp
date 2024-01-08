#include "StudentWorld.h"
#include "GameController.h"
#include <string>
#include <queue>
#include <sstream>
#include <iomanip>
#include <iterator>
#include <algorithm>
#include <cstdlib>
using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

// Memory Freeing Functions
///////////////////////////////////////////////////////////////////////////////////////////////////////////

// virtualDestructor
// Will do the same thing as cleanUp other than reset tick to 0
StudentWorld::~StudentWorld() {
	delete m_tunnelMan; // deletes tunnel man

	// iterate through m_Actors and delete all actors
	vector<Actor*>::iterator it;
	it = m_Actors.begin();
	while (it != m_Actors.end()) {
		delete* it;
		it++;
	}
	m_Actors.clear(); // clear m_Actors

	// iterates through 2d array of earth pointers
	// if not nullptr will delete earth object
	for (int y = 0; y < 60; y++) {
		for (int x = 0; x < 64; x++) {
			if (m_Earth[y][x] != nullptr) {
				delete m_Earth[y][x];
			}
		}
	}
}

// cleanUp function
// deletes tunnelMan
// deletes all earth objects
// deletes all remaining actors
 void StudentWorld::cleanUp() {
	tick = 0; // reset tick to 0
	delete m_tunnelMan; // deletes tunnel man
	
	// iterate through m_Actors and delete all actors
	vector<Actor*>::iterator it;
	it = m_Actors.begin();
	while (it != m_Actors.end()) {
		delete* it;
		it++;
	}
	m_Actors.clear(); // clear m_Actors

	// iterates through 2d array of earth pointers
	// if not nullptr will delete earth object
	for (int y = 0; y < 60; y++) {
		for (int x = 0; x < 64; x++) {
			if (m_Earth[y][x] != nullptr) {
				delete m_Earth[y][x];
			}
		}
	}
}

// removeDeadGameObjects function
 // will delete all dead actors and free up memory
 void StudentWorld::removeDeadGameObjects() {
	 // remove earths
	 // iterate through m_Earth
	 // if not nullptr and not alive will delete
	 for (int y = 0; y < 60; y++) {
		 for (int x = 0; x < 64; x++) {
			 if (m_Earth[y][x]!=nullptr && !m_Earth[y][x]->isAlive()) {
				 delete m_Earth[y][x];
				 m_Earth[y][x] = nullptr; // sets dangling pointers to nullptr
			 }
		 }
	 }

	 // remove dead actors
	 // iterator through m_Actors
	 // if not nullptr and not alive will delete and update the vector
	 vector<Actor*>::iterator it;
	 it = m_Actors.begin();
	 while (it != m_Actors.end()) {
		 if (*it!=nullptr && !(*it)->isAlive()) { 
			 if ((*it)->isProtester()) { // if deleting a protester will decrement the count
				 decrementProtesterCount();
			 }
			 delete* it; // delete that actor
			 it = m_Actors.erase(it); // shrink the vector and increment
		 }
		 else {
			 it++; // otherwise increment and go to the next element
		 }
	 }
	 return;
 }

 // Display Text Functions
 ///////////////////////////////////////////////////////////////////////////////////////////////////////////
 
 // formatHelper function
 // will format statistics into one string
 
 
 string StudentWorld::formatHelper(int level, int lives, int health, int squirts, int gold, int barrelsLeft, int sonar, int score) {
	 string neat;
	 // uses stringstream to format the display text

	 // format level
	 // 2 digits long with leading spaces
	 ostringstream lvl;
	 lvl.fill(' ');
	 lvl<<setw(2)<<level;
	 neat += "Lvl: ";
	 neat += lvl.str();

	 // format lives
	 // 1 digits long with leading spaces
	 ostringstream live;
	 live << lives;
	 neat += " Lives: ";
	 neat += live.str();

	 // format health
	 // 3 digits long with leading spaces
	 ostringstream hlth;
	 hlth.fill(' ');
	 hlth << setw(3) << health;
	 neat += " Hlth: ";
	 neat += hlth.str();
	 neat += "%";

	 // format water
	 // 2 digits long with leading spaces
	 ostringstream water;
	 water.fill(' ');
	 water << setw(2) << squirts;
	 neat += " Wtr: ";
	 neat += water.str();

	 // format gold
	 // 2 digits long with leading spaces
	 ostringstream gld;
	 gld.fill(' ');
	 gld << setw(2) << gold;
	 neat += " Gld: ";
	 neat += gld.str();
	 
	 // format oil
	 // 2 digits long with leading spaces
	 ostringstream oil;
	 oil.fill(' ');
	 oil << setw(2) << barrelsLeft;
	 neat += " Oil Left: ";
	 neat += oil.str();
	 
	 // format sonar
	 // 2 digits long with leading spaces
	 ostringstream son;
	 son.fill(' ');
	 son << setw(2) << sonar;
	 neat += " Sonar: ";
	 neat += son.str();

	 // format score
	 // 6 digits long with leading zeroes
	 ostringstream scr;
	 scr.fill('0');
	 scr << setw(6) << score;
	 neat += " Scr: ";
	 neat += scr.str();
	 return neat;
 }
 

 // setDisplayText function
 // will update GameStatText through setGameStatText()
 void StudentWorld::setDisplayText() {
	 // retrieve all statistics as of the current tick
	 int level = getLevel();
	 int lives = getLives();
	 int score = getScore();
	 int health = m_tunnelMan->getHealth()*10; // will return the percent of health left
	 int squirts = m_tunnelMan->getSquirts();
	 int gold = getTunnelMan()->getGold();
	 int barrelsLeft = numBarrels;
	 int sonar = getTunnelMan()->getSonar();

	 string s = formatHelper(level, lives, health, squirts, gold, barrelsLeft, sonar, score); // will format statistics into one string
	 setGameStatText(s); // will pass formatted statistics into setGameStatText so it will be displayed on the screen
 }

 // init
///////////////////////////////////////////////////////////////////////////////////////////////////////////

 // buriedHelper function
 // checks if inputted position is not within the mineshaft
 // from (0,0) to (25,59) ; (30,0) to (33,4); (34,0) to (63,59)
 // Returns true if so
 bool StudentWorld::buriedHelper(int x, int y) {
	 if ((x >= 0 && x < 26) && (y >= 0 && y < 60)) {
		 return true;
	 }
	 if ((x >= 26 && x < 34) && (y >= 0 && y < 5)) {
		 return true;
	 }
	 if ((x >= 34 && x < 64) && (y >= 0 && y < 60)) {
		 return true;
	 }
	 return false; // otherwise return false
 }


 // addEarthHelper function
 // will check the 4x4 grid at an inputted position for a boulder and return false if so
 bool StudentWorld::addEarthHelper(int x, int y) {
	 for (int i = 0; i < 4; i++) {
		 for (int j = 0; j < 4; j++) {
			 if (isValidCord(x - i, y - j)) {
				 vector<Actor*>::iterator it; // Have to iterate through actors to access each boulder
				 it = m_Actors.begin();
				 while (it != m_Actors.end()) {
					 if ((*it)->isBoulder() && (*it)->isAlive()) {
						 if (x - i == (*it)->getX() && y - j == (*it)->getY()) { // Check if there exists a boulder within the 4x4 grid of an inputted position
							 return false;
						 }
					 }
					 it++; // increment iterator
				 } // end while
			 } // end if
		 }
	 }
	 return true; // can add earth at inputted position
 }


 // findProperPosition function
 // will update input integers to be proper coordinates to add a game object to
 void StudentWorld::findProperPosition(int& x, int& y, bool isBoulder, bool isWaterPool) {
	 bool isProper = false;
	 int tempX;
	 int tempY;
		 while (isProper == false) {
			 tempX = rand() % 61;
			 if (isBoulder) { // If boulder y range is 20 to 56 inclusive
				 tempY = 20+rand() % 37;
			 }
			 else { tempY = rand() % 57; // If not boulder y range is 0 to 56 inclusive
			 }
			 
			 // Now check if these coordinates are within radius 6 of any other game object
			 // First check if m_Actors is empty
			 // If so updates x and y and exits preventing infinte loop
			 if (m_Actors.empty()) {
				 if (isWaterPool && addWaterHelper(tempX, tempY)) { // Checks if game object is a WaterPool and if so checks that there's no Earth objects in its 4x4 grid
					 x = tempX;
					 y = tempY;
					 return;
				 }
				 else if (!isWaterPool) { // If not WaterPool updates x and y and exits
					 x = tempX;
					 y = tempY;
					 return;
				 }
			 }
			 
			 // Otherwise iterates through actors
			 else {
				 vector<Actor*>::iterator it;
				 it = m_Actors.begin();
				 while (it != m_Actors.end()) { // iterate through list of actors
					 if ((*it)->getRadius(tempX, tempY, (*it)->getX(), (*it)->getY()) > 6) { // if radius is greater than 6 set isProper to true and increment it
						 isProper = true;
						 it++;
					 }
					 else { // otherwise set isProper to false and break
						 isProper = false;
						 break;
					 }
				 }
				 // Check if isProper is true
				 if (isProper) {
					 // If so check if isWaterPool and addWaterHelper are true
					 // Will update x and y and exit if so
					 if (isWaterPool && addWaterHelper(tempX, tempY)) {
						 x = tempX;
						 y = tempY;
						 return;
					 }
					 // Will update x and y and exit if so
					 // Otherwise check if not isWaterPool and not in the middle shaft or the top
					 else if (!isWaterPool && buriedHelper(tempX, tempY)) {
						 x = tempX;
						 y = tempY;
						 return;
					 }
					 // If it reaches here it means that it's a WaterPool and addWaterHelper is false so sets isProper to false and continues to try to find proper coordinates
					 else {
						 isProper = false;
					 }
				 }
			 } // end else
		 } // end while
 }


// init function
// initializes all data structures that track the world
// constructs the oil field
// initalizes all actors
 int StudentWorld::init() {
	 // first initializes humans
	 m_tunnelMan = new TunnelMan(this);

	 // initializies non-human and non-earth objects
	 int currentLevel = getLevel();

	 // set targetProtesters
	 ProtestersAlive = 0;
	 targetProtesters = min(15, int(2 + currentLevel*1.5));
	 ProtesterSpawnDelay = max(25, 200 - currentLevel);

	 // initialize Barrel objects
	 int L = min(2+currentLevel, 21); // amount of barrel objects to make for current level
	 numBarrels = L; // initalize numBarrels to equal L
	 for (int i = 0; i < numBarrels; i++) {
		 int x = 20;
		 int y = 20;
		 findProperPosition(x, y, false, false); // Find proper position for non boulder object
		 newActor(new Barrel(this, x, y)); // Creates new object and adds to m_Actors
	 }

	 // initialize Gold objects
	 int G = max(5 - currentLevel / 2, 2);
	 for (int i = 0; i < G; i++) {
		 int x = 20;
		 int y = 28;
		 findProperPosition(x, y, false, false); // Find proper position for non boulder object
		 newActor(new Gold(this, x, y, false)); // Creates new object and adds to m_Actors
	 }

	 // initialize Boulder objects
	 int B = min(currentLevel / 2 + 2, 9);
	 for (int i = 0; i < B; i++) {
		 int x = 20;
		 int y = 28;
		 findProperPosition(x, y, true, false); // Find proper position for boulder object
		 newActor(new Boulder(this, x, y)); // Creates new object and adds to m_Actors
	 }
	 

	 // initializes earth objects
	 // from (0,0) to (29,59) ; (30,0) to (33,4); (34,0) to (63,59)
	 // has to check if the position is occupied by a non-earth object

	 for (int y = 0; y < 60; y++) {
		 for (int x = 0; x < 30; x++) {
			 if (addEarthHelper(x, y)) {
				 m_Earth[y][x] = new Earth(this, x, y);
			 }
			 else {
				 m_Earth[y][x] = nullptr;
			 }
		 }
	 }
	 for (int y = 0; y < 4; y++) {
		 for (int x = 30; x < 34; x++) {
			 if (addEarthHelper(x, y)) {
				 m_Earth[y][x] = new Earth(this, x, y);
			 }
			 else {
				 m_Earth[y][x] = nullptr;
			 }
		 }
	 }
	 for (int y = 0; y < 60; y++) {
		 for (int x = 34; x < 64; x++) {
			 if (addEarthHelper(x, y)) {
				 m_Earth[y][x] = new Earth(this, x, y);
			 }
			 else {
				 m_Earth[y][x] = nullptr;
			 }
		 }
	 }


	 return GWSTATUS_CONTINUE_GAME;
 }

 // move
 ///////////////////////////////////////////////////////////////////////////////////////////////////////////

 // move function
 int StudentWorld::move() {
	 tick++; // increment tick

	 // check if tunnel man is alive
	 // if not return died status
	 if (!m_tunnelMan->isAlive()) {
		 return GWSTATUS_PLAYER_DIED;
	 }

	 // if still alive
	 // update the display text
	 setDisplayText();

	 // allow each actor to do something
	 mazeSolver(routeToExit, 60, 60, false); // generate all possible routes from non occupied positions to the exit
	 mazeSolver(routeToMan, m_tunnelMan->getX(), m_tunnelMan->getY(), true); // generate all possible routes to get to tunnelMan

	 m_tunnelMan->doSomething();
	 vector<Actor*>::iterator it;
	 it = m_Actors.begin();
	 while (it != m_Actors.end()) {
		 if ((*it)->isAlive()) { // check if actor is alive
			 (*it)->doSomething(); // if so will have it do something
		 }
		 it++;
	 }

	 // remove dead actors
	 removeDeadGameObjects();

	 // check if player died during tick
	 // if so returns death notifcation
	 if (!m_tunnelMan->isAlive()) {
		 return GWSTATUS_PLAYER_DIED;
	 }

	 // check if player collected all barrels of oil
	 // if so returns finished level notifcation
	 if (numBarrels == 0) {
		 playSound(SOUND_FINISHED_LEVEL);
		 return GWSTATUS_FINISHED_LEVEL;
	 }

	 // Add protesters to the world
	 int probHardCore = min(90, int(getLevel()) * 10 + 30);
	 bool addHardCore = rand() % 100 < probHardCore;
	 if (tick == 1) { // Always add one during the first tick
		 ProtestersAlive++;
		 ProtesterSpawnDelay = max(25, 200 - int(getLevel()));
		 if (addHardCore) {
			 newActor(new HardCoreProtester(this));
		 }
		 else {
			 newActor(new RegularProtester(this));
		 }
	 }
	 
	 else if (ProtesterSpawnDelay <= 0 && ProtestersAlive < targetProtesters) { // Check if its time to add a new one and there is a vacant spot to add one
		 ProtestersAlive++;
		 ProtesterSpawnDelay = max(25, 200 - int(getLevel()));
		 if (addHardCore) {
			 newActor(new HardCoreProtester(this));
		 }
		 else {
			 newActor(new RegularProtester(this));
		 }
	 }
	 else {
		 ProtesterSpawnDelay--;
	 }

	 // Add goodies to world
	 int currentLevel = getLevel();
	 int G = currentLevel * 25 + 300;
	 if (rand() % G == 1) { // Chances of adding a goodie in a single tick are 1/G
		 if (rand() % 5 == 1) { // 1/5 chance of adding a new SonarKit
			 int tickDuration = max(100, 300 - 10 * currentLevel);
			 newActor(new SonarKit(this, tickDuration));
		 }
		 else { // 4/5 chance of adding a new WaterPool
			 int tickDuration = max(100, 300 - 10 * currentLevel);
			 int x = 20;
			 int y = 28;
			 findProperPosition(x, y, false, true); // Find proper position for WaterPool object
			 newActor(new WaterPool(this, x, y, tickDuration)); // Creates new object and adds to m_Actors
		 }
	 }


	 // if tunnelMan is still alive return continue notification
	 if (m_tunnelMan->isAlive()) return GWSTATUS_CONTINUE_GAME;
 }

 // TunnelMan helper functions
 ///////////////////////////////////////////////////////////////////////////////////////////////////////////

 // isValidCord function
 // valid cordinates are from (0,0) to (63,63) inclusive
 bool StudentWorld::isValidCord(int x, int y) {
	 // return true if inside valid coordinates
	 if ((x >= 0 && x < 64) && (y >= 0 && y < 60)) return true;
	 // otherwise return false
	 return false;
 }

 // dig function
 bool StudentWorld::dig(int x, int y) {
	 bool dig = false;
	 for (int i = 0; i < 4; i++) {
		 for (int j = 0; j < 4; j++) {
			 if (isValidCord(x+i, y+j) && (accessEarth(x+i, y+j) != nullptr)) {
				 // if valid position will delete the earth at that point
				 if (m_Earth[y + j][x + i]->isAlive()) {
					 m_Earth[y + j][x + i]->setDead();
					 dig = true;
				 }
			 }
		 }
	 }
	 return dig;
}

 // illuminate function
 void StudentWorld::illuminate(int x, int y) {
	 // iterate through actors and if within radius 12 of inputted coordiantes and not already visible will be made visible
	 vector<Actor*>::iterator it;
	 it = m_Actors.begin();
	 while (it != m_Actors.end()) {
		 if (*it != nullptr && (*it)->isAlive()) {
			 if (getTunnelMan()->getRadius(x, y, (*it)->getX(), (*it)->getY())<=12 && !(*it)->isVisible()) {
				 (*it)->setVisible(true); // set game object to true if it passes condition
			 }
		 } // end if
		 it++; // increment iterator
	 } // end while


 }

 // dropGold function
 void StudentWorld::dropGold(int x, int y) {
	 newActor(new Gold(this, x, y, true)); // adds a Gold object at inputted coordinates in temporary state
 }

 // Boulder helper functions
 ///////////////////////////////////////////////////////////////////////////////////////////////////////////
 
 // checkStability function
 // returns true if there exists 4 earth objects directly underneath an inputted coordiante
 bool StudentWorld::checkStability(int x, int y) {
	 if (y > 0) { // added if here to prevent StudentWorld from accessing Earth at index -1
		 // check from y-1, x to x+3 inclusive
		 // If an earth object exists and is alive will return true
		 for (int i = 0;i < 4;i++) { 
			 if (accessEarth(x + i, y - 1) != nullptr && m_Earth[y - 1][x + i]->isAlive()) {
				 return true;
			 }
		 }
	 }
	 // Otherwise will return false
	 return false;
}

 // boulderOnBoulder function
 // returns true if a boulder is within radius 3 of another boulder
 // returns false otherwise
 bool StudentWorld::boulderOnBoulder(Boulder* b) {
	 int currentX = b->getX();
	 int currentY = b->getY();

	 vector<Actor*>::iterator it; // Have to iterate through actors to access each boulder
	 it = m_Actors.begin();
	 while (it != m_Actors.end()) {
		 if (((*it)->isBoulder() && (*it)->isAlive()) && (*it)!=b) { // Check if is boulder and is alive and is not the boulder that is falling
			 if ((*it)->getRadius(currentX, currentY-1, (*it)->getX(), (*it)->getY()) <= 3) { // Check if boulder is within radius 3 of boulder
				 return true; // return true if so
			 }
		 }
		 it++; // increment it
	 }
	 return false; // returns false otherwise
 }

 // WaterPool helper functions
 ///////////////////////////////////////////////////////////////////////////////////////////////////////////
 
 // addWaterBoulder function
 // returns true if can add WaterPool or Boulder at the inputted position
 bool StudentWorld::addWaterHelper(int x, int y) {
	 for (int i = 0; i < 4; i++) {
		 for (int j = 0; j < 4; j++) {
			 if (isValidCord(x + i, y + j) && (accessEarth(x + i, y + j) != nullptr)) {
				 if (m_Earth[y + j][x + i]->isAlive()) {
					 return false; // returns false if an Earth object exists within the 4x4 grid
				 }
			 }
		 }
	 }
	 return true; // if no Earth within grid return true
 }

 // Squirt helper functions
 ///////////////////////////////////////////////////////////////////////////////////////////////////////////

 // isOccupied function
 // returns true if coordiante inputted is occupied by Earth or a Boulder
 bool StudentWorld::isOccupied(int x, int y) {
	 // First check if occupied by Earth
	 if ((x >= 0 && x < 64) && (y >= 0 && y < 60)) { // Ensures does not try to access outside of 2d array of earth objects
		 for (int i = 0; i < 4; i++) { // Need to check the 4x4 square of pixels
			 for (int j = 0; j < 4; j++) {
				 if (accessEarth(x + i, y + j) != nullptr && m_Earth[y + j][x + i]->isAlive()) {
					 return true;
				 }
			 } // end inner for
		 } // end outer for
	 } // end if
	 // Next check if occupied by Boulder
	 return withinBoulderRadius(x, y);
 }

 // withinBoulderRadius function
 // will return true if actor is within radius 3 of a boulder
 bool StudentWorld::withinBoulderRadius(int x, int y) {
	 vector<Actor*>::iterator it; // Have to iterate through actors to access each boulder
	 it = m_Actors.begin();
	 while (it != m_Actors.end()) {
		 if ((*it)->isBoulder() && (*it)->isAlive()) { // Check if is boulder and is alive
			 if ((*it)->getRadius(x, y, (*it)->getX(), (*it)->getY()) <= 3) { // Check if actor is within radius 3 of boulder
				 return true; // return true if so
			 }
		 }
		 it++; // increment it
	 }
	 return false; // returns false otherwise
 }

 // Protester helper functions
 ///////////////////////////////////////////////////////////////////////////////////////////////////////////
 // annoyProtestersHelper function
 bool StudentWorld::annoyProtestersHelper(int amount, int x, int y) {
	 bool annoyedOne = false;
	 vector<Actor*>::iterator it; // Have to iterate through actors to access each protester
	 it = m_Actors.begin();

	 while (it != m_Actors.end()) {
		 if ((*it)->isProtester() && (*it)->isAlive()) { // Check if is protester and is alive
			 if ((*it)->getRadius(x, y, (*it)->getX(), (*it)->getY()) <= 3) { // Check if protester is within radius 3 of coordiantes
				 (*it)->getAnnoyed(amount);
				 annoyedOne = true;
			 }
		 }
		 it++; // increment it
	 }
	 return annoyedOne; // returns false otherwise
 }

 // briberyHelper function
 Actor* StudentWorld::briberyHelper(int x, int y) {
	 Protester* homie = nullptr;
	 vector<Actor*>::iterator it; // Have to iterate through actors to access each protester
	 it = m_Actors.begin();

	 while (it != m_Actors.end()) {
		 if ((*it)->isProtester() && (*it)->isAlive()) { // Check if is protester and is alive
			 if ((*it)->getRadius(x, y, (*it)->getX(), (*it)->getY()) <= 3) { // Check if protester is within radius 3 of coordiantes
				 return *it;
			 }
		 }
		 it++; // increment it
	 }
	 return nullptr; // otherwise returns nullptr
 }

 // accessEarth
 // either will return a pointer to the the earth object at inputted position or nullptr if coordinates are out of the array
 Earth* StudentWorld::accessEarth(int x, int y) {
	 if ((x >= 0 && x <= 63) && (y >= 0 && y <= 59)) // ensures that attempting to access y=60 will never crash the program
		 return m_Earth[y][x];
	 else
		 return nullptr;
 }

 // mazeSolver function
 // will fill each point in the 2d array with the optimal direction to move to reach (x,y) if it exists
 void StudentWorld::mazeSolver(GraphObject::Direction directions[64][64], int startX, int startY, bool predator) {
	 // initalize directions each times called
	 // each tick game world will change and that means the optimal route to the exit may change
	 for (int i = 0; i < 64; i++) {
		 for (int j = 0; j < 64; j++) {
			 directions[i][j] = GraphObject::Direction::none; // initalize each position as none
		 }
	 }
	 // also have to initalize numMoves 
	 if (predator) {
		 for (int i = 0; i < 64; i++) {
			 for (int j = 0; j < 64; j++) {
				 numMoves[i][j] = -1; // initalize each position as none
			 }
		 }
	 }

	 // Will use a queue to find the optimal route for each position in the game world
	 queue<node> storage;
	 node start(startX, startY); // will start from the desired end position and find all possible routes to each position in the game world if they exist
	 storage.push(start); // insert starting position into queue
	 node current(0, 0); 
	 int count = 0;
	 while (!storage.empty()) { // while queue is not empty
		 current = storage.front();
		 storage.pop(); // remove front node from the queue
		 int x = current.m_x;
		 int y = current.m_y;
		 
		 // Check left
		 if (m_tunnelMan->isValidMove(x - 1, y) && !isOccupied(x - 1, y) && directions[x - 1][y] == GraphObject::Direction::none) { // Must be a valid move to a position unoccupied by Earth or a boulder, that hasn't been visited already
			 storage.push(node(x - 1, y));
			 directions[x - 1][y] = GraphObject::Direction::right; // Since working backwards from ending position return opposite direction
			 if (predator) { // if sensing player will count moves from each position
				 numMoves[x - 1][y] = numMoves[x][y] + 1;
			 }
		 }
		 // Check right
		 if (m_tunnelMan->isValidMove(x + 1, y) && !isOccupied(x + 1, y) && directions[x + 1][y] == GraphObject::Direction::none) { // Must be a valid move to a position unoccupied by Earth or a boulder, that hasn't been visited already
			 storage.push(node(x + 1, y));
			 directions[x + 1][y] = GraphObject::Direction::left; // Since working backwards from ending position return opposite direction
			 if (predator) { // if sensing player will count moves from each position
				 numMoves[x + 1][y] = numMoves[x][y] + 1;
			 }
		 }

		 // Check down
		 if (m_tunnelMan->isValidMove(x, y-1) && !isOccupied(x, y-1) && directions[x][y-1] == GraphObject::Direction::none) { // Must be a valid move to a position unoccupied by Earth or a boulder, that hasn't been visited already
			 storage.push(node(x, y-1));
			 directions[x][y-1] = GraphObject::Direction::up; // Since working backwards from ending position return opposite direction
			 if (predator) { // if sensing player will count moves from each position
				 numMoves[x][y-1] = numMoves[x][y] + 1;
			 }
		 }
		 // Check up
		 if (m_tunnelMan->isValidMove(x, y + 1) && !isOccupied(x, y + 1) && directions[x][y + 1] == GraphObject::Direction::none) { // Must be a valid move to a position unoccupied by Earth or a boulder, that hasn't been visited already
			 storage.push(node(x, y + 1));
			 directions[x][y + 1] = GraphObject::Direction::down; // Since working backwards from ending position return opposite direction
			 if (predator) { // if sensing player will count moves from each position
				 numMoves[x][y + 1] = numMoves[x][y] + 1;
			 }
		 }
	 }
 }
