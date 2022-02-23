#include <iostream>
#include <chrono>
#include <SFML/Graphics.hpp>


struct Moves
{
	int index = 0, score = 0;
	Moves() {};
	Moves(int score) : score(score) {};
	
};
// Prototypes
int fsqrt(int num);

int CheckWinner(std::vector<int> board);

Moves Search(std::vector<int>& board, int depth, int alpha, int beta, bool maximizing);

// Grid
class Grid
{
public:
	std::vector<sf::RectangleShape> gridlines{};
	std::vector<sf::RectangleShape> boxes{};
	std::vector<sf::Text> letters{};
	std::vector<int> board{};
	int boardsize = 0;

	Grid()
	{
		gridlines.resize(4);
		
		// vertical
		gridlines[0].setPosition({ 1080/3, 0});
		gridlines[1].setPosition({ 1080/3*2, 0});
		// horizontal
		gridlines[2].setPosition({ 0, 1080/3});
		gridlines[3].setPosition({ 0, 1080/3*2});
		for (int i = 0; i < 4; i++)
		{
			if (i < 2)
				gridlines[i].setSize({ 20, 1040 });
			else
				gridlines[i].setSize({ 1040, 20 });
			
				

			gridlines[i].setFillColor(sf::Color::Black);
		}

		boxes.resize(9);
		letters.resize(9);

		boxes[0].setPosition({ 0, 0 });
		boxes[1].setPosition({ 1080 / 3, 0 });
		boxes[2].setPosition({ 1080 / 3 * 2, 0 });
		boxes[3].setPosition({ 0, 1080 / 3 });
		boxes[4].setPosition({ 1080 / 3, 1080 / 3 });
		boxes[5].setPosition({ 1080 / 3 * 2, 1080 / 3 });
		boxes[6].setPosition({ 0, 1080 / 3 * 2 });
		boxes[7].setPosition({ 1080 / 3, 1080 / 3 * 2 });
		boxes[8].setPosition({ 1080 / 3 * 2, 1080 / 3 * 2 });

		for (int i = 0; i < 9; i++)
		{
			boxes[i].setSize({ 1080/3, 1080/3 });
			boxes[i].setFillColor(sf::Color::White);
		}

		board.resize(9);
		for (auto i : board)
			i = 0;
	}
	Grid(int size)
	{
		this->boardsize = size * size;
		int gridsize = (size - 1) * 2;
		gridlines.resize(gridsize);

		// vertical
		for (int i = 0; i < size - 1; i++)
		{
			float x = 1080 / size * (i + 1);
			float y = 0;
			gridlines[i].setPosition({ x, y });
			gridlines[i].setSize({ 20, 1080 });
			std::cout << "grid[" << i << "]: " << "(" << x << ", " << y << ")\n";
		}
		// horizontal
		for (int i = size - 1; i < gridsize; i++)
		{
			float x = 0;
			float y = 1080 / size * (size - i + (size - 2));
			gridlines[i].setPosition({ x, y});
			gridlines[i].setSize({ 1080, 20 });
			std::cout << "grid[" << i << "]: " << "(" << x << ", " << y << ")\n";
		}
		//std::cout << gridsize << "\n";
		

		for (int i = 0; i < gridsize; i++)
		{
			gridlines[i].setFillColor(sf::Color::Black);
		}

		boxes.resize(size * size);
		letters.resize(size * size);

		for (int i = 0; i < size; i++)
		{
			float col = (1080 / size) * (i);
			for (int j = 0; j < size; j++)
			{
				float row = (1080 / size) * (j);
				boxes[(i * size) + j].setPosition({ row, col });
				std::cout << "boxes[" << i * size + j << "]: " << "(" << row << ", " << col << ")\n";
			}
			
		}

		for (int i = 0; i < size*size; i++)
		{
			boxes[i].setSize({ float(1080 / size), float(1080 / size) });
			boxes[i].setFillColor(sf::Color(0xefa16cff));
		}

		board.resize(size*size);
		for (auto i : board)
			i = 0;
	}
	int mouseover(sf::RenderWindow& window)
	{
		sf::Vector2i mouseposi = sf::Mouse::getPosition(window);
		sf::Vector2f mousepos = window.mapPixelToCoords(mouseposi);


		sf::Vector2f pos, size;
		for (int i = 0; i < boardsize; i++)
		{
			pos = boxes[i].getPosition();
			size = boxes[i].getSize();
			if (mousepos.x > pos.x && mousepos.x < pos.x + size.x && mousepos.y > pos.y && mousepos.y < pos.y + size.y)
				return i;
		}

		return -1;
	}
};


