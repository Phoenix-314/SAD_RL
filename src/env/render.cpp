#include "env/render.h"

#include <algorithm>
#include <array>
#include <cctype>
#include <cmath>
#include <filesystem>
#include <sstream>
#include <stdexcept>
#include <string_view>
#include <iostream>

#include "env/reversedIDs.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

namespace {
constexpr std::pair<int, int> kCardSize{220, 120};
constexpr int kSideSquareSize = 110;
constexpr int kConsoleHeight = 72;
constexpr int kConsoleHistoryLimit = 8;
constexpr int kHistoryLineLimit = 2;

std::string trim(const std::string& text)
{
    const auto begin = text.find_first_not_of(" \t\r\n");
    if (begin == std::string::npos) {
        return {};
    }
    const auto end = text.find_last_not_of(" \t\r\n");
    return text.substr(begin, end - begin + 1);
}

std::string joinArray(const std::array<int, 6>& values)
{
    std::ostringstream stream;
    stream << '[';
    for (std::size_t i = 0; i < values.size(); ++i) {
        if (i != 0) {
            stream << ", ";
        }
        stream << values[i];
    }
    stream << ']';
    return stream.str();
}

std::string joinArray(const std::array<int, 5>& values)
{
    std::ostringstream stream;
    stream << '[';
    for (std::size_t i = 0; i < values.size(); ++i) {
        if (i != 0) {
            stream << ", ";
        }
        stream << values[i];
    }
    stream << ']';
    return stream.str();
}

std::vector<Ent*> toVector(const std::array<Ent*, 5>& values)
{
    return std::vector<Ent*>{values.begin(), values.end()};
}

bool isPetrified(const Ent& ent, std::size_t index)
{
    return index < ent.petrified.size() && ent.petrified[index];
}

bool hasSpecialHp(const Ent& ent, std::size_t index)
{
    return index < ent.specialHP.size() && ent.specialHP[index];
}

SDL_Texture* loadTexture(SDL_Renderer* renderer, const std::filesystem::path& path)
{
    SDL_Texture* texture = IMG_LoadTexture(renderer, path.string().c_str());
    if (texture == nullptr) {
        throw std::runtime_error(std::string("Failed to load texture '") + path.string() + "': " + SDL_GetError());
    }
    return texture;
}

void drawText(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, int x, int y, SDL_Color color)
{
    if (font == nullptr || text.empty()) {
        return;
    }

    SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), color);
    if (surface == nullptr) {
        return;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (texture != nullptr) {
        SDL_Rect rect{x, y, surface->w, surface->h};
        SDL_RenderCopy(renderer, texture, nullptr, &rect);
        SDL_DestroyTexture(texture);
    }

    SDL_FreeSurface(surface);
}

void drawCenteredText(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, SDL_FRect bounds, SDL_Color color)
{
    if (font == nullptr || text.empty()) {
        return;
    }

    SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), color);
    if (surface == nullptr) {
        return;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (texture != nullptr) {
        SDL_Rect rect{
            static_cast<int>(bounds.x + (bounds.w - static_cast<float>(surface->w)) / 2.0f),
            static_cast<int>(bounds.y + (bounds.h - static_cast<float>(surface->h)) / 2.0f),
            surface->w,
            surface->h
        };
        SDL_RenderCopy(renderer, texture, nullptr, &rect);
        SDL_DestroyTexture(texture);
    }

    SDL_FreeSurface(surface);
}

void drawCircleOutline(SDL_Renderer* renderer, int centerX, int centerY, int radius, SDL_Color color)
{
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    constexpr int segments = 24;
    SDL_FPoint previous{static_cast<float>(centerX), static_cast<float>(centerY)};
    for (int i = 1; i <= segments; ++i) {
        const float angle = static_cast<float>(i) * 2.0f * 3.1415926535f / static_cast<float>(segments);
        const SDL_FPoint current{
            static_cast<float>(centerX) + std::cos(angle) * static_cast<float>(radius),
            static_cast<float>(centerY) + std::sin(angle) * static_cast<float>(radius)
        };
        SDL_RenderDrawLine(renderer, static_cast<int>(std::round(previous.x)), static_cast<int>(std::round(previous.y)), static_cast<int>(std::round(current.x)), static_cast<int>(std::round(current.y)));
        previous = current;
    }
}

std::filesystem::path pickFontPath()
{
    const std::filesystem::path fontPath = std::filesystem::path(__FILE__).parent_path().parent_path().parent_path() / "assets" / "segoeui.ttf";
    if (std::filesystem::exists(fontPath)) {
        return fontPath;
    }
    std::cout << "Warning: Font file not found at " << fontPath << ". No fonts can be rendered." << std::endl;
    return {};
}

std::string pointerTargetName(const std::map<int, std::string>& mapping, int value)
{
    const auto it = mapping.find(value);
    if (it == mapping.end()) {
        return std::to_string(value);
    }
    return it->second;
}

} // namespace

struct Render::FontHandle {
    TTF_Font* font = nullptr;
    int size = 0;
    std::filesystem::path path;
};

Render::Render(std::pair<int, int> size)
    : window_(nullptr)
    , renderer_(nullptr)
    , size_(size)
    , imageDir_(std::filesystem::path(__FILE__).parent_path().parent_path().parent_path() / "assets")
    , consolePrompt_(">>> ")
    , consoleBuffer_()
    , consoleHistory_()
    , consoleLines_()
{
}
void Render::initialize() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
        throw std::runtime_error(std::string("SDL_Init failed: ") + std::string(SDL_GetError()));
    }

    IMG_Init(IMG_INIT_PNG);
    // if (!IMG_Init(IMG_INIT_PNG)) {
    //     throw std::runtime_error(std::string("IMG_Init failed: ") + SDL_GetError());
    // }

    if (TTF_Init() < 0) {
        throw std::runtime_error(std::string("TTF_Init failed: ") + SDL_GetError());
    }

    window_ = SDL_CreateWindow("Ent Renderer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, size_.first, size_.second, SDL_WINDOW_RESIZABLE);
    if (window_ == nullptr) {
        throw std::runtime_error(std::string("SDL_CreateWindow failed: ") + SDL_GetError());
    }

    renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);
    if (renderer_ == nullptr) {
        throw std::runtime_error(std::string("SDL_CreateRenderer failed: ") + SDL_GetError());
    }

    SDL_SetRenderDrawBlendMode(renderer_, SDL_BLENDMODE_BLEND);
    SDL_SetWindowTitle(window_, "Slice and Dice - RL");
    SDL_SetRenderDrawColor(renderer_, 255, 220, 220, 255);
    SDL_RenderClear(renderer_);
    SDL_RenderPresent(renderer_);
}

