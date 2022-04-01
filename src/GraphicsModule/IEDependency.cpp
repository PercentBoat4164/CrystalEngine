#include <algorithm>
#include "IEDependency.hpp"

#include "IEDependent.hpp"

void IEDependency::addDependent(IEDependent *dependent) {
    if (!isDependencyOf(dependent)) {
        dependents.push_back(dependent);
    }
    if (!dependent->isDependentOn(this)) {
        dependent->addDependency(this);
    }
}

bool IEDependency::isDependencyOf(IEDependent *dependent) {
    return std::any_of(dependents.begin(), dependents.end(), [dependent](IEDependent *thisDependent) { return thisDependent == dependent; });
}

bool IEDependency::hasNoDependents() {
    return dependents.empty();
}

void IEDependency::removeDependent(IEDependent *dependent) {
    if (dependent->isDependentOn(this)) {
        dependent->removeDependency(this);
    }
    dependents.erase(std::find(dependents.begin(), dependents.end(), dependent));
    if (dependent->hasNoDependencies())
        dependent->~IEDependent();
}

void IEDependency::removeAllDependents() {
    for (IEDependent *dependent : dependents) {
        dependent->removeDependency(this);
    }
}

void IEDependency::wait() {
    for (IEDependent *dependent : dependents) {
        dependent->wait();
    }
}

IEDependency::~IEDependency() = default;