int main()
{
	sf::RenderWindow window(sf::VideoMode(1080, 1080, 32), "Tic Tac Toe", sf::Style::Default);
	window.setSize({ 500, 500 });
	window.setFramerateLimit(144u);
	sf::Event vent;
	int currentbox = 0;
	int player = 1;
	bool nextmove = 0;
	sf::Font font;
	if (!font.loadFromFile("C:/Windows/Fonts/LCALLIG.TTF"))
		return 0;
	Grid grid(3);
	std::cout << grid.board.size() << "\n";
	for (int i = 0; i < grid.board.size(); i++)
	{
		grid.letters[i].setPosition(grid.boxes[i].getPosition());
		grid.letters[i].setFillColor(sf::Color::Black);
		grid.letters[i].setFont(font);
		grid.letters[i].setString("");
		grid.letters[i].setCharacterSize(grid.boxes[0].getSize().x);
	}
	//nextmove = 1;	// Uncomment this to make AI go first
	bool gameover = 0, update = 1;
	sf::Text gameovertext;
	gameovertext.setFont(font);
	gameovertext.setPosition({ 0, 1080 / 3 });
	gameovertext.setFillColor(sf::Color::Blue);
	gameovertext.setCharacterSize(100u);
	
	while (window.isOpen())
	{
		int winner = CheckWinner(grid.board);
		if (winner != 0)
		{
			switch (winner)
			{
			case 1:
				gameovertext.setString("X wins!");
				break;
			case 2:
				gameovertext.setString("O wins!");
				break;
			case -1:
				gameovertext.setString("Tie, you both suck!");
				break;
			}
			gameover = 1;
		}

		if (nextmove)
		{
			for (auto i : grid.board)
				std::cout << i << ", ";
			std::cout << "\n";
			auto begin = std::chrono::high_resolution_clock::now();
			Moves move = Search(grid.board, 8, -LONG_MAX, LONG_MAX, 1);
			auto end = std::chrono::high_resolution_clock::now();
			std::cout << "Time taken: " << (float)(end-begin).count()/1000000000.f << "\n";
			int index = move.index;
			int score = move.score;
			if (grid.board[index] == 0)
				grid.board[index] = 2;

			//std::cout << "\n" << score << "\n";
			nextmove = 0;
		}


		while (window.pollEvent(vent))
		{
			switch (vent.type)
			{
			case sf::Event::Closed:
				window.close();
				break;
			case sf::Event::Resized:
				window.setView(window.getDefaultView());
			case sf::Event::MouseButtonPressed:
				switch (vent.mouseButton.button)
				{
				case sf::Mouse::Left:
					currentbox = grid.mouseover(window);
					if (grid.board[currentbox] == 0 && !gameover)
					{
						if (player == 1)
						{
							grid.board[currentbox] = 1; // x

							//Test(grid.board);
							//player = 2;	// for human player
						}
						else if (player == 2)
						{
							grid.board[currentbox] = 2; // o
							player = 1;
						}
						nextmove = 1;
						update = 1;
					}
					break;
				}
				break;
			}
		}
		if (update)
		{
			for (int i = 0; i < grid.board.size(); i++)
			{
				if (grid.board[i] == 0)
					grid.letters[i].setString("");
				if (grid.board[i] == 1)
					grid.letters[i].setString("X");
				if (grid.board[i] == 2)
					grid.letters[i].setString("O");
			}
		}

		window.clear(sf::Color::White);

		for (int i = 0; i < grid.boxes.size(); i++)
		{
			window.draw(grid.boxes[i]);
		}

		for (int i = 0; i < grid.gridlines.size(); i++)
		{
			window.draw(grid.gridlines[i]);
		}

		for (int i = 0; i < grid.letters.size(); i++)
		{
			window.draw(grid.letters[i]);
		}

		if (gameover)
		{
			window.draw(gameovertext);
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space))
			{
				for (int i = 0; i < grid.board.size(); i++)
				{
					grid.board[i] = 0;
				}
				gameover = 0;
				nextmove = 1;
				update = 1;
			}
		}
		window.display();
	}

	return 0;
}

