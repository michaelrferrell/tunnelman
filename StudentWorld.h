#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GraphObject.h"
#include "GameController.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
#include <vector>

class StudentWorld : public GameWorld
{
public:
	// constructor
	StudentWorld(std::string assetDir): GameWorld(assetDir), m_tunnelMan(nullptr), m_Actors(NULL){}

	// virtual functions
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	// virtual destructor
	// will call cleanUp since cleanUp frees all dynamically allocated memory
	virtual ~StudentWorld();

	// init function
	// initializes all actors required for the game
	// constructs the map/oil field
	virtual int init();

	// move function
	// updates the status text with latest information
	// acts all actors to do something
	// checks if player is alive after actors do something or if player won
	// deletes any actors that are no longer alive
	// returns either GWSTATUS_PLAYER_DIED, GWSTATUS_CONTINUE_GAME, GWSTATUS_FINISHED_LEVEL
	virtual int move();

	// cleanUp function
	// deletes every actor in the entire field
	// works as a destructor
	virtual void cleanUp();

	// accessor functions
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	// getRouteToExit function
	GraphObject::Direction getRouteToExit(int x, int y) { return routeToExit[x][y]; } // Will return the direction for the protester to move toward the exit

	// getRouteToTunnelMan function
	GraphObject::Direction getRouteToMan(int x, int y) { return routeToMan[x][y]; } // Will return the direction for the protester to move toward the TunnelMan

	// getMovesToTunnelMan function
	int getMovesToTunnelMan(int x, int y) { return numMoves[x][y]; } // Will return number of moves to reach TunnelMan from inputted position

	// getTunnelMan function
	// returns a pointer to the TunnelMan
	TunnelMan* getTunnelMan() { return m_tunnelMan; }

	// helper functions
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	// isValidCord function
	// returns true if x,y coordinates are valid
	// returns false otherwise
	bool isValidCord(int x, int y);

	// dig function
	// will delete the dirt at a point if coordinates are valid
	// will play sound if successful dig
	// if invalid coordinate or earth doesn't exist at point returns without doing anything
	bool dig(int x, int y);

	// removeDeadGameObjects function
	// deletes all actors that died during a tick
	void removeDeadGameObjects();

	// newActor function
	// will add newly created actor to m_Actors
	void newActor(Actor* a) { m_Actors.push_back(a); }


	// setDisplayText function
	// will pass a formatted string to setGameStatText
	// formatted string will include statistics such as level, lives, health, squirts, gold, barrelsLeft, sonar, and score
	void setDisplayText();

	// formatHelper function
	// will format the statistics into one string matching the example
	std::string formatHelper(int level, int lives, int health, int squirts, int gold, int barrelsLeft, int sonar, int score);

	// foundOil function
	// will decrement numBarrels by one
	void foundOil() { numBarrels--; }

	// findProperPosition function
	// will find a position to place a game object
	// ensures that within a radius of 6 of position there exists no other game objects
	void findProperPosition(int& x, int& y, bool isBoulder, bool isWaterPool);

	// checkStability function
	// returns true if there is at least one earth object in the 4 squares below a given position
	// otherwise returns false
	bool checkStability(int x, int y);

	// addWaterHelper function
	// checks if the 4x4 grid at an inputted position is free of Earth
	// Returns true if so and false otherwise
	bool addWaterHelper(int x, int y);

	// illuminate function
	// All hidden Gold or Barrel objects within radius 12 of inputted coordinates will be made visible
	void illuminate(int x, int y);

	// dropGold function
	// when called TunnelMan will drop a gold object onto the oil field that is pickupable by protesters
	void dropGold(int x, int y);

	// isOccupied function
	// Will return true if the location inputted is occupied by a boulder or Earth
	bool isOccupied(int x, int y);

	// withinBoulderRadius
	bool withinBoulderRadius(int x, int y);

	// boulderOnBoulder
	// Will return true if one boulder is within radius 3 of another
	bool boulderOnBoulder(Boulder* b);

	// addEarthHelper
	// will check the 4x4 square of an inputted position for a boulder
	// will return false if so
	bool addEarthHelper(int x, int y);

	// buriedHelper
	// will check if a given position starts buried underneath earth
	// will return true if so
	bool buriedHelper(int x, int y);

	// annoyProtesterHelper
	// annoys all protesters within a radius of 3 of inputted coordinates
	// will return true if it annoyed at least one and false if not
	bool annoyProtestersHelper(int amount, int x, int y);

	// briberyHelper
	// if a Protester is within a radius of 3 an inputted position will return a pointer to that Protester
	Actor* briberyHelper(int x, int y);

	// accessEarth
	// program has been crashing trying to access row 60 of array which doesn't exist so will fix by creating a function to attempt to access blocks of earth rather than through the array
	Earth* accessEarth(int x, int y);

	// mazeSolver function
	// Will fill inputted 2D array with directions towards inputted position
	// map[i][j] will return the optimal direction to move to reach (x,y)
	void mazeSolver(GraphObject::Direction map[64][64], int x, int y, bool predator); 

	// decrementProtesterCount function
	// decrements ProtestersAlive by one
	void decrementProtesterCount() { ProtestersAlive--; }

private:
	struct node {	// struct that is a point node that holds both a x and y coordinate
		node(int x, int y) : m_x(x), m_y(y){}
		int m_x;
		int m_y;
	};

	TunnelMan* m_tunnelMan; // pointer to tunnel man object
	Earth* m_Earth[60][64]; // 2d array of pointers to all earth objects
	std::vector<Actor*> m_Actors; // vector of pointer to all actors other than tunnel man and earth
	GraphObject::Direction routeToExit[64][64]; // 2D array of routes to the exit (60, 60)
	GraphObject::Direction routeToMan[64][64]; // 2D array of routes to the TunnelMan (60, 60)
	int numMoves[64][64]; // 2D array of number of moves to TunnelMan
	int tick; // an int that will return the current tick of the game
	int numBarrels; // number of barrels of oil left in the current level
	int targetProtesters; // number of target protesters per current level
	int ProtestersAlive;
	int ProtesterSpawnDelay; // number of ticks that must pass before a new Protester can be added
};

#endif // STUDENTWORLD_H_