Render::~Render()
{
    for (auto& [key, texture] : imageCache_) {
        (void)key;
        SDL_DestroyTexture(texture);
    }
    imageCache_.clear();

    for (auto& [key, handle] : fontCache_) {
        (void)key;
        if (handle != nullptr) {
            if (handle->font != nullptr) {
                TTF_CloseFont(handle->font);
            }
            delete handle;
        }
    }
    fontCache_.clear();

    if (renderer_ != nullptr) {
        SDL_DestroyRenderer(renderer_);
        renderer_ = nullptr;
    }

    if (window_ != nullptr) {
        SDL_DestroyWindow(window_);
        window_ = nullptr;
    }

    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

std::string Render::sideTypeToImageName(SideType sideType) const
{
    static const std::map<SideType, std::string> mapping{
        {SideType::DAMAGE, "DAMAGE.png"},
        {SideType::SHIELD, "SHIELD.png"},
        {SideType::HEAL, "HEAL.png"},
        {SideType::HEALSHIELD, "HEALSHIELD.png"},
        {SideType::MANA, "MANA.png"},
        {SideType::BLANK, "BLANK.png"},
        {SideType::REROLL, "REROLL.png"},
        {SideType::REDIRECTINCOMING, "REDIRECTINCOMING.png"},
        {SideType::SUMMON, "SUMMON.png"},
        {SideType::RECHARGE, "RECHARGE.png"},
        {SideType::RESURRECT, "RESURRECT.png"},
        {SideType::ENCHANT, "ENCHANT.png"},
        {SideType::KILL, "KILL.png"},
        {SideType::UNDYING, "UNDYING.png"},
        {SideType::DODGE, "DODGE.png"},
        {SideType::SELF_DAMAGE, "SELF_DAMAGE.png"},
        {SideType::DAMAGE_ALL, "DAMAGE_ALL.png"},
        {SideType::DAMAGE_EVERYONE, "DAMAGE_EVERYONE.png"},
        {SideType::DAMAGE_FLANKING, "DAMAGE_FLANKING.png"},
        {SideType::SHIELD_ALL, "SHIELD_ALL.png"},
        {SideType::HEAL_ALL, "HEAL_ALL.png"},
        {SideType::STUN, "STUN.png"},
    };

    const auto it = mapping.find(sideType);
    return it != mapping.end() ? it->second : std::string{"BLANK.png"};
}

std::pair<int, int> Render::getPosition(std::size_t index, const std::vector<Ent*>& entities, bool isEnemy, const Ent* ent) const
{
    const auto [width, height] = size_;
    constexpr int marginX = 40;
    constexpr int topMargin = 40;
    constexpr int bottomMargin = 180;

    const int cardW = kCardSize.first;
    const int cardH = ent != nullptr ? getCardHeight(*ent) : kCardSize.second;

    int y = (height - cardH) / 2;
    if (entities.size() > 1) {
        int totalSize = 0;
        for (const Ent* current : entities) {
            totalSize += std::max(1, static_cast<int>(current != nullptr ? current->size : 30));
        }

        if (totalSize <= 0) {
            totalSize = static_cast<int>(std::max<std::size_t>(1, entities.size()) * 30);
        }

        int priorSize = 0;
        for (std::size_t i = 0; i < std::min(index, entities.size()); ++i) {
            priorSize += std::max(1, static_cast<int>(entities[i] != nullptr ? entities[i]->size : 30));
        }

        const int availableHeight = std::max(0, height - topMargin - bottomMargin - cardH);
        y = topMargin + static_cast<int>(std::round((static_cast<double>(priorSize) / static_cast<double>(totalSize)) * availableHeight));
        y = std::min(std::max(y, topMargin), std::max(topMargin, height - bottomMargin - cardH));
    }

    int x = isEnemy ? width - marginX - cardW : marginX;
    if (isEnemy && ent != nullptr) {
        x = width - marginX - cardW + (ent->backrow ? 40 : 0);
    }

    return {x, y};
}

std::string Render::resolveReverseIdName(int value, const std::map<int, std::string>& mapping, const std::string& label) const
{
    const auto it = mapping.find(value);
    if (it == mapping.end()) {
        throw std::out_of_range("Missing " + label + " mapping for value " + std::to_string(value));
    }
    return it->second;
}

float Render::getRenderScale(const Ent& ent) const
{
    const int size = ent.size != 0 ? ent.size : 30;
    return static_cast<float>(size) / 30.0f;
}

int Render::getCardHeight(const Ent& ent) const
{
    return std::max(1, static_cast<int>(std::round(kCardSize.second * getRenderScale(ent))));
}

Render::FontHandle* Render::getFont(const std::string& key, int baseSize)
{
    const std::string cacheKey = key + ":" + std::to_string(baseSize);
    const auto it = fontCache_.find(cacheKey);
    if (it != fontCache_.end()) {
        return it->second;
    }

    auto* handle = new FontHandle{};
    handle->size = baseSize;

    const std::filesystem::path fontPath = pickFontPath();
    if (!fontPath.empty()) {
        handle->path = fontPath;
        handle->font = TTF_OpenFont(fontPath.string().c_str(), baseSize);
    }

    fontCache_.emplace(cacheKey, handle);
    return handle;
}

void Render::drawConsoleOverlay()
{
    int width = 0;
    int height = 0;
    SDL_GetWindowSize(window_, &width, &height);
    width -= 400;

    const int panelTop = height - kConsoleHeight;
    SDL_SetRenderDrawColor(renderer_, 20, 20, 24, 230);
    SDL_Rect overlay{0, panelTop, width, kConsoleHeight};
    SDL_RenderFillRect(renderer_, &overlay);

    SDL_SetRenderDrawColor(renderer_, 220, 220, 220, 255);
    SDL_RenderDrawRect(renderer_, &overlay);

    TTF_Font* historyFont = getFont("console_history", 18)->font;
    TTF_Font* promptFont = getFont("console", 22)->font;

    const int historyStartY = panelTop + 6;
    const std::size_t historyStart = consoleHistory_.size() > kHistoryLineLimit ? consoleHistory_.size() - kHistoryLineLimit : 0;
    for (std::size_t i = historyStart; i < consoleHistory_.size(); ++i) {
        drawText(renderer_, historyFont, consoleHistory_[i], 12, historyStartY + static_cast<int>((i - historyStart) * 18), {220, 220, 220, 255});
    }

    const int promptY = panelTop + kConsoleHeight - 28;
    drawText(renderer_, promptFont, consolePrompt_ + consoleBuffer_, 12, promptY, {255, 255, 255, 255});
}

void Render::submitConsoleLine(const std::string& line)
{
    consoleLines_.push_back(line);
    consoleHistory_.push_back(consolePrompt_ + line);
    if (consoleHistory_.size() > kConsoleHistoryLimit) {
        consoleHistory_ = std::vector<std::string>(consoleHistory_.end() - kConsoleHistoryLimit, consoleHistory_.end());
    }
}

void Render::addValueToConsoleHistory(std::string val)
{
    consoleHistory_.push_back("Value: " + val);
    if (consoleHistory_.size() > kConsoleHistoryLimit) {
        consoleHistory_ = std::vector<std::string>(consoleHistory_.end() - kConsoleHistoryLimit, consoleHistory_.end());
    }
}

std::optional<std::string> Render::handleConsoleEvent(const SDL_Event& event)
{
    if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_KP_ENTER) {
            const std::string line = consoleBuffer_;
            consoleBuffer_.clear();
            if (!trim(line).empty()) {
                submitConsoleLine(line);
            }
            return line;
        }

        if (event.key.keysym.sym == SDLK_BACKSPACE) {
            if (!consoleBuffer_.empty()) {
                consoleBuffer_.pop_back();
            }
            return std::nullopt;
        }

        if (event.key.keysym.sym == SDLK_TAB) {
            consoleBuffer_ += "    ";
            return std::nullopt;
        }

        return std::nullopt;
    }

    if (event.type == SDL_TEXTINPUT) {
        consoleBuffer_ += event.text.text;
    }

    return std::nullopt;
}

