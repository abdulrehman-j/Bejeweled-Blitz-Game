#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <time.h>
#include <Windows.h>
#include <sstream>

//Global variables defined to use througout the code
int window_x = 1200, window_y = 820; //size of window
int gems[8][8], selector[8][8] = { 0 }; //arrays that show gems and selector on screen
int animationChecker[2][8] = { 0 }; //first row stores the row index of last gem broken in col, second row stores the difference by which the gems should fall
int scoreCount = 0, countdown = 60; //game score && timer countdown
int screen = 0; //checks what screen to show: start, main or game over
int selector_x = 0, selector_y = 0; //current position of selector
int gemsBroken = 0; //number of gems broken



//Calculate the score of the game
void scorer(int num) {
    scoreCount = scoreCount + 10 * num;
}

//Checks if the two gems are swappable or not
bool isSwappable(int gems[][8]) {
    bool swappable = false;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8 ; j++) {
            if (j <= 5) {
                if (gems[i][j] % 5 == gems[i][j + 1] % 5 && gems[i][j + 1] % 5 == gems[i][j + 2] % 5) {
                    swappable = true;
                }
            }
            if (i <= 5) {
                if (gems[i][j] % 5 == gems[i + 1][j] % 5 && gems[i + 1][j] % 5 == gems[i + 2][j] % 5) {
                    swappable = true;
                }
            }
        }
    }
    return swappable;
}

//Checks if there is a destroyer gem when a match is made
void checkDestroyergem(int gems[][8], int row, int col) {
    int count = 0;
    if (gems[row][col] >= 11 && gems[row][col] <= 15) {
        for (int i = 0; i < 8; i++) {
            gems[i][col] = 0;
            count++;
        }
        for (int j = 0; j < 8; j++) {
            gems[row][j] = 0;
            count++;
        }
        if (col >= 1) {
            gems[row][col - 1] = 0;
            count++;
        }
        if (col <= 6) {
            gems[row][col + 1] = 0;
            count++;
        }
        if (row >= 1) {
            gems[row - 1][col] = 0;
            count++;
        }
        if (row <= 6) {
            gems[row + 1][col] = 0;
            count++;
        }
        if (row >= 1 && col >= 1) {
            gems[row - 1][col - 1] = 0;
            count++;
        }
        if (row <= 6 && col <= 6) {
            gems[row + 1][col + 1] = 0;
            count++;
        }
        if (row >= 1 && col <= 6) {
            gems[row - 1][col + 1] = 0;
            count++;
        }
        if (row <= 6 && col >= 1) {
            gems[row + 1][col - 1] = 0;
            count++;
        }
        scorer(count);
    }
}

//Checks if there is a flame gem when a match is made
void checkFlamegem(int gems[][8], int row, int col) {
    int count = 0;
    if(gems[row][col] >= 6 && gems[row][col] <= 10){
        if (col >= 1) {
            checkDestroyergem(gems, row, col - 1);
            gems[row][col - 1] = 0;
            count++;
        }
        if (col <= 6) {
            checkDestroyergem(gems, row, col + 1);
            gems[row][col + 1] = 0;
            count++;
        }
        if (row >= 1) {
            checkDestroyergem(gems, row - 1, col);
            gems[row - 1][col] = 0;
            count++;
        }
        if (row <= 6) {
            checkDestroyergem(gems, row + 1, col);
            gems[row + 1][col] = 0;
            count++;
        }
        if (row >= 1 && col >= 1) {
            checkDestroyergem(gems, row - 1, col - 1);
            gems[row - 1][col - 1] = 0;
            count++;
        }
        if (row <= 6 && col <= 6) {
            checkDestroyergem(gems, row + 1, col + 1);
            gems[row + 1][col + 1] = 0;
            count++;
        }
        if (row >= 1 && col <= 6) {
            checkDestroyergem(gems, row - 1, col + 1);
            gems[row - 1][col + 1] = 0;
            count++;
        }
        if (row <= 6 && col >= 1) {
            checkDestroyergem(gems, row + 1, col - 1);
            gems[row + 1][col - 1] = 0;
            count++;
        }
        scorer(count);
    }
}

