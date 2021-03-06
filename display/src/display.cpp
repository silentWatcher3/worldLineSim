#include "../display.hpp"
// @note @future -> Later try to get this into a single display.hpp for easier use with modifications later on

#include "adapter.hpp"
#include "verse.hpp"
#include "world_tree.hpp"

#ifdef _WIN32
#include <windows.h>
#endif // _WIN32


#include <iostream>
#include <chrono>
#include <thread>
#include <future>

using namespace nanogui;

using std::chrono::high_resolution_clock;
void Display::displayCurrentState() const {
	std::clog << "Total " << this->parent_verse->worldTree->num_nodes << " nodes in the world tree\n";
}

void Display::start_input_daemon()
{
	std::thread([&]() mutable {
		thread_local std::string entered_command;
		do {
			//std::future<void> cmd_future = std::async(std::launch::async, [&]() mutable { std::getline(std::cin, entered_command); });
			//cmd_future.wait();
			std::getline(std::cin, entered_command);

			std::clog << "\n\n\nGot command: \"" << entered_command << "\"\n\n" << std::endl;
		} while (this->enabled());
	}).detach();
}

void Display::startDisplay() {
	this->start_input_daemon();

	std::clog << '[' << (high_resolution_clock::now() - loggingStart).count() << ']' << " Display Started...\n";

	//FormHelper* gui = new FormHelper(display_screen);
	//gui->add_group("Multiverse");
	//gui->add_group("Legend");

	ref<TextBox> header = new TextBox(this, "WorldLine Simulation v0.271");
	header->set_editable(false);

	multiverse_window = new Window(this, "MultiVerse");
	multiverse_window->set_position({ 0 + 5, this->height() / 10 + 5 });	// it is (y,x)
	multiverse_window->set_layout(new GridLayout());
	multiverse_window->set_height(static_cast<int>(0.90f * this->height()) - 10);
	multiverse_window->set_width(static_cast<int>(0.80f * this->width()) - 5);


	for (auto i = 0; i < this->parent_verse->worldTree->num_nodes; ++i)
		new Window(multiverse_window, "Universe #" + std::to_string(i));


	legend_window = new Window(this, "Legend");
	legend_window->set_layout(new GroupLayout());
	legend_window->set_position({
		static_cast<int>(0.80f * this->width()) - 5 + 5 + 5,	// 80% if width +- margins 
		multiverse_window->position().y(),
		});
	legend_window->set_height(static_cast<int>(0.90f * this->height()) - 10);
	legend_window->set_width(static_cast<int>(0.20f * this->width()) - 5);

	new Label(legend_window, "All worlds continue on diff. threads, w/o blocking the display, or the verse");
	new Label(legend_window, "After chosing a particular world");
	new Label(legend_window, "Commands");

	auto command_area = new TextArea(legend_window);
	/*
	-   N - Namaste World(New)");
	-   P - Pause");
	-   R - Resume");
	-   T - Time Travel !!");
	-   L - Logs (of World)");
	-   V - Logs (of Verse)"
	*/

	this->set_visible(true);
	this->perform_layout();

	std::thread([&]() mutable {
		while (this->enabled())
		{
			header->set_height(0.10f * this->height());
			header->set_width(this->width());

			multiverse_window->set_position({ 0 + 5, this->height() / 10 + 5 });	// it is (y,x)
			multiverse_window->set_layout(new GridLayout());
			multiverse_window->set_height(static_cast<int>(0.90f * this->height()) - 10);
			multiverse_window->set_width(static_cast<int>(0.80f * this->width()) - 5);

			legend_window->set_position({
				static_cast<int>(0.80f * this->width()) - 5 + 5 + 5,	// 80% if width +- margins 
				multiverse_window->position().y(),
				});
			legend_window->set_height(static_cast<int>(0.90f * this->height()) - 10);
			legend_window->set_width(static_cast<int>(0.20f * this->width()) - 5);

			std::this_thread::sleep_for( std::chrono::milliseconds(1000 / 40) );	// 40Hz
		}
		}).detach();

	nanogui::mainloop(1000/60.0f);	// Refresh every 1000/60 seconds, ie. 60Hz
}

void Display::showExiting() {
	//clear();
	//clearAll();

	//mvwaddstr(stdscr, this->_terminal_y/2, (this->_terminal_x - 15)/2, "About to Exit !");
	std::clog << "Exiting...";
}

void Display::stopDisplay() {
	std::clog << "Exiting NOW!";
}


void Display::pauseRendering(){
	this->paused = true;
}

void Display::resumeRendering(){
	this->paused = false;
}