std::optional<std::string> Render::getConsoleLine()
{
    if (consoleLines_.empty()) {
        return std::nullopt;
    }

    std::string line = consoleLines_.front();
    consoleLines_.erase(consoleLines_.begin());
    return line;
}

std::optional<std::string> Render::readConsoleLineNonBlocking(const std::string& prompt, const State* state, bool clearConsole)
{
    if (clearConsole) {
        consolePrompt_ = prompt;
        consoleBuffer_.clear();
    }

    if (state != nullptr) {
        render(*state);
    }

    SDL_Event event{};
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            return std::nullopt;
        }

        if (event.type == SDL_KEYDOWN || event.type == SDL_TEXTINPUT) {
            const std::optional<std::string> result = handleConsoleEvent(event);
            if (result.has_value()) {
                return result;
            }
        }
    }

    drawConsoleOverlay();
    SDL_RenderPresent(renderer_);
    SDL_Delay(16);

    return std::nullopt;
}
std::optional<std::string> Render::readConsoleLine(const std::string& prompt, const State* state)
{
    consolePrompt_ = prompt;
    consoleBuffer_.clear();

    if (state != nullptr) {
        render(*state);
    }

    SDL_Event event{};
    while (true) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                return std::nullopt;
            }

            if (event.type == SDL_KEYDOWN || event.type == SDL_TEXTINPUT) {
                const std::optional<std::string> result = handleConsoleEvent(event);
                if (result.has_value()) {
                    return result;
                }
            }
        }

        drawConsoleOverlay();
        SDL_RenderPresent(renderer_);
        SDL_Delay(16);
    }
}

void Render::waitForInput()
{
    SDL_Event event{};
    while (true) {
        if (!SDL_WaitEvent(&event)) {
            continue;
        }

        if (event.type == SDL_QUIT) {
            return;
        }

        if (event.type == SDL_KEYDOWN && (event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_KP_ENTER)) {
            return;
        }
    }
}

void Render::drawShieldIndicator(const Ent& ent, int x, int y, int cardW, int cardH, bool isEnemy, int squareSize)
{
    if (ent.shield == 0) {
        return;
    }

    constexpr int triangleW = 36;
    constexpr int pad = 8;

    int tipX = 0;
    int baseX = 0;
    if (isEnemy) {
        tipX = x + pad;
        baseX = x + pad + triangleW;
    } else {
        tipX = x + cardW - pad;
        baseX = x + cardW - pad - triangleW;
    }

    const int midY = y + cardH / 2;
    std::array<SDL_FPoint, 3> points{};
    if (isEnemy) {
        points = {
            SDL_FPoint{static_cast<float>(tipX), static_cast<float>(midY)},
            SDL_FPoint{static_cast<float>(baseX), static_cast<float>(y + pad)},
            SDL_FPoint{static_cast<float>(baseX), static_cast<float>(y + cardH - pad)}
        };
    } else {
        points = {
            SDL_FPoint{static_cast<float>(tipX), static_cast<float>(midY)},
            SDL_FPoint{static_cast<float>(baseX), static_cast<float>(y + pad)},
            SDL_FPoint{static_cast<float>(baseX), static_cast<float>(y + cardH - pad)}
        };
    }

    for (auto& point : points) {
        if (!isEnemy) {
            point.x += static_cast<float>(squareSize + 64);
        } else {
            point.x -= static_cast<float>(squareSize + 64);
        }
    }

    SDL_SetRenderDrawColor(renderer_, 120, 120, 120, 255);
    SDL_RenderDrawLine(renderer_, static_cast<int>(std::round(points[0].x)), static_cast<int>(std::round(points[0].y)), static_cast<int>(std::round(points[1].x)), static_cast<int>(std::round(points[1].y)));
    SDL_RenderDrawLine(renderer_, static_cast<int>(std::round(points[1].x)), static_cast<int>(std::round(points[1].y)), static_cast<int>(std::round(points[2].x)), static_cast<int>(std::round(points[2].y)));
    SDL_RenderDrawLine(renderer_, static_cast<int>(std::round(points[2].x)), static_cast<int>(std::round(points[2].y)), static_cast<int>(std::round(points[0].x)), static_cast<int>(std::round(points[0].y)));

    TTF_Font* font = getFont("shield", 24)->font;
    const SDL_FRect bounds{std::min({points[0].x, points[1].x, points[2].x}), std::min({points[0].y, points[1].y, points[2].y}), static_cast<float>(triangleW), static_cast<float>(cardH)};
    drawCenteredText(renderer_, font, std::to_string(ent.shield), bounds, {255, 255, 255, 255});
}