//Checks if the dropping animation is still in process
bool isAnimationRunning(int animationChecker[][8]) {
    bool flag = false;
        for (int j = 0; j < 8; j++) {
            if (animationChecker[1][j] > 0)
                flag = true;
            if (animationChecker[1][j] <= 0)
                animationChecker[1][j] = 0;
        }
    return flag;
}

//Animates the gems dropping down when they break
void dropAnimation(int gems[][8], int animationChecker[][8]) {
    int count = 0;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            count = 0;
            if (animationChecker[0][j] == 0) {
                if (gems[i][j] == 0) {
                    for (int k = i; gems[k][j] == 0 && k < 8; k++) {
                        count++;
                    }
                    animationChecker[0][j] = count + i;
                    animationChecker[1][j] = count * 100;
                }
            }
        }
    }
}

//Shifts the gems down when gems are broken and replacing them with new ones
void shifting(int gems[][8])
{
    for (int x = 0; x < 8; x++)
    {
        for (int y = 0; y < 8; y++)
        {
            if (gems[x][y] == 0)
            {
                for (int i = x; i >= 1; i--)
                {
                    gems[i][y] = gems[i - 1][y];
                }
                gems[0][y] = rand() % 5 + 1;
            }
        }
    }
}

//Checks if there are consecutive gems in a row or column and then deletes them;
void checkMatch(int gems[][8]) {
    bool flag = false;
    for (int i = 0; i < 8 && flag == false; i++) {
        for (int j = 0; j < 8 && flag == false; j++) {
            //Check if there is a match in L shape in 4 different orientations
            if (j <= 5 && i <= 5) {
                if (gems[i][j] % 5 == gems[i][j + 1] % 5 && gems[i][j + 1] % 5 == gems[i][j + 2] % 5 && gems[i][j] % 5 == gems[i + 1][j] % 5 && gems[i + 1][j] % 5 == gems[i + 2][j] % 5) {
                    gemsBroken = 5;
                    scorer(gemsBroken);
                    checkDestroyergem(gems, i, j);
                    checkDestroyergem(gems, i, j + 1);
                    checkDestroyergem(gems, i, j + 2);
                    checkDestroyergem(gems, i + 1, j);
                    checkDestroyergem(gems, i + 2, j);
                    checkFlamegem(gems, i, j);
                    checkFlamegem(gems, i, j + 1);
                    checkFlamegem(gems, i, j + 2);
                    checkFlamegem(gems, i + 1, j);
                    checkFlamegem(gems, i + 2, j);
                    if (gems[i][j] % 5 == 0)
                        gems[i][j] = (gems[i][j] % 5) + 15;
                    else 
                        gems[i][j] = (gems[i][j] % 5) + 10;
                    gems[i][j + 1] = 0;
                    gems[i][j + 2] = 0;
                    gems[i + 1][j] = 0;
                    gems[i + 2][j] = 0;
                    dropAnimation(gems, animationChecker);
                    shifting(gems);
                    flag = true;
                }
                else if (gems[i][j] % 5 == gems[i + 1][j] % 5 && gems[i + 1][j] % 5 == gems[i + 2][j] % 5 && gems[i + 2][j] % 5 == gems[i + 2][j + 1] % 5 && gems[i + 2][j + 1] % 5 == gems[i + 2][j + 2] % 5) {
                    gemsBroken = 5;
                    scorer(gemsBroken);
                    checkDestroyergem(gems, i, j);
                    checkDestroyergem(gems, i + 1, j);
                    checkDestroyergem(gems, i + 2, j);
                    checkDestroyergem(gems, i + 2, j + 1);
                    checkDestroyergem(gems, i + 2, j + 2);
                    checkFlamegem(gems, i, j);
                    checkFlamegem(gems, i + 1, j);
                    checkFlamegem(gems, i + 2, j);
                    checkFlamegem(gems, i + 2, j + 1);
                    checkFlamegem(gems, i + 2, j + 2);
                    gems[i][j] = 0;
                    gems[i + 1][j] = 0;
                    if (gems[i + 2][j] % 5 == 0)
                        gems[i + 2][j] = (gems[i + 2][j] % 5) + 15;
                    else 
                        gems[i + 2][j] = (gems[i + 2][j] % 5) + 10;
                    gems[i + 2][j + 1] = 0;
                    gems[i + 2][j + 2] = 0;
                    dropAnimation(gems, animationChecker);
                    shifting(gems);
                    flag = true;
                }
                else if (gems[i][j] % 5== gems[i][j + 1] % 5 && gems[i][j + 1] % 5 == gems[i][j + 2] % 5 && gems[i][j + 2] % 5 == gems[i + 1][j + 2] % 5 && gems[i + 1][j + 2] % 5 == gems[i + 2][j + 2] % 5) {
                    gemsBroken = 5;
                    scorer(gemsBroken);
                    checkDestroyergem(gems, i, j);
                    checkDestroyergem(gems, i, j + 1);
                    checkDestroyergem(gems, i, j + 2);
                    checkDestroyergem(gems, i + 1, j + 2);
                    checkDestroyergem(gems, i + 2, j + 2);
                    checkFlamegem(gems, i, j);
                    checkFlamegem(gems, i, j + 1);
                    checkFlamegem(gems, i, j + 2);
                    checkFlamegem(gems, i + 1, j + 2);
                    checkFlamegem(gems, i + 2, j + 2);
                    gems[i][j] = 0;
                    gems[i][j + 1] = 0;
                    if (gems[i][j + 2] % 5 == 0)
                        gems[i][j + 2] = (gems[i][j + 2] % 5) + 15;
                    else
                        gems[i][j + 2] = (gems[i][j + 2] % 5) + 10;
                    gems[i + 1][j + 2] = 0;
                    gems[i + 2][j + 2] = 0;
                    dropAnimation(gems, animationChecker);
                    shifting(gems);
                    flag = true;
                }
            }
            if (j >= 2 && i <= 5) {
                if (gems[i][j] % 5 == gems[i + 1][j] % 5 && gems[i + 1][j] % 5 == gems[i + 2][j] % 5 && gems[i + 2][j] % 5 == gems[i + 2][j - 1] % 5 && gems[i + 2][j - 1] % 5 == gems[i + 2][j - 2] % 5) {
                    gemsBroken = 5;
                    scorer(gemsBroken);
                    checkDestroyergem(gems, i, j);
                    checkDestroyergem(gems, i + 1, j);
                    checkDestroyergem(gems, i + 2, j);
                    checkDestroyergem(gems, i + 2, j - 1);
                    checkDestroyergem(gems, i + 2, j - 2);
                    checkFlamegem(gems, i, j);
                    checkFlamegem(gems, i + 1, j);
                    checkFlamegem(gems, i + 2, j);
                    checkFlamegem(gems, i + 2, j - 1);
                    checkFlamegem(gems, i + 2, j - 2);
                    gems[i][j] = 0;
                    gems[i + 1][j] = 0;
                    if (gems[i + 2][j] % 5 == 0)
                        gems[i + 2][j] = (gems[i + 2][j] % 5) + 15;
                    else
                        gems[i + 2][j] = (gems[i + 2][j] % 5) + 10;
                    gems[i + 2][j - 1] = 0;
                    gems[i + 2][j - 2] = 0;
                    dropAnimation(gems, animationChecker);
                    shifting(gems);
                    flag = true;
                }
            }
            if (j <= 3) {
                //Check if 5 same in row
                if (gems[i][j] % 5 == gems[i][j + 1] % 5 && gems[i][j + 1] % 5 == gems[i][j + 2] % 5 && gems[i][j + 2] % 5 == gems[i][j + 3] % 5 && gems[i][j + 3] % 5 == gems[i][j + 4] % 5) {
                    gemsBroken = 5;
                    scorer(gemsBroken);
                    checkDestroyergem(gems, i, j);
                    checkDestroyergem(gems, i, j + 1);
                    checkDestroyergem(gems, i, j + 2);
                    checkDestroyergem(gems, i, j + 3);
                    checkDestroyergem(gems, i, j + 4);
                    checkFlamegem(gems, i, j);
                    checkFlamegem(gems, i, j + 1);
                    checkFlamegem(gems, i, j + 2);
                    checkFlamegem(gems, i, j + 3);
                    checkFlamegem(gems, i, j + 4);
                    if (gems[i][j] % 5 == 0)
                        gems[i][j] = (gems[i][j] % 5) + 10;
                    else
                        gems[i][j] = (gems[i][j] % 5) + 5;
                    gems[i][j + 1] = 0;
                    gems[i][j + 2] = 0;
                    gems[i][j + 3] = 0;
                    gems[i][j + 4] = 0;
                    dropAnimation(gems, animationChecker);
                    shifting(gems);
                    flag = true;
                }
            }
            if (i <= 3){
                //Check if 5 same in column
                if (gems[i][j] % 5 == gems[i + 1][j] % 5 && gems[i + 1][j] % 5 == gems[i + 2][j] % 5 && gems[i + 2][j] % 5 == gems[i + 3][j] % 5 && gems[i + 3][j] % 5 == gems[i + 4][j] % 5) {
                    gemsBroken = 5;
                    scorer(gemsBroken);
                    checkDestroyergem(gems, i, j);
                    checkDestroyergem(gems, i + 1, j);
                    checkDestroyergem(gems, i + 2, j);
                    checkDestroyergem(gems, i + 3, j);
                    checkDestroyergem(gems, i + 4, j);
                    checkDestroyergem(gems, i, j);
                    checkFlamegem(gems, i + 1, j);
                    checkFlamegem(gems, i + 2, j);
                    checkFlamegem(gems, i + 3, j);
                    checkFlamegem(gems, i + 4, j);
                    if (gems[i][j] % 5 == 0)
                        gems[i][j] = (gems[i][j] % 5) + 10;
                    else
                        gems[i][j] = (gems[i][j] % 5) + 5;
                    gems[i + 1][j] = 0;
                    gems[i + 2][j] = 0;
                    gems[i + 3][j] = 0;
                    gems[i + 4][j] = 0;
                    dropAnimation(gems, animationChecker);
                    shifting(gems);
                    flag = true;
                }
            }
            if (j <= 4) {
                //check if 4 same in a row
                if (gems[i][j] % 5 == gems[i][j + 1] % 5 && gems[i][j + 1] % 5 == gems[i][j + 2] % 5 && gems[i][j + 2] % 5 == gems[i][j + 3] % 5) {
                    gemsBroken = 4;
                    scorer(gemsBroken);
                    checkDestroyergem(gems, i, j);
                    checkDestroyergem(gems, i, j + 1);
                    checkDestroyergem(gems, i, j + 2);
                    checkDestroyergem(gems, i, j + 3);
                    checkFlamegem(gems, i, j);
                    checkFlamegem(gems, i, j + 1);
                    checkFlamegem(gems, i, j + 2);
                    checkFlamegem(gems, i, j + 3);
                    if (gems[i][j] % 5 == 0)
                        gems[i][j] = (gems[i][j] % 5) + 10;
                    else
                        gems[i][j] = (gems[i][j] % 5) + 5;
                    gems[i][j + 1] = 0;
                    gems[i][j + 2] = 0;
                    gems[i][j + 3] = 0;
                    dropAnimation(gems, animationChecker);
                    shifting(gems);
                    flag = true;
                }
            }
            if (i <= 4){
                //check if 4 same in a row
                if (gems[i][j] % 5 == gems[i + 1][j] % 5 && gems[i + 1][j] % 5 == gems[i + 2][j] % 5 && gems[i + 2][j] % 5 == gems[i + 3][j] % 5) {
                    gemsBroken = 4;
                    scorer(gemsBroken);
                    checkDestroyergem(gems, i, j);
                    checkDestroyergem(gems, i + 1, j);
                    checkDestroyergem(gems, i + 2, j);
                    checkDestroyergem(gems, i + 3, j);
                    checkFlamegem(gems, i, j);
                    checkFlamegem(gems, i + 1, j);
                    checkFlamegem(gems, i + 2, j);
                    checkFlamegem(gems, i + 3, j);
                    if (gems[i][j] % 5 == 0)
                        gems[i][j] = (gems[i][j] % 5) + 10;
                    else 
                        gems[i][j] = (gems[i][j] % 5) + 5;
                    gems[i + 1][j] = 0;
                    gems[i + 2][j] = 0;
                    gems[i + 3][j] = 0;
                    dropAnimation(gems, animationChecker);
                    shifting(gems);
                    flag = true;
                }
            }
            if (j <= 5) {
                 //check if 3 same in a column
                if (gems[i][j] % 5 == gems[i][j + 1] % 5 && gems[i][j + 1] % 5 == gems[i][j + 2] % 5) {
                    gemsBroken = 3;
                    scorer(gemsBroken);
                    checkDestroyergem(gems, i, j);
                    checkDestroyergem(gems, i, j + 1);
                    checkDestroyergem(gems, i, j + 2);
                    checkFlamegem(gems, i, j);
                    checkFlamegem(gems, i, j + 1);
                    checkFlamegem(gems, i, j + 2);
                    gems[i][j] = 0;
                    gems[i][j + 1] = 0;
                    gems[i][j + 2] = 0;
                    dropAnimation(gems, animationChecker);
                    shifting(gems);
                    flag = true;
                }
            }
            if (i <= 5) {
                //check if 3 same in a column
                if (gems[i][j] % 5 == gems[i + 1][j] % 5 && gems[i + 1][j] % 5 == gems[i + 2][j] % 5) {
                    gemsBroken = 3;
                    scorer(gemsBroken);
                    checkDestroyergem(gems, i, j);
                    checkDestroyergem(gems, i + 1, j);
                    checkDestroyergem(gems, i + 2, j);
                    checkFlamegem(gems, i, j);
                    checkFlamegem(gems, i + 1, j);
                    checkFlamegem(gems, i + 2, j);
                    gems[i][j] = 0;
                    gems[i + 1][j] = 0;
                    gems[i + 2][j] = 0;
                    dropAnimation(gems, animationChecker);
                    shifting(gems);
                    flag = true;
                }
            }
        }
    }
}

