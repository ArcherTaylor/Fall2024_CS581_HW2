/*
 Name: Weston Archer Taylor
 Email: wataylor1@crimson.ua.edu
 Course Section: CS 581
 Homework #: 1
 Instructions to compile the program: g++ -std=c++11 -Wall -O -o HW1 HW1.cpp
 Instructions to run the program: ./HW1 <board_size> <max_generations> [-DEBUG_VISUAL]
 Board size must be greater than or equal to 3. Toggle -DEBUG_VISUAL to see each generation visualized, including changes
*/

#include <iostream>
#include <chrono>
#include <thread>
#include <cstdlib>

//This function determines the state of the cell in the next generation and updates the future generation layer of the board
void DetermineNextCellState(bool*** board, int i, int j){
    int alive_neighbors = 0;
    //Counts the number of alive neighbors in a 3x3 grid centered at (i, j)
    for(int k = i-1; k <= i+1; k++){
        for(int l = j-1; l <= j+1; l++){
            if(k != i || l != j){
                if(board[0][k][l] == 1){
                    alive_neighbors++;
                }
            }
        }
    }
    if(board[0][i][j] == 1){
        if(alive_neighbors < 2 || alive_neighbors > 3){
            board[1][i][j] = 0;
        }
        else{
            board[1][i][j] = 1;
        }
    }
    else{
        if(alive_neighbors == 3){
            board[1][i][j] = 1;
        }
        else{
            board[1][i][j] = 0;
        }
    }
}

void UpdateBoard(bool*** board, int board_size){
    //Swaps the pointers for the layers, makes the future generation the current generation and discards the soon-to-die current generation
    bool** temp = board[0];
    board[0] = board[1];
    board[1] = temp;
}

void PrintCurrentBoardDebug(bool*** board, int board_size){
    for(int i = 0; i < board_size+2; i++){
        for(int j = 0; j < board_size+2; j++){
            std::cout << board[0][i][j] << " ";
        }
        std::cout << std::endl;
    }
}

bool*** AllocateBoardSpace(int board_size){
    bool*** allocation = (bool***)malloc(sizeof(bool**) * 2);
    for (int layer = 0; layer < 2; ++layer) {
        allocation[layer] = (bool**)malloc(sizeof(bool*) * (board_size+2));
        for (int row = 0; row < board_size+2; ++row) {
            allocation[layer][row] = (bool*)malloc(sizeof(bool) * (board_size+2));
        }
    }
    return allocation;
}

void InitializeBoard(bool*** board, int board_size){
    //Initialize the future generation board to 0's, and the current generation to either 1 or 0 randomly.
    for(int layer = 0; layer < 2; layer++){
        for(int row = 0; row < board_size+2; row++){
            for(int cell = 0; cell < board_size+2; cell++){
                if(layer == 0){
                    if(row == 0 || row == board_size+1 || cell == 0 || cell == board_size+1){
                        board[0][row][cell] = 0;
                        continue;
                    }
                    board[0][row][cell] = rand() % 2;
                    continue;
                }
                board[layer][row][cell] = 0;
            }
        }
    }
}

void FreeBoard(bool*** board, int board_size){
    for (int layer = 0; layer < 2; ++layer) {
        for (int i = 0; i < board_size + 2; ++i) {
            free(board[layer][i]);  // Free each row
        }
        free(board[layer]);  // Free each 2D array
    }
    free(board); // Free the 3D array
}

void PrintBoard(bool*** board, int board_size){
    //Check if the system is a WIN32 system or not (for correct terminal clearing)
    #ifdef _WIN32
        system("CLS");
    #else
        system("clear");
    #endif

    //Print each cell, including borders
    for(int i = 0; i <= board_size+1; i++) {
        for(int j = 0; j <= board_size+1; j++) {
            if (board[0][i][j] == 1) {
                std::cout << "1 ";  // Live cell
            } else {
                std::cout << "0 ";  // Dead cell
            }
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

int main(int argc, char *argv[]){

    //Boolean for managing the DEBUG_VISUAL flag
    bool debug_visual = false;

    //Ensure seeded randomization
    srand(time(0));

    // Check if -DEBUG_VISUAL is in the command-line arguments
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "-DEBUG_VISUAL") {
            debug_visual = true;
            break;
        }
    }
 
    //Check if the number of arguments is correct before proceeding
    if (argc < 3 || (argc == 3 && debug_visual) || (argc > 3 && !debug_visual && std::string(argv[3]) != "-DEBUG_VISUAL")) {
        std::cout << "Usage: ./HW1 <board_size> <max_generations> [-DEBUG_VISUAL]" << std::endl;
        return -1;
    }

    //Assign our 'n' values and max_gen values from the arguments
    int n = atoi(argv[1]);
    int max_gen = atoi(argv[2]);

    //Halt program if the n < 3
    if(n < 3){
        std::cout << "Please enter a value for n that is greater than 3" << std::endl;
        return -1;
    }

    //Allocates space for the board of size (n+2) x (n+2) x 2
    bool*** board = AllocateBoardSpace(n);
    //Initialize's the board (both layers) to 0's
    InitializeBoard(board, n);

    //Start the clock for performance measuring
    auto clock_start = std::chrono::high_resolution_clock::now();

    //Run the game for max_gen
    for(int gen = 0; gen <= max_gen; gen++){
        //Print the board and see it visually, will only turn on if DEBUG_VISUAL is enabled.
        if (debug_visual) {
            PrintBoard(board, n);
            std::cout << "Generation: " << gen << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
        
        //Iterate over each cell in each generation, excluding border cells
        for(int i = 1; i < n+1; i++){
            for(int j = 1; j < n+1; j++){
                DetermineNextCellState(board, i, j);
            }
        }
        UpdateBoard(board, n);
    }

    //Stop the clock for performance measuring
    auto clock_end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = clock_end - clock_start;

    std::cout << "Time taken for " << max_gen << " generations on a size " << n << " by " << n << " board: " << elapsed.count() << " seconds" << std::endl;

    //Memory calculations
    size_t element_size = sizeof(bool);
    size_t num_elements = (n+2) * (n+2) * 2;
    size_t total_memory_usage = num_elements * element_size;
    std::cout << "Total memory usage by board array: " << total_memory_usage << " bytes" << std::endl;


    //Free up the space used for the game board
    FreeBoard(board, n);

    return 0;
}