std::vector<std::pair<int, int>> Render::getConditionIconPositions(int x, int y, int cardW, int cardH, int count) const
{
    (void)cardW;
    if (count <= 0) {
        return {};
    }

    constexpr int iconSize = 16;
    constexpr int spacing = 4;
    constexpr int maxPerRow = 6;
    const int rowHeight = iconSize + spacing;
    const int rows = (count + maxPerRow - 1) / maxPerRow;

    std::vector<std::pair<int, int>> positions;
    positions.reserve(static_cast<std::size_t>(count));

    const int startX = x + 8;
    const int startY = y + cardH - 8 - (rows * rowHeight) + spacing;

    for (int i = 0; i < count; ++i) {
        const int row = i / maxPerRow;
        const int col = i % maxPerRow;
        positions.emplace_back(startX + col * (iconSize + spacing), startY + row * rowHeight);
    }

    return positions;
}

void Render::drawHpBar(const Ent& ent, int x, int y, int cardW, int cardH)
{
    const int maxHp = std::max(1, ent.maxHP);
    const int currentHp = std::max(0, ent.hp);

    int cellsPerRow = 10;
    int cellSize = 8;
    int spacing = 2;
    if (maxHp <= 15) {
        cellsPerRow = 5;
        cellSize = 12;
        spacing = 3;
    }
    const int barWidth = cellsPerRow * cellSize + (cellsPerRow - 1) * spacing;
    const int startX = x + (cardW - barWidth) / 2;
    const int startY = y + 8;

    for (int i = 0; i < maxHp; ++i) {
        const int row = i / cellsPerRow;
        const int col = i % cellsPerRow;
        const int cellX = startX + col * (cellSize + spacing);
        const int cellY = startY + row * (cellSize + spacing);
        SDL_Rect rect{cellX, cellY, cellSize, cellSize};

        if (i < currentHp) {
            SDL_SetRenderDrawColor(renderer_, 220, 40, 40, 255);
            SDL_RenderFillRect(renderer_, &rect);
            if (ent.armor != 0) {
                SDL_SetRenderDrawColor(renderer_, 180, 180, 180, 255);
                SDL_RenderDrawRect(renderer_, &rect);
            }

            if (hasSpecialHp(ent, static_cast<std::size_t>(i + 1))) {
                const float centerX = static_cast<float>(cellX + cellSize / 2);
                const float centerY = static_cast<float>(cellY + cellSize / 2);
                SDL_SetRenderDrawColor(renderer_, 255, 255, 255, 255);
                SDL_RenderDrawLine(renderer_, static_cast<int>(centerX - 2), static_cast<int>(centerY), static_cast<int>(centerX + 2), static_cast<int>(centerY));
                SDL_RenderDrawLine(renderer_, static_cast<int>(centerX), static_cast<int>(centerY - 2), static_cast<int>(centerX), static_cast<int>(centerY + 2));
            }
        } else {
            SDL_SetRenderDrawColor(renderer_, 120, 80, 140, 255);
            SDL_RenderDrawRect(renderer_, &rect);
        }
    }

    if (ent.hp < 0) {
        TTF_Font* font = getFont("negative_hp", 40)->font;
        drawCenteredText(renderer_, font, std::to_string(ent.hp), SDL_FRect{static_cast<float>(x), static_cast<float>(startY - 4), static_cast<float>(cardW), 32.0f}, {220, 40, 40, 255});
    }
}

void Render::drawDiceLayout(const Ent& ent, int x, int y, int cardW, int cardH, bool isEnemy, int squareSize)
{
    (void)cardW;
    if (ent.sides.empty()) {
        return;
    }

    const int offsetX = ent.shield != 0 ? squareSize + 110 : squareSize + 80;
    const int centerX = isEnemy ? x - offsetX - 80 : x + kCardSize.first + offsetX;
    const int layoutY = y + (cardH - 90) / 2;
    const int tileSize = std::min(static_cast<int>(cardH / 3.5f), 40);
    constexpr int spacing = 10;

    const std::array<std::pair<int, int>, 6> positions{{
        {0, 0},
        {-tileSize - spacing, tileSize + spacing},
        {0, tileSize + spacing},
        {tileSize + spacing, tileSize + spacing},
        {2 * (tileSize + spacing), tileSize + spacing},
        {0, 2 * (tileSize + spacing)}
    }};

    for (std::size_t idx = 0; idx < positions.size(); ++idx) {
        const Side* side = nullptr;
        if (idx == 0) {
            side = &ent.sides[0];
        } else if (idx <= 4) {
            side = &ent.sides[idx];
        } else {
            side = &ent.sides[5];
        }

        if (side == nullptr) {
            continue;
        }

        const std::string imageName = sideTypeToImageName(side->type);
        const std::filesystem::path imagePath = imageDir_ / imageName;
        if (!std::filesystem::exists(imagePath)) {
            continue;
        }

        const std::pair<std::string, int> imageKey{imageName, tileSize};
        SDL_Texture* texture = nullptr;
        const auto it = imageCache_.find(imageKey);
        if (it == imageCache_.end()) {
            texture = loadTexture(renderer_, imagePath);
            imageCache_.emplace(imageKey, texture);
        } else {
            texture = it->second;
        }

        const int drawX = centerX + positions[idx].first;
        const int drawY = layoutY + positions[idx].second;
        SDL_Rect target{drawX, drawY, tileSize, tileSize};
        SDL_RenderCopy(renderer_, texture, nullptr, &target);

        if (isPetrified(ent, idx)) {
            SDL_SetRenderDrawColor(renderer_, 120, 120, 120, 255);
            SDL_RenderDrawLine(renderer_, drawX + 4, drawY + 4, drawX + tileSize - 4, drawY + tileSize - 4);
            SDL_RenderDrawLine(renderer_, drawX + tileSize - 4, drawY + 4, drawX + 4, drawY + tileSize - 4);
        }

        if (static_cast<int>(idx) == ent.currentSideNum) {
            SDL_SetRenderDrawColor(renderer_, 255, 255, 255, 255);
            SDL_Rect border{drawX - 2, drawY - 2, tileSize + 4, tileSize + 4};
            SDL_RenderDrawRect(renderer_, &border);
            SDL_Rect b{drawX - 1, drawY - 1, tileSize + 2, tileSize + 2};
            SDL_RenderDrawRect(renderer_, &b);
            SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);
            SDL_Rect b2{drawX - 3, drawY - 3, tileSize + 6, tileSize + 6};
            SDL_RenderDrawRect(renderer_, &b2);
            SDL_Rect b3{drawX - 4, drawY - 4, tileSize + 8, tileSize + 8};
            SDL_RenderDrawRect(renderer_, &b3);
        }

        TTF_Font* font = getFont("side_value", 20)->font;
        drawText(renderer_, font, std::to_string(side->value), drawX, drawY, {255, 255, 255, 255});
    }
}