int main()
{
    srand(time(0));

    sf::RenderWindow window(sf::VideoMode(window_x, window_y), "Bejeweled Blitz");
    window.setFramerateLimit(100);

    //Defining names for textures
    sf::Texture bg, sbg, ebg; //textures of background, start background and end background
    sf::Texture gridTexture;
    sf::Texture gem1, gem2, gem3, gem4, gem5, gem6, gem7, gem8, gem9, gem10, gem11, gem12, gem13, gem14, gem15; 
    sf::Texture indicator, indicator2;
    sf::Font text;

    //Loading Textures
    bg.loadFromFile("images\\background.jpg");
    sbg.loadFromFile("images\\startbackground.jpg");
    ebg.loadFromFile("images\\endbackground.png");
    gridTexture.loadFromFile("images\\grid.png");
    gem1.loadFromFile("images\\bluegem.png");
    gem2.loadFromFile("images\\redgem.png");
    gem3.loadFromFile("images\\greengem.png");
    gem4.loadFromFile("images\\yellowgem.png");
    gem5.loadFromFile("images\\orangegem.png");
    gem6.loadFromFile("images\\firebluegem.png");
    gem7.loadFromFile("images\\fireredgem.png");
    gem8.loadFromFile("images\\firegreengem.png");
    gem9.loadFromFile("images\\fireyellowgem.png");
    gem10.loadFromFile("images\\fireorangegem.png");
    gem11.loadFromFile("images\\destroyerbluegem.png");
    gem12.loadFromFile("images\\destroyerredgem.png");
    gem13.loadFromFile("images\\destroyergreengem.png");
    gem14.loadFromFile("images\\destroyeryellowgem.png");
    gem15.loadFromFile("images\\destroyerorangegem.png");
    indicator.loadFromFile("images\\indicator.png");
    indicator2.loadFromFile("images\\indicator2.png");
    text.loadFromFile("times.ttf");

    //Defining shapes and texts
    sf::RectangleShape background(sf::Vector2f(window_x, window_y)), startbackground(sf::Vector2f(window_x, window_y)), endbackground(sf::Vector2f(window_x, window_y));;
    sf::RectangleShape grid(sf::Vector2f(800, 800));
    sf::RectangleShape square(sf::Vector2f(100, 100)), selection(sf::Vector2f(100, 100)), scoreBox(sf::Vector2f(200, 100)), timerBox(sf::Vector2f(200, 100));
    sf::RectangleShape gameOverBox(sf::Vector2f(700, 400));
    sf::Text score, scoreText, time, timerText, startText, gameOverText, endText, endText2, endscoreText, scoreEnd;

    //Score details
    scoreBox.setFillColor(sf::Color(0, 0, 255, 100));
    scoreBox.setPosition(sf::Vector2f(100, 50));
    score.setFont(text);
    score.setPosition(sf::Vector2f(175, 100));
    scoreText.setFont(text);
    scoreText.setPosition(sf::Vector2f(150, 60));
    scoreText.setCharacterSize(30);
    scoreText.setString("SCORE");
    scoreText.setStyle(sf::Text::Bold);

    //Timer Details
    timerBox.setFillColor(sf::Color(0, 0, 255, 100));
    timerBox.setPosition(sf::Vector2f(100, 650));
    time.setFont(text);
    time.setPosition(sf::Vector2f(185, 700));
    timerText.setFont(text);
    timerText.setCharacterSize(30);
    timerText.setPosition(sf::Vector2f(160, 660));
    timerText.setString("TIME");
    timerText.setStyle(sf::Text::Bold);
    sf::Clock clock;

    //Start Menu text
    startText.setFont(text);
    startText.setString("PRESS SPACE KEY TO START");
    startText.setCharacterSize(50);
    startText.setStyle(sf::Text::Bold);
    startText.setPosition(sf::Vector2f(250, 700));

    //Game over box
    gameOverBox.setFillColor(sf::Color(113, 64, 199, 100));
    gameOverBox.setPosition(sf::Vector2f(250, 210));
    gameOverText.setString("GAME OVER");
    gameOverText.setFont(text);
    gameOverText.setCharacterSize(70);
    gameOverText.setPosition(sf::Vector2f(380, 210));
    gameOverText.setStyle(sf::Text::Bold);
    endscoreText.setFont(text);
    endscoreText.setString("SCORE:");
    endscoreText.setCharacterSize(100);
    endscoreText.setStyle(sf::Text::Bold);
    endscoreText.setPosition(sf::Vector2f(340, 350));
    scoreEnd.setFont(text);
    scoreEnd.setPosition(sf::Vector2f(710, 350));
    scoreEnd.setCharacterSize(100);
    endText.setFont(text);
    endText.setString("PRESS SPACE KEY TO RETURN TO MAIN MENU");
    endText.setCharacterSize(40);
    endText.setStyle(sf::Text::Bold);
    endText.setPosition(sf::Vector2f(150, 700));
    endText2.setFont(text);
    endText2.setString("PRESS ESCAPE KEY TO EXIT");
    endText2.setCharacterSize(40);
    endText2.setStyle(sf::Text::Bold);
    endText2.setPosition(sf::Vector2f(315, 745));


    //setting textures of objects 
    background.setTexture(&bg);
    startbackground.setTexture(&sbg);
    endbackground.setTexture(&ebg);
    endbackground.setFillColor(sf::Color(255, 255, 255, 100));
    grid.setTexture(&gridTexture);
    grid.setPosition(390, 10);
    grid.setFillColor(sf::Color(255, 255, 255, 100));
    selection.setTexture(&indicator);

    //When the game is running 
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::Closed) window.close();

            // If a key is pressed
            if (event.type == sf::Event::KeyPressed)
            {
                switch (event.key.code) {
                    //change from start screen to main screen or end screen to start screen
                    case sf::Keyboard::Space:
                        if (screen == 0) {
                            scoreCount = 0;
                            for (int i = 0; i < 8; i++)
                                for (int j = 0; j < 8; j++)
                                    gems[i][j] = rand() % 5 + 1;
                            for (int i = 0; i < 2; i++)
                                for (int j = 0; j < 8; j++)
                                    animationChecker[i][j] = 0;
                            selector_x = 0;
                            selector_y = 0;
                            screen = 1;
                            clock.restart();
                        }
                        else if (screen == 2) {
                            screen = 0;
                        }
                        break;
                    // If escape is pressed, close the application
                    case  sf::Keyboard::Escape:
                        if (screen == 1)
                            screen = 2;
                        else 
                            window.close();
                        break;
                    // Process the up, down, left, right and enter keys
                    case sf::Keyboard::Enter:
                        if (!isAnimationRunning(animationChecker) && screen == 1) {
                            selection.setTexture(&indicator2);
                            if (selector[selector_y][selector_x] == 1) {
                                selector[selector_y][selector_x] = 0;
                                selection.setTexture(&indicator);
                            }
                            else if (selector[selector_y][selector_x] == 0)
                                selector[selector_y][selector_x] = 1;
                        }
                        break;
                    case sf::Keyboard::Up:
                        if (!isAnimationRunning(animationChecker) && screen == 1) {
                            selection.setTexture(&indicator);
                            if (selector[selector_y][selector_x] == 1) {
                                if (selector_y - 1 >= 0) {
                                    std::swap(gems[selector_y][selector_x], gems[selector_y - 1][selector_x]);
                                    if (!isSwappable(gems)) {
                                        std::swap(gems[selector_y][selector_x], gems[selector_y - 1][selector_x]);
                                    }
                                    selector[selector_y][selector_x] = 0;
                                    selector_y--;
                                }
                                else
                                    selector_y = 7;
                            }
                            else if (selector_y - 1 >= 0)
                                selector_y--;
                            else
                                selector_y = 7;
                        }
                        break;
                    case sf::Keyboard::Down:
                        if (!isAnimationRunning(animationChecker) && screen == 1) {
                            selection.setTexture(&indicator);
                            if (selector[selector_y][selector_x] == 1) {
                                if (selector_y + 1 < 8) {
                                    std::swap(gems[selector_y][selector_x], gems[selector_y + 1][selector_x]);
                                    if (!isSwappable(gems)) {
                                        std::swap(gems[selector_y][selector_x], gems[selector_y + 1][selector_x]);
                                    }
                                    selector[selector_x][selector_y] = 0;
                                    selector_y++;
                                }
                                else
                                    selector_y = 0;
                            }
                            else if (selector_y + 1 < 8)
                                selector_y++;
                            else
                                selector_y = 0;
                        }
                        break;
                    case sf::Keyboard::Left:
                        if (!isAnimationRunning(animationChecker) && screen == 1) {
                            selection.setTexture(&indicator);
                            if (selector[selector_y][selector_x] == 1) {
                                if (selector_x - 1 >= 0) {
                                    std::swap(gems[selector_y][selector_x], gems[selector_y][selector_x - 1]);
                                    if (!isSwappable(gems)) {
                                        std::swap(gems[selector_y][selector_x], gems[selector_y][selector_x - 1]);
                                    }
                                    selector[selector_y][selector_x] = 0;
                                    selector_x--;
                                }
                                else
                                    selector_x = 7;
                            }
                            else if (selector_x - 1 >= 0)
                                selector_x--;
                            else
                                selector_x = 7;
                        }
                        break;
                    case sf::Keyboard::Right:
                        if (!isAnimationRunning(animationChecker) && screen == 1) {
                            selection.setTexture(&indicator);
                            if (selector[selector_y][selector_x] == 1) {
                                if (selector_x + 1 < 8) {
                                    std::swap(gems[selector_y][selector_x], gems[selector_y][selector_x + 1]);
                                    if (!isSwappable(gems)) {
                                        std::swap(gems[selector_y][selector_x], gems[selector_y][selector_x + 1]);
                                    }
                                    selector[selector_y][selector_x] = 0;
                                    selector_x++;
                                }
                                else
                                    selector_x = 0;
                            }
                            else if (selector_x + 1 < 8)
                                selector_x++;
                            else
                                selector_x = 0;
                        }
                        break;
                    default: break;
                }
            }
        }

        //Start screen
        if (screen == 0) {
            window.clear();
            window.draw(startbackground);
            window.draw(startText);
            window.display();
        }

        //Game Screen
        else if (screen == 1) {
            // UPdates Selector Position
            selection.setPosition(390.0 + selector_x * 100, 10.0 + selector_y * 100);

            //only checks if gems are matching when dropdown animatin is complete 
            if (!isAnimationRunning(animationChecker))
                checkMatch(gems);

            //updates the score
            std::stringstream scoreString;
            scoreString << scoreCount; //saves the score in string datatype
            score.setString(scoreString.str().c_str());

            //updates timer
            int timer = clock.getElapsedTime().asSeconds();
            std::stringstream timerString;
            timerString << countdown - timer; //saves the timer in string datatype
            time.setString(timerString.str().c_str());

            window.clear();
            window.draw(background);
            window.draw(grid);
            window.draw(scoreBox);
            window.draw(scoreText);
            window.draw(score);
            window.draw(timerBox);
            window.draw(timerText);
            window.draw(time);
            window.draw(selection);

            for (int i = 0; i < 8; i++) {
                for (int j = 0; j < 8; j++) {
                    if (animationChecker[0][j] == 0 || animationChecker[0][j] <= i) {
                        square.setPosition(390 + j * 100, 10 + i * 100);
                        if (gems[i][j] == 1) {
                            square.setTexture(&gem1);
                        }
                        else if (gems[i][j] == 2) {
                            square.setTexture(&gem2);
                        }
                        else if (gems[i][j] == 3) {
                            square.setTexture(&gem3);
                        }
                        else if (gems[i][j] == 4) {
                            square.setTexture(&gem4);
                        }
                        else if (gems[i][j] == 5) {
                            square.setTexture(&gem5);
                        }
                        else if (gems[i][j] == 6) {
                            square.setTexture(&gem6);
                        }
                        else if (gems[i][j] == 7) {
                            square.setTexture(&gem7);
                        }
                        else if (gems[i][j] == 8) {
                            square.setTexture(&gem8);
                        }
                        else if (gems[i][j] == 9) {
                            square.setTexture(&gem9);
                        }
                        else if (gems[i][j] == 10) {
                            square.setTexture(&gem10);;
                        }
                        else if (gems[i][j] == 11) {
                            square.setTexture(&gem11);
                        }
                        else if (gems[i][j] == 12) {
                            square.setTexture(&gem12);
                        }
                        else if (gems[i][j] == 13) {
                            square.setTexture(&gem13);
                        }
                        else if (gems[i][j] == 14) {
                            square.setTexture(&gem14);
                        }
                        else if (gems[i][j] == 15) {
                            square.setTexture(&gem15);;
                        }
                        window.draw(square);
                    }
                    else if (animationChecker[0][j] > i) {
                        square.setPosition(390 + j * 100, 10 + i * 100 - animationChecker[1][j]);
                        if (gems[i][j] == 1) {
                            square.setTexture(&gem1);
                        }
                        else if (gems[i][j] == 2) {
                            square.setTexture(&gem2);
                        }
                        else if (gems[i][j] == 3) {
                            square.setTexture(&gem3);
                        }
                        else if (gems[i][j] == 4) {
                            square.setTexture(&gem4);
                        }
                        else if (gems[i][j] == 5) {
                            square.setTexture(&gem5);;
                        }
                        else if (gems[i][j] == 6) {
                            square.setTexture(&gem6);
                        }
                        else if (gems[i][j] == 7) {
                            square.setTexture(&gem7);
                        }
                        else if (gems[i][j] == 8) {
                            square.setTexture(&gem8);
                        }
                        else if (gems[i][j] == 9) {
                            square.setTexture(&gem9);
                        }
                        else if (gems[i][j] == 10) {
                            square.setTexture(&gem10);;
                        }
                        else if (gems[i][j] == 11) {
                            square.setTexture(&gem11);
                        }
                        else if (gems[i][j] == 12) {
                            square.setTexture(&gem12);
                        }
                        else if (gems[i][j] == 13) {
                            square.setTexture(&gem13);
                        }
                        else if (gems[i][j] == 14) {
                            square.setTexture(&gem14);
                        }
                        else if (gems[i][j] == 15) {
                            square.setTexture(&gem15);;
                        }
                        window.draw(square);
                        animationChecker[1][j] = animationChecker[1][j] - 10;
                        if (animationChecker[1][j] <= 0)
                            animationChecker[0][j] = 0;
                        if (!isAnimationRunning(animationChecker)) {
                            for (int i = 0; i < 2; i++)
                                for (int j = 0; j < 8; j++)
                                    animationChecker[i][j] = 0;
                        }
                        Sleep(7);
                    }
                }
            }
            window.display();
            if (countdown - timer == 0)
                screen = 2;
        }

        //Game over screen
        if (screen == 2) {
            std::stringstream scoreEndString;
            scoreEndString << scoreCount; //saves the score in string datatype
            scoreEnd.setString(scoreEndString.str().c_str());

            window.clear();
            window.draw(endbackground);
            window.draw(gameOverBox);
            window.draw(gameOverText);
            window.draw(endscoreText);
            window.draw(scoreEnd);
            window.draw(endText);
            window.draw(endText2);
            window.display();
        }
    }
    return 0;
}