int fsqrt(int num)
{
	for (int i = num; i > 0; i--)
	{
		if (i*i == num)
		{
			//std::cout << "num: " << num << ", i: " << i << "\n";
			return i;
		}
	}
	return -1;
}

int CheckWinner(std::vector<int> board)
{
	int size = fsqrt(board.size());

	//int size = sqrt(board.size());
	bool win = 0;
	
	// Horizontal
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size - 1; j++)
		{
			int index = i * size + j;
			if (board[index] != 0)
			{
				if (board[index] == board[index + 1])
					win = 1;
				else
				{
					win = 0;
					break;
				}
			}
			else
			{
				win = 0;
				break;
			}
		}
		if (win)
		{
			//std::cout << "Horizontal: row " << i << "\n";
			return board[i * size];
		}
	}

	// Vertical
	// outer loop will be left-right
	for (int i = 0; i < size; i++)
	{
		// inner loop will be top-bottom
		for (int j = 0; j < size - 1; j++) // < size-1 to check index+1
		{
			int index = j * size + i; // basically [j][i]
			if (board[index] != 0)
			{
				if (board[i] == board[index + size])
				{
					//std::cout << "board[" << index << "] == board[" << index + size << "]\n";
					win = 1;
				}
				else
				{
					//std::cout << "board[" << index << "] != board[" << index + size << "]\n";
					win = 0;
					break;
				}
			}
			else
			{
				win = 0;
				break;
			}
		}
		if (win)
		{
			//std::cout << "Vertical: col " << i << "\n";
			return board[i]; // doesn't matter since it'll be the same col as the j's
		}
	}

	// Diagonal
	for (int i = 0; i < board.size(); i += size + 1)
	{
		if (board[0] != 0)
		{
			if (board[0] == board[i])
			{
				win = 1;
			}
			else
			{
				win = 0;
				break;
			}
		}
		else
		{
			win = 0;
			break;
		}
	}
	if (win)
	{
		//std::cout << "Diagonal: Forward\n";
		return board[0];
	}
	// Diagonal Reverse
	//int step = 0;
	//if (size != 1)
	int step = size - 1;
	//else
		//step = 1;
	for (int i = step; i < board.size() - step + 1; i += step)
	{
		if (board[step] != 0)
		{
			if (board[step] == board[i])
			{
				win = 1;
			}
			else
			{
				win = 0;
				break;
			}
		}	
		else
		{
			win = 0;
		}
	}

	if (win)
	{
		//std::cout << "Diagonal: Backward\n";
		return board[step];
	}

	int tiecount = 0;
	for (int i = 0; i < board.size(); i++)
	{
		if (board[i] == 0)
			tiecount++;
	}

	if (tiecount == 0)
		return -1;

	return 0;
}

