#pragma once

#include "Cannon.h"

class Arrow : public Cannon {
public:
    // Конструктор передаємо tile в базовий клас і ініціалізуємо нові поля
    Arrow(std::string tile, bool active = false)
        : Cannon(tile), _isVisible(active) {
    }

    bool IsVisible() const {
        return _isVisible;
    }
	std::string GetPreviousTile() const { return _prevTile; }

    void SetPreviousTile(std::string tile) { _prevTile = tile; }
    void SetVisibility(bool active) { _isVisible = active; }

private:
    std::string _prevTile = " ";
    bool _isVisible;
};
