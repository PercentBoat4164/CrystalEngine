#pragma once

#include <memory>
#include <string>
#include <vector>

class IEAsset;

/**
 * The IEAspect class is an interface that is designed to allow the addition of new aspect types.
 * Inherit from this class to be eligible to be added as an aspect to an asset.
 * The pre-designed aspect classes are:
 *  - Renderable
 *  - More to be added
 * The user is free to add aspects as they see fit.
 * @brief An interface class used to create aspects of an IEAsset.
 */
class IEAspect {
public:
    virtual ~IEAspect() = default;

    std::vector<std::weak_ptr<IEAsset>> associatedAssets{};  // A vector of assets that this aspect belongs to
};