Moves Search(std::vector<int>& board, int depth, int alpha, int beta, bool maximizing)
{
	//std::cout << "Searching...\n";

	int winner = 0;
	int empties = 0;
	int size = fsqrt(board.size());
	int boardsize = board.size();
	//int size = 3;
	std::vector<Moves> moves;
	for (int i = 0; i < boardsize; i++)
	{
		if (board[i] == 0)
			empties++;
	}

	int player = 0, other = 0;
	if (maximizing)
	{
		player = 2;
		other = 1;
	}
	else
	{
		player = 1;
		other = 2;
	}

	if (depth == 0)
	{		
		// Check rows
		int score = 0;
		for (int i = 0; i < size - 1; i += size)
		{
			for (int j = 0; j < boardsize; j++)
			{
				if (board[i + j] != 0)
				{
					if (board[i + j] == player && board[i + j + 1] == player)
					{
						score += 2;
					}
					if (board[i + j] == other && board[i + j + 1] == other)
					{
						score -= 2;
					}
				}
			}
		}
		// Check columns
		score = 0;
		for (int i = 0; i < size - 1; i++)
		{
			for (int j = 0; j < boardsize - size; j += size)
			{
				if (board[i + j] != 0)
				{
					if (board[i + j] == player && board[i + j + 1] == player)
					{
						score += 2;
					}
					if (board[i + j] == other && board[i + j + 1] == other)
					{
						score -= 2;
					}
				}
			}
		}
		// Check forward diagonals
		for (int i = 0; i < boardsize - size - 1; i += size + 1)
		{
			if (board[i] != 0)
			{
				if (board[i] == player && board[i + size + 1] == player)
				{
					score += 2;
				}
				if (board[i] == other && board[i + size + 1] == other)
				{
					score -= 2;
				}
			}
		}
		// Check backward diagonals
		for (int i = size; i < boardsize - size + 1; i += size - 1)
		{
			if (board[i] != 0)
			{
				if (board[i] == player && board[i + size - 1] == player)
				{
					score += 2;
				}
				if (board[i] == other && board[i + size - 1] == other)
				{
					score -= 2;
				}
			}
		}
		if (!maximizing)
			empties *= -1;
		return Moves(score + empties);
	}
	winner = CheckWinner(board);
	if (winner != 0)
	{
		int score = 10;
		
		switch (winner)
		{
		case 1:
			return Moves(-score * empties - 1);
			break;
		case 2:
			return Moves(score * empties + 1);
			break;
		case -1:
			return Moves(0);
			break;
		default:
			std::cout << "wtf\n";
			break;
		}
	}

	for (int i = 0; i < boardsize; i++)
	{
		if (board[i] == 0)
		{
			Moves move;
			move.index = i;
			board[i] = player;
			if (maximizing)
			{
				move.score = Search(board, depth - 1, alpha, beta, 0).score;
				alpha = alpha > move.score ? alpha : move.score;
				if (beta <= alpha)
				{
					moves.push_back(move);
					board[i] = 0;
					break;
				}
			}
			else
			{
				move.score = Search(board, depth - 1, alpha, beta, 1).score;
				beta = beta < move.score ? beta : move.score;
				if (beta <= alpha)
				{
					moves.push_back(move);
					board[i] = 0;
					break;
				}
			}
			moves.push_back(move);
			board[i] = 0;
		}
	}

	int bestmove = 0;
	if (maximizing) // ai
	{
		int bestscore = -LONG_MAX;
		for (int i = 0; i < moves.size(); i++)
		{
			if (moves[i].score > bestscore)
			{
				bestmove = i;
				bestscore = moves[i].score;
			}
		}
	}
	else
	{
		int bestscore = LONG_MAX;
		for (int i = 0; i < moves.size(); i++)
		{
			if (moves[i].score < bestscore)
			{
				bestmove = i;
				bestscore = moves[i].score;
			}
		}
	}
	//std::cout << "Score: " << moves[bestmove].score << "\n";
	//std::cout << "moves[" << bestmove << "] = " << moves[bestmove].index << "\n";
	//std::cout << bestmove << "\n";
	//std::cout << moves[bestmove].index << "\n";
	return moves[bestmove];
}