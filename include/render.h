#pragma once

#include <cstddef>
#include <filesystem>
#include <map>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "ents.h"
#include "state.h"

struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;
union SDL_Event;


class Render {
public:
	explicit Render(std::pair<int, int> size);
	~Render();
	void initialize();

	Render(const Render&) = delete;
	Render& operator=(const Render&) = delete;
	Render(Render&&) = delete;
	Render& operator=(Render&&) = delete;

	void render(const State& state);
	std::optional<std::string> readConsoleLine(const std::string& prompt = ">>> ", const State* state = nullptr);
	std::optional<std::string> readConsoleLineNonBlocking(const std::string& prompt, const State* state, bool clearConsole = false);
	void waitForInput();
	void addValueToConsoleHistory(std::string val);

private:
	struct FontHandle;

	std::string sideTypeToImageName(SideType sideType) const;
	std::pair<int, int> getPosition(std::size_t index, const std::vector<Ent*>& entities, bool isEnemy = false, const Ent* ent = nullptr) const;
	std::string resolveReverseIdName(int value, const std::map<int, std::string>& mapping, const std::string& label) const;
	float getRenderScale(const Ent& ent) const;
	int getCardHeight(const Ent& ent) const;
	FontHandle* getFont(const std::string& key, int baseSize);

	void drawConsoleOverlay();
	void submitConsoleLine(const std::string& line);
	std::optional<std::string> handleConsoleEvent(const SDL_Event& event);
	std::optional<std::string> getConsoleLine();

	void drawShieldIndicator(const Ent& ent, int x, int y, int cardW, int cardH, bool isEnemy, int squareSize);
	std::vector<std::pair<int, int>> getConditionIconPositions(int x, int y, int cardW, int cardH, int count) const;
	void drawHpBar(const Ent& ent, int x, int y, int cardW, int cardH);
	void drawDiceLayout(const Ent& ent, int x, int y, int cardW, int cardH, bool isEnemy, int squareSize);
	void drawSidePreview(const std::optional<Side>& side, int x, int y, int size = 24);
	void renderStateOverlay(const State& state);
	void renderEnt(const Ent& ent, std::pair<int, int> position, bool isEnemy = false);
	void renderAttackLines(const State& state);

private:
	SDL_Window* window_;
	SDL_Renderer* renderer_;
	std::pair<int, int> size_;
	std::filesystem::path imageDir_;
	std::map<std::string, FontHandle*> fontCache_;
	std::map<std::pair<std::string, int>, SDL_Texture*> imageCache_;
	std::string consolePrompt_;
	std::string consoleBuffer_;
	std::vector<std::string> consoleHistory_;
	std::vector<std::string> consoleLines_;
};