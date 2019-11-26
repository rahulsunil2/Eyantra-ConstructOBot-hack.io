#include<iostream>
#include<conio.h>
/*
 * Coordinate List
 * First Row -> nodename
 * Second Row -> x-coordinate
 * Third Row -> y-cordinate
 * Index -> [nodeName][x][y]
 */
int coordinates[3][16] = {
	{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
	{2, 1, 1, 1, 1, 1, 1, 1, 2, 3,  3,  3,  3,  3,  3,  3},
	{1, 1, 2, 3, 4, 5, 6, 7, 7, 7,  6,  5,  4,  3,  2,  1}
};

/*
 * Distance Graph
 * 16 Rows and 16 Columns for every node and the distance between them.
 * Index -> [Source][Target]
 * Eg: distance_graph[5][6] -> gives the shortest distance between 5 and 6.
 */
int distance_graph[16][16] = {
	{0, 1, 2, 3, 4, 5, 6, 7, 8, 7, 6, 5, 4, 3, 2, 1},
	{1, 0, 1, 2, 3, 4, 5, 6, 7, 7, 6, 5, 5, 4, 3, 2},
	{2, 1, 0, 1, 2, 3, 4, 5, 6, 6, 5, 4, 5, 5, 4, 3},
	{3, 2, 1, 0, 1, 2, 3, 4, 5, 5, 4, 3, 4, 5, 5, 4},
	{4, 3, 2, 1, 0, 1, 2, 3, 4, 4, 3, 2, 3, 4, 5, 5},
	{5, 4, 3, 2, 1, 0, 1, 2, 3, 3, 2, 1, 2, 3, 4, 5},
	{6, 5, 4, 3, 2, 1, 0, 1, 2, 3, 3, 2, 3, 4, 5, 6},
	{7, 6, 5, 4, 3, 2, 1, 0, 1, 2, 3, 3, 4, 5, 6, 7},
	{8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 2, 3, 4, 5, 6, 7},
	{7, 7, 6, 5, 4, 3, 3, 2, 1, 0, 1, 2, 3, 4, 5, 6},
	{6, 6 ,5, 4, 3, 2, 3, 3, 2, 1, 0, 1, 2, 3, 4, 5},
	{5, 5, 4, 3, 2, 1, 2, 3, 3, 2, 1, 0, 1, 2, 3, 4},
	{4, 5, 5, 4, 3, 2, 3, 4, 4, 3, 2, 1, 0, 1, 2, 3},
	{3, 4, 5, 5, 4, 3, 4, 5, 5, 4, 3, 2, 1, 0, 1, 2},
	{2, 3, 4, 5, 5, 4, 5, 6, 6, 5, 4, 3, 2, 1, 0, 1},
	{1, 2, 3, 4, 5, 5, 6, 7, 7, 6, 5, 4, 3, 2, 1, 0}
};

/*
 * Adjacency List
 * Index -> [nodename][nearestNode]
 * Eg: adjacency_list[3] -> gives the nearest nodes of 3.
 */
int adjacency_list[16][4] = {
	{ 1, 15, -1, -1}, { 0,  2, -1, -1}, { 1,  3, -1, -1}, { 2,  4, -1, -1},
	{ 3,  5, -1, -1}, { 4,  6, 11, -1}, { 5,  7, -1, -1}, { 6,  8, -1, -1},
	{ 7,  9, -1, -1}, { 8, 10, -1, -1}, { 9, 11, -1, -1}, { 5, 10, 12, -1},
	{11, 13, -1, -1}, {12, 14, -1, -1}, {13, 15, -1, -1}, {14,  0, -1, -1}
};

/*
 * Movement Array
 * Index -> [movements]
 * Eg: movement_array -> gives the movements the bot should make to reach the target.
 */
int movement_array[16] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};

/*
 * Bot Orientation
 * North -> 1, East -> 2, South -> 3, West -> 4
 * The bot is initially facing South(3) from node0 after moving forward from START.
 */
int botOrientation = 3;

/*
 *
 * Function Name: nextNode
 * Input: currentNodeArray, targetNode
 * Output: minNode
 * Logic: Finds the nextNode the bot should move to reach the targetNode in the shortest way possible.
 * Example Call: nextNode({1, 15, -1, -1}, 3} -> it returns 1 as the bot should move towards node1 next to reach node3.
 *
 */
