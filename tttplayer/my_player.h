#pragma once
#include "player.h"
#include <string>

class GoodPlayer : public Player {
	std::string _name;
	Point _last_move;
	Mark _my_mark;

	bool block_last_move(const GameView& game, Point& result);
	bool use_adv(const GameView& game, Point& result);
	void choose_side(Point& left, Point& right, Point& result, const Boundary& b, const GameView& game);
	void validate_move(int i, Point& result, const Boundary& b, const GameView& game);

public:
	GoodPlayer(const std::string& name) : _name(name) {}
	std::string get_name() const override { return _name; }
	Point play(const GameView& game) override;
	void assign_mark(Mark player_mark) override;
	void notify(const GameView&, const Event&) override;
};

class BasicObserver : public Observer {
	std::ostream& _out;

	std::ostream& _print_mark(Mark mark);
public:
	BasicObserver(std::ostream& out_stream) : _out(out_stream) {}
	void notify(const GameView&, const Event& event) override;
};

class DebugObserver : public Observer {
	std::ostream& _out;
	std::ostream& _print_mark(Mark mark);

public:
	DebugObserver(std::ostream& out_stream) : _out(out_stream) {}
	void notify(const GameView&, const Event& event) override;
};