void Render::drawSidePreview(const std::optional<Side>& side, int x, int y, int size)
{
    if (!side.has_value()) {
        return;
    }

    const std::string imageName = sideTypeToImageName(side->type);
    const std::filesystem::path imagePath = imageDir_ / imageName;
    if (std::filesystem::exists(imagePath)) {
        const std::pair<std::string, int> imageKey{imageName, size};
        SDL_Texture* texture = nullptr;
        const auto it = imageCache_.find(imageKey);
        if (it == imageCache_.end()) {
            texture = loadTexture(renderer_, imagePath);
            imageCache_.emplace(imageKey, texture);
        } else {
            texture = it->second;
        }

        SDL_Rect target{x, y, size, size};
        SDL_RenderCopy(renderer_, texture, nullptr, &target);
    }

    TTF_Font* font = getFont("state_side_value", 16)->font;
    drawText(renderer_, font, std::to_string(side->value), x, y, {255, 255, 255, 255});
}

void Render::renderStateOverlay(const State& state)
{
    int width = 0;
    int height = 0;
    SDL_GetWindowSize(window_, &width, &height);

    TTF_Font* font = getFont("state_text", 18)->font;
    TTF_Font* smallFont = getFont("state_text_small", 16)->font;
    const int lineHeight = 22;
    const int leftX = 20;
    const int leftY = height - kConsoleHeight - 150;

    drawText(renderer_, font, "SpellData=" + joinArray(state.spellData), leftX, leftY, {0, 0, 0, 255});
    drawText(renderer_, font, "rerolls: " + std::to_string(state.rerolls), leftX, leftY + lineHeight, {0, 0, 0, 255});

    const int rightX = leftX + 320;
    drawText(renderer_, font, "turn: " + std::to_string(state.turn), rightX, leftY, {0, 0, 0, 255});
    if (state.lastTarget != nullptr) {
        drawText(renderer_, font, "lastTarget: " + std::to_string(state.lastTarget->sourceID) + "-" + std::to_string(state.lastTarget->positionID), rightX, leftY + lineHeight, {0, 0, 0, 255});
    } else {
        drawText(renderer_, font, "lastTarget: None", rightX, leftY + lineHeight, {0, 0, 0, 255});
    }
    drawText(renderer_, font, "prevLastStrikeVal: " + std::to_string(state.prevLastStrikeVal), rightX, leftY + 2 * lineHeight, {0, 0, 0, 255});
    drawText(renderer_, font, "lastStrikeVal: " + std::to_string(state.lastStrikeVal), rightX, leftY + 3 * lineHeight, {0, 0, 0, 255});
    drawText(renderer_, font, "lastDie:", rightX, leftY + 4 * lineHeight, {0, 0, 0, 255});
    drawSidePreview(state.lastDie, rightX + 70, leftY + 4 * lineHeight - 2);

    const int midX = width / 2 - 220;
    const int midY = height - 150;
    drawText(renderer_, font, "mana: " + std::to_string(state.mana), midX, midY, {0, 0, 0, 255});
    drawText(renderer_, font, "fortoldMana: " + std::to_string(state.fortoldMana), midX, midY + lineHeight, {0, 0, 0, 255});

    const int midRightX = midX + 240;
    drawText(renderer_, font, "level: " + std::to_string(state.level), midRightX, midY, {0, 0, 0, 255});
    drawText(renderer_, font, "playerLevels: " + joinArray(state.playerLevels), midRightX, midY + lineHeight, {0, 0, 0, 255});

    const int topX = width / 2 - 220;
    const int topY = 12;
    std::vector<std::string> imminentNames;
    for (const Ent* ent : state.imminentSummons) {
        if (ent == nullptr) {
            continue;
        }
        try {
            imminentNames.push_back(resolveReverseIdName(ent->sourceID, entsReversedIDs, "sourceID"));
        } catch (...) {
            imminentNames.push_back(std::to_string(ent->sourceID));
        }
    }

    drawText(renderer_, font, "imminentSummons:", topX, topY, {0, 0, 0, 255});
    drawText(renderer_, smallFont, imminentNames.empty() ? std::string{"[]"} : [&]() {
        std::ostringstream stream;
        for (std::size_t i = 0; i < imminentNames.size(); ++i) {
            if (i != 0) {
                stream << ", ";
            }
            stream << imminentNames[i];
        }
        return stream.str();
    }(), topX, topY + lineHeight, {0, 0, 0, 255});

    const int reinfX = topX + 260;
    std::vector<std::string> reinfNames;
    for (const Ent* ent : state.reinforcements) {
        if (ent == nullptr) {
            continue;
        }
        try {
            reinfNames.push_back(resolveReverseIdName(ent->sourceID, entsReversedIDs, "sourceID"));
        } catch (...) {
            reinfNames.push_back(std::to_string(ent->sourceID));
        }
    }

    drawText(renderer_, font, "reinforcements:", reinfX, topY, {0, 0, 0, 255});
    drawText(renderer_, smallFont, reinfNames.empty() ? std::string{"[]"} : [&]() {
        std::ostringstream stream;
        for (std::size_t i = 0; i < reinfNames.size(); ++i) {
            if (i != 0) {
                stream << ", ";
            }
            stream << reinfNames[i];
        }
        return stream.str();
    }(), reinfX, topY + lineHeight, {0, 0, 0, 255});

    const int rightInfoX = width - 360;
    const int rightInfoY = height - 150;
    drawText(renderer_, font, "enemyDamageD: " + std::to_string(state.enemyDamageD), rightInfoX, rightInfoY, {0, 0, 0, 255});
    drawText(renderer_, font, "hexiaD: " + std::to_string(state.hexiaD), rightInfoX, rightInfoY + lineHeight, {0, 0, 0, 255});
    drawText(renderer_, font, "bansheeD: " + std::to_string(state.bansheeD), rightInfoX, rightInfoY + 2 * lineHeight, {0, 0, 0, 255});
    drawText(renderer_, font, "brambleD: " + std::to_string(state.brambleD), rightInfoX, rightInfoY + 3 * lineHeight, {0, 0, 0, 255});
    drawText(renderer_, font, "madnessD: " + std::to_string(state.madnessD), rightInfoX, rightInfoY + 4 * lineHeight, {0, 0, 0, 255});
    drawText(renderer_, font, "handD: " + std::to_string(state.handD), rightInfoX, rightInfoY + 5 * lineHeight, {0, 0, 0, 255});
    drawText(renderer_, font, "warchiefD: " + std::to_string(state.warchiefD), rightInfoX, rightInfoY + 6 * lineHeight, {0, 0, 0, 255});
}

