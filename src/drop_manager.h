#ifndef DROP_MANAGER_H
#define DROP_MANAGER_H

#include <raylib.h>
#include <vector>
#include "entity.h"
#include "player.h"

class DropManager {
private:
    float gravity;
    float tileSize;
    std::vector<Drop> drops;

public:
    DropManager(float gravity, float tileSize);
    void addDrop(Vector2 position, Texture2D sprite);
    void updateDrops();
    void collectDrops(Player& player, std::vector<Texture2D>& inventory);
    void drawDrops() const;
};

#endif // DROP_MANAGER_H