# Typing_Game
Authur: Nazia Hasan

This project is a terminal-based typing test built using C and Ncurses library.

It offers real-time feedback, multiple difficulty levels, live statistics, and an engaging 60-second typing challenge!

⸻

Features
	•	Input redirection to load a custom word list.
	•	Difficulty levels:
	•	Easy: Short words (1–4 letters)
	•	Medium: Medium-length words (5–7 letters)
	•	Hard: Longer words (8–19 letters)
	•	Real-time character feedback:
	•	Correct input in green
	•	Incorrect input in red
	•	Supports backspace corrections
	•	Live Statistics during typing:
	•	WPM (Words per Minute)
	•	Keystrokes
	•	Mistakes
	•	CPS (Characters per Second)
	•	Accuracy (%)
	•	Game Controls:
	•	ESC key: Quit anytime
	•	4 key: Restart the test anytime
	•	Final summary shown after time ends or manual quit.

⸻

How to Run
	1.	Make sure GCC and Ncurses library are installed.
	•	On Ubuntu/Debian:
     sudo apt-get install libncurses5-dev libncursesw5-dev
     
2.	Clone this repository or download the .c file.

3.	Compile the program:
    gcc typing_test.c -o typing_test -lncurses
    
4.Run the program with a word list redirected as input:
  ./typing_test < wordlist.txt
  
5.Controls

Key    Action
Enter Start typing
ESC   Quit anytime
4     Restart typing test
Backspace  Delete the last typed character