void Render::renderEnt(const Ent& ent, std::pair<int, int> position, bool isEnemy)
{
    const int x = position.first;
    const int y = position.second;
    const int cardW = kCardSize.first;
    const int cardH = getCardHeight(ent);
    const int squareSize = static_cast<int>(kSideSquareSize * 0.8f * getRenderScale(ent));

    const int squareX = isEnemy ? x - squareSize : x + cardW;
    const int squareY = y;

    SDL_SetRenderDrawColor(renderer_, 80, 80, 80, 255);
    SDL_Rect outer{x, y, cardW, cardH};
    SDL_RenderDrawRect(renderer_, &outer);

    const bool muted = ent.dodged || ent.intangibled;
    SDL_SetRenderDrawColor(renderer_, muted ? 40 : 0, muted ? 40 : 0, muted ? 45 : 0, 255);
    SDL_Rect inner{x + 6, y + 6, cardW - 12, cardH - 12};
    SDL_RenderFillRect(renderer_, &inner);

    if (ent.dead) {
        SDL_SetRenderDrawColor(renderer_, 220, 40, 40, 255);
        SDL_RenderDrawLine(renderer_, x + 10, y + 10, x + cardW - 10, y + cardH - 10);
        SDL_RenderDrawLine(renderer_, x + cardW - 10, y + 10, x + 10, y + cardH - 10);
    }

    TTF_Font* metaFont = getFont("meta_text", 18)->font;
    try {
        const std::string sourceName = resolveReverseIdName(ent.sourceID, entsReversedIDs, "sourceID");
        drawText(renderer_, metaFont, "sourceID: " + std::to_string(ent.sourceID) + ", [" + sourceName + "]", x + 6, std::max(4, y - 18), {0, 0, 0, 255});
    } catch (...) {
        drawText(renderer_, metaFont, "sourceID: " + std::to_string(ent.sourceID), x + 6, std::max(4, y - 18), {0, 0, 0, 255});
    }

    if (ent.spellID != -1) {
        try {
            const std::string spellName = resolveReverseIdName(ent.spellID, spellsReversedIDs, "spellID");
            drawText(renderer_, metaFont, "spellID: " + std::to_string(ent.spellID) + ", [" + spellName + "]", x + 6, y + cardH + 4, {0, 0, 0, 255});
        } catch (...) {
            drawText(renderer_, metaFont, "spellID: " + std::to_string(ent.spellID), x + 6, y + cardH + 4, {0, 0, 0, 255});
        }
    }

    SDL_SetRenderDrawColor(renderer_, ent.usedDie ? 140 : 255, ent.usedDie ? 0 : 255, ent.usedDie ? 0 : 255, 255);
    SDL_Rect square{squareX, squareY, squareSize, squareSize};
    SDL_RenderDrawRect(renderer_, &square);
    SDL_SetRenderDrawColor(renderer_, muted ? 40 : 0, muted ? 40 : 0, muted ? 45 : 0, 255);
    SDL_Rect squareInner{squareX + 6, squareY + 6, squareSize - 12, squareSize - 12};
    SDL_RenderFillRect(renderer_, &squareInner);

    struct StatItem {
        std::string label;
        SDL_Color color;
    };

    std::vector<StatItem> statItems;
    if (ent.weaken != 0) {
        statItems.push_back({"-" + std::to_string(ent.weaken), {180, 80, 220, 255}});
    }
    if (ent.smith != 0) {
        statItems.push_back({"+" + std::to_string(ent.smith), {180, 180, 180, 255}});
    }
    if (ent.boost != 0) {
        statItems.push_back({"+" + std::to_string(ent.boost), {220, 40, 40, 255}});
    }
    if (ent.permaboost != 0) {
        statItems.push_back({"+" + std::to_string(ent.permaboost), {255, 255, 255, 255}});
    }

    if (!statItems.empty()) {
        const int iconW = 18;
        const int gap = 6;
        const int totalW = static_cast<int>(statItems.size()) * iconW + static_cast<int>(statItems.size() - 1) * gap;
        const int startX = squareX + std::max(4, (squareSize - totalW) / 2);
        const int startY = std::max(4, squareY - 20);
        TTF_Font* statFont = getFont("square_stat", 18)->font;

        for (std::size_t i = 0; i < statItems.size(); ++i) {
            drawText(renderer_, statFont, statItems[i].label, startX + static_cast<int>(i) * (iconW + gap), startY, statItems[i].color);
        }
    }

    drawHpBar(ent, x, y, cardW, cardH);

    struct EffectItem {
        std::string name;
        int value;
        SDL_Color color;
    };

    std::vector<EffectItem> effects;
    if (ent.poison != 0) {
        effects.push_back({"poison", ent.poison, {0, 180, 0, 255}});
    }
    if (ent.regen != 0) {
        effects.push_back({"regen", ent.regen, {220, 40, 40, 255}});
    }
    if (ent.vulnerable != 0) {
        effects.push_back({"vulnerable", ent.vulnerable, {220, 140, 0, 255}});
    }
    if (ent.incomingPoison != 0) {
        effects.push_back({"incomingPoison", ent.incomingPoison, {120, 220, 0, 255}});
    }
    if (ent.incomingDamage != 0) {
        effects.push_back({"incomingDamage", ent.incomingDamage, {220, 220, 0, 255}});
    }

    std::vector<std::pair<int, int>> effectPositions;
    if (!effects.empty()) {
        constexpr int iconSize = 16;
        constexpr int spacing = 4;
        constexpr int maxPerRow = 3;
        const int rowHeight = iconSize + spacing;
        const int totalWidth = maxPerRow * (iconSize + spacing) - spacing;
        const int startX = isEnemy ? x + cardW - 6 - totalWidth : x + 10;
        const int startY = y + 14;

        for (std::size_t i = 0; i < effects.size(); ++i) {
            const int row = static_cast<int>(i) / maxPerRow;
            const int col = static_cast<int>(i) % maxPerRow;
            effectPositions.emplace_back(startX + col * (iconSize + spacing), startY + row * rowHeight);
        }
    }

    for (std::size_t i = 0; i < effects.size() && i < effectPositions.size(); ++i) {
        const auto& effect = effects[i];
        std::string sign = effect.value > 0 ? "+" : "";
        if (effect.name == "poison") {
            sign = effect.value > 0 ? "+" : "-";
        } else if (effect.name == "regen" || effect.name == "vulnerable" || effect.name == "incomingPoison" || effect.name == "incomingDamage") {
            sign = effect.value > 0 ? "+" : "";
        }

        TTF_Font* effectFont = getFont("effect_" + effect.name, 18)->font;
        drawText(renderer_, effectFont, sign + std::to_string(std::abs(effect.value)), effectPositions[i].first, effectPositions[i].second, effect.color);
    }

    if (ent.dodged) {
        constexpr int checkerSize = 6;
        for (int yy = 0; yy < 24; yy += checkerSize * 2) {
            for (int xx = 0; xx < 24; xx += checkerSize * 2) {
                SDL_SetRenderDrawColor(renderer_, 140, 140, 140, 255);
                SDL_Rect light{x + cardW - 34 + xx, y + 32 + yy, checkerSize, checkerSize};
                SDL_RenderFillRect(renderer_, &light);
                SDL_SetRenderDrawColor(renderer_, 90, 90, 90, 255);
                SDL_Rect dark{x + cardW - 34 + xx + checkerSize, y + 32 + yy + checkerSize, checkerSize, checkerSize};
                SDL_RenderFillRect(renderer_, &dark);
            }
        }
    }

    if (ent.intangibled) {
        const int noteX = x + cardW - 18;
        const int noteY = y + 32;
        SDL_RenderDrawLine(renderer_, noteX, noteY + 10, noteX, noteY + 2);
        SDL_RenderDrawLine(renderer_, noteX + 2, noteY + 8, noteX + 2, noteY);
        drawCircleOutline(renderer_, noteX + 2, noteY + 12, 3, {255, 255, 255, 255});
        drawCircleOutline(renderer_, noteX + 2, noteY + 2, 3, {255, 255, 255, 255});
    }

    const Side& currentSide = ent.currentSide;
    const bool drawCurrentSide = true; // currentSide.type != SideType::BLANK || currentSide.value != -1;
    if (drawCurrentSide) {
        const std::string imageName = sideTypeToImageName(currentSide.type);
        const std::filesystem::path imagePath = imageDir_ / imageName;
        if (std::filesystem::exists(imagePath)) {
            const std::pair<std::string, int> imageKey{imageName, squareSize - 20};
            SDL_Texture* texture = nullptr;
            const auto it = imageCache_.find(imageKey);
            if (it == imageCache_.end()) {
                texture = loadTexture(renderer_, imagePath);
                imageCache_.emplace(imageKey, texture);
            } else {
                texture = it->second;
            }

            SDL_Rect target{squareX + 10, squareY + 10, squareSize - 20, squareSize - 20};
            SDL_RenderCopy(renderer_, texture, nullptr, &target);

            TTF_Font* currentSideFont = getFont("current_side_value", 18)->font;
            drawText(renderer_, currentSideFont, std::to_string(currentSide.value), squareX + squareSize - 6 - 8, squareY + squareSize - 6 - 8, {255, 255, 255, 255});
        }
    }

    if (ent.exerted) {
        SDL_SetRenderDrawColor(renderer_, 180, 80, 220, 255);
        SDL_RenderDrawLine(renderer_, squareX + 8, squareY + 8, squareX + squareSize - 8, squareY + squareSize - 8);
        SDL_RenderDrawLine(renderer_, squareX + squareSize - 8, squareY + 8, squareX + 8, squareY + squareSize - 8);
        SDL_RenderDrawLine(renderer_, squareX + 8, squareY + squareSize - 8, squareX + squareSize - 8, squareY + squareSize - 8);
        SDL_RenderDrawLine(renderer_, squareX + 8, squareY + 8, squareX + squareSize - 8, squareY + 8);
    }

    const int conditionCount = (ent.undead ? 1 : 0)
        + (ent.petrifyAmount != 0 ? 1 : 0)
        + (ent.toexert ? 1 : 0)
        + (ent.cleansed ? 1 : 0)
        + (ent.inflictedPained ? 1 : 0)
        + (ent.inflictedExerted ? 1 : 0)
        + (ent.inflictedDeathed ? 1 : 0)
        + (ent.inflictedSingleUse ? 1 : 0);

    const std::vector<std::pair<int, int>> conditionPositions = getConditionIconPositions(x, y, cardW, cardH, conditionCount);
    std::size_t posIndex = 0;
    const std::array<std::pair<const char*, bool>, 8> conditionFlags{{
        {"undead", ent.undead},
        {"petrify", ent.petrifyAmount != 0},
        {"toexert", ent.toexert},
        {"cleansed", ent.cleansed},
        {"pain", ent.inflictedPained},
        {"exerted", ent.inflictedExerted},
        {"deathed", ent.inflictedDeathed},
        {"singleuse", ent.inflictedSingleUse}
    }};

    for (const auto& [name, enabled] : conditionFlags) {
        if (!enabled) {
            continue;
        }

        if (posIndex >= conditionPositions.size()) {
            break;
        }

        const auto [iconX, iconY] = conditionPositions[posIndex];
        if (std::string_view{name} == "undead") {
            drawCircleOutline(renderer_, iconX + 8, iconY + 8, 8, {255, 255, 255, 255});
        } else if (std::string_view{name} == "petrify") {
            SDL_SetRenderDrawColor(renderer_, 180, 180, 180, 255);
            SDL_Rect rect{iconX + 1, iconY + 1, 14, 14};
            SDL_RenderDrawRect(renderer_, &rect);
            TTF_Font* petrifyFont = getFont("petrify", 18)->font;
            drawText(renderer_, petrifyFont, std::to_string(ent.petrifyAmount), iconX + 2, iconY + 2, {180, 180, 180, 255});
        } else if (std::string_view{name} == "toexert") {
            SDL_SetRenderDrawColor(renderer_, 220, 220, 220, 255);
            SDL_RenderDrawLine(renderer_, iconX + 4, iconY + 2, iconX + 4, iconY + 14);
            SDL_RenderDrawLine(renderer_, iconX + 2, iconY + 4, iconX + 6, iconY + 10);
        } else if (std::string_view{name} == "cleansed") {
            SDL_SetRenderDrawColor(renderer_, 255, 255, 255, 255);
            SDL_RenderDrawLine(renderer_, iconX + 2, iconY + 14, iconX + 14, iconY + 8);
            SDL_RenderDrawLine(renderer_, iconX + 14, iconY + 8, iconX + 2, iconY + 2);
            SDL_RenderDrawLine(renderer_, iconX + 2, iconY + 2, iconX + 2, iconY + 14);
        } else {
            const std::string_view nameView{name};
            const char letter = nameView == "singleuse" ? 'S' : static_cast<char>(std::toupper(static_cast<unsigned char>(nameView.front())));
            TTF_Font* conditionFont = getFont("condition_letter", 18)->font;
            drawText(renderer_, conditionFont, std::string(1, letter), iconX + 3, iconY + 2, {180, 80, 220, 255});
        }
        ++posIndex;
    }

    drawShieldIndicator(ent, x, y, cardW, cardH, isEnemy, squareSize);
    drawDiceLayout(ent, x, y, cardW, cardH, isEnemy, squareSize);

    const int usageValue = isEnemy ? ent.timesDamaged : ent.timesUsed;
    if (usageValue != 0) {
        TTF_Font* usageFont = getFont("usage_value", 22)->font;
        drawCenteredText(renderer_, usageFont, std::to_string(usageValue), SDL_FRect{static_cast<float>(squareX), static_cast<float>(squareY + squareSize), static_cast<float>(squareSize), 24.0f}, {220, 40, 40, 255});
    }
}