int nextNode(int currentNodeArray[], int targetNode) {
	int i = 0, minNode;
	int minDistance = 100;
	while (currentNodeArray[i] != -1) {
		int distance = distance_graph[currentNodeArray[i]][targetNode];
		if (distance < minDistance) {
			minDistance = distance;
			minNode = currentNodeArray[i];
		}
		i++;
	}
	return minNode;
}

/*
 *
 * Function Name: botMover
 * Input: currentNode, nextNode
 * Output: void
 * Logic: Updates the botMover array based path guiding it to move forward by how many nodes and to take proper turns.
 * 1 -> forward_wls(1)
 * 2 -> left_turn_wls()
 * 3 -> right_turn_wls()
 * 
 * Example Call: botMover(1, 2) -> if the bot is facing towards North at node1, then it moves forward to node2.
 *
 */
void botMover(int currentNode, int nextNode)
{

	int x = (coordinates[1][nextNode] - coordinates[1][currentNode]),
		y = (coordinates[2][nextNode] - coordinates[2][currentNode]);

	if (
		(currentNode == 1 && nextNode == 2) || (currentNode == 15 && nextNode == 0) ||
		(currentNode == 9 && nextNode == 10) || (currentNode == 7 && nextNode == 8)){
		printf("CASE 1");
		right_turn_wls();
		forward_wls(1);
	}
	else if (
		(currentNode == 1 && nextNode == 0) || (currentNode == 15 && nextNode == 14) ||
		(currentNode == 9 && nextNode == 8) || (currentNode == 7 && nextNode == 6)){
		printf("CASE 2");
		left_turn_wls();
		forward_wls(1);
	}

	else if (
		(y > 0 && botOrientation == 1) || (y < 0 && botOrientation == 3) ||
		(x > 0 && botOrientation == 2) || (x < 0 && botOrientation == 4)){
		printf("CASE 3");
		forward_wls(1);
	}
	else if (
		(y > 0 && botOrientation == 2) || (y < 0 && botOrientation == 4) ||
		(x > 0 && botOrientation == 3) || (x < 0 && botOrientation == 1)){
		printf("CASE 4");
		left_turn_wls();
		forward_wls(1);
	}
	else if (
		(y > 0 && botOrientation == 3) || (y < 0 && botOrientation == 1) ||
		(x > 0 && botOrientation == 4) || (x < 0 && botOrientation == 2)){
		printf("CASE 5");
		left_turn_wls();
		left_turn_wls();
		forward_wls(1);
	}
	else if (
		(y > 0 && botOrientation == 4) || (y < 0 && botOrientation == 2) ||
		(x > 0 && botOrientation == 1) || (x < 0 && botOrientation == 3)){
		printf("CASE 6");
		right_turn_wls();
		forward_wls(1);
	}

	if (y > 0)
		botOrientation = 1;
	else if (y < 0)
		botOrientation = 3;
	else if (x > 0)
		botOrientation = 2;
	else if(x < 0)
		botOrientation = 4;
	
}

/*
 *
 * Function Name: pathPlanning
 * Input: sourceNode, targetNode
 * Output: void
 * Logic: Updates the movement_array based on the shortest path that the bot should take.
 * Example Call: nextNode(1, 3} -> It updates the movement_array to {2, 3} which means move to node2 and then to node3.
 *
 */
void pathPlanning(int sourceNode, int targetNode)
	{
		for (int i = 0; i < 16; i++)
			movement_array[i] = -1;
		int currentNode = sourceNode;
		movement_array[0] = currentNode;
		int i = 1;
		while (currentNode != targetNode)
		{
			currentNode = nextNode(adjacency_list[currentNode], targetNode);
			movement_array[i] = currentNode;
			i++;
		}
}

/*
 *
 * Function Name: followPathBot
 * Input: sourceNode, targetNode
 * Output: void
 * Logic: Plans a path for the bot and makes it run through that path
 * Example Call: followPathBot(0, 6) -> Moves the bot from 0 to 6
 *
 */
void followPathBot(int sourceNode, int targetNode){
	int i = 1;
	printf("\nPATH FROM %d --> %d\n", sourceNode, targetNode);
	pathPlanning(sourceNode, targetNode);
	while (movement_array[i] != -1)
	{
		printf("\n%d -> %d\n", movement_array[i - 1], movement_array[i]);
		botMover(movement_array[i - 1], movement_array[i]);
		i++;
	}
}

	int main()
{
	pathPlanning(0, 6);
	int i = 0;
	while (movement_array[i] != -1){
		printf("%d, ", movement_array[i]);
		i++;
	}
	return 0;
}