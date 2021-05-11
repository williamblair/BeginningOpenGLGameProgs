#ifndef ENTITY_TYPE_H_INCLUDED
#define ENTITY_TYPE_H_INCLUDED

#include <string>

enum class EntityType
{
    OGRO,
    PLAYER,
    ROCKET,
    EXPLOSION,
    LANDSCAPE,
    TREE
};

static inline std::string EntityTypeToString(EntityType entity)
{
    switch (entity)
    {
    case EntityType::OGRO: return "OGRO";
    case EntityType::PLAYER: return "PLAYER";
    case EntityType::ROCKET: return "ROCKET";
    case EntityType::EXPLOSION: return "EXPLOSION";
    case EntityType::LANDSCAPE: return "LANDSCAPE";
    case EntityType::TREE: return "TREE";
    }
    return std::string();
}

#endif // ENTITY_TYPE_H_INCLUDED