void Render::renderAttackLines(const State& state)
{
    const std::vector<Ent*> playerEntities = toVector(state.players);
    const std::vector<Ent*> enemyEntities = state.enemies;

    const std::vector<std::pair<int, int>> playerPositions = [&]() {
        std::vector<std::pair<int, int>> positions;
        positions.reserve(playerEntities.size());
        for (std::size_t i = 0; i < playerEntities.size(); ++i) {
            positions.push_back(getPosition(i, playerEntities, false, playerEntities[i]));
        }
        return positions;
    }();

    const std::vector<std::pair<int, int>> enemyPositions = [&]() {
        std::vector<std::pair<int, int>> positions;
        positions.reserve(enemyEntities.size());
        for (std::size_t i = 0; i < enemyEntities.size(); ++i) {
            positions.push_back(getPosition(i, enemyEntities, true, enemyEntities[i]));
        }
        return positions;
    }();

    for (std::size_t i = 0; i < state.enemyTargets.size() && i < enemyPositions.size(); ++i) {
        const auto& attackData = state.enemyTargets[i];
        const auto monPos = enemyPositions[i];
        for (const Ent* target : attackData) {
            if (target == nullptr || target->sourceID < 0) {
                continue;
            }

            const auto playerIt = std::find(playerEntities.begin(), playerEntities.end(), target);
            if (playerIt == playerEntities.end()) {
                continue;
            }

            const std::size_t j = static_cast<std::size_t>(std::distance(playerEntities.begin(), playerIt));
            const auto heroPos = playerPositions[j];
            const int startX = monPos.first + kCardSize.first / 2;
            const int startY = monPos.second + getCardHeight(*enemyEntities[i]) / 2;
            const int endX = heroPos.first + kCardSize.first / 2 + 400;
            const int endY = heroPos.second + getCardHeight(*playerEntities[j]) / 2;

            SDL_SetRenderDrawColor(renderer_, 220, 40, 40, 255);
            SDL_RenderDrawLine(renderer_, startX, startY, endX, endY);
        }
    }
}

void Render::render(const State& state)
{
    SDL_SetRenderDrawColor(renderer_, 255, 220, 220, 255);
    SDL_RenderClear(renderer_);

    for (std::size_t i = 0; i < state.players.size(); ++i) {
        const Ent* ent = state.players[i];
        if (ent == nullptr) {
            continue;
        }
        const auto pos = getPosition(i, toVector(state.players), false, ent);
        renderEnt(*ent, pos, false);
    }

    for (std::size_t i = 0; i < state.enemies.size(); ++i) {
        const Ent* ent = state.enemies[i];
        if (ent == nullptr) {
            continue;
        }
        const auto pos = getPosition(i, state.enemies, true, ent);
        renderEnt(*ent, pos, true);
    }

    renderAttackLines(state);
    renderStateOverlay(state);
    drawConsoleOverlay();
    SDL_RenderPresent(renderer_);
}