std::shared_ptr<DisplayAdapter> Display::newNodeAdapter(World_Node* node){
	// @note - Not using the_occupy_currently
	//this->main_area->updateDimen();
	//this->pauseRendering();

	//int y_corner;
	//int x_corner;
	//int index_num = queue.size();
	//if(index_num == 8){
	//	queue.pop();

	//	index_num = 7;
	//}

	//int bigBox_coord_x;
	//int bigBox_coord_y;	// the big box's corner coord

	//bigBox_coord_y = ((main_area->getmax_y()+1)/2) * (index_num / 4);	// first row or second row
	//bigBox_coord_x = ((main_area->getmax_x()+1)/4) * (index_num % 4);

	//if( index_num %2 == 0 ){
	//	y_corner = bigBox_coord_y;
	//}else{
	//	y_corner = bigBox_coord_y + 2;	// 2 lines below
	//}

	//	// here what we want is to `horizonatlly center` the DisplayAdapter inside this big_box
	//x_corner = bigBox_coord_x + ( (main_area->getmax_x()+1)/4 - adapters_width ) / 2;

	//std::shared_ptr<DisplayAdapter> adapter{ 
	//	new DisplayAdapter(
	//		this->shared_from_this(), 
	//		node, 
	//		this->adapters_height, 
	//		this->adapters_width,
	//		y_corner,
	//		x_corner
	//	)
	//};

	//queue.push(adapter->node, adapter);

	//adapter->update();	// get it on screen
	//this->main_area->refresh();

	//return adapter;
	return nullptr;
}

/*void Display::helpScreen(){
	//using namespace std::chrono_literals;
	//this->curses_init();	// @note - single_term itself manages if curses is already initialised

	//this->resumeRendering();
	//this->main_area->disable();
	//this->legend_area->disable();

	//SubWindow helpArea{ 0, 0, 3, 0 };	// don't make it a raw pointer, since for it's destructor to be called, it's scope should end, which won't happen if we use new SubWindow, with raw pointer

	//helpArea.box();
	//auto async_input = this->get_async_input();
	//
	//helpArea.addstr(1, 1, "Help Guide");
	//helpArea.nladdstr("====================");

	//helpArea.newline();
	//helpArea.nladdstr("*All worlds continue on diff. threads,w/o blocking the display, or the verse");

	//helpArea.newline();

	//helpArea.nladdstr("- Type the id to chose a particular world");

	//helpArea.nladdstr("- Commands");
	//helpArea.nladdstr("-   N - Namaste World(New)");
	//helpArea.nladdstr("-   P - Pause");
	//helpArea.nladdstr("-   R - Resume");
	//helpArea.nladdstr("-   T - Time Travel !!");
	//helpArea.nladdstr("-   L - Logs (of World)");
	//helpArea.nladdstr("-   V - Logs (of Verse)");

	//helpArea.addstr(-3, 1, "If you find a problem...");
	//helpArea.nladdstr("Please solve it yourselves");
	//helpArea.nladdstr(":copy: AdityaG15 :D");

	//top_area->addstr(title, position::MIDDLE);

	//int cur_dimen_y, cur_dimen_x;
	//getmaxyx(stdscr, this->_terminal_y, this->_terminal_x);
	//while( true ){
	//	main_area->updateDimen();
	//	legend_area->updateDimen();

	//	getmaxyx(stdscr, cur_dimen_y, cur_dimen_x);
	//	if( cur_dimen_y != _terminal_y || cur_dimen_x != _terminal_x ){
	//		this->clearAll(); // to clean out the previous borders
	//	}

	//	top_area->box();
	//	main_area->box();
	//	legend_area->box();

	//	this->refreshAll();

	//	if( async_input.wait_for(100ms) == std::future_status::ready ){
	//		char c = async_input.get();
	//		try{
	//			if(c == Display::QUIT_KEY){
	//				this->reset_curses();	// free up memory

	//				return;
	//			}
	//			helpArea.refresh();
	//		}catch(std::future_error&){
	//			raise(SIGTERM);

	//			this->parent_verse->kaal_day("Display");
	//			return;
	//		}

	//		if( c == Display::QUIT_KEY )	return;
	//		helpArea.refresh();
	//		async_input = this->get_async_input();	// restart that task
	//	}

	//	std::this_thread::sleep_for(200ms);
	//}

	//this->optionScreen();
}*/

void Display::updateScreen(){
	//using namespace std::chrono_literals;
	//if ( ! this->dispMutex.try_lock_for(1s)) {	// there will be gap of ATLEAST 1 second before any updations happen on screen
	//	return;	// so repeating threads in same interval of time, will exit
	//}

	//this->dispMutex.lock();
	//if( !top_area || !main_area || !legend_area )
	//	this->runInitTasks();

	//legend_area->addstr(1, 1, "Legend");

	//legend_area->nladdstr("*All worlds continue on diff. threads,w/o blocking the display, or the verse");

	//legend_area->newline();

	//legend_area->nladdstr("- Type the id to chose a particular world");

	//legend_area->nladdstr("- Commands");
	//legend_area->nladdstr("-   N - Namaste World(New)");
	//legend_area->nladdstr("-   P - Pause");
	//legend_area->nladdstr("-   R - Resume");
	//legend_area->nladdstr("-   T - Time Travel !!");
	//legend_area->nladdstr("-   L - Logs (of World)");
	//legend_area->nladdstr("-   V - Logs (of Verse)");

	//legend_area->addstr(-3, 1, "If you find a problem...");
	//legend_area->nladdstr("Please solve it yourselves");
	//legend_area->nladdstr(":copy: AdityaG15 :D");

	//int cur_dimen_y, cur_dimen_x;
	//getmaxyx(stdscr, this->_terminal_y, this->_terminal_x);

	//main_area->updateDimen();
	//legend_area->updateDimen();

	//getmaxyx(stdscr, cur_dimen_y, cur_dimen_x);
	//if( cur_dimen_y != _terminal_y || cur_dimen_x != _terminal_x ){
	//	wclear(stdscr); // to clean out the previous borders
	//}

	//top_area->box();
	//main_area->box();
	//legend_area->box();

	//top_area->moveCursor(1,1);
	//top_area->addstr(title, position::MIDDLE);

	//wrefresh(stdscr);
	//this->dispMutex.unlock();

}

#if 0
void Display::runInitTasks(){

	this->curses_init();	// also handles the case when curses wis already initialised

	if( !top_area ) this->top_area.reset( new SubWindow( 3, 0, 0, 0 ) );
	if( !main_area ) this->main_area.reset( new SubWindow( 0, 0.8f * _terminal_x, 3, 0 ) );
	if( !legend_area ) this->legend_area.reset( new SubWindow( 0, 0, 3, 0.8f*_terminal_x ) );

	if( top_area->enabled ) this->top_area->enable();
	if( main_area->enabled ) this->top_area->enable();
	if( legend_area->enabled ) this->top_area->enable();

	this->resumeRendering();
	std::thread(&Display::startEventLoop, this).detach();	// the event loop should be on a different thread

}

// @deprecated
void Display::render(){
	using namespace std::chrono_literals;

	printScreen();	// start of with an already printed screen

	auto async_input = this->get_async_input();

	int cur_dimen_y, cur_dimen_x;
	getmaxyx(stdscr, this->_terminal_y, this->_terminal_x);
	while( ! this->paused ){
		main_area->updateDimen();
		legend_area->updateDimen();

		getmaxyx(stdscr, cur_dimen_y, cur_dimen_x);
		if( cur_dimen_y != _terminal_y || cur_dimen_x != _terminal_x ){
			wclear(stdscr); // to clean out the previous borders
		}

		top_area->box();
		main_area->box();
		legend_area->box();

		top_area->moveCursor(1,1);
		top_area->addstr(title, position::MIDDLE);

		wrefresh(stdscr);

			// time to take input
		if( async_input.wait_for(100ms) == std::future_status::ready ){
			legend_area->addstr(static_cast<int>(0.75f*legend_area->getmax_y()), 1, "You entered -> ");
			char c;
			try{
				c = async_input.get();
				legend_area->addch(c);

				if(c == Display::QUIT_KEY){
					this->reset_curses();	// free up memory
					this->pauseRendering();

					// @note - Or maybe use inter thread communication here, using condition_variable, so to wait till verse tells all done, then this will return, and the destructor again sends signal to verse after all things done in destructor, then verse can be sure that all destructed
					// @note - UNCOMMENT next line, later
					// return this->parent_verse->kaal_day("Display");    // also passing the source so that it doesn't actually try to end this
					return;
				}else if(c == Display::HELP_KEY)
				{
					this->clearAll();
					return this->helpScreen();
				}
				
				legend_area->refresh();
			}catch(std::future_error & e){
				raise(SIGTERM);

				this->parent_verse->kaal_day("Display");
				return;
			}

			if( c == Display::QUIT_KEY )	return;
			legend_area->refresh();
			async_input = this->get_async_input();	// restart that task
		}

		std::this_thread::sleep_for(200ms);
	}
}
#endif

Display::Display(Verse* parent) :
	//title("WorldLine Simulator v0.271", "Created by Aditya Gupta and Steins; Gate"),
	parent_verse(parent),
	loggingStart(std::chrono::high_resolution_clock::now()),
	Screen({ 800,1000 }, "WorldLine Simulator v0.271"/*, false, true */)
{
	this->shortcut_map = {
		{":help", [&]() mutable {
			this->help_window->set_visible(true);
			this->legend_window->set_visible(false);
			this->multiverse_window->set_visible(false);
		}},
		{":about", [&]() mutable {
			// @future - No plan to add this now
		}},
		{":web", [&]() mutable {
			//nanogui::ImageView* 
		}},
		{":pause", [&]() mutable {
			
		}}
	};

	this->inc_ref();

//#ifdef _WIN32
//	MessageBoxA(nullptr, "Hi message", NULL, MB_ICONERROR | MB_OK);
//	return;
//#endif // _WIN32

	Color background_col = { 20,20,23, 255 };
	this->set_background(background_col);
}

Display::~Display() {
	nanogui::shutdown();
}
